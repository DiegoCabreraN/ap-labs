
include ../../common.mk

APP_NAME=space-invaders
CC=gcc
CFLAGS=-Wall -pthread

build:
	${CC} ${CFLAGS} ${APP_NAME}.c -o ${APP_NAME}.o

test: build
	@echo Test Game
	./${APP_NAME}.o debug

clean:
	rm -rf ${APP_NAME}.o