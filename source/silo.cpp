/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file silo.cpp
 *   Implementation of all external API functions.
 *****************************************************************************/

#include "../silo.h"
#include "osmemory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <malloc.h>
#include <topo.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

size_t siloGetAllocationUnitSize(void)
{
    return siloOSMemoryGetGranularity(false);
}

// --------

void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode)
{
    void* allocatedBuffer = NULL;
    int32_t numaNodeOSIndex = topoGetNUMANodeOSIndex(numaNode);
    
    // Verify that the supplied NUMA node index is within range.
    // If so, attempt to allocate the buffer.
    if (0 <= numaNodeOSIndex)
        allocatedBuffer = siloOSMemoryAllocNUMA(size, numaNodeOSIndex);
    
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

void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec)
{
    return siloOSMemoryAllocMultiNUMA(count, spec);
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
            const SSiloAllocationSpec& spec = (*specToFree)[i];
            siloOSMemoryFreeNUMA(spec.ptr, spec.size);
        }

        // Delete the metadata for the just-freed allocation.
        siloPointerMapDelete(ptr);
    }
}
