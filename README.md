# JoyMQTT_Cpp

This repository contains a program written in C++ to read commands from a Joystick (Logitech F710) and send it to a MQTT broker at a given IP address and port. It also contains a program that subscribes to the broker, for debugging. The implementation is based on [SDL2](https://www.libsdl.org/) and [mosquitto](https://mosquitto.org/).


## Prerequisites
### Visual Studio IDE
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/)
* Make sure to install the workload package: **Desktop development with C++** with **MSVC v143 build tools**
### SDL2
* [SDL2 releases](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.6)
* Download and extract the zip file referring to the Visual C++ compiler: **SDL2-devel-"sdl_version"-VC.zip** 
* Add the **"path_to_SDL/lib/x64"** directory to the **Path**
environment variable
### Mosquitto
* [Mosquitto download page](https://mosquitto.org/download/)
* Download and run the windows x64 mosquitto installer: **mosquitto-mosquitto_version-install-windows-x64.exe**
* Add the directory where mosquitto was installed (**"installation_path/mosquitto"**) to the **Path** environment variable

### JSON
* [Single include JSON library](https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp)
* Download raw file


### Installation
