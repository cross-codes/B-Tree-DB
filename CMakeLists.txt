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
