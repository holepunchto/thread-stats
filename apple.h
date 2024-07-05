#include <mach/mach.h>

#include "types.h"

static inline int
thread_stats__apple (thread_stats_t *stats, size_t *len) {
  kern_return_t res;

  thread_array_t threads;
  mach_msg_type_number_t threads_len = *len;

  res = task_threads(mach_task_self(), &threads, &threads_len);

  if (res != KERN_SUCCESS) {
    return -1;
  }

  task_info_data_t info;

  for (size_t i = 0, n = threads_len; i < n; i++) {
    thread_stats_t *stat = &stats[i];

    mach_msg_type_number_t info_len;

    info_len = TASK_INFO_MAX;
    res = thread_info(threads[i], THREAD_BASIC_INFO, (thread_info_t) info, &info_len);

    if (res != KERN_SUCCESS) {
      return -1;
    }

    thread_basic_info_t basic_info = (thread_basic_info_t) info;

    stat->cpu_usage = (float) basic_info->cpu_usage / (float) TH_USAGE_SCALE;
  }

  *len = threads_len;

  return 0;
}
