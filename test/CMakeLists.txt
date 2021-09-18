

set(TEST_SOURCES
        reflectiveTest.cpp
        toTreelikeMultilineStringTest.cpp
        nlohmannJsonTransformationTest.cpp
        )

add_executable(unitTests ${TEST_SOURCES})

target_include_directories(unitTests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(unitTests gtest gtest_main)

add_test(unitTests unitTests)
