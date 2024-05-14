file (GLOB SOURCES
        *.cpp
)
add_executable(lab3 ${SOURCES})

target_link_libraries(lab3 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

add_test(
        NAME lab3
        COMMAND lab3
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)