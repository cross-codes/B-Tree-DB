<div align="center">
<h1>🌳 B-Tree-DB </h1>

An implementation of an SQLite - like database written in C++ with no
external dependencies.

The app is features a REPL, a query parser and a
B-Tree implementation based off of Introduction to Algorithms (4th ed).

</div>

---

# Pending features

Remove hardcoded row format
Implement B-Tree
Use modern C++ equivalents instead of unsafe `memcpy` calls

# Build instructions

To build the project, make sure you have `Cmake` installed, and modify
the `CMakeLists.txt` file to change the `C++` version to a one that your
compiler supports. Note that the project needs `C++ 11` or higher, for
smart pointer support.

Then, execute the following commands

```bash

mkdir build
cd build
cmake ..
cmake --build .
```

---

# Development

Note that every new source file you add must be included in the
`CMakeLists.txt` file, under `set(SOURCE_FILES...)`

```cmake
cmake_minimum_required(VERSION 3.20)

project(SQLite3-Clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(SOURCE_FILES
  src/main.cc
  src/meta/meta.cc
  src/Statement.cc
  src/Row.cc
  src/Table.cc
  src/parse/InsertStatement.cc
  src/parse/SelectStatement.cc
)
add_executable(db ${SOURCE_FILES})
target_include_directories(db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```

---

Project started on: 02/07/2025
