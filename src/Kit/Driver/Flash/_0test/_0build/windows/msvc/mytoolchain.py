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

# Set project specific 'base' (i.e always used) options. Note: Catch2 requires C++17 or newer
base_win32           = BuildValues()        # Do NOT comment out this line
base_win32.cflags    = '/W3 /WX /EHsc '  # /EHsc enables exceptions /std:c++17

# Set project specific 'optimized' options
optimized_win32          = BuildValues()    # Do NOT comment out this line
optimized_win32.cflags   = '/O2'

# Set project specific 'debug' options
debug_win32          = BuildValues()       # Do NOT comment out this line
debug_win32.cflags   = '/D "KIT_DEBUG"'


#-------------------------------------------------
# ONLY edit this section if you are have more than
# ONE build configuration/variant 
#-------------------------------------------------

win32_build_opts = { 'user_base':base_win32, 
                     'user_optimized':optimized_win32, 
                     'user_debug':debug_win32
                   }
               
build_variants = { 'win32':win32_build_opts,
                 }    

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================


# Select Module that contains the desired toolchain
from nqbplib.toolchains.windows.vc12.console_exe import ToolChain

# Function that instantiates an instance of the toolchain
def create():
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, 'win32' )
    return tc 
