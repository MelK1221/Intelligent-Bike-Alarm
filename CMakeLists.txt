cmake_minimum_required(VERSION 3.10)
project(Intelligent_Bike_Alarm C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ====== Only build firmware on non-macOS (optional) ======
if(NOT APPLE)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    file(GLOB SRC_FILES
        "src/*.c"
        "tasks/*.c"
        "drivers/*.c"
    )
    add_executable(Intelligent_Bike_Alarm ${SRC_FILES})
endif()

# ====== Unit Tests ======
enable_testing()

file(GLOB TEST_SOURCES "tests/*_test.cpp")
set(TESTABLE_CODE
    # src/controller_state.c
    tasks/activate_buzzer.c
    tasks/bt_alert.c
    # Add more C files to test as needed
)
set(MOCKS
    tests/mock_buzzer.c
    # Add more mocks if needed
)

# --- GoogleTest via FetchContent ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

if(TEST_SOURCES)
    add_executable(BikeAlarmTests ${TEST_SOURCES} ${TESTABLE_CODE} ${MOCKS})
    target_include_directories(BikeAlarmTests PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/include
    )
    target_compile_definitions(BikeAlarmTests PRIVATE UNIT_TEST)
    target_link_libraries(BikeAlarmTests gtest gtest_main pthread)
    add_test(NAME BikeAlarmTests COMMAND BikeAlarmTests)
endif()