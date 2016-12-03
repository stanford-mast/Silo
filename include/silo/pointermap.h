/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
 *************************************************************************//**
 * @file pointermap.h
 *   Declaration of map interface for holding allocation information.
 *   Individual virtual addresses are mapped to size and type information.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Holds a combination of virtual address and allocation size.
/// Can be used to represent a multi-node array when held in a list.
struct SSiloAllocationSpec
{
    void* ptr;                                                              ///< Base virtual address.
    size_t size;                                                            ///< Allocation size, in bytes.
};

 
// -------- FUNCTIONS ------------------------------------------------------ //

/// Submits a set of memory addresses to the pointer map, all of which correspond to a single allocation.
/// The base address, used to uniquely identify this specific allocation, is taken as the `ptr` field in `specs[0]`.
/// For a simple buffer, `count` should be 1. If it is greater than 1, then a multi-node array is being allocated piece-wise, with each piece needing to be stored individually.
/// @param [in] count Number of array pieces to be added to the allocation.
/// @param [in] specs Address and size specifications for each piece of the allocation.
/// @return Pointer to the newly-created vector containing the allocation specifications, or `NULL` if there was an error adding the address to the map.
const std::vector<SSiloAllocationSpec>* siloPointerMapSubmit(uint32_t count, const SSiloAllocationSpec* specs);

/// Retrieves information about a set of memory addresses from the pointer map, all of which correspond to a single allocation.
/// The base address must be specified as a parameter.
/// Note that the returned vector is not removed from the map, but can be accessed in a read-only manner.
/// @param [in] ptr Base address for the allocation of interest.
/// @return Pointer to a vector containing the allocation specifications for the base address, or `NULL` if the address does not exist in the map.
const std::vector<SSiloAllocationSpec>* siloPointerMapRetrieve(void* ptr);

/// Destroys the mapping information associated with the specified base address.
/// Intended to be called once the allocation has been freed by the application.
/// @param [in] ptr Base address for the allocation of interest.
void siloPointerMapDelete(void* ptr);
