
# loops over pta_assert_files and run "ass1-pta $bc_file"
message(STATUS "Adding python test for pta_assert_files")
file(GLOB pta_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/pta/*.ll")
foreach(filename ${pta_assert_files})
    message(STATUS "Adding test for ${filename}")
    add_test(
            NAME ass1-pta-py/${filename}
            COMMAND python3 Main.py -pta ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

message(STATUS "Adding python test for icfg_assert_files")
file(GLOB icfg_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/icfg/*.ll")
foreach(filename ${icfg_assert_files})
    add_test(
            NAME ass1-icfg-py/${filename}
            COMMAND python3 Main.py -icfg ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

message(STATUS "Adding python test for taint_assert_files")
file(GLOB taint_assert_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../Tests/testcases/taint/*.ll")
foreach(filename ${taint_assert_files})
    add_test(
            NAME ass1-taint-py/${filename}
            COMMAND python3 Main.py -taint ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

