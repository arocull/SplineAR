# SplineAR
Spline Animation and Rigging (AKA SplineAR or splinear) - A tool that aims to effectively combine vector images, raster drawing techniques, and 2D animation.

The tool is not designed to be accurate, but it is designed to be fast and go beyond the limitations of vector and raster images.


## Plan
Current features:
- Drawing of basic strokes
- Rough OpenGL rastering of said strokes

Current goals:
- Raster vector object data in OpenCL
- Create bezier curves while drawing
- Basic user interface
- Animation timeline

Planned features:
- Drawing mode (draw basic objects, acts like a raster-based drawing program)
- Fine-tune mode (basic vector editing, not too complicated so users do not get caught in it)
- Animation mode (draw splines and have objects align to them)
- Custom shader creation and use
- Customizable brushes for both drawing and animation
- Exporting to vector or raster formats, and eventually gif or video formats

Far future ideas:
- Texture mapping (map raster textures to vector objects)
- Potentially allow the tool to work in a 3D space for even faster character and animation creation (without complicating the creation process)

## Dependencies
Uses a mixture of OpenGL and OpenCL, with GLEW and GLFW as assistants. Requires installation of the following libraries.
(If you use a non-NVidia card, be sure to get the corresponding OpenCL library instead).

`$ sudo apt install nvidia-opencl-dev libgl1-mesa-dev mesa-common-dev libglew-dev libglfw3-dev libglm-dev libftgl2 libftgl-dev`

Extra configuration may be necessary for platforms aside from Linux.

List of dependencies:
- OpenGL
- OpenCL 2.2 or greater
- GLFW3
- GLEW
- pthread
- glm
- FreeType

## Build and Run
This uses a makefile. Simply sit in the main directory that this README is located in, and run the commands:

`$ make`
`$ make run`

The rest should be taken care of.

## Configuration
Program configurations can be modified and set inside of [config.h](/src/config.h). Currently only allows for configuration of window resolution and debug mode.

## Documentation
GLFW [documentation](https://www.glfw.org/docs/latest/).
OpenCL [documentation](https://www.khronos.org/registry/OpenCL/specs/2.2/pdf/OpenCL_C.pdf) (or the best I could find).

Because FreeType asks for advertisement, you can find more about it [here](https://www.freetype.org/).
They have a GitLab group [here](https://gitlab.freedesktop.org/freetype).

SplineAR code contributions should include comments to describe what they do, why they're there, how they work, etc.
They do not need to be extremely in depth, but are helpful for other contributors, and if you come back at a later time.
Functions and classes should always have at least one line above them (in header or `.cpp` file) that describe what they do, so IDE's can pick up on them.