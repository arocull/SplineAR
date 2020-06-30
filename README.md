# Splinear
A 2D/3D drawing and animation tool


## Dependencies
Uses a mixture of OpenGL and OpenCL. Requires installation of the following libraries.

`$ sudo apt install nvidia-opencl-dev libgl1-mesa-dev mesa-common-dev`

Currently only runs on NVidia graphics cards (?) on Linux.

## Build and Run
This uses a makefile. Simply sit in the main directory that this README is located in, and run the commands:

`$ make`
`$ make run`

The rest should be taken care of.

## Configuration
Program configurations can be modified and set inside of [config.h](/src/config.h). Currently only allows for configuration of window resolution and debug mode.