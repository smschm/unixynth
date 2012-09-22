#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "portaudio.h"

#define SAMPLE_RATE 44100.0
#define FRAMES_PER_BUFFER 1024
#define CHANNELS 2

#define SAMPLE_TYPE paFloat32
#define SAMPLE_SIZE 4

int fd;
void shutdown(int);
void on_interrupt(int);


int main(int argc, char ** argv) {

	struct sigaction s;
	sigset_t ss;
	sigemptyset(&ss);
	s.sa_handler = &on_interrupt;
	sigemptyset(&s.sa_mask);
	s.sa_flags = 0;
	s.sa_restorer = NULL;
	sigaction(SIGINT, &s, NULL);

	PaDeviceIndex odev;
	PaDeviceInfo * odev_info;
	PaStreamParameters odev_p;
	PaStream * ostream;

	int bufsz = FRAMES_PER_BUFFER * CHANNELS * SAMPLE_SIZE;
	int bufrd = 0;
	int i;
	char * buf = (char*) malloc(bufsz);
	
	Pa_Initialize();
	odev = Pa_GetDefaultOutputDevice();
	odev_info = Pa_GetDeviceInfo(odev);
	
	odev_p.device = odev;
	odev_p.channelCount = CHANNELS;
	odev_p.sampleFormat = SAMPLE_TYPE;
	odev_p.suggestedLatency = odev_info->defaultLowOutputLatency;
	odev_p.hostApiSpecificStreamInfo = NULL;
	
	i = Pa_OpenStream(&ostream, NULL, &odev_p, SAMPLE_RATE, FRAMES_PER_BUFFER,
	             paNoFlag, NULL, NULL);
	//printf("s = %d\n",i);
	Pa_StartStream(ostream);
	fd = open(argv[1], O_RDONLY);
	
	for (;;) {
		bufrd = 0;
		while (bufrd < bufsz) {
			i = read(fd, buf+bufrd, bufsz-bufrd);
			//printf("read %d\n", i);
			bufrd += i;
			if (i == 0)
				shutdown(1);
		}
		Pa_WriteStream(ostream, buf, FRAMES_PER_BUFFER);
	}
}

void on_interrupt(int sig) {
	printf("trapped SIGINT\n");
	shutdown(0);
}


void shutdown(int reason) {
	if (reason == 1)
		printf("done: EOF on pipe.\n");
	Pa_Terminate();
	close(fd);
	exit(0);
}
