/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file alloc.cpp
 *   Implementation of functions that allocate memory. 
 *   This file contains Linux-specific functions.
 *****************************************************************************/

#include "../silo.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <hwloc.h>
#include <topo.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec)
{
    return NULL;
}
