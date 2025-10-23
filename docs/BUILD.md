# Building Invelog

## Prerequisites

### Required
- **CMake** 3.15 or higher
- **C++ Compiler** with C++17 support:
  - Windows: Visual Studio 2017 or newer, MinGW-w64, or Clang
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+ or Clang 5+

### Optional
- **Git** for version control
- **Ninja** or **Make** build system

## Windows Build Instructions

### Using Visual Studio

1. **Open Command Prompt or PowerShell**

2. **Navigate to project directory**
   ```powershell
   cd "path\to\Invelog"
   ```

3. **Create build directory**
   ```powershell
   mkdir build
   cd build
   ```

4. **Generate Visual Studio project**
   ```powershell
   cmake ..
   ```
   
   Or specify Visual Studio version:
   ```powershell
   cmake -G "Visual Studio 17 2022" ..
   ```

5. **Build the project**
   ```powershell
   cmake --build . --config Release
   ```

### Using MinGW

1. **Create build directory**
   ```powershell
   mkdir build
   cd build
   ```

2. **Generate Makefiles**
   ```powershell
   cmake -G "MinGW Makefiles" ..
   ```

3. **Build**
   ```powershell
   cmake --build .
   ```

## Linux/macOS Build Instructions

1. **Navigate to project directory**
   ```bash
   cd /path/to/Invelog
   ```

2. **Create build directory**
   ```bash
   mkdir build && cd build
   ```

3. **Generate build files**
   ```bash
   cmake ..
   ```

4. **Build**
   ```bash
   cmake --build . -j$(nproc)
   ```
   
   Or using make directly:
   ```bash
   make -j$(nproc)
   ```

## Build Options

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Release Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Build Outputs

After a successful build, you will find the following executables:

### Windows (Visual Studio)
```
build/
└── bin/
    ├── Debug/
    │   ├── invelog.exe              # Demo application (debug)
    │   └── invelog_server.exe       # Database server (debug)
    └── Release/
        ├── invelog.exe              # Demo application (release)
        └── invelog_server.exe       # Database server (release)
```

### Linux/macOS
```
build/
└── bin/
    ├── invelog              # Demo application
    └── invelog_server       # Database server
```

## Running the Executables

### Demo Application
```powershell
# Windows
.\bin\Release\invelog.exe

# Linux/macOS
./bin/invelog
```

### Database Server
```powershell
# Windows - with local database
.\bin\Release\invelog_server.exe --local ./data --port 8080 --api-key myKey

# Linux/macOS
./bin/invelog_server --local ./data --port 8080 --api-key myKey
```

See [SERVER_QUICKSTART.md](SERVER_QUICKSTART.md) for more server options.

### Specify Install Prefix
```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/install/path ..
```

## IDE Integration

### Visual Studio Code
1. Install "C/C++" and "CMake Tools" extensions
2. Open project folder
3. CMake will auto-configure
4. Use the build button or `Ctrl+Shift+B`

### Visual Studio
1. Open the folder as a CMake project
2. VS will automatically configure
3. Build using `Ctrl+Shift+B` or Build menu

### CLion
1. Open project folder
2. CLion will automatically detect CMakeLists.txt
3. Build using `Ctrl+F9`

## Troubleshooting

### CMake not found
- Ensure CMake is installed and in your PATH
- Windows: Download from https://cmake.org/download/
- Linux: `sudo apt install cmake` or equivalent

### Compiler not found
- Windows: Install Visual Studio or MinGW
- Linux: `sudo apt install build-essential`
- macOS: Install Xcode Command Line Tools

### C++17 not supported
- Update your compiler to a version that supports C++17

## Running Tests

Once test suite is implemented:
```bash
cd build
ctest
```

## Clean Build

To start fresh:
```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

## Next Steps

After building, refer to:
- [QUICKSTART.md](QUICKSTART.md) for getting started guide
- [ARCHITECTURE.md](ARCHITECTURE.md) for system architecture
- [SERVER_QUICKSTART.md](SERVER_QUICKSTART.md) for database server setup
- [API_DOCUMENTATION.md](API_DOCUMENTATION.md) for REST API reference
- Example code in `examples/` directory:
  - `database_examples.cpp` - Database backend usage examples
  - `server_client_demo.cpp` - Server/client interaction demo
