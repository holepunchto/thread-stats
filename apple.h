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
  mach_msg_type_number_t info_len;

  info_len = TASK_INFO_MAX;
  res = thread_info(mach_thread_self(), THREAD_IDENTIFIER_INFO, (thread_info_t) info, &info_len);

  thread_identifier_info_t identifier_info = (thread_identifier_info_t) info;

  int64_t self = identifier_info->thread_id;

  for (size_t i = 0, n = threads_len; i < n; i++) {
    thread_stats_t *stat = &stats[i];

    info_len = TASK_INFO_MAX;
    res = thread_info(threads[i], THREAD_IDENTIFIER_INFO, (thread_info_t) info, &info_len);

    if (res != KERN_SUCCESS) {
      return -1;
    }

    thread_identifier_info_t identifier_info = (thread_identifier_info_t) info;

    stat->id = identifier_info->thread_id;

    stat->self = self == stat->id;

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
