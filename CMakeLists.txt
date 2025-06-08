cmake_minimum_required(VERSION 3.15)
project(DocumentAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Descargar Crow single-header
file(DOWNLOAD 
    https://raw.githubusercontent.com/CrowCpp/Crow/master/include/crow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crow.h
    SHOW_PROGRESS
)

include_directories(include)

add_executable(document_api src/main.cpp)
target_link_libraries(document_api PRIVATE pthread)