# Mandelbrot Set Viewer Written in C++

## Prerequisites

### G++
- You will need the g++ compiler to compile the project using the makefile
- If you have another compiler, you can compile manually (just look at the makefile)

### SFML
- You will need SFML installed on your system and added to your PATH for compilation to work
- SFML can be found here: https://www.sfml-dev.org/index.php

### OpenCL
- If you care to try the OpenCL version of the program (by using `make opencl`), you will need a functional installation of OpenCL

### Permissions

(mac) You will have to allow a shit ton of libs to run by going to system preferences/security & privacy/general 

## Usage

After compiling with `make`, the binary will be in bin/mandelbrot

`make` will compile a basic version of the program. It is multithreaded but written in standard c++.

`make opencl` will compile a different (faster) version. It requires a functional OpenCL installation and **is not guaranteed to work**.
This version can be ran on either a GPU or CPU. If it works but does not run on the right device, you can try to edit the code in `openclStuff.c` where the device is chosen (it is chosen from an array, with the default device being at index 0. You can try other indices.

### Keybindings
- Enter zooms in (4x)
- Arrow keys or WASD move the target cursor (the place where it will zoom in)
  - Pressing shift while using movement keys makes the cursor move faster
- Escape zooms out
- Pressing I increases the current number of iterations and redraws
- Pressing K will (randomly) change a constant within the code that affects the coloring
- Pressinc C will (randomly) change the hue of the image
