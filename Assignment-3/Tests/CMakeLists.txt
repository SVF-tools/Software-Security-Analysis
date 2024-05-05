include_directories(../ ${Z3MGR_DIR})
add_executable(assign-3 test-absexe.cpp)
target_link_libraries(assign-3 ${LIB} assign3)
set_target_properties(assign-3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# loops over ae_assert_files and run "ae $bc_file"
file(GLOB ae_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/ae/*.ll")
foreach(filename ${ae_assert_files})
    add_test(
            NAME ae_assert_tests/${filename}
            COMMAND assign-3 ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

file(GLOB buf_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/buf/*.ll")
foreach(filename ${buf_files})
    add_test(
            NAME buf_files/${filename}
            COMMAND assign-3 -overflow ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
include(CTest)


