// Copyright (c) 2017-2026, Lawrence Livermore National Security, LLC and other CEED contributors.
// All Rights Reserved. See the top-level LICENSE and NOTICE files for details.
//
// SPDX-License-Identifier: BSD-2-Clause
//
// This file is part of CEED:  http://github.com/ceed

// 4AIAD: stub for the CPU JiT backend, which is not built on Windows (it needs dlopen and a
// compiler at run time). Upstream always builds cpu-gen and therefore has no weak stub for it.
#include "ceed-backend-weak.h"
// LCOV_EXCL_START
#include "../ceed-backend-list-cpu-gen.h"
// LCOV_EXCL_STOP
#undef CEED_BACKEND
