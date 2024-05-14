include_directories(../ ${Z3MGR_DIR})
add_executable(ass3 test-ae.cpp)
target_link_libraries(ass3 ${LIB} assign3)
set_target_properties(ass3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# loops over ae_assert_files and run "ae $bc_file"
file(GLOB ae_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/ae/*.ll")
foreach(filename ${ae_assert_files})
    add_test(
            NAME ass3-ae/${filename}
            COMMAND ass3 ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

file(GLOB buf_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/buf/*.ll")
foreach(filename ${buf_files})
    add_test(
            NAME ass3-buf/${filename}
            COMMAND ass3 -overflow ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
include(CTest)


