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
Need to set the working directory to the source code's root folder, so that the shader files can be found.
```
hush {file.obj}
```

## Example Scenes
The included example scenes are from:
http://casual-effects.com/data/index.html