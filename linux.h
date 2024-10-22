#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

static inline int
thread_stats__linux (thread_stats_t *stats, size_t *len) {
  int err;

  int self = getpid();
  clock_t clock_ticks = sysconf(_SC_CLK_TCK);

  int fd = open("/proc/uptime", O_RDONLY);

  if (fd == -1) {
    return -1;
  }

  char buf[4096 + 1 /* NULL */];
  ssize_t buf_len;

  buf_len = read(fd, buf, 4096);

  err = close(fd);
  assert(err == 0);

  if (buf_len == -1) {
    return -1;
  }

  float uptime;

  sscanf(buf, "%f", &uptime);

  snprintf(buf, 4096, "/proc/%d/task", self);

  DIR *dir = opendir(buf);

  if (dir == NULL) {
    return -1;
  }

  struct dirent *entry;

  int threads_len = 0;

  while (threads_len < *len && (entry = readdir(dir))) {
    // Skip "." and ".." folders
    if (strstr(entry->d_name, ".")) {
      continue;
    }

    int pid = atoi(entry->d_name);

    snprintf(buf, 4096, "/proc/%d/task/%d/stat", self, pid);

    int fd = open(buf, O_RDONLY);

    if (fd == -1) {
      continue;
    }

    buf_len = read(fd, buf, 4096);

    err = close(fd);
    assert(err == 0);

    if (buf_len == -1) {
      continue;
    }

    unsigned long utime, stime;
    unsigned long long starttime;

    // https://www.man7.org/linux/man-pages//man5/proc_pid_stat.5.html
    sscanf(buf, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d  %*d %*d %llu", &utime, &stime, &starttime);

    thread_stats_t *stat = &stats[threads_len];

    stat->id = pid;
    stat->self = self == pid;
    stat->cpu_usage = (utime + stime) / (uptime - ((float) starttime / (float) clock_ticks));

    ++threads_len;
  }

  err = closedir(dir);
  assert(err == 0);

  *len = threads_len;

  return 0;
}
