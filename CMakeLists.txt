cmake_minimum_required(VERSION 3.10)
project(mecsim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Using C compiler  : ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")


find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


set(PHYSICS_SOURCES 
    physics/OdeSolver.cpp
    physics/ForceGenerator.cpp
    physics/System.cpp
)

set(RENDER_SOURCES 
    render/vao.cpp
    render/Animator.cpp
)

set(THIRD_PARTY_SOURCES
    third_party/glad/glad.c
)

add_executable(            mecsim main.cpp ${PHYSICS_SOURCES})
target_include_directories(mecsim PRIVATE physics)
target_compile_options(    mecsim PRIVATE -Wall -Wextra)


add_executable(            2dscene 2dscene.cpp ${PHYSICS_SOURCES} ${RENDER_SOURCES} ${THIRD_PARTY_SOURCES})
target_link_libraries(     2dscene PRIVATE glfw)
target_include_directories(2dscene PRIVATE physics render third_party ${GLM_INCLUDE_DIRS})
# target_compile_options(    2dscene PRIVATE -Wall -Wextra)

