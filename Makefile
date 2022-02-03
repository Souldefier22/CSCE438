#makefile

all: crsd crc

crc: crc.c interface.h
	g++ crc.c -lpthread -o crc

crsd: crsd.c interface.h
	g++ crsd.c -lpthread -o crsd