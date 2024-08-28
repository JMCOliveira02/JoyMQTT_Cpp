# JoyMQTT_Cpp

This repository contains a program written in C++ to read commands from a Joystick (Logitech F710) and send it to a MQTT broker at a given IP address and port. It also contains a program that subscribes to the broker, for debugging. The implementation is based on [SDL2](https://www.libsdl.org/) and [mosquitto](https://mosquitto.org/).


## Prerequisites
### Visual Studio IDE
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/)
* Make sure to install the workload package: **Desktop development with C++** with **MSVC v143 build tools**
### SDL2
* [SDL2 releases](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.6)

* Download and extract the zip file referring to the Visual C++ compiler: **SDL2-devel-"sdl_version"-VC.zip** (it is recommended to extract the contents of the zip file to the path "C:" for no additional configuration with the Visual Studio solution, this will require you to change the name of the extracted folder to "SDL2")


* Add the **"C:/SDL2/lib/x64"** directory to the **Path**
environment variable
### Mosquitto
* [Mosquitto download page](https://mosquitto.org/download/)

* Download and run the windows x64 mosquitto installer: **mosquitto-mosquitto_version-install-windows-x64.exe** (it is recommended to install directly on the path "C:" for no additional configuration with the Visual Studio solution)

* Add the directory where mosquitto was installed (**"C:/mosquitto"**) to the **Path** environment variable

### JSON
* [Single include JSON library](https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp)
* Download raw file ( it is recommended to download the file to the directory **"C:\JSON"**, for no additional configuration with the Visual Studio solution )

### Additional notes
* There should now be three folders present in the **C:** directory: **"SDL2"**, **"mosquitto"** and **"JSON"**
* If you decide not to install in the **C:** directory, or you already have installed these dependencies on different locations in your computer, you will be required to configure the projects inside Visual Studio to make sure the paths to these dependencies are correct


## Installation
* Clone this repository to your pc
```sh
git clone https://github.com/JMCOliveira02/joyMQTT_Cpp_2.git
```
* The folder **Solution** contains the visual studio file solution **JoyMQTT_Cpp.sln**. Open the solution with **Visual Studio 2022**
* Using **Visual Studio 2022**, it should be possible to compile and run each of the projects, with no additional configurations

## Usage


