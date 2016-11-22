/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file pointermap.cpp
 *   Implementation of map data structures for holding allocation information.
 *   Individual virtual addresses are mapped to size and type information.
 *****************************************************************************/

#include "pointermap.h"

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <unordered_map>


// -------- LOCALS --------------------------------------------------------- //

/// Holds all information about memory allocated through this library.
/// Maps base address information to size (for end-user convenience) and tracks piece-wise allocations of multi-node arrays.
static std::unordered_map<void*, const std::vector<SSiloAllocationSpec>*> siloPointerMap;


// -------- FUNCTIONS ------------------------------------------------------ //
// See "pointermap.h" for documentation.

const std::vector<SSiloAllocationSpec>* siloPointerMapSubmit(uint32_t count, const SSiloAllocationSpec* specs)
{
    // Sanity check.
    if (1 > count)
        return NULL;

    // Extract the base address and check if it already exists.
    void* baseAddress = specs[0].ptr;

    if (0 != siloPointerMap.count(baseAddress))
        return NULL;

    // Create a vector of all allocation specifications and submit them to the map.
    std::vector<SSiloAllocationSpec>* allocationVector = new std::vector<SSiloAllocationSpec>(count);

    for (uint32_t i = 0; i < count; ++i)
        allocationVector->push_back(specs[i]);

    siloPointerMap.insert({baseAddress, allocationVector});
    
    return allocationVector;
}

// --------

const std::vector<SSiloAllocationSpec>* siloPointerMapRetrieve(void* ptr)
{
    const std::vector<SSiloAllocationSpec>* result = NULL;

    if (0 != siloPointerMap.count(ptr))
        result = siloPointerMap[ptr];

    return result;
}

// --------

void siloPointerMapDelete(void* ptr)
{
    if (0 != siloPointerMap.count(ptr))
    {
        delete siloPointerMap[ptr];
        siloPointerMap.erase(ptr);
    }
}
