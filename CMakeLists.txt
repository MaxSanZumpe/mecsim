cmake_minimum_required(VERSION 3.10)
project(mecsim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Using C compiler  : ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")


find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


set(PHYSICS_SOURCES 
    physics/OdeSolver.cpp
    physics/ForceGenerator.cpp
    physics/LinearSolver.cpp
    physics/collisions.cpp
    physics/System.cpp
    physics/Constraint.cpp
    physics/util.cpp
    physics/sparse.cpp
)

set(RENDER_SOURCES 
    render/vao.cpp
)

set(EDITOR_SOURCES
    engine/sim_object.cpp
    engine/sim_scene.cpp
    engine/vertices.cpp
)

set(THIRD_PARTY_SOURCES
    third_party/glad/glad.c
)

add_executable(            mecsim examples/main.cpp ${PHYSICS_SOURCES})
target_include_directories(mecsim PRIVATE physics)
target_compile_options(    mecsim PRIVATE -Wall)


add_executable(            2dscene examples/2dscene.cpp ${PHYSICS_SOURCES} ${RENDER_SOURCES} ${EDITOR_SOURCES} ${THIRD_PARTY_SOURCES})
target_link_libraries(     2dscene PRIVATE glfw)
target_include_directories(2dscene PRIVATE physics render engine third_party ${GLM_INCLUDE_DIRS})
target_compile_options(    2dscene PRIVATE -Wall)

