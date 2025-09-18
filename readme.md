
## Installation

### 1. Install vcpkg

```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Build vcpkg
./bootstrap-vcpkg.sh  # On macOS/Linux

export VCPKG_ROOT=/path/to/vcpkg
```

### 2. Install Dependencies

```bash
vcpkg install glew glfw3 glm opencv4 opengl
```

### 3. Clean, Build and Run

#### Clean Project
```bash
rm -rf build
```

#### Build Project

**macOS (Apple Silicon)**
```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_TARGET_TRIPLET=arm64-osx

cmake --build build -j
```

**Linux**
```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

cmake --build build -j
```

#### Run Application
```bash
./build/app
```
## Errory 

```bash
rm -rf build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build -j
```