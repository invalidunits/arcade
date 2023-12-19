cmake_minimum_required(VERSION 3.1...3.27)
project(arcade VERSION 1.0.0)


# EMBED ROM FILES 
file(GLOB_RECURSE ROM "src/rom/*.*")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}src/crom)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
file(GLOB_RECURSE SRC "src/*.cxx" "src/*.hxx" "src/*.h" "src/*.c" "src/*.cc")
find_package(SDL2 REQUIRED)






add_executable(arcade_machine ${SRC} foo.hh) 

add_custom_command(
    OUTPUT foo.hh
    COMMAND cmake -E copy_directory_if_different "${CMAKE_CURRENT_SOURCE_DIR}/rom" "${CMAKE_BINARY_DIR}/rom"
)





target_link_libraries(arcade_machine
    -lmingw32
    -lSDL2main
    -lSDL2
    -lSDL2_ttf
    -lsetupapi 
    -lhid
)

include_directories("src/")

