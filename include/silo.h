/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016
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

/// Allocates a simple virtually-contiguous buffer on a single NUMA node.
/// Analogous to standard NUMA-aware `malloc`-type functions.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode Zero-based index of the NUMA node on which to allocate the memory.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloSimpleBufferAlloc(size_t size, uint32_t numaNode);

/// Allocates a multi-node array, whose dimensions are specified piecewise.
/// The result is a virtually-contiguous memory buffer potentially physically backed by different NUMA nodes.
/// NUMA awareness can be achieved without adding an additional software indirection step, relying instead on hardware memory address translation.
/// @param [in] count Number of pieces of the array to allocate.
/// @param [in, out] spec Pointer to an array of specifications, each of which fully determines a piece of the multi-node array. On output, `size` is modified to reflect actual allocated size of each piece.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloMultinodeArrayAlloc(uint32_t count, SSiloMemorySpec* spec);

/// Deallocates memory allocated using Silo.
/// Only call this function with addresses returned by Silo's memory allocation functions.
/// @param [in] ptr Pointer to the start of the allocated buffer which should be deallocated.
void siloFree(void* ptr);

#ifdef __cplusplus
}
#endif
