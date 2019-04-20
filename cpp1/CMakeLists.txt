cmake_minimum_required(VERSION 3.7)
project(cpp1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        opengl_utils/matrix.cpp
        opengl_utils/matrix.hpp
        opengl_utils/mesh.cpp
        opengl_utils/mesh.hpp
        opengl_utils/obj_loader.cpp
        opengl_utils/obj_loader.hpp
        opengl_utils/shader.cpp
        opengl_utils/shader.hpp
        opengl_utils/texture.cpp
        opengl_utils/texture.hpp
        main.cpp
        particle.h
        sph.cpp
        sph.h
        vec3.h)

add_executable(cpp1 ${SOURCE_FILES})