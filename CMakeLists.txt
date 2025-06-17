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

# Find all test files ending with _test.cpp in the tests directory
file(GLOB TEST_SOURCES "tests/*_test.cpp")

# List any production C files you want to compile into the test binary (for testing static functions, etc.)
set(TESTABLE_CODE
    # src/controller_state.c
    tasks/activate_buzzer.c
    tasks/bt_alert.c
    tasks/motion_detect.c
    tasks/rfid.c
    tasks/sleep_check.c
    # Add more C files to test as needed
)

# List all test mocks (now just your unified mocks file)
set(MOCKS
    tests/test_mocks.c
    # Add more mocks here if needed in the future
)

# --- GoogleTest via find_package ---
find_package(GTest REQUIRED)

if(TEST_SOURCES)
    add_executable(BikeAlarmTests ${TEST_SOURCES} ${TESTABLE_CODE} ${MOCKS})
    target_include_directories(BikeAlarmTests PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/tasks
    )
    # This define is critical for exposing last_scan_time and other test hooks!
    target_compile_definitions(BikeAlarmTests PRIVATE UNIT_TEST)
    target_link_libraries(BikeAlarmTests PRIVATE GTest::gtest GTest::gtest_main pthread)
    add_test(NAME BikeAlarmTests COMMAND BikeAlarmTests)
endif()