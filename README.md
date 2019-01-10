# hush

## About
Hush is an interactive path tracer renderer with denoising. 

Initially written for the final project of an undergraduate computer graphics course, Hush is a project to guide my education in rendering. 

## Compiling
Dependencies
- GLAD
- GLM
- GLFW3
- Threading Building Blocks (TBB)
- Embree

### MacOS & Linux
        mkdir build
        cd build
		cmake -DCMAKE_BUILD_TYPE=Release ..
		make

### Windows
Untested, but this should 

## Usage
./hush {file.obj}