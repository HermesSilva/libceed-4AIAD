// Copyright (c) 2017-2026, Lawrence Livermore National Security, LLC and other CEED contributors.
// All Rights Reserved. See the top-level LICENSE and NOTICE files for details.
//
// SPDX-License-Identifier: BSD-2-Clause
//
// This file is part of CEED:  http://github.com/ceed

#include <ceed.h>
#include <ceed/backend.h>
#include <stdarg.h>

CEED_INTERN int CeedRegister_Weak(const char *name, int num_prefixes, ...);

// 4AIAD: COFF has no weak symbols; the CMake build compiles a stub only for backends it does not build.
#if defined(_WIN32)
#define CEED_WEAK_ATTR
#else
#define CEED_WEAK_ATTR __attribute__((weak))
#endif

#define CEED_BACKEND(name, num_prefixes, ...) \
  CEED_INTERN int CEED_WEAK_ATTR name(void) { return CeedRegister_Weak(__func__, num_prefixes, __VA_ARGS__); }
