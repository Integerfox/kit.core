#---------------------------------------------------------------------------
# This python module is used to customize a supported toolchain for your 
# project specific settings.
#---------------------------------------------------------------------------

from nqbplib.base import BuildValues
from nqbplib.my_globals import *
import os, copy 

#===================================================
# BEGIN EDITS/CUSTOMIZATIONS
#---------------------------------------------------

FINAL_OUTPUT_NAME = 'hw_wdogenable'

prj_dir       = os.path.dirname(os.path.abspath(__file__))
bsp_path      = os.path.join( "src", "Kit", "Bsp", "ST", "NUCLEO-F413ZH" )
bsp_mx        = os.path.join( bsp_path, "MX" )
sdk_root      = os.path.join( NQBP_PKG_ROOT(), "xpkgs", "stm32f4-sdk")
bsp_mx_root   = os.path.join( NQBP_PKG_ROOT(), bsp_mx )
freertos_root = os.path.join( NQBP_PKG_ROOT(), "xpkgs", "freertos-v10")
sysview_root  = os.path.join( NQBP_PKG_ROOT(), bsp_path, "SeggerSysView" )

base_release = BuildValues()
target_flags             = '-DUSE_STM32F4XX_NUCLEO_144 -DSTM32F413xx -DHAL_IWDG_MODULE_ENABLED'
base_release.cflags      = f' -Wall {target_flags} -Werror -DENABLE_BSP_SEGGER_SYSVIEW -I{sysview_root}'
base_release.cppflags    = ' -std=c++11 -Wno-int-in-bool-context'
base_release.asmflags    = f' {target_flags}'
base_release.firstobjs   = f'_BUILT_DIR_.{bsp_mx}/Core/Src'
base_release.firstobjs   = base_release.firstobjs + f' {bsp_mx}/../stdio.o'

optimized_release = BuildValues()
debug_release = BuildValues()

release_opts = { 'user_base':base_release, 
                 'user_optimized':optimized_release, 
                 'user_debug':debug_release
               }

build_variants = { 'stm32':release_opts,
                 }  

#---------------------------------------------------
# END EDITS/CUSTOMIZATIONS
#===================================================

from nqbplib.toolchains.windows.arm_gcc_stm32.stm32F4 import ToolChain

def create():
    lscript  = 'STM32F413ZHTx_FLASH.ld'
    tc = ToolChain( FINAL_OUTPUT_NAME, prj_dir, build_variants, sdk_root, bsp_mx_root, freertos_root, lscript, "stm32" )
    return tc
