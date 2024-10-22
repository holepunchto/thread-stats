cmake_minimum_required(VERSION 3.25)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-napi REQUIRED PATHS node_modules/cmake-napi)

project(thread_stats C)

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
elseif(target MATCHES "linux|android")
  target_compile_definitions(
    ${thread_stats_bare}
    PRIVATE
      THREAD_STATS_LINUX=1
  )
endif()

add_napi_module(thread_stats_node)

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
elseif(target MATCHES "linux|android")
  target_compile_definitions(
    ${thread_stats_node}
    PRIVATE
      THREAD_STATS_LINUX=1
  )
endif()
