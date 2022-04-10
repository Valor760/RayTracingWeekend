cmake_minimum_required(VERSION 3.16)

project(RayTracing_Weekend)

set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

# INCLUDES
# include_directories("3rdparty/GLFW/include")


# LIBS
# link_directories("3rdparty/GLFW/lib")

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})


# file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
# add_custom_target(copy_assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/Debug
# )
# add_dependencies(${PROJECT_NAME} copy_assets)

# add_library(GLAD "3rdparty/glad/glad.c")
# set(LIBS glfw3 GLAD)

# target_link_libraries(${PROJECT_NAME} ${LIBS})