#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#---------------------------------------------------------------------------

# get definition of the Options structure
from nqbplib.base import BuildValues
from nqbplib.my_globals import *
import os

# Capture project/build directory
prjdir = os.path.dirname(os.path.abspath(__file__))

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'a.exe'


#
# For build config/variant: "win32"
#

# Construct option structs
base_win32      = BuildValues()
optimized_win32 = BuildValues()
debug_win32     = BuildValues()

# Set 'base' options
base_win32.cflags     = '-m32 -std=c++11 -Wall -Werror -x c++ -D_CRT_SECURE_NO_WARNINGS'
base_win32.linkflags  = '-m32'

# Set 'Optimized' options
optimized_win32.cflags    = '-O3'

# Set 'debug' options


#-------------------------------------------------
# ONLY edit this section if you are have more than
# ONE build configuration/variant 
#-------------------------------------------------

win32_opts = { 'user_base':base_win32,
               'user_optimized':optimized_win32,
               'user_debug':debug_win32
             }
               
build_variants = { 'win32':win32_opts,
                 }    


#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.clang_msvc.console_exe import ToolChain


# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, "win32" )
    return tc 
