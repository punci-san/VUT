cmake_minimum_required(VERSION 3.1.0)

project(chess)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Set build directory
set(CMAKE_BINARY_DIR  ${dir}/build)

set (CMAKE_CXX_FLAGS "-std=c++17 -lcrypt -Wall")

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(chess_SRCS
    GUI/main.cpp
    GUI/reset_graphic.cpp
    GUI/speedbox_graphic.cpp
    GUI/modebox_graphic.cpp
    GUI/movebox_graphic.cpp
    GUI/coordinates_graphic.cpp
    GUI/dialog_graphic.cpp
    GUI/messageWindow.cpp
    GUI/chess_game.cpp
    GUI/game_menu.cpp
    GUI/board.cpp
    API/movement/ChessGame.cpp
    API/movement/Square.cpp
    API/movement/Figure.cpp
    API/parsing/Parser.cpp
)


set(chess_HDRS
    GUI/includes/reset_graphic.h
    GUI/includes/speedbox_graphic.h
    GUI/includes/modebox_graphic.h
    GUI/includes/movebox_graphic.h
    GUI/includes/coordinates_graphic.h
    GUI/includes/dialog_graphic.h
    GUI/includes/messageWindow.h
    GUI/includes/chess_game.h
    GUI/includes/game_menu.h
    GUI/includes/board.h
)

# Tell CMake to create  executable
add_executable(chess  ${chess_SRCS} ${chess_HDRS})

# Use the Widgets module from Qt 5
target_link_libraries(chess Qt5::Widgets)
target_link_libraries(chess Qt5::Core)
target_link_libraries(chess Qt5::Gui)
