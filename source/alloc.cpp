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
 *   This file contains platform-independent functions.
 *****************************************************************************/

#include "../silo.h"
#include "memory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <topo.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode)
{
    void* allocatedBuffer = NULL;
    int32_t numaNodeOSIndex = topoGetNUMANodeOSIndex(numaNode);
    
    // Verify that the supplied NUMA node index is within range.
    // If so, attempt to allocate the buffer.
    if (0 <= numaNodeOSIndex)
        allocatedBuffer = siloMemoryAllocNUMA(size, numaNodeOSIndex);
    
    // If allocation was successful, add the address to the map.
    if (NULL != allocatedBuffer)
    {
        SSiloAllocationSpec allocatedSpec;
        allocatedSpec.ptr = allocatedBuffer;
        allocatedSpec.size = size;

        siloPointerMapSubmit(1, &allocatedSpec);
    }
    
    return allocatedBuffer;
}
