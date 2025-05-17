# loops over ae_assert_files and run "ae $bc_file"
file(GLOB ae_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/ae/*.ll")
foreach(filename ${ae_assert_files})
    add_test(
            NAME ass3-ae-py/${filename}
            COMMAND python3 test-ae.py ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

file(GLOB buf_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/buf/*.ll")
foreach(filename ${buf_files})
    add_test(
            NAME ass3-buf-py/${filename}
            COMMAND python3 test-ae.py ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()
include(CTest)


