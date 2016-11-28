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
#include <cstdlib>
#include <hwloc.h>
#include <malloc.h>
#include <topo.h>
#include <vector>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode)
{
	void* allocatedBuffer = NULL;
	hwloc_obj_t numaNodeObject = topoGetNUMANodeObjectAtIndex(numaNode);
    
	// Verify that the supplied NUMA node index is within range.
	if (NULL != numaNodeObject)
	{
		if (1 == hwloc_bitmap_weight(numaNodeObject->nodeset))
		{
			// NUMA node object contains a valid nodeset, so use it.
			allocatedBuffer = hwloc_alloc_membind_nodeset(topoGetSystemTopologyObject(), size, numaNodeObject->nodeset, HWLOC_MEMBIND_BIND, 0);
		}
		else
		{
			// NUMA node object does not contain a valid nodeset, likely because this is a single-node system without actual NUMA objects.
			allocatedBuffer = hwloc_alloc(topoGetSystemTopologyObject(), size);
		}
	}

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
