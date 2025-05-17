include_directories(../ ${GTRAV_DIR})
file(GLOB SOURCES
        Assignment_1.cpp
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test.cpp")
set(LIB ${SVF_LIB} ${llvm_libs})
add_library(assign-1 ${SOURCES})
target_link_libraries(assign-1 ${LIB})
set_target_properties(assign-1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_executable(ass1 test.cpp)
target_link_libraries(ass1 ${LIB} assign-1)
set_target_properties(ass1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# loops over pta_assert_files and run "ass1-pta $bc_file"
message(STATUS "Adding test for pta_assert_files")
file(GLOB pta_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/pta/*.ll")
foreach(filename ${pta_assert_files})
    message(STATUS "Adding test for ${filename}")
    add_test(
            NAME ass1-pta/${filename}
            COMMAND ass1 -pta ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

message(STATUS "Adding test for icfg_assert_files")
file(GLOB icfg_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/icfg/*.ll")
foreach(filename ${icfg_assert_files})
    add_test(
            NAME ass1-icfg/${filename}
            COMMAND ass1 -icfg ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

message(STATUS "Adding test for taint_assert_files")
file(GLOB taint_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/taint/*.ll")
foreach(filename ${taint_assert_files})
    add_test(
            NAME ass1-taint/${filename}
            COMMAND ass1 -taint ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

