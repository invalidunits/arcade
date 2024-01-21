cmake_minimum_required(VERSION 3.1...3.27)
project(arcade VERSION 1.0.0)


# EMBED ROM FILES 
file(GLOB_RECURSE ROM "src/rom/*.*")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}src/crom)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
file(GLOB_RECURSE SRC "src/*.cxx" "src/*.hxx" "src/*.h" "src/*.c" "src/*.cc")
find_package(SDL2 REQUIRED)






add_executable(arcade_machine ${SRC} ROM) 
target_compile_definitions(arcade_machine
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:DEBUG>
)


add_custom_command(
    OUTPUT ROM
    COMMAND cmake -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/rom" "${CMAKE_BINARY_DIR}/rom"
)





if (MSYS OR MINGW) 
    target_link_libraries(arcade_machine
    -lmingw32 
    )
target_link_libraries(arcade_machine
    -lSDL2main
    -lSDL2
    -lSDL2_ttf
)

endif()
if (WIN32)
    target_link_libraries(arcade_machine
    -lsetupapi 
    -lhid
    )
endif()

include_directories("src/")

