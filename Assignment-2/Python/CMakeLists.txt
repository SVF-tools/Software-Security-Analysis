message(STATUS "Adding test for sse_assert_files")
file(GLOB ass2files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/sse/*.ll")
message(STATUS "file for sse_assert_files: ${ass2files}")
foreach(filename ${ass2files})
    message(STATUS "Adding test for ${filename}")
    add_test(
            NAME ass2-py/${filename}
            COMMAND python3 Main.py ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()