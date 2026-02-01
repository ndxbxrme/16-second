set(TEST_TARGET sixteen_second_tests)

add_executable(${TEST_TARGET}
  test_sanity.cpp
  test_memory_buffer.cpp
  test_state_machine.cpp
  test_overdub.cpp
  test_rate_stepper.cpp
  test_smoother.cpp
  test_feedback_model.cpp
  test_limiter.cpp
  test_lfo.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/MemoryBuffer.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/StateMachine.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/Overdub.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/RateStepper.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/Smoother.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/FeedbackModel.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/Limiter.cpp
  ${CMAKE_SOURCE_DIR}/Source/dsp/LFO.cpp
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
