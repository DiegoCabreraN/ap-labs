#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

void textcolor(int attr, int fg, int bg);
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

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
}

int infof(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter("INFO:", format, 1, 6, 8, args);
  fflush(stdout);
  va_end(args);
}

int warnf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter("WARNING:", format, 1, 3, 8, args);
  fflush(stdout);
  va_end(args);
}

int errorf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter("ERROR:", format, 1, 1, 8, args);
  fflush(stdout);
  va_end(args);
}

int panicf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  genericPrinter("PANIC:", format, 1, 7, 1, args);
  fflush(stdout);
  va_end(args);
  raise(SIGABRT);
}