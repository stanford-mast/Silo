/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file memory.cpp
 *   Implementation of functions that (de)allocate memory.
 *   This file contains platform-independent functions.
 *****************************************************************************/

#include "../silo.h"
#include "memory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <malloc.h>
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

// --------

void siloFree(void* ptr)
{
    const std::vector<SSiloAllocationSpec>* specToFree = siloPointerMapRetrieve(ptr);

    if (NULL == specToFree)
        free(ptr);
    else
    {
        // Free each piece that was allocated.
        for (size_t i = 0; i < specToFree->size(); ++i)
        {
            const SSiloAllocationSpec& spec = specToFree->at(i);
            siloMemoryFreeNUMA(spec.ptr, spec.size);
        }

        // Delete the metadata for the just-freed allocation.
        siloPointerMapDelete(ptr);
    }
}
