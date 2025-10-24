# pgRenderCore
PGRenderCore: library agnostic, multiplatform graphics development library




## Compiling the library

### In Linux:

To compile the library in Linux, you need to have CMake and a C++ compiler installed. Assuming you are using a modern Ubuntu Desktop system, follow these steps:

Install the required dependencies:

```bash
sudo apt-get install build-essential git make cmake \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libpipewire-0.3-dev \
libwayland-dev libdecor-0-dev liburing-dev libxinerama-dev
```

Clone the repository in some directory:

```bash
git clone https://github.com/fjabad/pgRenderCore
cd pgRenderCore
```

Create a build directory and navigate into it:

```bash
mkdir build
cd build
```

Run CMake to configure the project and generate the build files:

```bash
cmake .. 
```

Build the library using make:

```bash
make -j$(nproc)
```