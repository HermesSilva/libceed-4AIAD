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
| **CMake build** for MSVC v145, alongside the upstream Makefile | AIAD and its other dependencies are built with CMake on Windows |
| **clang-cl** for C sources that need C99 variable-length arrays (QFunctions) | MSVC's C compiler does not support VLAs |
| **CUDA backends** (`/gpu/cuda/*`) built with nvcc + MSVC, NVRTC JIT on Windows | GPU execution on the workstation's NVIDIA card |
| Windows paths, DLL exports and runtime JIT include paths | needed for a shared library on Windows |
| Backends without Windows support (for example LIBXSMM) disabled | they do not build on Windows |

The public C API is not changed; MFEM, Palace and HONEE consume this fork as a drop-in libCEED.

## Status

**No source changes yet.** At this point the only differences from upstream are this README, the
original README renamed to `README.upstream.md`, and `doc/sphinx/source/conf.py`, which now copies
`README.upstream.md` so that the Sphinx documentation still builds. The Windows conversion is
planned in the AIAD roadmap (spikes S4 and S7).

## Branches

| Branch | Content |
|--------|---------|
| `main` | mirror of `CEED/libCEED` `main`, never modified |
| `main-4AIAD` (default) | `main` + the Windows changes, rebased periodically onto upstream |

## License

Same as upstream: **BSD 2-Clause** (see [LICENSE](LICENSE)). Changes made in this fork are
released under the same license.
