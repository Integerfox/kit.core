#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#
# Notes:
#    - ONLY edit/add statements in the sections marked by BEGIN/END EDITS
#      markers.
#    - Maintain indentation level and use spaces (it's a python thing) 
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
import os


#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

# Set the name for the final output item
FINAL_OUTPUT_NAME = 'hello_uart'

# magic paths
sdk_root     = os.path.join( NQBP_PKG_ROOT(), "xpkgs", "pico-sdk" )
bsp_rel_root = os.path.join( "src", "Kit", "Bsp", "RPi", "baremetal_arm_pico2w" )
linkerscript = os.path.join( sdk_root, 'src', 'rp2_common', 'pico_crt0', 'rp2350', 'memmap_default.ld')

# SDK Build options
sdk_opt = ' -DPICO_32BIT=1' + \
          ' -DPICO_BUILD=1' + \
          ' -DPICO_ON_DEVICE=1' + \
          ' -DPICO_COPY_TO_RAM=0' + \
          ' -DPICO_USE_BLOCKED_RAM=0' + \
          ' -DPICO_CXX_ENABLE_EXCEPTIONS=0' + \
          ' -DPICO_NO_FLASH=0' + \
          ' -DPICO_NO_HARDWARE=0' + \
          ' -DPICO_CXX_ENABLE_EXCEPTIONS=0'

# Wifi build options
wifi_opt = ' -DPICO_CYW43_SUPPORTED=1' + \
           ' -DLIB_PICO_CYW43_ARCH=1' + \
           ' -DPICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1' + \
           ' -DLIB_PICO_ASYNC_CONTEXT_THREADSAFE_BACKGROUND=1' + \
           ' -DCYW43_LWIP=0'


# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
common_flags             = f' {wifi_opt} {sdk_opt}'
base_release.cflags      = f' -Wall {common_flags}'
base_release.cppflags    = ' -std=gnu++11'
base_release.asmflags    = f' {common_flags}'

# Set project specific 'optimized' options
optimized_release = BuildValues()    # Do NOT comment out this line

# Set project specific 'debug' options
debug_release = BuildValues()       # Do NOT comment out this line

#-------------------------------------------------
# ONLY edit this section if you are have more than
# ONE build configuration/variant 
#-------------------------------------------------

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimized_release, 
                 'user_debug':debug_release
               }
               

               
# Add new variant option dictionary to # dictionary of 
# build variants
build_variants = { 'pico':release_opts,
                 }  

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

# Capture project/build directory
import os
prjdir = os.path.dirname(os.path.abspath(__file__))


# Select Module that contains the desired toolchain
from nqbplib.toolchains.linux.arm_gcc_rp2xxx.w_stdio_uart_sdk2x import ToolChain

# Function that instantiates an instance of the toolchain
def create():
    lscript  = 'STM32F413ZHTx_FLASH.ld'
    tc = ToolChain( FINAL_OUTPUT_NAME, prjdir, build_variants, NQBP_PKG_ROOT(), bsp_rel_root, "rp2350", "pico2_w", sdk_root, "pico", linkerscript )
    return tc
