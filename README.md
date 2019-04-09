Blend2D - Getting Started and Sample Applications
-------------------------------------------------

This repository contains few samples that use [Blend2D](https://blend2d.com) rendering engine. Many samples come from [Getting Started](https://blend2d.com/getting-started.html) page. It's expected (by default) that the directory structure is similar to the directory structure described in the Getting Started page.


Building
--------

Use the following commands to fetch asmjit, blend2d, and these samples:

```bash
# Download source packages from Git.
$ git clone --depth=1 https://github.com/asmjit/asmjit --branch next-wip
$ git clone --depth=1 https://github.com/blend2d/blend2d
$ git clone --depth=1 https://github.com/blend2d/bl-samples

# Create build directory and build 'bl-samples'.
$ cd bl-samples
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
```

After you have these 3 projects cloned use cmake to create your `bl-samples` project, the rest should be handled automatically.

Alternatively you can pick a configure script from `tools` directory if there is a suitable one for your configuration.


Resources
---------

Some applications require files that are stored in `resources` directory to work properly. Copying all resources to the build directory should be handled by the build script, but if it fails for whatever reason just copy the content of `resources` to your build directory. If these files are missing the sample should report an error.

All files within resources should be freely redistributable:

  - resources/texture.jpeg (Public Domain) - downloaded from [publicdomainpictures.net](https://www.publicdomainpictures.net/en/view-image.php?image=9670&picture=colorful-autumn-leaves).
  - resources/NotoSans-Regular.ttf (OFL Version 1.1) - downloaded from [Google WebFonts Repo](https://github.com/google/fonts/).

License
-------

All code samples can be distributed under either Public Domain (UNLICENSE) or Zlib license.
