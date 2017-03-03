/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file osmemory.h
 *   Declaration of helpers related to memory (de)allocation functionality.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include "../silo.h"

#include <cstdint>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Retrieves the system-specific allocation granularity.
/// This is a platform-specific operation.
/// @param [in] useLargePageSupport `true` to indicate that large-page size should be considered, `false` otherwise.
/// @return Allocation unit size, the minimum size of each distinct piece of a multi-node array.
size_t siloOSMemoryGetGranularity(bool useLargePageSupport);

/// Rounds the provided allocation size to the nearest multiple of the system's allocation granularity.
/// This is a platform-independent operation.
/// @param [in] unroundedSize Unrounded size, in bytes.
/// @param [in] useLargePageSupport `true` to indicate that large-page size should be considered, `false` otherwise.
/// @return `unroundedSize`, rounded to the nearest multiple of the allocation granularity.
size_t siloOSMemoryRoundAllocationSize(size_t unroundedSize, bool useLargePageSupport);

/// Allocates a memory buffer on the specified NUMA node.
/// This is a platform-specific operation.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode OS-specific index of the NUMA node on which to allocate the memory.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloOSMemoryAllocNUMA(size_t size, uint32_t numaNode);

/// Allocates a multi-node array.
/// This is aplatform-specific operation.
/// @param [in] count Number of pieces of the array to allocate.
/// @param [in] spec Pointer to an array of specifications, each of which fully determines a piece of the multi-node array.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloOSMemoryAllocMultiNUMA(uint32_t count, const SSiloMemorySpec* spec);

/// Deallocates the specified memory buffer.
/// This is a platform-specific operation.
/// @param [in] ptr Pointer to the start of the allocated buffer which should be deallocated.
/// @param [in] size Number of bytes originally allocated.
void siloOSMemoryFreeNUMA(void* ptr, size_t size);
