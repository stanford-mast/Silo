###############################################################################
# Silo
#   Multi-platform topology-aware memory management library.
###############################################################################
# Authored by Samuel Grossman
# Department of Electrical Engineering, Stanford University
# Copyright (c) 2016
###############################################################################
# Makefile
#   Build script for GNU-compatible Linux operating systems.
###############################################################################


# --------- PROJECT PROPERTIES ------------------------------------------------

PROJECT_NAME                = silo
PLATFORM_NAME               = linux

SOURCE_DIR                  = source
INCLUDE_DIR                 = include/$(PROJECT_NAME)

OUTPUT_BASE_DIR				= output
OUTPUT_DOCS_DIR				= $(OUTPUT_BASE_DIR)/docs
OUTPUT_DIR                  = $(OUTPUT_BASE_DIR)/$(PLATFORM_NAME)
OUTPUT_FILE                 = lib$(PROJECT_NAME).a

C_SOURCE_SUFFIX             = .c
CXX_SOURCE_SUFFIX           = .cpp


# --------- TOOL SELECTION AND CONFIGURATION ----------------------------------

CC                          = gcc
CXX                         = g++
AR							= ar

CCFLAGS                     = -O3 -Wall -fPIC -std=c11 -march=core-avx-i -mno-vzeroupper -I$(INCLUDE_DIR) -D_GNU_SOURCE
CXXFLAGS                    = -O3 -Wall -fPIC -std=c++0x -march=core-avx-i -mno-vzeroupper -I$(INCLUDE_DIR)
ARFLAGS                     = 


# --------- FILE ENUMERATION --------------------------------------------------

OBJECT_FILE_SUFFIX          = .o
DEP_FILE_SUFFIX             = .d

C_SOURCE_FILES              = $(filter-out $(wildcard $(SOURCE_DIR)/*-*$(C_SOURCE_SUFFIX)), $(wildcard $(SOURCE_DIR)/*$(C_SOURCE_SUFFIX))) $(wildcard $(SOURCE_DIR)/*-$(PLATFORM_NAME)$(C_SOURCE_SUFFIX))
CXX_SOURCE_FILES            = $(filter-out $(wildcard $(SOURCE_DIR)/*-*$(CXX_SOURCE_SUFFIX)), $(wildcard $(SOURCE_DIR)/*$(CXX_SOURCE_SUFFIX))) $(wildcard $(SOURCE_DIR)/*-$(PLATFORM_NAME)$(CXX_SOURCE_SUFFIX))
ALL_SOURCE_FILES            = $(C_SOURCE_FILES) $(CXX_SOURCE_FILES)

OBJECT_FILES_FROM_SOURCE    = $(patsubst $(SOURCE_DIR)/%, $(OUTPUT_DIR)/%$(OBJECT_FILE_SUFFIX), $(ALL_SOURCE_FILES))
DEP_FILES_FROM_SOURCE       = $(patsubst $(SOURCE_DIR)/%, $(OUTPUT_DIR)/%$(DEP_FILE_SUFFIX), $(ALL_SOURCE_FILES))


# --------- TOP-LEVEL RULE CONFIGURATION --------------------------------------

.PHONY: silo docs clean help


# --------- TARGET DEFINITIONS ------------------------------------------------

silo: $(OUTPUT_DIR)/$(OUTPUT_FILE)

docs: | $(OUTPUT_DOCS_DIR)
	@doxygen

help:
	@echo ''
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Targets:'
	@echo '    silo'
	@echo '        Default target.'
	@echo '        Builds Silo as a static library.'
	@echo '    docs'
	@echo '        Builds HTML and LaTeX documentation using Doxygen.'
	@echo '    clean'
	@echo '        Removes all output files, including binary and documentation.'
	@echo '    help'
	@echo '        Shows this information.'
	@echo ''


# --------- BUILDING AND CLEANING RULES ---------------------------------------

$(OUTPUT_DIR)/$(OUTPUT_FILE): $(OBJECT_FILES_FROM_SOURCE)
	@echo '   AR        $@'
	@$(AR) $(ARFLAGS) rcs $@ $^
	@echo 'Build completed: $(PROJECT_NAME).'

clean:
	@echo '   RM        $(OUTPUT_BASE_DIR)'
	@rm -rf $(OUTPUT_BASE_DIR)
	@echo 'Clean completed: $(PROJECT_NAME).'


# --------- COMPILING AND ASSEMBLING RULES ------------------------------------

$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

$(OUTPUT_DOCS_DIR):
	@mkdir -p $(OUTPUT_DOCS_DIR)

$(OUTPUT_DIR)/%$(C_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(C_SOURCE_SUFFIX) | $(OUTPUT_DIR)
	@echo '   CC        $@'
	@$(CC) $(CCFLAGS) -MD -MP -c -o $@ $<

$(OUTPUT_DIR)/%$(CXX_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(CXX_SOURCE_SUFFIX) | $(OUTPUT_DIR)
	@echo '   CXX       $@'
	@$(CXX) $(CXXFLAGS) -MD -MP -c -o $@ $<

-include $(DEP_FILES_FROM_SOURCE)
