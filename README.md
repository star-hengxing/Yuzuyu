# Yuzuyu Visual Novel Engine

The name Yuzuyu comes from the translation of the character 柊ゆずゆ in [フレラバ](https://ja.wikipedia.org/wiki/%E3%83%95%E3%83%AC%E3%83%A9%E3%83%90_%E3%80%9CFriend_to_Lover%E3%80%9C).

> This is work in progress. Do not using it in production.

# Getting started

## Dependencies

- [xmake](https://xmake.io/#/guide/installation)
- Requires C++20 compiler.

## Setup

### Windows 10

Install msvc toolchain.

Recommend use [scoop](https://scoop.sh/) as package manager.

```sh
scoop install xmake vulkansdk
```

### Arch Linux

TODO

## Build

Clone repo, then try

```sh
xmake f -m debug -y
xmake build sandbox
```

After build, try

```sh
xmake run sandbox
```

If you want to accelerate to compile with C++20 stl header units, try
```sh
xmake f --modules=y
```
> Only msvc support now.

## Test

```sh
xmake f -m debug --test=y
xmake build -g test
xmake run -g test
```

# Dependent third-party libraries

- [vulkan](https://vulkan.lunarg.com/sdk/home)
- [volk](https://github.com/zeux/volk)
- [vk-bootstrap](https://github.com/charles-lunarg/vk-bootstrap)
- [vulkan-memory-allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [glfw](https://github.com/glfw/glfw)
- [sdl](https://www.libsdl.org/)
- [spdlog](https://github.com/gabime/spdlog)
- [fast_io](https://github.com/cppfastio/fast_io)
- [libpng](https://github.com/glennrp/libpng)
- [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo)
- [avir](https://github.com/avaneev/avir)
- [freetype](https://github.com/freetype/freetype)
- [ffmpeg](https://github.com/FFmpeg/FFmpeg)
- [toml++](https://github.com/marzer/tomlplusplus)
- [boost_ut](https://github.com/boost-ext/ut)
