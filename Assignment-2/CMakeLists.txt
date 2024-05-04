include_directories(${Z3MGR_DIR})
file(GLOB SOURCES
        Assignment-2.cpp
        ${Z3MGR_DIR}/Z3Mgr.cpp
)
set(LIB ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})
add_library(assign2 ${SOURCES})
target_link_libraries(assign2 ${LIB})
set_target_properties(assign2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_subdirectory(Tests)