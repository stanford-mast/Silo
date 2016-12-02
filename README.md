Silo is a multi-platform topology-aware memory management library for x86-based systems containing one or more NUMA nodes.
It features a simple API for allocating and deallocating memory in a NUMA-aware manner, including in ways not automatically supported by the underlying system.
For example, Silo keeps track of all buffers and arrays it allocates to simplify the deallocation procedure.

In addition to simple buffer allocation (in the style of most existing `malloc`-type functions) Silo supports multi-node array allocation.
A multi-node array is a virtually-contiguous buffer physically backed by memory on different NUMA nodes.
This mode of allocation is useful for the purpose of obtaining NUMA awareness without adding an explicit software layer of indirection, relying on hardware address translation instead.

Silo is implemented using C++.


# Requirements

To build and link with Silo, the following are required.

- 64-bit x86-compatible processor with support for AVX instructions
  
  Silo has been tested with Intel processors of the Sandy Bridge generation and newer.
  Non-Intel processors may also be compatible if they support the required instructions.
  Do not attempt to use it on older-generation processors, at the risk of encountering "Illegal Instruction" errors.
  
- Windows 10 Pro or Ubuntu 14.04
  
  64-bit versions of these operating systems are required.
  Other Linux distributions are likely compatible, but they have not been tested.

- **Topo**
  
  Topo provides some helpers and wrappers around the `hwloc` library.

- [**libnuma**](http://oss.sgi.com/projects/libnuma/) (Linux only)
  
  Silo makes use of functions in this library for certain memory allocation operations.
  It is likely available as a package.
  No equivalent library is required on Windows; all needed functions are built into the operating system.

- [**hwloc**](https://www.open-mpi.org/projects/hwloc/) and its dependencies
  
  Silo makes use of this library to obtain system topology information in a platform-independent manner.
  It has been tested with version 1.11.4.
  Some distributions of Linux may make this library available as a package.
  Otherwise, and in all cases on Windows, it must be downloaded and installed manually.


# Building

On all platforms, Silo compiles to a static library.

The Windows build system is based on Visual Studio 2015 Community Edition. Compilation is known to work from the graphical interface, but command-line build is also likely possible.

To build on Linux, just type `make` from within the repository directory.


# Linking and Using

Projects that make use of Silo should include the top-level silo.h header file and nothing else.

Assuming a Linux-based C-language project that uses Silo and consists of a single source file called `main.c`, the following command would build and link with Silo.

    gcc main.c -lsilo -ltopo -lhwloc -lnuma -lpciaccess -lxml2


# Getting Started

Documentation is available and can be built using Doxygen.
It covers both the external API in silo.h and Silo's internals, the latter being of interest only to those who wish to modify the implementation of Silo.

On Linux, type `make docs` to compile the documentation. On Windows, run the Doxygen tool using the repository directory as the working directory (it may be necessary to create the output directory manually first).

The remainder of this section is designed to facilitate a quick understanding of the Silo API.
It explains key concepts and defines key terms, providing pointers to relevant API functions and data structures.
Source code examples are provided immediately following.


## Concepts and Terminology

Coming soon.


## Examples

Coming soon.


# Copyright

Silo is licensed under BSD 3-clause (see "LICENSE" in the top-level source code directory).

Copyright (c) 2016 Stanford University, Department of Electrical Engineering.
Authored by Samuel Grossman.
