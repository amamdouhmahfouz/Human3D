cmake_minimum_required(VERSION 3.18)
project( OpencvTest )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package( OpenCV REQUIRED )

get_filename_component(DIR_ONE_ABOVE ../ ABSOLUTE)

set(ENABLE_ASSIMP   ON CACHE BOOL "Add Open Asset Import Library (assimp) to the project" FORCE)

#### added
#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#find_package(GLEW REQUIRED)
#message(STATUS "find glew")
####

# These are the options we select for building GLFW as a library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Add variables to use High Performance Graphics Card if available
# add_subdirectory(third_party/glfw)                       # Build the GLFW project to use later as a library
add_subdirectory(lib)
message(STATUS "ADDED lib")


add_definitions(-DGLEW_STATIC)
add_subdirectory(third_party/glew EXCLUDE_FROM_ALL)

set(GLAD_SOURCE third_party/glad/src/gl.c)

set(IMGUI_SOURCES
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_demo.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_impl/imgui_impl_glfw.cpp
        third_party/imgui/imgui_impl/imgui_impl_opengl3.cpp
        )

# Combine all vendor source files together into a single variable
#set(THIRD_PARTY_SOURCES ${GLAD_SOURCE} ${IMGUI_SOURCES})
set(THIRD_PARTY_SOURCES ${GLAD_SOURCE} ${IMGUI_SOURCES})

include_directories( 
    ${OpenCV_INCLUDE_DIRS} 
    ${DIR_ONE_ABOVE}/include/
    third_party/glfw/include
    third_party/glad/include
    third_party/glm
    third_party/imgui
    third_party/utils
    )

file(GLOB ALL_SOURCES
    ${DIR_ONE_ABOVE}/src/*.cpp
)
message(STATUS "files: ${ALL_SOURCES}")

add_executable( opencv2_test opencv2_test.cpp )
target_link_libraries( opencv2_test ${OpenCV_LIBS} )

add_executable( displayImg_test displayImg_test.cpp )
target_link_libraries( displayImg_test ${OpenCV_LIBS} )


message(STATUS "DIR_ONE_ABOVE: ${DIR_ONE_ABOVE}")
set(PIXEL_SRCS ${DIR_ONE_ABOVE}/src/Pixel.cpp)
message(STATUS "PIXEL_SRCS: ${PIXEL_SRCS}")

add_executable( pixel_test pixel_test.cpp ${PIXEL_SRCS})
target_link_libraries( pixel_test  )


add_executable(glfw_test glfw_test.cpp ${THIRD_PARTY_SOURCES} )
target_link_libraries(glfw_test glfw)

#add_subdirectory(third_party)
#message(STATUS "ADDED assimp")
add_executable(assimp_test assimp_test.cpp ${THIRD_PARTY_SOURCES})

message(STATUS "ASSIMP ENABLED out")
target_link_libraries(assimp_test glfw assimp)


add_executable(ui_test ui_test.cpp ${DIR_ONE_ABOVE}/src/UI.cpp ${THIRD_PARTY_SOURCES})
target_link_libraries(ui_test glfw)

#target_include_directories(GLEW_210 PUBLIC glew-2.1.0/include)
#link_libraries(${GLEW_LIBRARIES})
add_executable(drawPoint_test drawPoint_test.cpp ${THIRD_PARTY_SOURCES})
target_link_libraries(drawPoint_test 
    PRIVATE glfw
    PRIVATE libglew_static 
)

add_executable(drawTriangle_test drawTriangle_test.cpp ${THIRD_PARTY_SOURCES})
target_link_libraries(drawTriangle_test 
    PRIVATE glfw
    PRIVATE libglew_static 
)

add_executable(mesh_test mesh_test.cpp ${DIR_ONE_ABOVE}/src/Mesh.cpp ${THIRD_PARTY_SOURCES})
target_link_libraries(mesh_test glfw)

add_executable(model_test model_test.cpp ${DIR_ONE_ABOVE}/src/Model.cpp ${DIR_ONE_ABOVE}/src/Mesh.cpp ${THIRD_PARTY_SOURCES})
target_link_libraries(model_test glfw assimp)

add_executable(buildSSM_test buildSSM_test.cpp ${ALL_SOURCES} ${THIRD_PARTY_SOURCES})
target_link_libraries(buildSSM_test glfw assimp)

add_executable(strings_test strings_test.cpp)

add_executable(point_test point_test.cpp  ${DIR_ONE_ABOVE}/src/Point.cpp)

add_executable(objloader_test objloader_test.cpp ${DIR_ONE_ABOVE}/src/ObjLoader.cpp ${ALL_SOURCES} ${THIRD_PARTY_SOURCES})
target_link_libraries(objloader_test glfw assimp)