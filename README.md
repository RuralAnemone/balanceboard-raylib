# Wii Balance Board GUI

a simple GUI built with [raylib](https://raylib.com) ([github](https://github.com/raysan5/raylib/releases)) for robotics, demonstrating weight and position of whatever the heck it's called[^1].

I hope this is competition legal (:

## building

### requirements

- #### cmake (minimum version x.x)

  - download it from <https://cmake.org/download/> and follow the installation steps for your OS.

- #### wiiuse

  - follow the [installation instructions](https://github.com/wiiuse/wiiuse?tab=readme-ov-file#platforms-and-dependencies).

  - make and install `wiiuse`.

- #### raylib

  - find your OS in [this list](https://www.raylib.com/#supported-platforms) and follow the installation instructions.

### compiling

clone the repo and cd into it. then:

```bash
mkdir build && cd build
cmake ..
make
```

## running

- turn your computer's bluetooth on

- disconnect from the wii balance board that you want to use

- in the build subdirectory, run

```bash
./demo
```

- while it's searching for wiimotes, open up the balance board's battery cover and press the red "SYNC" button.

- step on and have fun (:

[^1]: posture???
