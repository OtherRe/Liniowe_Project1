cmake_minimum_required(VERSION 3.10)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories (${SBSProject_SOURCE_DIR}/src)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)
add_executable(aisdiLinearTests ./test/test_main.cpp ./test/LinkedListTests.cpp ./test/VectorTests.cpp)
add_executable(aisdiPerformanceTest ./src/main.cpp)
target_link_libraries(aisdiLinearTests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(boostUnitTestsRun aisdiLinearTests)

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
      --force-new-ctest-process --output-on-failure
      --build-config "$<CONFIGURATION>"
      DEPENDS aisdiLinearTests)
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
      --force-new-ctest-process --output-on-failure
      DEPENDS aisdiLinearTests)
endif()
