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
#            .inc 
#            .asminc
#            .cflags
#            .cppflags
#            .asmflags
#            .linkflags
#            .linklibs
#           
#---------------------------------------------------------------------------

# get definition of the Options structure
from nqbplib.base import BuildValues
from nqbplib.my_globals import *
from nqbplib.utils import catch2_inc, catch2_lib
import os, copy

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'a.exe'

# Link unittest directory by object module so that Catch's self-registration mechanism 'works'
unit_test_objects = '_BUILT_DIR_.src/Kit/Container/_0test'

# Use Catch2 as a static library
my_build_dir = r'windows\msvc'
catch2_inc   = catch2_inc()
catch2_lib   = catch2_lib( my_build_dir ) + '.lib'
NQBP_PRE_PROCESS_SCRIPT( 'build_catch2.py' )
NQBP_PRE_PROCESS_SCRIPT_ARGS( my_build_dir )


#
# For build config/variant: "win32" 
#

# Set project specific 'base' (i.e always used) options
base_win32           = BuildValues()        # Do NOT comment out this line
base_win32.cflags    = '/W3 /WX /EHsc '  # /EHsc enables exceptions /std:c++17
base_win32.firstobjs = unit_test_objects
base_win32.inc       = catch2_inc
base_win32.linklibs  = catch2_lib

# Set project specific 'optimized' options
optimized_win32          = BuildValues()    # Do NOT comment out this line
optimized_win32.cflags   = '/O2'

# Set project specific 'debug' options
debug_win32          = BuildValues()       # Do NOT comment out this line
debug_win32.cflags   = '/D "KIT_DEBUG"'


#-------------------------------------------------
# ONLY edit this section if you are ADDING options
# for build configurations/variants OTHER than the
# 'release' build
#-------------------------------------------------

win32_build_opts = { 'user_base':base_win32, 
                     'user_optimized':optimized_win32, 
                     'user_debug':debug_win32
                   }
               
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'win32':win32_build_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))

# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.vc12.console_exe import ToolChain

# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, 'win32' )
    return tc 
