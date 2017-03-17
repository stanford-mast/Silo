/*****************************************************************************
 * Silo
 *   Multi-platform topology-aware memory management library.
 *   Supports multiple styles of NUMA-aware memory allocation.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file consume.h
 *   Declaration of helpers that facilitate testing and faulting memory.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <cstdint>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Consumes a byte. Returns the same input it is provided.
/// Intended to ensure the compiler does not optimize away test memory operations.
/// @param [byte] Byte to consume.
/// @return `byte` is returned.
uint8_t siloConsumeByte(uint8_t byte);
