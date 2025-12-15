cmake_minimum_required(VERSION 3.28)
project(Tetris LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

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
    src/game_manager.cpp
    src/game.cpp
    icon/resource.rc
    )
if(WIN32)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MTd")     
        target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    else()
        target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
        target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall        
        -Wextra
        -Wpedantic
        -Werror      
    )
    endif()
endif()
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

copy_resource_dir(icon)
copy_resource_dir(fonts)
copy_resource_dir(audio)
