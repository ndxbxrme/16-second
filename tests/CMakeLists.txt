set(TEST_TARGET sixteen_second_tests)

add_executable(${TEST_TARGET}
  test_sanity.cpp
  test_memory_buffer.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/MemoryBuffer.cpp
)

target_link_libraries(${TEST_TARGET}
  PRIVATE
    Catch2::Catch2WithMain
)

target_include_directories(${TEST_TARGET}
  PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
)

include(CTest)
include(Catch)
catch_discover_tests(${TEST_TARGET})
