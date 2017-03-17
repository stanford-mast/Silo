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

/// Consumes a byte. Does nothing with it.
/// Intended to be invoked on the result of a byte read from memory that is used as a test.
/// Ensures the compiler does not optimize away the read from memory.
void siloConsumeByte(uint8_t byte);
