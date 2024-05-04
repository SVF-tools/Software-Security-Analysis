include_directories(../ ${GTRAV_DIR})
add_executable(assign-1-taint test-taint.cpp)
target_link_libraries(assign-1-taint ${LIB} assign1)
set_target_properties(assign-1-taint PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
        NAME assign1-taint
        COMMAND assign-1-taint
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_executable(assign-1-icfg test-icfg.cpp)
target_link_libraries(assign-1-icfg ${LIB} assign1)
set_target_properties(assign-1-icfg PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
        NAME assign1-icfg
        COMMAND assign-1-icfg
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_executable(assign-1-pta test-pta.cpp)
target_link_libraries(assign-1-pta ${LIB} assign1)
set_target_properties(assign-1-pta PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
        NAME assign1-pta
        COMMAND assign-1-pta
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)