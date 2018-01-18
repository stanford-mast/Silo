/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file silo.h
 *   Declaration of external API functions.
 *   Top-level header file for this library, to be included externally.
 *****************************************************************************/

#pragma once

#include <stdlib.h>
#include <stdint.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Provides information about each piece of a multi-node array.
/// Size is specified in bytes and will be rounded up to the system's memory allocation granularity.
typedef struct SSiloMemorySpec
{
    size_t size;                                                            ///< Size, in bytes, of the memory region to allocate.
    uint32_t numaNode;                                                      ///< Zero-based index of the NUMA node on which to allocate the memory.
} SSiloMemorySpec;


// -------- FUNCTIONS ------------------------------------------------------ //
#ifdef __cplusplus
extern "C" {
#endif

/// Retrieves the allocation granularity.
/// This is a system-specific value and is a multiple of the virtual page size.
/// Each piece of a multi-node array will be a multiple of the system allocation granularity.
/// @return System-specific allocation granularity.
size_t siloGetAllocationUnitSize(void);

/// Retrieves the OS index of the NUMA node to which the specified virtual address is bound.
/// Performs a test 1-byte read from the supplied address, which need not be part of a buffer allocated by Silo.
/// @param [in] address Virtual address to check.
/// @return OS index of the NUMA node to which the virtual address is bound, or a negative value in the event of an error.
int32_t siloGetNUMANodeForVirtualAddress(void* address);

/// Allocates a simple virtually-contiguous buffer on a single NUMA node.
/// Analogous to standard NUMA-aware `malloc`-type functions.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode Zero-based index of the NUMA node on which to allocate the memory.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode);

/// Allocates a simple virtually-contiguous buffer on a single NUMA node.
/// The backing NUMA node is the same as the node on which the calling thread is currently executing.
/// @param [in] size Number of bytes to allocate.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloSimpleBufferAllocLocal(size_t size);

/// Allocates a multi-node array, whose dimensions are specified piecewise.
/// The result is a virtually-contiguous memory buffer potentially physically backed by different NUMA nodes.
/// NUMA awareness can be achieved without adding an additional software indirection step, relying instead on hardware memory address translation.
/// The size of each piece will be rounded to the nearest multiple of the system's allocation granularity (one or more pages).
/// If no piece is large enough to result in non-zero allocation units, this function will fail.
/// Actual allocated size of each piece is available on return from this function.
/// @param [in] count Number of pieces of the array to allocate.
/// @param [in] spec Pointer to an array of specifications, each of which fully determines a piece of the multi-node array.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloMultinodeArrayAlloc(uint32_t count, const SSiloMemorySpec* spec);

/// Deallocates memory allocated using Silo.
/// Only call this function with addresses returned by Silo's memory allocation functions.
/// @param [in] ptr Pointer to the start of the allocated buffer which should be deallocated.
void siloFree(void* ptr);

#ifdef __cplusplus
}
#endif
