CC=gcc
CFLAGS= -lfftw3 -lm
DEP= modified_monofft.c fft_sender.c fft.c

all: $(DEP)
	$(CC) $(DEP) $(CFLAGS) -o sender 

clean:
	rm -rf sender
