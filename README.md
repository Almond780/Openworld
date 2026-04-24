## Current version: Stable v0.0.00
# Openworld
**Openworld is a game engine from scratch with OpenGL API. To be used as an template for games.**

## Hardware
I used an MX330 with 2gb of Vram & i7150u & 16gb of RAM to run this program, so every section/level can have 500,000-1,000,000 vertices on the MX330 wth camera culling & LOD. So in thoery ~90,000,000 vertices for camera culling & LOD but thats not going to happen any time soon.

## Setup
I used QT creator with some packages in ArchLinux but if you want to run this engine ask AI, sorry but if your not running anything but ArchLinux I can't help you ):. But if you running ArchLinux install these packages via "sudo pacman -S Qt creator". "sudo pacman -S glfw-x11" (GLFW), "sudo pacman -S glm" (GLM), "sudo pacman -S gcc cmake" (GCC/Cmake), OpenGL is already installed in drivers, GLAD (OpenGL function loader (Core Profile 3.3)) and stb_easy_font https://github.com/nothings/stb. And your good and keep the file paths its easier that way. 

As i said before im running a MX330 you can remove __NV_PRIME_RENDER_OFFLOAD=1 & __GLX_VENDOR_LIBRARY_NAME=nvidia if you don't want nividia v-sync.

**Its highly recommended to put v-sync on at 60fps no higher becuase if so the engine will break shuch as deltatime, loading screen and ect.**

## Credit
With crediting my work, I recommend to do the same as below.

- `stb_easy_font.h` by Sean Barrett (MIT/Public Domain) – https://github.com/nothings/stb
