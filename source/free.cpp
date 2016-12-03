/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file free.cpp
 *   Implementation of functions that deallocate memory. 
 *****************************************************************************/

#include "../silo.h"
#include "memory.h"
#include "pointermap.h"

#include <cstdlib>
#include <malloc.h>
#include <topo.h>
#include <vector>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "silo.h" for documentation.

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
