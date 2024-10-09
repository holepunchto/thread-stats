#include <assert.h>
#include <bare.h>
#include <js.h>

#include "types.h"

#if THREAD_STATS_APPLE
#include "apple.h"
#elif THREAD_STATS_LINUX
#include "linux.h"
#endif

static js_value_t *
thread_stats (js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t len = 1024;
  thread_stats_t stats[1024];

#if THREAD_STATS_APPLE
  err = thread_stats__apple(stats, &len);
#elif THREAD_STATS_LINUX
  err = thread_stats__linux(stats, &len);
#elif THREAD_STATS_ANDROID
  len = err = 0;
#else
  err = -1;
#endif

  if (err < 0) {
    js_throw_error(env, NULL, "Could not get thread stats");
    return NULL;
  }

  js_value_t *result;
  err = js_create_array_with_length(env, len, &result);
  assert(err == 0);

  for (size_t i = 0; i < len; i++) {
    js_value_t *thread;
    err = js_create_object(env, &thread);
    assert(err == 0);

    err = js_set_element(env, result, i, thread);
    assert(err == 0);

#define V(name, property, type) \
  { \
    js_value_t *value; \
    err = js_create_##type(env, stats[i].property, &value); \
    assert(err == 0); \
    err = js_set_named_property(env, thread, name, value); \
    assert(err == 0); \
  }

    V("id", id, int64)
    V("cpuUsage", cpu_usage, double)
#undef V

    bool self = stats[i].self;

    js_value_t *value;
    err = js_get_boolean(env, self, &value);
    assert(err == 0);

    err = js_set_named_property(env, thread, "self", value);
    assert(err == 0);
  }

  return result;
}

static js_value_t *
thread_stats_exports (js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("threadStats", thread_stats);
#undef V

  return exports;
}

BARE_MODULE(thread_stats, thread_stats_exports)
