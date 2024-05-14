include_directories(../ ${Z3MGR_DIR})
add_executable(assign-2 test-sse.cpp)
target_link_libraries(assign-2 ${LIB} ass2)
set_target_properties(assign-2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


file(GLOB ass2files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/testcases/sse/*.ll")
foreach(filename ${ass2files})
    add_test(
            NAME ass2/${filename}
            COMMAND assign-2 ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
