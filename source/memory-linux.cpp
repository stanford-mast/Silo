/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file memory-linux.cpp
 *   Implementation of functions that (de)allocate memory. 
 *   This file contains Linux-specific functions.
 *****************************************************************************/

#include "../silo.h"
#include "memory.h"

#include <cstdint>
#include <cstdlib>
#include <numa.h>


 // -------- FUNCTIONS ------------------------------------------------------ //
 // See "memory.h" and "silo.h" for documentation.

void* siloMemoryAllocNUMA(size_t size, uint32_t numaNode)
{
    return numa_alloc_onnode(size, (int)numaNode);
}

// --------

void siloMemoryFreeNUMA(void* ptr, size_t size)
{
    numa_free(ptr, size);
}

// --------

void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec)
{
    return NULL;
}
