cmake_minimum_required(VERSION 3.12...3.27)
project(arcade VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 17)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
file(GLOB_RECURSE SRC RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cxx" "src/*.hxx" "src/*.h" "src/*.c" "src/*.cc")

set(ROM_GENERATED_CPP "${CMAKE_BINARY_DIR}/rom_generated.cxx")
list(REMOVE_ITEM SRC "src/rom/rom.cxx")

add_custom_command(
    OUTPUT "${ROM_GENERATED_CPP}"
    COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tools/generate_rom_data.py" "${CMAKE_SOURCE_DIR}" "${ROM_GENERATED_CPP}"
    DEPENDS "${CMAKE_SOURCE_DIR}/tools/generate_rom_data.py" "${CMAKE_SOURCE_DIR}/src/rom/rom.cxx"
    VERBATIM
)


 
add_executable(arcade_machine ${SRC} "${ROM_GENERATED_CPP}") 
target_compile_definitions(arcade_machine
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:DEBUG>
)


if(EMSCRIPTEN)
    # Force the output file format to be an HTML page instead of a JS/Wasm file
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # Pass the SDL version flag to both compile and link steps
    # For SDL2, use: "-sUSE_SDL=2"
    # For SDL3, use: "-sUSE_SDL=3"
    target_compile_options(arcade_machine PRIVATE
        "-pthread"
        "-g"
    )

    target_link_options(arcade_machine PRIVATE 
        "-sUSE_SDL=2"
        "-sUSE_SDL_TTF=2"
        "-sUSE_SDL_MIXER=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sNO_DISABLE_EXCEPTION_CATCHING"
        "-sOFFSCREEN_FRAMEBUFFER=1"
        "-sUSE_WEBGL2=1"
        "-g"
    )
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)


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
        "-framework CoreFoundation"
        "-framework IOKIT"
    )
else()
    target_link_libraries(arcade_machine
        -lSDL2main
        -lSDL2
        -lSDL2_mixer
        -lSDL2_ttf
    )
endif()
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

