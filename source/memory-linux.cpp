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
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <numa.h>
#include <topo.h>
#include <unistd.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Determines the allocation unit size, with or without considering large page support.
/// This is a Linux-specific helper function.
/// @return Allocation unit size, the minimum size of each distinct piece of a multi-node array.
static inline uint32_t siloLinuxGetAllocationUnitSize(void)
{
    return (uint32_t)sysconf(_SC_PAGESIZE);
}

/// Rounds the requested allocation size to one of a multiple of the allocation granularity.
/// This is a Linux-specific helper function.
/// @param [in] unroundedSize Unrounded size, in bytes.
/// @return `unroundedSize`, rounded to the nearest multiple of the allocation granularity.
static size_t siloLinuxRoundRequestedAllocationSize(size_t unroundedSize)
{
    const size_t allocationUnitSize = siloLinuxGetAllocationUnitSize();
    
    const size_t quotient = unroundedSize / allocationUnitSize;
    const size_t remainder = unroundedSize % allocationUnitSize;
    
    if (remainder >= (allocationUnitSize / 2))
        return allocationUnitSize * (quotient + 1);
    else
        return allocationUnitSize * quotient;
}


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

void* siloMultinodeArrayAlloc(uint32_t count, SSiloMemorySpec* spec)
{
    // Get the minimum allocation unit size.
    const size_t allocationUnitSize = siloLinuxGetAllocationUnitSize();
    
    // Compute the total number of bytes requested and granted, and simultaneously verify the passed NUMA node indices.
    size_t totalRequestedBytes = 0;
    size_t totalActualBytes = 0;
    
    for (uint32_t i = 0; i < count; ++i)
    {
        if (0 > topoGetNUMANodeOSIndex(spec[i].numaNode))
            return NULL;
        
        totalRequestedBytes += spec[i].size;
        spec[i].size = siloLinuxRoundRequestedAllocationSize(spec[i].size);
        totalActualBytes += spec[i].size;
    }
    
    // Verify that sufficient space was actually allocated on each node to justify even using this function.
    if (0 == totalActualBytes)
        return NULL;
    
    // Add sufficient additional space to allocate to the last NUMA node to ensure coverage of the total requested size.
    while (totalActualBytes < totalRequestedBytes)
    {
        totalActualBytes += allocationUnitSize;
        spec[count].size += allocationUnitSize;
    }
    
    // Reserve the entire virtual address space on the first NUMA node.
    void* allocatedBuffer = siloMemoryAllocNUMA(totalActualBytes, topoGetNUMANodeOSIndex(spec[0].numaNode));
    if (NULL == allocatedBuffer)
        return NULL;
    
    // Move each piece beyond the first to the correct NUMA node.
    uint8_t* moveBaseAddress = (uint8_t*)allocatedBuffer + spec[0].size;
    for (uint32_t i = 1; i < count; ++i)
    {
        // Move the current piece to the specified NUMA node.
        numa_tonode_memory((void*)moveBaseAddress, spec[i].size, topoGetNUMANodeOSIndex(spec[i].numaNode));
        
        // Advance to the next address to move.
        moveBaseAddress += spec[i].size;
    }
    
    // Submit the allocated buffer to the pointer map.
    SSiloAllocationSpec allocatedSpec;
    allocatedSpec.ptr = allocatedBuffer;
    allocatedSpec.size = totalActualBytes;
    siloPointerMapSubmit(1, &allocatedSpec);
    
    return allocatedBuffer;
}
