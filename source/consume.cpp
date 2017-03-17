/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file consume.cpp
 *   Implementation of helpers that facilitate testing and faulting memory.
 *****************************************************************************/

#include <cstdint>


 // -------- FUNCTIONS ------------------------------------------------------ //
 // See "consume.h" for documentation.

uint8_t siloConsumeByte(uint8_t byte)
{
    return byte;
}
