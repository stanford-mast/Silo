/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osmemory-windows.cpp
 *   Implementation of functions that (de)allocate memory. 
 *   This file contains Windows-specific functions.
 *****************************************************************************/

#include "../silo.h"
#include "osmemory.h"
#include "pointermap.h"

#include <cstdint>
#include <cstdlib>
#include <topo.h>
#include <vector>
#include <Psapi.h>
#include <Windows.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Allocates virtual memory at the specified starting address, optionally using large-page support.
/// This is a Windows-specific helper function.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode OS-specific index of the NUMA node on which to allocate the memory.
/// @param [in] startPtr Pointer that holds the desired starting address of the allocated region.
/// @param [in] shouldCommit Indicates that the virtual memory region should be committed, rather than just reserved in the process' virtual address space.
/// @param [in] useLargePageSupport `true` to indicate that large-page support should be requested, `false` otherwise.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
static void* siloWindowsMemoryAllocAtNUMA(size_t size, uint32_t numaNode, void* startPtr, bool shouldCommit, bool useLargePageSupport)
{
    return VirtualAllocExNuma(GetCurrentProcess(), startPtr, size, MEM_RESERVE | (shouldCommit ? MEM_COMMIT : 0) | (useLargePageSupport ? MEM_LARGE_PAGES : 0), PAGE_READWRITE, numaNode);
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osmemory.h" for documentation.

size_t siloOSMemoryGetGranularity(bool useLargePageSupport)
{
    SYSTEM_INFO systemInfo;
    size_t allocationUnitSize = 0, largePageSize = 0;

    GetSystemInfo(&systemInfo);

    if (useLargePageSupport)
        largePageSize = (size_t)GetLargePageMinimum();

    // Return the largest of the system-reported allocation granularity, page size, and (if applicable) large page size.
    allocationUnitSize = (size_t)systemInfo.dwAllocationGranularity;

    if (systemInfo.dwPageSize > allocationUnitSize)
        allocationUnitSize = (size_t)systemInfo.dwPageSize;

    if (largePageSize > allocationUnitSize)
        allocationUnitSize = largePageSize;

    return allocationUnitSize;
}

// --------

int32_t siloOSMemoryGetNUMANodeForVirtualAddress(void* address)
{
    PSAPI_WORKING_SET_EX_INFORMATION addressInfo;
    addressInfo.VirtualAddress = address;

    if (0 == QueryWorkingSetEx(GetCurrentProcess(), (void*)&addressInfo, sizeof(addressInfo)))
        return -1;

    if (!addressInfo.VirtualAttributes.Valid)
        return -1;

    return (int32_t)addressInfo.VirtualAttributes.Node;
}

// --------

void* siloOSMemoryAllocNUMA(size_t size, uint32_t numaNode)
{
    return siloWindowsMemoryAllocAtNUMA(size, numaNode, NULL, true, siloOSMemoryShouldAutoEnableLargePageSupport(size));
}

// --------

void* siloOSMemoryAllocLocalNUMA(size_t size)
{
    PROCESSOR_NUMBER processorNumber;
    USHORT numaNode;

    // Query the operating system to determine the NUMA node identifier for the current thread.
    GetCurrentProcessorNumberEx(&processorNumber);
    if (0 == GetNumaProcessorNodeEx(&processorNumber, &numaNode))
        return NULL;
    
    return siloOSMemoryAllocNUMA(size, (uint32_t)numaNode);
}

// --------

void siloOSMemoryFreeNUMA(void* ptr, size_t size)
{
    VirtualFreeEx(GetCurrentProcess(), ptr, 0, MEM_RELEASE);
}

// --------

void* siloOSMemoryAllocMultiNUMA(uint32_t count, const SSiloMemorySpec* spec)
{
    // Figure out if large page support is worth it.
    size_t totalRequestedBytes = 0;
    
    for (uint32_t i = 0; i < count; ++i)
        totalRequestedBytes += spec[i].size;
    
    const bool useLargePageSupport = siloOSMemoryShouldAutoEnableLargePageSupport(totalRequestedBytes);
    
    // Get the minimum allocation unit size.
    const size_t allocationUnitSize = siloOSMemoryGetGranularity(useLargePageSupport);
    
    // Compute the total number of bytes requested and granted, and simultaneously verify the passed NUMA node indices.
    size_t totalActualBytes = 0;
    std::vector<size_t> actualBytes(count);
    
    for (uint32_t i = 0; i < count; ++i)
    {
        if (0 > topoGetNUMANodeOSIndex(spec[i].numaNode))
            return NULL;
        
        actualBytes[i] = siloOSMemoryRoundAllocationSize(spec[i].size, useLargePageSupport);
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

    // Reserve the entire virtual address space, as a way of checking for sufficient virtual address space and getting a base address.
    void* allocatedBuffer = siloWindowsMemoryAllocAtNUMA(totalActualBytes, 0, NULL, false, useLargePageSupport);
    if (NULL == allocatedBuffer)
        return NULL;

    // Create an array of allocation specs into which to store information about each array piece.
    SSiloAllocationSpec* allocationSpecs = new SSiloAllocationSpec[count];

    // Free the reserved virtual address space, for future piece-wise allocation.
    siloOSMemoryFreeNUMA(allocatedBuffer, totalActualBytes);

    // Allocate each piece of the multi-node array.
    uint32_t numAllocated = 0;
    bool allocationSuccessful = true;
    for (; numAllocated < count; ++numAllocated)
    {
        // Attempt to allocate a piece of the array and bail if the attempt results in failure.
        void* allocationResult = siloWindowsMemoryAllocAtNUMA(actualBytes[numAllocated], topoGetNUMANodeOSIndex(spec[numAllocated].numaNode), allocatedBuffer, true, useLargePageSupport);
        if (NULL == allocationResult)
        {
            allocationSuccessful = false;
            break;
        }
        
        // Record the piece that was allocated.
        allocationSpecs[numAllocated].ptr = allocatedBuffer;
        allocationSpecs[numAllocated].size = actualBytes[numAllocated];
        
        // Advance to the next piece.
        allocatedBuffer = (void*)((size_t)allocatedBuffer + actualBytes[numAllocated]);
    }
    
    // Check for success or failure.
    if (false == allocationSuccessful)
    {
        // If failed, free the pieces that actually were allocated.
        for (uint32_t i = 0; i < numAllocated; ++i)
            siloOSMemoryFreeNUMA(allocationSpecs[i].ptr, allocationSpecs[i].size);

        allocatedBuffer = NULL;
    }
    else
    {
        // If succeeded, return the base address of the allocated multi-node array and store its metadata.
        allocatedBuffer = allocationSpecs[0].ptr;
        siloPointerMapSubmit(numAllocated, allocationSpecs);
    }
    
    delete[] allocationSpecs;
    return allocatedBuffer;
}
