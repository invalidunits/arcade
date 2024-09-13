cmake_minimum_required(VERSION 3.12...3.27)
project(arcade VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 17)


# EMBED ROM FILES 
file(GLOB_RECURSE ROM "src/rom/*.*")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}src/crom)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
file(GLOB_RECURSE SRC "src/*.cxx" "src/*.hxx" "src/*.h" "src/*.c" "src/*.cc")
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)





add_definitions(-Dfoo)   
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


if (APPLE)
    # Incbin does not support bitcode. disable bitcode and then disable the warning.
    target_compile_definitions(arcade_machine PRIVATE -DINCBIN_SILENCE_BITCODE_WARNING)
    set_target_properties(arcade_machine PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")

    # link libraries
    link_directories("/opt/homebrew/Cellar/sdl2/2.30.7/lib")
    link_directories("/opt/homebrew/Cellar/sdl2_mixer/2.8.0/lib")
    link_directories("/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib")
    target_link_libraries(arcade_machine
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
    )
    target_link_libraries(arcade_machine "-framework CoreFoundation")
    target_link_libraries(arcade_machine "-framework IOKIT")
else()
    target_link_libraries(arcade_machine
        -lSDL2main
        -lSDL2
        -lSDL2_mixer
        -lSDL2_ttf
    )
endif()


if (MSYS OR MINGW) 
    target_link_libraries(arcade_machine
    -lmingw32 
    )

endif()
if (WIN32)
    target_link_libraries(arcade_machine
    -lsetupapi 
    -lhid
    )
endif()

if (APPLE) 
	include_directories("/opt/homebrew/include")
endif()


include_directories("src/")

