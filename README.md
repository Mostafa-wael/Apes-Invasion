# GameX
Repo for CMP2023's computer graphics project. The aim is to use OpenGL (plus some helper libraries) to create a game.

## Included Libraries

- [glfw 3.3](https://github.com/glfw/glfw)
- [glad 2](https://github.com/Dav1dde/glad/tree/glad2)
- [glm 0.9.9.8](https://github.com/g-truc/glm)
- [imgui v1.78](https://github.com/ocornut/imgui)
- [json 3.9.1](https://github.com/nlohmann/json)
- [flags](https://github.com/sailormoon/flags)
- [stb](https://github.com/nothings/stb)
- [tinyobjloader v1.0.6](https://github.com/tinyobjloader/tinyobjloader)
- [tinygltf v2.4.0](https://github.com/syoyo/tinygltf)
- [bullet physics](https://github.com/bulletphysics/bullet3)


## Steps to build bullet statically (Tested on Ubuntu 20.04)
- When you clone or pull from this repo, you should have bullet in `vendor/`
- Go into `vendor/bullet/`
- Execute `cmake .` to generate build files
- Execute `sudo make -j12` for a 12 thread machine for example.

After everything finishes, you should find a bunch of bullet related `.a` files under `/usr/local/lib/`

_**HEED MY WARNING: NEVER USE SCALING WITH BULLET PHYSICS OR IT WILL BREAK**_

