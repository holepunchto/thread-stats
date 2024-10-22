#include <assert.h>
#include <node_api.h>

#include "types.h"

#if THREAD_STATS_APPLE
#include "apple.h"
#elif THREAD_STATS_LINUX
#include "linux.h"
#endif

static napi_value
thread_stats (napi_env env, napi_callback_info info) {
  int err;

  size_t len = 1024;
  thread_stats_t stats[1024];

#if THREAD_STATS_APPLE
  err = thread_stats__apple(stats, &len);
#elif THREAD_STATS_LINUX
  err = thread_stats__linux(stats, &len);
#else
  err = -1;
#endif

  if (err < 0) {
    napi_throw_error(env, NULL, "Could not get thread stats");
    return NULL;
  }

  napi_value result;
  err = napi_create_array_with_length(env, len, &result);
  assert(err == 0);

  for (size_t i = 0; i < len; i++) {
    napi_value thread;
    err = napi_create_object(env, &thread);
    assert(err == 0);

    err = napi_set_element(env, result, i, thread);
    assert(err == 0);

#define V(name, property, type) \
  { \
    napi_value value; \
    err = napi_create_##type(env, stats[i].property, &value); \
    assert(err == 0); \
    err = napi_set_named_property(env, thread, name, value); \
    assert(err == 0); \
  }

    V("id", id, int64)
    V("cpuUsage", cpu_usage, double)
#undef V

    bool self = stats[i].self;

    napi_value value;
    err = napi_get_boolean(env, self, &value);
    assert(err == 0);

    err = napi_set_named_property(env, thread, "self", value);
    assert(err == 0);
  }

  return result;
}

static napi_value
thread_stats_exports (napi_env env, napi_value exports) {
  int err;

#define V(name, fn) \
  { \
    napi_value val; \
    err = napi_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = napi_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("threadStats", thread_stats);
#undef V

  return exports;
}

NAPI_MODULE(thread_stats, thread_stats_exports)
