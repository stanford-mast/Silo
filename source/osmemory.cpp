/*****************************************************************************
* Silo
*   Multi-platform topology-aware memory management library.
*   Supports multiple styles of NUMA-aware memory allocation.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file osmemory.cpp
*   Implementation of functions that (de)allocate memory.
*   This file contains platform-independent functions.
*****************************************************************************/

#include "osmemory.h"


// -------- CONSTANTS ------------------------------------------------------ //

/// Minimum buffer allocation size before Silo will automatically enable large page support.
static size_t kSiloAutoLargePageMinimumSize = 1048576;


// -------- FUNCTIONS ------------------------------------------------------ //
// See "osmemory.h" for documentation.

size_t siloOSMemoryRoundAllocationSize(size_t unroundedSize, bool useLargePageSupport)
{
    const size_t allocationUnitSize = siloOSMemoryGetGranularity(useLargePageSupport);

    const size_t quotient = unroundedSize / allocationUnitSize;
    const size_t remainder = unroundedSize % allocationUnitSize;

    if (remainder >= (allocationUnitSize / 2))
        return allocationUnitSize * (quotient + 1);
    else
        return allocationUnitSize * quotient;
}

// --------

bool siloOSMemoryShouldAutoEnableLargePageSupport(size_t unroundedSize)
{
    return (unroundedSize >= kSiloAutoLargePageMinimumSize);
}
