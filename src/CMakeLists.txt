add_library(CLI STATIC ./cli/cli.cpp)
add_library(Commands STATIC ./commands/commands.cpp)
add_library(DataHandler STATIC ./dataHandler/dataHandler.cpp)

# Add include directories
target_include_directories(CLI PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Commands PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(DataHandler PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add dependency
target_link_libraries(CLI PRIVATE Commands)
target_link_libraries(Commands PRIVATE DataHandler)

# Install package
install(TARGETS CLI Commands DataHandler
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)