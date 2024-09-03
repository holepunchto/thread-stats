#include <string.h>
#include <unistd.h>

#include "types.h"

static inline int
thread_stats__linux (thread_stats_t *stats, size_t *len) {
  int err;

  int self = getpid();
  clock_t clock_ticks = sysconf(_SC_CLK_TCK);

  float uptime;

  int uptime_fd;
  uptime_fd = open("/proc/uptime", O_RDONLY);

  if (uptime_fd == -1) {
    return -1;
  }

  char uptime_buf[4096 + 1];
  int uptime_buf_read;

  uptime_buf_read = read(uptime_fd, uptime_buf, 4096);

  if (uptime_buf_read == -1) {
    return -1;
  }

  err = close(uptime_fd);

  if (err == -1) {
    return -1;
  }

  sscanf(uptime_buf, "%f", &uptime);

  DIR *self_task_dir;

  char self_task_dir_path[4096 + 1];
  struct dirent *self_task_dir_entry;

  sprintf(self_task_dir_path, "/proc/%d/task", self);

  self_task_dir = opendir(self_task_dir_path);

  if (self_task_dir == NULL) {
    return -1;
  }

  int threads_len = 0;

  while (threads_len < *len && (self_task_dir_entry = readdir(self_task_dir))) {
    // skip "." and ".." folders
    if (strstr(self_task_dir_entry->d_name, ".")) {
      continue;
    }

    int pid = atoi(self_task_dir_entry->d_name);

    char stat_path[4096 + 1];
    sprintf(stat_path, "/proc/%d/task/%d/stat", self, pid);

    int stat_fd;
    stat_fd = open(stat_path, O_RDONLY);

    if (stat_fd == -1) {
      continue;
    }

    char stat_buf[4096 + 1];
    int stat_buf_read;

    stat_buf_read = read(stat_fd, stat_buf, 4096);

    if (stat_buf_read == -1) {
      continue;
    }

    err = close(stat_fd);

    if (err == -1) {
      break;
    }

    unsigned long utime, stime;
    unsigned long long starttime;

    // https://www.man7.org/linux/man-pages//man5/proc_pid_stat.5.html
    sscanf(stat_buf, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d  %*d %*d %llu", &utime, &stime, &starttime);

    thread_stats_t *stat = &stats[threads_len];

    stat->id = pid;
    stat->self = self == pid;
    stat->cpu_usage = (utime + stime) / (uptime - (starttime / clock_ticks));

    ++threads_len;
  }

  err = closedir(self_task_dir);

  if (err == -1) {
    return -1;
  }

  *len = threads_len;

  return 0;
}
