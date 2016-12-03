/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file memory-windows.cpp
 *   Implementation of functions that (de)allocate memory. 
 *   This file contains Windows-specific functions.
 *****************************************************************************/

#include "../silo.h"
#include "memory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <hwloc.h>
#include <topo.h>
#include <Windows.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Allocates virtual memory at the specified starting address, optionally using large-page support.
/// This is a Windows-specific helper function.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode OS-specific index of the NUMA node on which to allocate the memory.
/// @param [in] startPtr Pointer that holds the desired starting address of the allocated region.
/// @param [in] useLargePageSupport `true` to indicate that large-page support should be requested, `false` otherwise.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
static void* siloWidowsMemoryAllocAtNUMA(size_t size, uint32_t numaNode, void* startPtr, bool useLargePageSupport)
{
    return VirtualAllocExNuma(GetCurrentProcess(), startPtr, size, MEM_COMMIT | MEM_RESERVE | (useLargePageSupport ? MEM_LARGE_PAGES : 0), PAGE_READWRITE, numaNode);
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "memory.h" and "silo.h" for documentation.

void* siloMemoryAllocNUMA(size_t size, uint32_t numaNode)
{
    return siloWidowsMemoryAllocAtNUMA(size, numaNode, NULL, false);
}

// --------

void siloMemoryFreeNUMA(void* ptr, size_t size)
{
    VirtualFreeEx(GetCurrentProcess(), ptr, 0, MEM_RELEASE);
}

// --------

void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec)
{
    return NULL;
}
