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
- [imguizmo](https://github.com/CedricGuillemet/ImGuizmo)


## Steps to fix GLFW platform specific errors (dlfcn.h / windows.h)
When cloning or pulling you might get some errors when building if someone on another OS has uploaded cache files. These errors usually have something with:
- `dlfcn.h` when building on windows with cache files from linux.
- `windows.h` when building on linux with cache files from windows.
  
You can fix this easily by running `killCaches` with powershell on windows/linux and it will recursively delete `CMakeCache.txt, Makefile, and CMakeFiles`.
Then go to `vendor/glfw` and run `cmake . -G "Unix Makefiles"`
Then go to the project root and and configure and build, it should go smoothely go this time.

_**HEED MY WARNING: NEVER USE SCALING WITH BULLET PHYSICS OR IT WILL BREAK**_

