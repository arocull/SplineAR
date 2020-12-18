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

`$ sudo apt install nvidia-opencl-dev libgl1-mesa-dev mesa-common-dev libglew-dev libglfw3-dev libglm-dev`

Extra configuration may be necessary for platforms aside from Linux.

List of dependencies:
- OpenGL
- OpenCL 2.2 or greater
- GLFW3
- GLEW
- pthread
- glm

## Build and Run
This uses a makefile. Simply sit in the main directory that this README is located in, and run the commands:

`$ make`
`$ make run`

The rest should be taken care of.

## Configuration
Program configurations can be modified and set inside of [config.h](/src/config.h). Currently only allows for configuration of window resolution and debug mode.