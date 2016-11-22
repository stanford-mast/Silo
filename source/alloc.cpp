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
 *****************************************************************************/

#include "../silo.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <hwloc.h>
#include <malloc.h>
#include <topo.h>
#include <vector>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode)
{
    hwloc_obj_t numaNodeObject = topoGetNUMANodeObjectAtIndex(numaNode);
    
    printf("NUMA node %u\n", numaNode);

    unsigned int i;
    hwloc_bitmap_foreach_begin(i, numaNodeObject->nodeset);
    printf(" -> bit %u is set\n", i);
    hwloc_bitmap_foreach_end();
    
    return NULL;
}

// --------

void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec)
{
    return NULL;
}

// --------

void siloFree(void* ptr)
{
    const std::vector<SSiloAllocationSpec>* specToFree = siloPointerMapRetrieve(ptr);

    if (NULL == ptr)
        free(ptr);
    else
    {
        // Free each piece that was allocated.
        for (size_t i = 0; i < specToFree->size(); ++i)
        {
            const SSiloAllocationSpec& spec = specToFree->at(i);
            hwloc_free(topoGetSystemTopologyObject(), spec.ptr, spec.size);
        }

        // Delete the metadata for the just-freed allocation.
        siloPointerMapDelete(ptr);
    }
}
