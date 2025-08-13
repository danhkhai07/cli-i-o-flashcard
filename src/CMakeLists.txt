add_library(CLI SHARED ./cli/cli.cpp)
add_library(Commands SHARED ./commands/commands.cpp)
add_library(DataHandler SHARED ./data/dataHandler.cpp)
add_library(Card SHARED ./data/card.cpp)

# Define export DLL symbols
target_compile_definitions(CLI PRIVATE CLI_EXPORT_DLL)
target_compile_definitions(Commands PRIVATE COMMANDS_EXPORT_DLL)
target_compile_definitions(DATAHANDLER PRIVATE DATAHANDLER_EXPORT_DLL)
target_compile_definitions(CARd PRIVATE CARD_EXPORT_DLL)

# Add include directories
target_include_directories(CLI PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Commands PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(DataHandler PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(Card PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Data directory
target_include_directories(DataHandler PRIVATE ${CMAKE_SOURCE_DIR}/data)

# Add dependencies
target_link_libraries(CLI PRIVATE Commands)
target_link_libraries(Commands PRIVATE DataHandler)
target_link_libraries(DataHandler PRIVATE Card)

# Install package
install(TARGETS CLI Commands DataHandler Card
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)