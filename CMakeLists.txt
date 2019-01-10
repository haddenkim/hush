cmake_minimum_required(VERSION 3.0.0)
project(hush VERSION 0.1.0)

# C++ version
set (CMAKE_CXX_STANDARD 11)

# set build directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add source files
file(GLOB_RECURSE SOURCES 
	"src/*.cpp"
	"src/*.vs"
	"src/*.fs")

# program executable
add_executable(${PROJECT_NAME} "src/main.cpp")

# source lib
add_library(hush_core STATIC ${SOURCES})
target_include_directories(hush_core PUBLIC src)
target_link_libraries(${PROJECT_NAME} hush_core)

## libraries
# GLM
FIND_PACKAGE(GLM REQUIRED)
INCLUDE_DIRECTORIES(hush_core ${GLM_INCLUDE_DIR})

# GLFW
FIND_PACKAGE(GLFW3 REQUIRED)
INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIRS})
# target_link_libraries(hush_core ${GLFW3_STATIC_LIBRARIES})
target_compile_definitions(hush_core PUBLIC GLFW_INCLUDE_NONE) # GLAD loads gl headers
target_link_libraries(hush_core glfw)

# GLAD
set(GLAD_DIR "lib/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PUBLIC "${GLAD_DIR}/include")
target_include_directories(hush_core PUBLIC "${GLAD_DIR}/include")
target_link_libraries(hush_core "glad")

# TINYOBJ
set (TINYOBJ_DIR "lib/tiny_obj_loader")
add_library("tinyobj" "${TINYOBJ_DIR}/tiny_obj_loader.cc")
target_include_directories("tinyobj" PUBLIC "${TINYOBJ_DIR}")
target_include_directories(hush_core PUBLIC "${TINYOBJ_DIR}")
target_link_libraries(hush_core "tinyobj")

# STB IMAGE
set (STB_DIR "lib/stb")
target_include_directories(hush_core PUBLIC "${STB_DIR}")

# IMGUI
set (IMGUI_DIR "lib/imgui-1.66b")
add_library(ImGui 	"${IMGUI_DIR}/imgui.cpp"
					"${IMGUI_DIR}/imgui_demo.cpp"
					"${IMGUI_DIR}/imgui_draw.cpp"
					"${IMGUI_DIR}/imgui_widgets.cpp"
					# "${IMGUI_DIR}/imgui_impl_glfw.cpp"
					# "${IMGUI_DIR}/imgui_impl_opengl3.cpp"
)
target_include_directories(ImGui PUBLIC "${IMGUI_DIR}")
target_include_directories(hush_core PUBLIC "${IMGUI_DIR}")
target_compile_definitions(hush_core PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(hush_core ImGui)

# Intel TBB
FIND_PACKAGE(tbb REQUIRED) # tbb needed to be lowercase
INCLUDE_DIRECTORIES(hush_core ${TBB_INCLUDE_DIRS})
target_link_libraries(hush_core tbb)

# Embree
find_package(embree 3.0 REQUIRED)
INCLUDE_DIRECTORIES(${EMBREE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(hush_core ${EMBREE_LIBRARIES})

# Embree "common"
# SET(TASKING_TBB      ON )
# ADD_DEFINITIONS(-DTASKING_TBB)
# LIST(APPEND ISPC_DEFINITIONS -DTASKING_TBB)
# add_subdirectory(lib/embree/common)
# include_directories(lib/embree/common)
