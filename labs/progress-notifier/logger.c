#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <syslog.h>
#include "logger.h"

int syslogOn = 0;
int logPriority = 0;

void textcolor(int attr, int fg, int bg);

int initLogger(char *logType)
{
  if (strcmp(logType, "syslog") == 0)
  {
    syslogOn = 1;
  }
  printf("Initializing Logger on: %s\n", logType);
  return 0;
}

void textcolor(int attr, int fg, int bg)
{
  char command[13];
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}

void genericPrinter(char *type, const char *format, int attr, int fg, int bg, va_list args)
{
  if(syslogOn){
    openlog("Custom Logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
    vsyslog(logPriority, format, args);
    closelog();
  }
  else{
    textcolor(attr, fg, bg);
    printf("%s", type);
    textcolor(0, -1, -1);
    printf(" ");
    vprintf(format, args);
    printf("\n");
  }
}

int infof(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  logPriority = LOG_INFO;
  genericPrinter("INFO:", format, 1, 6, 8, args);
  fflush(stdout);
  va_end(args);
}

int warnf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  logPriority = LOG_WARNING;
  genericPrinter("WARNING:", format, 1, 3, 8, args);
  fflush(stdout);
  va_end(args);
}

int errorf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  logPriority = LOG_ERR;
  genericPrinter("ERROR:", format, 1, 1, 8, args);
  fflush(stdout);
  va_end(args);
}

int panicf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  logPriority = LOG_ALERT;
  genericPrinter("PANIC:", format, 1, 7, 1, args);
  fflush(stdout);
  va_end(args);
  raise(SIGABRT);
}
