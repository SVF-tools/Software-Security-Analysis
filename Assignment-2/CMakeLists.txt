include_directories(${Z3MGR_DIR})
file(GLOB SOURCES
        *.cpp
        ${Z3MGR_DIR}/*.cpp
)
set(LIB ${SVF_LIB} ${llvm_libs} ${Z3_LIBRARIES})
add_library(assign-2 ${SOURCES})
target_link_libraries(assign-2 ${LIB})
set_target_properties(assign-2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
add_subdirectory(Tests)