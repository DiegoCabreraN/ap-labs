
include ../../common.mk

APP_NAME=space-invaders
CC=gcc
CFLAGS=-Wall -pthread

build:
	${CC} ${CFLAGS} ${APP_NAME}.c -o ${APP_NAME}.o

run: build
	@echo Test Game
	./${APP_NAME}.o

clean:
	rm -rf ${APP_NAME}.o