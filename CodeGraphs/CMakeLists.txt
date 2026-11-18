file (GLOB SOURCES
    *.cpp
)
add_executable(codegraphs ${SOURCES})

target_link_libraries(codegraphs ${SVF_LIB} ${llvm_libs})

set_target_properties(codegraphs PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
