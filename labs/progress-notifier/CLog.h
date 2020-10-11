#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#define DEBUG_COLOR 0
#define INFO_COLOR 1
#define WARN_COLOR 2
#define ERROR_COLOR 3
#define PANIC_COLOR 4
#define PANIC_BCKG 5

int logConfig[6] = { 4, 2, 3, 1, 7, 1 };

void textcolor(int attr, int fg, int bg)
{
  char command[13];
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}

void genericPrinter(char *type, const char *format, int attr, int fg, int bg, va_list args)
{
  textcolor(attr, fg, bg);
  printf("%s", type);
  textcolor(0, -1, -1);
  printf(" ");
  vprintf(format, args);
  printf("\n");
}

int infof(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter(
    "INFO:",
    format,
    1,
    logConfig[INFO_COLOR],
    8,
    args
  );
  fflush(stdout);
  va_end(args);
}

int debugf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter(
    "DEBUG:",
    format,
    1,
    logConfig[DEBUG_COLOR],
    8,
    args
  );
  fflush(stdout);
  va_end(args);
}

int warnf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter(
    "WARNING:",
    format,
    1,
    logConfig[WARN_COLOR],
    8,
    args
  );
  fflush(stdout);
  va_end(args);
}

int errorf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter(
    "ERROR:",
    format,
    1,
    logConfig[ERROR_COLOR],
    8,
    args
  );
  fflush(stdout);
  va_end(args);
}

int panicf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter(
    "PANIC:",
    format, 1,
    logConfig[PANIC_COLOR],
    logConfig[PANIC_BCKG],
    args
  );
  fflush(stdout);
  va_end(args);
  raise(SIGABRT);
}
