# libCEED for Tootega AIAD (Windows)

This is a fork of **[CEED/libCEED](https://github.com/CEED/libCEED)**, maintained for
**Tootega AIAD** (Artificial Intelligence-Aided Design), a Windows-only CAD/CAE tool built on
OpenUSD, DirectX 12 and CUDA.

> The original README is kept unchanged in **[README.upstream.md](README.upstream.md)**.
> Everything about libCEED itself (API, backends, examples) is documented there and in the
> [upstream documentation](https://libceed.org).

## Why this fork exists

libCEED is a shared dependency of three AIAD components: MFEM (matrix-free operators on the GPU),
[palace-4AIAD](https://github.com/HermesSilva/palace-4AIAD) (electromagnetics) and
[honee-4AIAD](https://github.com/HermesSilva/honee-4AIAD) (compressible flow). AIAD runs **only on
Windows**, while libCEED is built with Makefiles and exercised mainly on Linux. This fork makes it
build and run natively on Windows with its CUDA backends.

## What changes (and why)

| Change | Why |
|--------|-----|
| **CMake build** (`CMakeLists.txt`) for MSVC v145 (`cl`, C17), static library, alongside the upstream Makefile | AIAD and its other dependencies are built with CMake + Visual Studio 2026 on Windows |
| C99 variable-length arrays replaced by `CEED_VLA` (`_alloca` under `_MSC_VER`), marked `4AIAD` | MSVC's C compiler does not support VLAs |
| `/Zc:preprocessor` and `__restrict__=__restrict` exported to consumers | standard `_Pragma`/`__VA_OPT__` and the GNU `restrict` spelling in the public headers |
| CPU backends `ref`, `blocked`, `opt`, `avx` (`/arch:AVX2`); `cpu-gen` JIT not built | the JIT needs `dlopen` and a runtime C compiler |
| CUDA backends (`cuda-ref`, `cuda-shared`, `cuda-gen`) behind `CEED_WITH_CUDA` | GPU execution on the workstation's NVIDIA card |

The public C API is not changed; MFEM, Palace and HONEE consume this fork as a drop-in libCEED.

## Status

The CPU build compiles with Visual Studio 2026 (MSVC 19.51, toolset v145):

```bat
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl ^
      -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL -DCEED_WITH_CUDA=OFF
cmake --build build && cmake --install build --prefix <prefix>
```

Tootega AIAD builds it with the Visual Studio project `libceed.vcxproj` (+ `libceed4AIAD.props`), same sources
and settings, into `libceed.lib`. It installs `ceed.lib` and a CMake package (`find_package(libCEED CONFIG)`, target `libCEED::ceed`).
The CUDA backends are wired in CMake but not validated yet.

## Branches

| Branch | Content |
|--------|---------|
| `main` | mirror of `CEED/libCEED` `main`, never modified |
| `main-4AIAD` (default) | `main` + the Windows changes, rebased periodically onto upstream |

## License

Same as upstream: **BSD 2-Clause** (see [LICENSE](LICENSE)). Changes made in this fork are
released under the same license.
