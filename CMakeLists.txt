cmake_minimum_required(VERSION 4.0.3)
project(Testtask LANGUAGES CXX)

# include(cTest)
# enable_testing()

# add_executable(Testtask main.cpp library.cpp library.h)

# add_compile_options(-finput-charset=UTF-8)

# set_property(TARGET Testtask PROPERTY CXX_STANDART 17)
set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_subdirectory(logger)
add_subdirectory(app)

# target_link_libraries(Testtask SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image)