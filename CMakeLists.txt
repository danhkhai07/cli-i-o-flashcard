cmake_minimum_required(VERSION 3.12)
project(CLIFlashcard)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Preferred install prefix 
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "CMake install prefix is set to ${CMAKE_SOURCE_DIR}/install")
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE STRING "Install prefix" FORCE)
endif()

# Disable certain unnecessary warnings
add_compile_options(/wd4251)

# Add executable
add_executable(quiz ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Add include path
target_include_directories(quiz PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Add subdirectory
add_subdirectory(src)

# Add dependency
target_link_libraries(quiz PRIVATE CLI)

### INSTALL PACKAGE
# Executable 
install(TARGETS quiz
    RUNTIME DESTINATION bin
)

install (DIRECTORY data
    DESTINATION .
)

install (FILES LICENSE README.md
    DESTINATION .
)