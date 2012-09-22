#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char ** argv) {
	int fd, i;
	fd = open(argv[1], O_WRONLY);

	float buf[256];
	for (i = 0; i < 256; i++) {
		buf[i] = ((float) i)/256.0 - 0.5;
	}

	for(;;) {
		write(fd, buf, sizeof(float)*256);
	}
}
