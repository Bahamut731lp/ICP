# Almost all CMake files should start with this
cmake_minimum_required(VERSION 3.15...4.0)

project(
  ModernCMakeExample
  VERSION 1.0
  LANGUAGES CXX)

# Use a modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cesta k vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(OpenCV REQUIRED)

# Collect your sources
file(GLOB SOURCES "src/lib/*.cpp")

# Executable target (drop .exe on macOS/Linux)
add_executable(app main.cpp ${SOURCES})

# --- OpenCV ---
# If CMake can't auto-find Homebrew OpenCV, pass -DOpenCV_DIR=... at configure time (see below)
find_package(OpenCV 4 REQUIRED COMPONENTS core imgcodecs highgui imgproc)

# Link OpenCV to your app
target_link_libraries(app PRIVATE ${OpenCV_LIBS})

# Include dirs (often not needed because OpenCV adds them via targets, but harmless)
target_include_directories(app PRIVATE ${OpenCV_INCLUDE_DIRS})
