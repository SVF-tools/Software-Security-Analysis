file (GLOB SOURCES
    *.cpp
)
add_executable(svfir ${SOURCES})

target_link_libraries(svfir ${SVF_LIB} ${llvm_libs})

set_target_properties( svfir PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
