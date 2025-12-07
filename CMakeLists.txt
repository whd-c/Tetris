cmake_minimum_required(VERSION 3.28)
project(Tetris LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(NOT WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "")
else()
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -pthread")
endif()


find_program(CMAKE_CXX_COMPILER g++)
find_program(CMAKE_C_COMPILER gcc)

if(NOT CMAKE_CXX_COMPILER)
  message(FATAL_ERROR "g++ not found. Please ensure it is in the system PATH.")
endif()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/colors.cpp 
    src/tetromino.cpp
    src/render.cpp
    favicon/resource.rc
    )
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)

function(copy_resource_dir dir_name)
    set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/${dir_name}")
    set(DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dir_name}")
    file(COPY "${SOURCE_DIR}/" DESTINATION "${DEST_DIR}")
endfunction()

copy_resource_dir(favicon)
copy_resource_dir(fonts)
copy_resource_dir(audio)
