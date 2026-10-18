# 4AIAD: CMake build of libCEED for Windows (MSVC ABI, static library). Upstream builds with GNU
# make only. This file mirrors the Makefile's source selection for the backends we use:
#   CPU: ref, blocked, opt, avx (always), cpu-gen JIT is not built (it needs dlopen + a runtime
#   compiler); GPU: cuda-ref, cuda-shared, cuda-gen (CEED_WITH_CUDA).
# libCEED is C99 with VLAs and GNU attributes, which cl.exe does not accept: configure it with
# clang-cl (same MSVC ABI, CRT and linker).
cmake_minimum_required(VERSION 3.24)
project(libCEED VERSION 1.0.1 LANGUAGES C CXX)

option(CEED_WITH_AVX "Build the AVX backends" ON)
option(CEED_WITH_CUDA "Build the CUDA backends" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
if(CEED_WITH_CUDA)
  enable_language(CUDA)
  find_package(CUDAToolkit REQUIRED)
endif()

# Interface, gallery and weak registration stubs (filtered like the Makefile's libceed.c).
file(GLOB CEED_SOURCES CONFIGURE_DEPENDS interface/ceed*.c gallery/*.c gallery/*/ceed*.c)
list(FILTER CEED_SOURCES EXCLUDE REGEX
  "interface/ceed-(cuda|hip|fortran|jit-tools-deprecated|jit-source-root-default)\\.c$")

set(CEED_BACKENDS_BUILT ref blocked opt)
if(CEED_WITH_AVX)
  list(APPEND CEED_BACKENDS_BUILT avx)
endif()
foreach(b IN LISTS CEED_BACKENDS_BUILT)
  file(GLOB _src CONFIGURE_DEPENDS backends/${b}/*.c)
  list(APPEND CEED_SOURCES ${_src})
endforeach()
if(CEED_WITH_CUDA)
  file(GLOB _src CONFIGURE_DEPENDS backends/cuda/*.c backends/cuda/*.cpp backends/cuda-ref/*.c
       backends/cuda-ref/*.cpp backends/cuda-ref/kernels/*.cu backends/cuda-shared/*.c
       backends/cuda-gen/*.c backends/cuda-gen/*.cpp)
  list(APPEND CEED_SOURCES interface/ceed-cuda.c ${_src})
endif()

# Weak stubs: GNU linkers let the real backend override a weak symbol; MSVC has no weak symbols,
# so a stub is compiled only for the backends that are not built. cpu-gen has no upstream stub.
list(APPEND CEED_SOURCES backends/weak/ceed-backend-weak.c backends/weak/ceed-cpu-gen-weak.c)
foreach(b memcheck sve sme xsmm hip sycl magma avx cuda)
  if(b STREQUAL "avx" AND CEED_WITH_AVX)
    continue()
  endif()
  if(b STREQUAL "cuda" AND CEED_WITH_CUDA)
    continue()
  endif()
  list(APPEND CEED_SOURCES backends/weak/ceed-${b}-weak.c)
endforeach()

add_library(ceed STATIC ${CEED_SOURCES})
target_include_directories(ceed PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
# Symbol visibility attributes are ELF-only.
target_compile_definitions(ceed PUBLIC CEED_SKIP_VISIBILITY _CRT_SECURE_NO_WARNINGS)
if(CEED_WITH_AVX)
  set_source_files_properties(backends/avx/ceed-avx-tensor.c PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma")
endif()
if(CEED_WITH_CUDA)
  target_compile_definitions(ceed PRIVATE CEED_CUDA_DIR="${CUDAToolkit_LIBRARY_ROOT}")
  target_link_libraries(ceed PUBLIC CUDA::cudart_static CUDA::cuda_driver CUDA::nvrtc_static CUDA::cublas)
endif()

# Build metadata and the JIT source root (installed headers; GPU backends JIT from them).
execute_process(COMMAND git describe --dirty=+ --always WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE CEED_GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
set_source_files_properties(interface/ceed-config.c PROPERTIES COMPILE_DEFINITIONS
  "CEED_GIT_VERSION=\"${CEED_GIT_DESCRIBE}\";CEED_BUILD_CONFIGURATION=\"// Build Configuration: CMake (4AIAD)\"")
set_source_files_properties(interface/ceed-jit-source-root-install.c PROPERTIES COMPILE_DEFINITIONS
  "CEED_JIT_SOURCE_ROOT_DEFAULT=\"${CMAKE_INSTALL_PREFIX}/include/\"")

include(GNUInstallDirs)
install(TARGETS ceed EXPORT libCEEDTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT libCEEDTargets NAMESPACE libCEED:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libCEED)
include(CMakePackageConfigHelpers)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/libCEEDConfig.cmake
  "include(CMakeFindDependencyMacro)\n"
  "if(${CEED_WITH_CUDA})\n  find_dependency(CUDAToolkit)\nendif()\n"
  "include(\${CMAKE_CURRENT_LIST_DIR}/libCEEDTargets.cmake)\n")
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/libCEEDConfigVersion.cmake
  COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libCEEDConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libCEEDConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libCEED)
