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
from nqbplib.utils import catch2_inc
import os, copy

# Capture project/build directory
prjdir = os.path.dirname(os.path.abspath(__file__))

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'catch2'

catch2_inc  = catch2_inc()


#
# For build config/variant: "win32" 
#

# Set project specific 'base' (i.e always used) options
base_win32           = BuildValues()        # Do NOT comment out this line
base_win32.cflags    = '/W3 /WX /EHsc '
base_win32.inc       = catch2_inc


# Set project specific 'optimized' options
optimized_win32          = BuildValues()    # Do NOT comment out this line
optimized_win32.cflags   = '/O2'

# Set project specific 'debug' options
debug_win32          = BuildValues()       # Do NOT comment out this line
debug_win32.cflags   = '/D "_MY_APP_DEBUG_SWITCH_"'


#-------------------------------------------------
# ONLY edit this section if you are have more than
# ONE build configuration/variant 
#-------------------------------------------------

win32_opts = { 'user_base':base_win32, 
               'user_optimized':optimized_win32, 
               'user_debug':debug_win32
             }
               
               
# build variants
build_variants = { 'win32':win32_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.vc12.static_lib import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, 'win32' )
    return tc 
