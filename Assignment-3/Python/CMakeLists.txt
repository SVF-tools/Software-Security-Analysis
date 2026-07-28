find_package(Python3 COMPONENTS Interpreter REQUIRED)

# loops over ae_assert_files and run "ae $bc_file"
file(GLOB ae_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/ae/*.ll")
foreach(filename ${ae_assert_files})
    add_test(
            NAME ass3-ae-py/${filename}
            COMMAND ${Python3_EXECUTABLE} test-ae.py ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

file(GLOB buf_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/buf/*.ll")
foreach(filename ${buf_files})
    add_test(
            NAME ass3-buf-py/${filename}
            COMMAND ${Python3_EXECUTABLE} test-ae.py ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

set(ASS3_LEVEL1_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/level-1")

# Level 1 contains the small, self-validating public cases. The public Level 2
# fixture is run manually.
file(GLOB case_files RELATIVE "${ASS3_LEVEL1_ROOT}"
        "${ASS3_LEVEL1_ROOT}/*.ll")
list(SORT case_files)
foreach(filename ${case_files})
    set(test_name "ass3-level-1-py/${filename}")
    add_test(
            NAME ${test_name}
            COMMAND ${Python3_EXECUTABLE} test-ae.py "${ASS3_LEVEL1_ROOT}/${filename}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_tests_properties(${test_name} PROPERTIES
            TIMEOUT 180
            LABELS "ass3;ass3-level-1;python")
endforeach()
include(CTest)
