# Installing OpenGL on Linux
[mesa-dev](https://superuser.com/questions/1099227/installing-opengl-in-linux)
[common-dev](https://askubuntu.com/questions/795647/how-to-install-opengl-4-0-or-4-5-for-developing)


# Examples and Tutorials
## Basic OpenCL uses
A quick test I wrote that combines OpenCL and SDL located [here](https://gist.github.com/Masteralan/5095f99b3afc7087ddb46ef8cfb20bb3)

Old NVidia [OpenCL Demos](https://developer.nvidia.com/opencl) (not Linux-compatible)

A list of [math functions](https://www.khronos.org/registry/OpenCL/specs/2.2/html/OpenCL_C.html#math-functions) for OpenCL

OpenCL [best practices](http://developer.download.nvidia.com/compute/cuda/3_2_prod/toolkit/docs/OpenCL_Best_Practices_Guide.pdf) guide by NVidia


## Interoperability between OpenGL and OpenCL
Basic 2-Directional Texture Manipulation [Gist by Twinklebear](https://gist.github.com/Twinklebear/5393665)

A basic [tutorial](http://www.cmsoft.com.br/opencl-tutorial/openclopengl-interoperation-textures/) showing some functions of interpolability

A [tutorial by Intel](https://software.intel.com/content/www/us/en/develop/articles/opencl-and-opengl-interoperability-tutorial.html) giving more in-depth understandings of what is going on, with another one on [surface sharing](https://software.intel.com/content/www/us/en/develop/articles/sharing-surfaces-between-opencl-and-opengl-43-on-intel-processor-graphics-using-implicit.html)


Different operating systems requires different methods for fetching the OpenGL context when configuring OpenCL. Here is a [list](https://github.com/glfw/glfw/issues/104) of them.

And finally, a list of OpenCL [Error Codes](https://streamhpc.com/blog/2013-04-28/opencl-error-codes/)


## Shape Filling
Testing if a point is inside a [complex polygon with spline curves](https://alienryderflex.com/polyspline/)



# Utilities for Later
[Multithreading](https://stackoverflow.com/questions/21663555/run-two-functions-at-the-same-time)
Max O'Cull suggested [pthreads](https://randu.org/tutorials/threads/)

Passing [structs to kernels](https://stackoverflow.com/questions/52009087/passing-c-structures-to-opencl-kernel) in OpenCL

Drawing [non-convex polygons](https://stackoverflow.com/questions/25422846/how-to-force-opengl-to-draw-a-non-convex-filled-polygon) in OpenGL

Textures in OpenGL [here](https://learnopengl.com/Getting-started/Textures)

Invoking [multiple functions](https://stackoverflow.com/questions/7196552/opencl-is-it-possible-to-invoke-another-function-from-within-a-kernel) within an OpenCL Kernel.

Including other CL Kernels in a [CL header](https://stackoverflow.com/questions/30514189/how-to-include-header-correctly-in-the-opencl-kernel).