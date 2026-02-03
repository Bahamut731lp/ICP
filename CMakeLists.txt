cmake_minimum_required(VERSION 3.15...4.0)

project(
  ModernCMakeExample
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# 1. Musíme najít všechny potřebné balíčky (včetně imgui)
find_package(OpenCV CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED) # <-- PŘIDÁNO

# Collect your sources
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Executable target
add_executable(app main.cpp ${SOURCES})

# Include dirs
# Better include handling
target_include_directories(app PRIVATE 
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/lib" # This specifically fixes your miniaudio error
    ${OpenCV_INCLUDE_DIRS}
)

# 2. Linkování knihoven
# Všimni si, že přidávám imgui::imgui. 
# Díky tomu, že máš v vcpkg.json zapnuté "features", 
# tento cíl v sobě už obsahuje i ty implementace pro GLFW a OpenGL3.
target_link_libraries(app PRIVATE 
    ${OpenCV_LIBS} 
    GLEW::GLEW 
    glfw 
    glm::glm
    imgui::imgui # <-- PŘIDÁNO
)