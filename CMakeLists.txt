cmake_minimum_required(VERSION 3.15)
project(mygame)

set(CMAKE_CXX_STANDARD 17)

# Download and build raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# Your game
add_executable(mygame main.cpp)
if(WIN32)
    enable_language(RC)
    target_sources(mygame PRIVATE app.rc)
    target_include_directories(mygame PRIVATE ${CMAKE_SOURCE_DIR})
endif()
target_link_libraries(mygame raylib)

# Put the finished game in a clean "dist" folder next to CMakeLists.txt
set_target_properties(mygame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist
)

# No console window on Windows
if(WIN32)
    set_target_properties(mygame PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Bake the compiler's runtime into the .exe so it needs no extra .dll files (MinGW)
if(MINGW)
    target_link_options(mygame PRIVATE -static)
endif()

# Copy resources next to the .exe after every build
add_custom_command(TARGET mygame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:mygame>/resources
)