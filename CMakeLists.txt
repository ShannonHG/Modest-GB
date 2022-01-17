cmake_minimum_required(VERSION 3.20)

project(GameBoyEmulator)

set(CMAKE_CXX_STANDARD 20)
set(THIRD_PARTY_DIR Third-Party/)
set(IMGUI_DIR ${THIRD_PARTY_DIR}imgui/)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}backends/)

# Add third party projects .
add_subdirectory(${THIRD_PARTY_DIR}nativefiledialog-extended)
add_subdirectory(${THIRD_PARTY_DIR}yaml-cpp)
add_subdirectory(${THIRD_PARTY_DIR}SDL)

# Set source files.
file(GLOB_RECURSE GB_SOURCE_FILES 
    Source/*.cpp 
    ${IMGUI_DIR}imgui.cpp 
    ${IMGUI_DIR}imgui_draw.cpp 
    ${IMGUI_DIR}imgui_tables.cpp 
    ${IMGUI_DIR}imgui_widgets.cpp 
    ${IMGUI_BACKENDS_DIR}imgui_impl_sdlrenderer.cpp 
    ${IMGUI_BACKENDS_DIR}imgui_impl_sdl.cpp)

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${GB_SOURCE_FILES})

# Set include directories.
file(GLOB GB_INCLUDE_DIRS Include/*/)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE 
    Include
    ${GB_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR})

# Link to third party libraries.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nfd)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE yaml-cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2main)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2-static)