file (GLOB SOURCES
        CPP/*.cpp
)
add_executable(lab3 ${SOURCES})

target_link_libraries(lab3 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

foreach (i RANGE 1 8)
    add_test(
            NAME lab3_test${i}
            COMMAND lab3 test${i}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach ()