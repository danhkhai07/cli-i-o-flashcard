add_library(CLI STATIC ./cli/cli.cpp)
add_library(Commands STATIC ./commands/commands.cpp)
add_library(DataHandler STATIC ./data/dataHandler.cpp)
add_library(Card STATIC ./data/card.cpp)

# Add include directories
target_include_directories(CLI PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Commands PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(DataHandler PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Card PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add dependency
target_link_libraries(CLI PRIVATE Commands)
target_link_libraries(Commands PRIVATE DataHandler)
target_link_libraries(DataHandler PRIVATE Card)
target_link_libraries(CLI PRIVATE DataHandler)

# Install package
install(TARGETS CLI Commands DataHandler Card
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)