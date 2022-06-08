cmake_minimum_required(VERSION 3.10)
project(Veille)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(LibGLW REQUIRED)

file(GLOB Veille_SRC
    "${CMAKE_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/*.rc"
)

add_executable(Veille WIN32 ${Veille_SRC})
set_property(TARGET Veille PROPERTY CXX_STANDARD 17)
set_target_properties(Veille PROPERTIES SUFFIX ".scr")

target_link_libraries(Veille PRIVATE
    glm::glm
    SDL2::SDL2 SDL2::SDL2main
    GLEW::GLEW
)

target_include_directories(Veille
    PUBLIC ${LIBGLW_INCLUDE_DIR}
    PUBLIC ${SDL2_INCLUDE_DIR}
    PUBLIC ${GLM_INCLUDE_DIRS}
    PUBLIC ${GLEW_INCLUDE_DIR})
    
target_compile_definitions(Veille 
    PRIVATE GLM_ENABLE_EXPERIMENTAL=1)

target_link_libraries(Veille
    ${OPENGL_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    ${SDL2_LIBRARY} 
    ${SDL2_SDLMAIN_LIBRARY}
    )

target_link_libraries (Veille
    debug ${LIBGLW_LIBRARY_DEBUG}
    optimized ${LIBGLW_LIBRARY_RELEASE})