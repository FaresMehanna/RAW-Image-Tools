# RAW Image Tools
This repository will contain several tools to manipulate RAW images.
The repository contains various parts that handle generic operations which make it good to be used to analyze the images and do experiments with the RAW data but it is not most optimized to be used with specific images type in operational environment.

## Features:
- Handle generic bit depth from 2 to 16 in most containers and operations.
- Deep and shallow copy are supported in most operations to save memory.
- Iterators are used also to prevent deep copy when accessing the data sequentially.
- While the core library supports generic images with generic pixel bits, it is still optimized enough when dealing with 8-bits and 16-bits images.

## Parts:
- core: which contains all core components and core operations and image containers.
- lib_pnm: library support generating 8-bit images (colored/gray) in (ASCII/binary) in PNM format.
- lib_dng: library support generating DNG image for a given Bayer image (uncompressed or lossless) based on [raw2dng](https://github.com/apertus-open-source-cinema/misc-tools-utilities/tree/master/raw2dng).
- T872_challenge: for information [Apertus C++ Challenge](https://lab.apertus.org/T872).
- LJ92_eval: evaluator kit for JPEG 1992 lossless encoder.

### LJ92_eval:
- Take only single RAW12 image as a parameter.
- Output 3 DNG images, 12-bits uncompressed, LJ92 12-bits compressed and LJ92 8-bits compressed.
- It does output also several info on the terminal showing information about the compression ratio, LJ92 encoder details and a lot of information about SSSS classes which can help to determine the best Huffman tree.