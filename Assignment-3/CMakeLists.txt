include_directories(${Z3MGR_DIR})
file(GLOB SOURCES
        *.cpp
)
set(LIB ${SVF_LIB} ${llvm_libs})
add_library(assign3 ${SOURCES})
target_link_libraries(assign3 ${LIB})
set_target_properties(assign3 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_subdirectory(Tests)