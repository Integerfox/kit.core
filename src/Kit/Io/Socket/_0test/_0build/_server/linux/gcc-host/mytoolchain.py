#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#
# Notes:
#    - ONLY edit/add statements in the sections marked by BEGIN/END EDITS
#      markers.
#    - Maintain indentation level and use spaces (it's a python thing) 
#    - rvalues must be enclosed in quotes (single ' ' or double " ")
#    - The structure/class 'BuildValues' contains (at a minimum the
#      following data members.  Any member not specifically set defaults
#      to null/empty string
#            .inc             # C/C++ search path include directories 
#            .asminc          # Assembly search path include directories
#            .c_only_flags    # C only compiler flags
#            .cflags          # C and C++ compiler flags
#            .cppflags        # C++ only compiler flags
#            .asmflags        # Assembly compiler flags
#            .linkflags       # Linker flags
#            .linklibs        # Linker libraries
#            .firstobjs       # Object files to be unconditionally linked first
#            .lastobjs        # Object files to be unconditionally linked last
#           
#---------------------------------------------------------------------------

# get definition of the Options structure
from nqbplib.base import BuildValues
from nqbplib.my_globals import *
from nqbplib.utils import config_catch2
import os, copy

# Capture project/build directory
prjdir = os.path.dirname(os.path.abspath(__file__))


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'b.out'

# Using Catch2
(catch2_inc, catch2_lib, unit_test_objects) = config_catch2( prjdir, 'linux/gcc-host', 'a' )


# 
# For build config/variant: "posix64" 
#

# Construct option structs
base_posix64      = BuildValues()
optimized_posix64 = BuildValues()
debug_posix64     = BuildValues()

# Set project specific 'base' (i.e always used) options. Note: Catch2 requires C++17 or newer
base_posix64.cflags    = '-m64 -std=c++17 -Wall -Werror -x c++ -fprofile-arcs -ftest-coverage -fprofile-update=atomic'
base_posix64.inc       = catch2_inc
base_posix64.firstobjs = unit_test_objects
base_posix64.linkflags = '-m64 -fprofile-arcs'
base_posix64.linklibs  = f'-lgcov {catch2_lib}'

# Set project specific 'optimized' options
optimized_posix64.cflags    = '-O3'

# Set project specific 'debug' options


#
# For build config/variant: "posix32"
#

# Set project specific 'base' (i.e always used) options. Note: Catch2 requires C++17 or newer
base_posix32           = BuildValues()        # Do NOT comment out this line
base_posix32.cflags    = '-m32 -std=c++17 -Wall -Werror -x c++ -fprofile-arcs -ftest-coverage -fprofile-update=atomic'
base_posix32.inc       = catch2_inc
base_posix32.firstobjs = unit_test_objects
base_posix32.linkflags = '-m32 -fprofile-arcs'
base_posix32.linklibs  = f'-lgcov {catch2_lib}'


# Set project specific 'optimized' options
optimized_posix32           = BuildValues()    # Do NOT comment out this line
optimized_posix32.cflags    = '-O3'

# Set project specific 'debug' options
debug_posix32           = BuildValues()       # Do NOT comment out this line



#-------------------------------------------------
# ONLY edit this section if you are have more than
# ONE build configuration/variant 
#-------------------------------------------------

posix32_opts = { 'user_base':base_posix32, 
                 'user_optimized':optimized_posix32, 
                 'user_debug':debug_posix32
               }
               
               
posix64_opts = { 'user_base':base_posix64, 
                 'user_optimized':optimized_posix64, 
                 'user_debug':debug_posix64
               }
  
        
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'posix':posix32_opts,
                   'posix64':posix64_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

# Select Module that contains the desired toolchain
from nqbplib.toolchains.linux.gcc.console_exe import ToolChain

# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "posix64" )
    return tc 
