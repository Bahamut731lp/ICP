cmake_minimum_required(VERSION 3.15...4.0)

project(
  ModernCMakeExample
  VERSION 1.0
  LANGUAGES CXX)

# Use a modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Collect your sources
file(GLOB SOURCES "src/lib/*.cpp")

# Executable target (drop .exe on macOS/Linux)
add_executable(app main.cpp ${SOURCES})

# Include dirs (often not needed because targets add them automatically)
target_include_directories(app PRIVATE ${OpenCV_INCLUDE_DIRS})

# Link libraries using vcpkg targets
target_link_libraries(app PRIVATE ${OpenCV_LIBS} GLEW::GLEW glfw OpenGL::GL)
