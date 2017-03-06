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


// -------- VERSION INFORMATION -------------------------------------------- //

/// 32-bit unsigned integer that represents the version of Silo.
/// Incremented each time a change is made that affects the API.
/// - Version 1: Initial release.
#define SILO_LIBRARY_VERSION                    0x00000001


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

/// Retrieves and returns the compiled Silo library version.
/// @return Silo library version number.
uint32_t siloGetLibraryVersion(void);

/// Retrieves the allocation granularity.
/// This is a system-specific value and is a multiple of the virtual page size.
/// Each piece of a multi-node array will be a multiple of the system allocation granularity.
/// @return System-specific allocation granularity.
size_t siloGetAllocationUnitSize(void);

/// Allocates a simple virtually-contiguous buffer on a single NUMA node.
/// Analogous to standard NUMA-aware `malloc`-type functions.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode Zero-based index of the NUMA node on which to allocate the memory.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode);

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
