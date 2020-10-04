#define _XOPEN_SOURCE 700
#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ftw.h>
#include "logger.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + 200))
#define MAX_DIRS 256

int inotifyFd;
int lastDir;

int fd_limit = 20;
int flags = FTW_CHDIR | FTW_DEPTH | FTW_MOUNT;

char *subdirs[MAX_DIRS];
char *descriptors[MAX_DIRS];
char *lastName;

int nftwfunc(const char *filename, const struct stat *statptr,
             int fileflags, struct FTW *pfwt)
{
  if (fileflags == FTW_DP && pfwt->level < 2)
  {
    char name[128];
    strcpy(name, filename);
    subdirs[lastDir++] = strcat(name, "/");
  }
  return 0;
}

void refreshDirs(){
  char *root = subdirs[lastDir - 1];
  lastDir = 0;
  nftw(root, nftwfunc, fd_limit, flags);

  for (int j = 0; j < lastDir; j++) {
    int wd = inotify_add_watch(inotifyFd, subdirs[j], IN_ALL_EVENTS);
    descriptors[wd] = strdup(subdirs[j]);
    if (wd == -1)
      exit(-1);
  }
}

static void displayInotifyEvent(int inotifyInstance, struct inotify_event *i)
{
  if (i->mask & IN_ISDIR)
  {
    if (i->mask & IN_CREATE)
    {
      refreshDirs();
      infof("- [Directory - Create] - %s%s/\n", descriptors[i->wd], i->name);
    }
   if (i->mask & IN_MOVED_FROM)
      lastName = strdup(i->name);
    if (i->mask & IN_MOVED_TO)
      infof("- [Directory - Rename] - %s%s/ -> %s%s/\n", descriptors[i->wd], lastName, descriptors[i->wd], i->name);
    if (i->mask & IN_DELETE)
      infof("- [Directory - Removal] - %s%s/\n", descriptors[i->wd], i->name);
  }
  else
  {
    if (i->mask & IN_CREATE)
      infof("- [File - Create] - %s%s\n", descriptors[i->wd], i->name);
    if (i->mask & IN_DELETE)
      infof("- [File - Removal] - %s%s\n", descriptors[i->wd], i->name);
    if (i->mask & IN_ATTRIB)
      infof("- [File - Rename] - %s%s -> %s%s\n", descriptors[i->wd], lastName, descriptors[i->wd], i->name);
    if (i->mask & IN_MOVED_FROM)
      lastName = strdup(i->name);
  }
}

int main(int argc, char *argv[])
{
  char buf[BUF_LEN] __attribute__((aligned(8)));
  ssize_t numRead;
  char *p;
  struct inotify_event *event;

  if (argc < 2 || strcmp(argv[1], "--help") == 0)
    errorf("Usage is: %s <PATHNAME>\n", argv[0]);
  if (argc < 2)
  {
    exit(-1);
  }

  inotifyFd = inotify_init();
  if (inotifyFd == -1)
    exit(-1);

  char *startpath = argv[1];
  nftw(startpath, nftwfunc, fd_limit, flags);

  // add watch for events
  for (int j = 0; j < lastDir; j++)
  {
    int wd = inotify_add_watch(inotifyFd, subdirs[j], IN_ALL_EVENTS);
    descriptors[wd] = strdup(subdirs[j]);
    if (wd == -1)
      exit(-1);
  }
  infof("Starting File/Directory Monitor on %s\n", argv[1]);
  infof("---------------------------------------------------\n", argv[1]);

  for (;;)
  {
    numRead = read(inotifyFd, buf, BUF_LEN);
    if (numRead == 0)
      panicf("read() from inotify fd returned 0!\n");

    if (numRead == -1)
      exit(-1);

    for (p = buf; p < buf + numRead;)
    {
      event = (struct inotify_event *)p;
      displayInotifyEvent(inotifyFd, event);

      p += sizeof(struct inotify_event) + event->len;
    }
  }
  exit(0);
}
