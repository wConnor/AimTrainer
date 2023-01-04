# AimTrainer
## Introduction
AimTrainer is a 2D point-and-click game in which the player is presented with a screen of targets that they are to quickly and accurately click using their mouse. The main purpose of AimTrainer is to assist the player with training their reflexes for use in other games, such as first-person shooters.

The player is able to set options such as the amount of targets to have on screen, how long they wish to play, the amount of total targets to click, as well as their crosshair. This makes AimTrainer a game that anyone can play, no matter their skill.

## Compilation
### Linux
Once AimTrainer has been cloned, run the following commands:

```
$ mkdir build && cd build
$ cmake ..
$ make -j$(nproc)
```

The binary can then be executed using `./src/AimTrainer`.

### Windows
TODO: either rewrite CMakeLists.txt in a way usable for Windows or provide a Visual Studio project file.

## Screenshots
TODO: populate 'screenshots' section.
