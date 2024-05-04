file(GLOB SOURCES
        *.cpp
)
add_executable(lab1 ${SOURCES})
set_target_properties(lab1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(
        NAME Lab-Exercise-1
        COMMAND lab1
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)