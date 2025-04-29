include_directories( ${Z3MGR_DIR})
file(GLOB SOURCES
        *.cpp
        ${Z3MGR_DIR}/*.cpp
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-sse.cpp")
set(LIB ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})
add_library(assign-2 ${SOURCES})
target_link_libraries(assign-2 ${LIB})
set_target_properties(assign-2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_executable(ass2 test-sse.cpp)
target_link_libraries(ass2 ${LIB} assign-2)
set_target_properties(ass2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Adding test for sse_assert_files")
file(GLOB ass2files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/sse/*.ll")
message(STATUS "file for sse_assert_files: ${ass2files}")
foreach(filename ${ass2files})
    message(STATUS "Adding test for ${filename}")
    add_test(
            NAME ass2/${filename}
            COMMAND ass2 ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
