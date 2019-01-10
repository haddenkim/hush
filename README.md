# hush
Hush is an interactive path tracer renderer with denoising. 

Initially written for the final project of an undergraduate computer graphics course, Hush is a project to guide my education in rendering. 

## Compiling
Dependencies
- GLAD
- GLM
- GLFW3
- Threading Building Blocks (TBB)
- Embree

### MacOS
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Windows & Linux
Untested. 
I tested compiling with GCU 8.2 and Clang 9.1 on MacOS, so I hope this compiles cross-platform.

## Usage
./hush {file.obj}
