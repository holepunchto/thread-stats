cmake_minimum_required(VERSION 3.25)

project(thread_stats C)

include(bare)
include(napi)

bare_target(target)

add_bare_module(thread_stats_bare)

target_sources(
  ${thread_stats_bare}
  PRIVATE
    binding.c
)

if(target MATCHES "darwin|ios")
  target_compile_definitions(
    ${thread_stats_bare}
    PRIVATE
      THREAD_STATS_APPLE=1
  )
elseif(target MATCHES "linux")
  target_compile_definitions(
    ${thread_stats_bare}
    PRIVATE
      THREAD_STATS_LINUX=1
  )
elseif(target MATCHES "android")
  target_compile_definitions(
    ${thread_stats_bare}
    PRIVATE
      THREAD_STATS_ANDROID=1
  )
endif()

add_napi_module(thread_stats_node)

bare_include_directories(compat NAPI)

target_include_directories(
  ${thread_stats_node}
  PRIVATE
    ${compat}
)

target_sources(
  ${thread_stats_node}
  PRIVATE
    binding.c
)

if(target MATCHES "darwin|ios")
  target_compile_definitions(
    ${thread_stats_node}
    PRIVATE
      THREAD_STATS_APPLE=1
  )
elseif(target MATCHES "linux")
  target_compile_definitions(
    ${thread_stats_node}
    PRIVATE
      THREAD_STATS_LINUX=1
  )
elseif(target MATCHES "android")
  target_compile_definitions(
    ${thread_stats_node}
    PRIVATE
      THREAD_STATS_ANDROID=1
  )
endif()
