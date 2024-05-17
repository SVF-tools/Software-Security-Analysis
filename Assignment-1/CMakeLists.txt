include_directories(${GTRAV_DIR})
file(GLOB SOURCES
        Assignment-1.cpp
)
set(LIB ${SVF_LIB} ${llvm_libs})
add_library(assign-1 ${SOURCES})
target_link_libraries(assign-1 ${LIB})
set_target_properties(assign-1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_subdirectory(Tests)
