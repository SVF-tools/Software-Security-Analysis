file (GLOB SOURCES
        *.cpp
)
add_executable(lab2 ${SOURCES})

target_link_libraries(lab2 ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})

set_target_properties(lab2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

foreach(idx RANGE 7)
    add_test(
            NAME Lab-Exercise-2-${idx}
            COMMAND lab2 ${idx}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()