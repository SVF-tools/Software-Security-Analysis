file (GLOB SOURCES
        CPP/*.cpp
)
add_executable(lab2 ${SOURCES})

target_link_libraries(lab2 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

foreach (i RANGE 0 10)
    add_test(
            NAME lab2_test${i}
            COMMAND lab2 test${i}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach ()
