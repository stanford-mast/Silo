/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osmemory-linux.cpp
 *   Implementation of functions that (de)allocate memory. 
 *   This file contains Linux-specific functions.
 *****************************************************************************/

#include "../silo.h"
#include "consume.h"
#include "osmemory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <numa.h>
#include <numaif.h>
#include <sched.h>
#include <topo.h>
#include <unistd.h>
#include <vector>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osmemory.h" for documentation.

size_t siloOSMemoryGetGranularity(bool useLargePageSupport)
{
    return (size_t)sysconf(_SC_PAGESIZE);
}

// --------

int32_t siloOSMemoryGetNUMANodeForVirtualAddress(void* address)
{
    int nodeResult = -1;
    
    if (0 != move_pages(0, 1, &address, NULL, &nodeResult, 0))
        return -1;
    
    if (-EFAULT == nodeResult)
    {
        // If the page was not yet faulted into memory, the requested information will not be available.
        // A write is sometimes required to cause a fault, so try a write here and query again.
        uint8_t* byteAddress = (uint8_t*)address;
        *byteAddress = siloConsumeByte(*byteAddress);
        
        if (0 != move_pages(0, 1, &address, NULL, &nodeResult, 0))
            return -1;
    }
    
    return (int32_t)nodeResult;
}

// --------

void* siloOSMemoryAllocNUMA(size_t size, uint32_t numaNode)
{
    return numa_alloc_onnode(size, (int)numaNode);
}

// --------

void* siloOSMemoryAllocLocalNUMA(size_t size)
{
    return numa_alloc_onnode(size, numa_node_of_cpu(sched_getcpu()));
}

// --------

void siloOSMemoryFreeNUMA(void* ptr, size_t size)
{
    numa_free(ptr, size);
}

// --------

void* siloOSMemoryAllocMultiNUMA(uint32_t count, const SSiloMemorySpec* spec)
{
    // Get the minimum allocation unit size.
    const size_t allocationUnitSize = siloOSMemoryGetGranularity(false);
    
    // Compute the total number of bytes requested and granted, and simultaneously verify the passed NUMA node indices.
    size_t totalRequestedBytes = 0;
    size_t totalActualBytes = 0;
    std::vector<size_t> actualBytes(count);
    
    for (uint32_t i = 0; i < count; ++i)
    {
        if (0 > topoGetNUMANodeOSIndex(spec[i].numaNode))
            return NULL;
        
        totalRequestedBytes += spec[i].size;
        actualBytes[i] = siloOSMemoryRoundAllocationSize(spec[i].size, false);
        totalActualBytes += actualBytes[i];
    }
    
    // Verify that sufficient space was actually allocated on each node to justify even using this function.
    if (0 == totalActualBytes)
        return NULL;
    
    // Add sufficient additional space to allocate to the last NUMA node to ensure coverage of the total requested size.
    while (totalActualBytes < totalRequestedBytes)
    {
        totalActualBytes += allocationUnitSize;
        actualBytes[count - 1] += allocationUnitSize;
    }
    
    // Reserve the entire virtual address space on the first NUMA node.
    void* allocatedBuffer = siloOSMemoryAllocNUMA(totalActualBytes, topoGetNUMANodeOSIndex(spec[0].numaNode));
    if (NULL == allocatedBuffer)
        return NULL;
    
    // Move each piece beyond the first to the correct NUMA node.
    uint8_t* moveBaseAddress = (uint8_t*)allocatedBuffer + actualBytes[0];
    for (uint32_t i = 1; i < count; ++i)
    {
        // Move the current piece to the specified NUMA node.
        numa_tonode_memory((void*)moveBaseAddress, actualBytes[i], topoGetNUMANodeOSIndex(spec[i].numaNode));
        
        // Advance to the next address to move.
        moveBaseAddress += actualBytes[i];
    }
    
    // Submit the allocated buffer to the pointer map.
    SSiloAllocationSpec allocatedSpec;
    allocatedSpec.ptr = allocatedBuffer;
    allocatedSpec.size = totalActualBytes;
    siloPointerMapSubmit(1, &allocatedSpec);
    
    return allocatedBuffer;
}
