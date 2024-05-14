file (GLOB SOURCES
        *.cpp
)
add_executable(lab2 ${SOURCES})

target_link_libraries(lab2 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

add_test(
        NAME lab2
        COMMAND Lab-Exercise-2
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)