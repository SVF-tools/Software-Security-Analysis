include_directories(${Z3MGR_DIR})
file(GLOB SOURCES
        *.cpp
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-ae.cpp")
set(LIB ${SVF_LIB} ${llvm_libs})
add_library(assign3 ${SOURCES})
target_link_libraries(assign3 ${LIB})
set_target_properties(assign3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_executable(ass3 test-ae.cpp)
target_link_libraries(ass3 ${LIB} assign3)
set_target_properties(ass3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# loops over ae_assert_files and run "ae $bc_file"
file(GLOB ae_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/ae/*.ll")
foreach(filename ${ae_assert_files})
    add_test(
            NAME ass3-ae-cpp/${filename}
            COMMAND ass3 ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

file(GLOB buf_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/../Tests "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/buf/*.ll")
foreach(filename ${buf_files})
    add_test(
            NAME ass3-buf-cpp/${filename}
            COMMAND ass3 -overflow ${CMAKE_CURRENT_SOURCE_DIR}/../Tests/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

set(ASS3_LEVEL1_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/level-1")

# Level 1 contains the small, self-validating public cases. The public Level-2
# fixture is run manually; no Level-3 fixture is published.
file(GLOB case_files RELATIVE "${ASS3_LEVEL1_ROOT}"
        "${ASS3_LEVEL1_ROOT}/cases/*/public/*/*.ll")
list(SORT case_files)
foreach(filename ${case_files})
    set(test_name "ass3-level-1-cpp/${filename}")
    add_test(
            NAME ${test_name}
            COMMAND ass3 "${ASS3_LEVEL1_ROOT}/${filename}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    set_tests_properties(${test_name} PROPERTIES
            TIMEOUT 120
            LABELS "ass3;ass3-level-1;cpp")
endforeach()
include(CTest)
