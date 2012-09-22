unixynth
========

        gcc -o pasink -lportaudio pasink.c
        gcc -o saw saw.c
        mkfifo /tmp/noise
        ./pasink /tmp/noise & ; ./saw /tmp/noise