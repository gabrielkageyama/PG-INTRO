cmake_minimum_required(VERSION 3.14)
project(PG_Lista01 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_library(pg_common STATIC
    Common/glad.c
    Common/OpenGLUtils.cpp
)
target_include_directories(pg_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
)
target_link_libraries(pg_common PUBLIC glfw OpenGL::GL)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pg_common PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>
    )
endif()

function(add_pg_exercise target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE pg_common)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_pg_exercise(Lista1_Ex1 src/Exercicios/Lista1/Ex1/main.cpp)
add_pg_exercise(Lista1_Ex2 src/Exercicios/Lista1/Ex2/main.cpp)
add_pg_exercise(Lista1_Ex3 src/Exercicios/Lista1/Ex3/main.cpp)
add_pg_exercise(Lista1_Ex4 src/Exercicios/Lista1/Ex4/main.cpp)
