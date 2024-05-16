file(GLOB SOURCES
        *.cpp
)
add_executable(lab1 ${SOURCES})
set_target_properties(lab1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

foreach (i RANGE 1 3)
    add_test(
            NAME lab1_test${i}
            COMMAND lab1 test${i}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach ()