#ifndef THREAD_STATS_TYPES
#define THREAD_STATS_TYPES

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t id;
  bool self;
  float cpu_usage;
} thread_stats_t;

#endif // THREAD_STATS_TYPES
