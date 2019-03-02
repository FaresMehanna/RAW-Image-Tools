# RAW Image Tools
This repo will contain several tools to manipulate RAW images.
The repo contains various parts that handle generic operations which make it good to be used to analyze the images and do experiments with the RAW data but it is not most optimized to be used with specific images type in operational environment.

## Features:
- Handle generic bit depth from 2 to 16 in most containers and operations.
- Deep and shallow copy are supported in most operations to save memory.
- Iterators are used also to prevent deep copy when accessing the data sequentially.
- While the core library supports generic images with generic pixel bits, it is still optimized enough when dealing with 8-bits and 16-bits images.

## Parts:
- core: which contains all core components and core operations and image containers.
- lib_pnm: library support generating 8-bit images (colored/gray) in (ascii/binary).
- T872_challenge: for information [Apertus C++ Challenge](https://lab.apertus.org/T872).