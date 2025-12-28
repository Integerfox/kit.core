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
FINAL_OUTPUT_NAME = 'hw_basic2'

# Path to SDK and the ST CubeMX generated BSP files
prj_dir       = os.path.dirname(os.path.abspath(__file__))
bsp_path      = os.path.join( "src", "Kit", "Bsp", "ST", "freertos_NUCLEO-F413ZH" )
bsp_mx        = os.path.join( bsp_path, "MX" )
sdk_root      = os.path.join( NQBP_PKG_ROOT(), "xpkgs", "stm32f4-sdk")
bsp_mx_root   = os.path.join( NQBP_PKG_ROOT(), bsp_mx )
freertos_root = os.path.join( NQBP_PKG_ROOT(), "xpkgs", "freertos-v10")
sysview_root  = os.path.join( NQBP_PKG_ROOT(), bsp_path, "SeggerSysView" )

#
# For build config/variant: 
#

# Set project specific 'base' (i.e always used) options
base_release = BuildValues()        # Do NOT comment out this line
target_flags             = '-DUSE_STM32F4XX_NUCLEO_144 -DSTM32F413xx'
base_release.cflags      = f' -Wall {target_flags} -Werror -DENABLE_BSP_SEGGER_SYSVIEW -I{sysview_root}'
base_release.cppflags    = ' -std=c++11 -Wno-int-in-bool-context'
base_release.asmflags    = f' {target_flags}'
base_release.linkflags   = '-Wl,--no-warn-rwx-segments'
base_release.firstobjs   = f'_BUILT_DIR_.{bsp_mx}/Core/Src'

# Set project specific 'optimized' options
optimized_release = BuildValues()    # Do NOT comment out this line

# Set project specific 'debug' options
debug_release = BuildValues()       # Do NOT comment out this line
#debug_release.cflags = '-D_MY_APP_DEBUG_SWITCH_'


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
build_variants = { 'stm32':release_opts,
                 }  

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================


# Select Module that contains the desired toolchain
from nqbplib.toolchains.linux.arm_gcc_stm32.stm32F4 import ToolChain

# Function that instantiates an instance of the toolchain
def create():
    lscript  = 'STM32F413ZHTx_FLASH.ld'
    tc = ToolChain( FINAL_OUTPUT_NAME, prj_dir, build_variants, sdk_root, bsp_mx_root, freertos_root, lscript, "stm32" )
    return tc

