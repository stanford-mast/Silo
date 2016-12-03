/*****************************************************************************
* Silo
*   Multi-platform topology-aware memory management library.
*   Supports multiple styles of NUMA-aware memory allocation.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016
*************************************************************************//**
* @file memory.h
*   Declaration of helpers related to memory (de)allocation functionality.
*   Not intended for external use.
*****************************************************************************/

#pragma once

#include <cstdint>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Allocates a memory buffer on the specified NUMA node.
/// This is a platform-specific operation.
/// @param [in] size Number of bytes to allocate.
/// @param [in] numaNode OS-specific index of the NUMA node on which to allocate the memory.
/// @return Pointer to the start of the allocated buffer, or NULL on allocation failure.
void* siloMemoryAllocNUMA(size_t size, uint32_t numaNode);

/// Deallocates the specified memory buffer.
/// This is a platform-specific operation.
/// @param [in] ptr Pointer to the start of the allocated buffer which should be deallocated.
/// @param [in] size Number of bytes originally allocated.
void siloMemoryFreeNUMA(void* ptr, size_t size);
