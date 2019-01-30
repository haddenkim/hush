cmake_minimum_required(VERSION 3.0.0)
project(hush VERSION 0.1.0)

# C++ version
set (CMAKE_CXX_STANDARD 11)

# set build directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Source
# program executable
add_executable(${PROJECT_NAME} "src/main.cpp")

# c warning level
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif(MSVC)

# source lib
file(GLOB_RECURSE SOURCES 
	"src/*.cpp")
add_library(hush_core STATIC ${SOURCES})
target_include_directories(hush_core PUBLIC src)
target_link_libraries(${PROJECT_NAME} hush_core)

# shader files
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
file(GLOB_RECURSE SHADERS RELATIVE ${SHADER_DIR}
	"src/*.vs"
	"src/*.fs")
foreach(shaderFile ${SHADERS})
	configure_file(	${SHADER_DIR}/${shaderFile} 
					${CMAKE_BINARY_DIR}/shaders/${shaderFile} 
					COPYONLY
	)
	
endforeach(shaderFile ${SHADERS})


## Included Libraries
# TINYOBJ
set (TINYOBJ_DIR "lib/tiny_obj_loader")
add_library(TINYOBJ "${TINYOBJ_DIR}/tiny_obj_loader.cc")
target_include_directories(TINYOBJ PUBLIC "${TINYOBJ_DIR}")
target_link_libraries(hush_core TINYOBJ)

# STB IMAGE (header only)
set (STB_DIR "lib/stb")
target_include_directories(hush_core PUBLIC "${STB_DIR}")

# IMGUI
set (IMGUI_DIR "lib/imgui-1.66b")
add_library(IMGUI 	"${IMGUI_DIR}/imgui.cpp"
					"${IMGUI_DIR}/imgui_demo.cpp"
					"${IMGUI_DIR}/imgui_draw.cpp"
					"${IMGUI_DIR}/imgui_widgets.cpp"
					# "${IMGUI_DIR}/imgui_impl_glfw.cpp"
					# "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)
target_include_directories(IMGUI PUBLIC "${IMGUI_DIR}")
target_compile_definitions(hush_core PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(hush_core IMGUI)


## External Libraries
# GLM
find_package(glm REQUIRED)
target_include_directories(hush_core PUBLIC ${GLM_INCLUDE_DIRS})

# GLFW
find_package(glfw3 REQUIRED)
target_include_directories(hush_core PUBLIC ${GLFW3_INCLUDE_DIRS})
target_compile_definitions(hush_core PUBLIC GLFW_INCLUDE_NONE) # GLAD loads gl headers
target_link_libraries(hush_core glfw ${GLFW_LIBRARIES})

# GLAD
find_package(glad REQUIRED)
target_include_directories(hush_core PUBLIC ${GLAD_INCLUDE_DIRS})
target_link_libraries(hush_core glad ${GLAD_LIBRARIES} -ldl) # linux glad needed -ldl linked

# Intel TBB
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)	# needed due to tbb-homebrew bug https://github.com/Homebrew/homebrew-core/pull/36175
find_package(TBB REQUIRED) 
target_include_directories(hush_core PUBLIC ${TBB_INCLUDE_DIRS})
target_link_libraries(hush_core ${TBB_IMPORTED_TARGETS})

# Embree
find_package(EMBREE 3.0 REQUIRED)
target_include_directories(hush_core PUBLIC ${EMBREE_INCLUDE_DIRS})
target_link_libraries(hush_core ${EMBREE_LIBRARIES})
