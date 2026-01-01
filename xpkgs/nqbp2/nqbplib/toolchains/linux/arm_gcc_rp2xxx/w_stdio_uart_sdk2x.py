#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   gcc-arm-none-eabi 
#   Target:     Raspberry Pi Pico and Pico 2 (RP2040/RP2350)
#   Output:     .BIN|ELF|UF2 file
#   SDK:        pico-sdk v2.x
#
# TODO: FIXME: Need to run pioasm tool to generate the cyw43_bus_pio_spi.pio.h
#              header file.  Currently is prebuilt and included in the BSP.                  
#------------------------------------------------------------------------------

import sys, os
from nqbplib import base
from nqbplib import utils
from nqbplib import my_globals


class ToolChain( base.ToolChain ):

    #--------------------------------------------------------------------------
    def __init__( self, exename, prjdir, build_variants, abs_repo_root, bsp_rel_path, 
                  mcu_part_num, board, abs_sdk_root, default_variant, linker_script,
                  env_error=None ):

        mcu_part_num = mcu_part_num.lower()
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname     = 'GCC Arm-Cortex (none-eabi) Compiler'
        self._cc         = 'arm-none-eabi-gcc' 
        self._asm        = 'arm-none-eabi-gcc' 
        self._ld         = 'arm-none-eabi-gcc' 
        self._ar         = 'arm-none-eabi-ar' 
        self._objcpy     = 'arm-none-eabi-objcopy' 
        self._objdmp     = 'arm-none-eabi-objdump' 
        self._printsz    = 'arm-none-eabi-size'
        self._pad_chksum = os.path.join( abs_sdk_root, 'src', mcu_part_num, 'boot_stage2', "pad_checksum" )
        self._picotool   = 'picotool' # FIXME: os.path.join( sdk_root, 'tools', 'picotool' )
        self._pioasm     = 'pioasm'   # FIXME: os.path.join( sdk_root, 'tools', 'pioasm' )
        self._asm_ext    = 'asm'    
        self._asm_ext2   = 'S'   

        self._clean_pkg_dirs.extend( ['_pico'] )
        self._base_release.exclude_clangd.extend(['-std=gnu11', '-mcpu=cortex-m33', '-mcmse', '-march=armv8-m.main+fp+dsp', '-mcpu=cortex-m0plus'])

        # Define paths
        bsp_src_path = os.path.join( abs_repo_root, bsp_rel_path )  # The BSP directory must contain a pico/ sub-directory with the board config_autogen.h & version.h files
        sdk_src_path = os.path.join( abs_sdk_root, 'src' )
        sdk_lib_path = os.path.join( abs_sdk_root, 'lib' )
        self._base_release.inc = self._base_release.inc + \
                ' -I' + sdk_lib_path + '/cyw43-driver/src' + \
                ' -I' + sdk_lib_path + '/cyw43-driver/firmware' + \
                ' -I' + bsp_src_path + \
                ' -I' + sdk_src_path + '/boards/include' + \
                ' -I' + sdk_src_path + '/common/boot_picobin_headers/include' + \
                ' -I' + sdk_src_path + '/common/boot_picoboot_headers/include' + \
                ' -I' + sdk_src_path + '/common/hardware_claim/include' + \
                ' -I' + sdk_src_path + '/common/pico_base_headers/include' + \
                ' -I' + sdk_src_path + '/common/pico_binary_info/include' + \
                ' -I' + sdk_src_path + '/common/pico_bit_ops_headers/include' + \
                ' -I' + sdk_src_path + '/common/pico_divider_headers/include' + \
                ' -I' + sdk_src_path + '/common/pico_stdlib_headers/include' + \
                ' -I' + sdk_src_path + '/common/pico_sync/include' + \
                ' -I' + sdk_src_path + '/common/pico_time/include' + \
                ' -I' + sdk_src_path + '/common/pico_util/include' + \
                ' -I' + sdk_src_path + '/rp2_common/boot_bootrom_headers/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_adc/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_base/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_boot_lock/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_clocks/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_dcp/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_divider/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_dma/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_exception/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_flash/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_gpio/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_hazard3/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_i2c/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_irq/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_pio/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_pll/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_pwm/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_rcp/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_resets/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_riscv/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_spi/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_sync/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_sync_spin_lock/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_ticks/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_timer/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_uart/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_vreg/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_watchdog/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_xip_cache/include' + \
                ' -I' + sdk_src_path + '/rp2_common/hardware_xosc/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_async_context/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_atomic/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_bootrom/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_cyw43_arch/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_cyw43_driver/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_driver/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_double/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_flash/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_float/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_int64_ops/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_malloc/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_mem_ops/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_multicore/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_platform_common/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_platform_compiler/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_platform_panic/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_platform_sections/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_printf/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_rand/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_runtime/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_runtime_init/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_status_led/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_stdio/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_stdio_uart/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_time_adapter/include' + \
                ' -I' + sdk_src_path + '/rp2_common/pico_unique_id/include ' + \
                ' -I' + sdk_src_path + f'/{mcu_part_num}/boot_stage2/include' + \
                ' -I' + sdk_src_path + f'/{mcu_part_num}/hardware_regs/include' + \
                ' -I' + sdk_src_path + f'/{mcu_part_num}/hardware_structs/include' + \
                ' -I' + sdk_src_path + f'/{mcu_part_num}/pico_platform/include'

        
        #
        skd_libopts = '-DLIB_BOOT_STAGE2_HEADERS=1' + \
            ' -DLIB_PICO_ATOMIC=1' + \
            ' -DLIB_PICO_BIT_OPS=1' + \
            ' -DLIB_PICO_BIT_OPS_PICO=1' + \
            ' -DLIB_PICO_CLIB_INTERFACE=1' + \
            ' -DLIB_PICO_CRT0=1' + \
            ' -DLIB_PICO_CXX_OPTIONS=1' + \
            ' -DLIB_PICO_DIVIDER=1' + \
            ' -DLIB_PICO_DIVIDER_COMPILER=1' + \
            ' -DLIB_PICO_DOUBLE=1' + \
            ' -DLIB_PICO_DOUBLE_PICO=1' + \
            ' -DLIB_PICO_FLASH=1' + \
            ' -DLIB_PICO_FLOAT=1' + \
            ' -DLIB_PICO_FLOAT_PICO=1' + \
            ' -DLIB_PICO_FLOAT_PICO_VFP=1' + \
            ' -DLIB_PICO_INT64_OPS=1' + \
            ' -DLIB_PICO_INT64_OPS_COMPILER=1' + \
            ' -DLIB_PICO_MALLOC=1' + \
            ' -DLIB_PICO_MEM_OPS=1' + \
            ' -DLIB_PICO_MEM_OPS_COMPILER=1' + \
            ' -DLIB_PICO_NEWLIB_INTERFACE=1' + \
            ' -DLIB_PICO_PLATFORM=1' + \
            ' -DLIB_PICO_PLATFORM_COMMON=1' + \
            ' -DLIB_PICO_PLATFORM_COMPILER=1' + \
            ' -DLIB_PICO_PLATFORM_PANIC=1' + \
            ' -DLIB_PICO_PLATFORM_SECTIONS=1' + \
            ' -DLIB_PICO_PRINTF=1' + \
            ' -DLIB_PICO_PRINTF_PICO=1' + \
            ' -DLIB_PICO_RUNTIME=1' + \
            ' -DLIB_PICO_RUNTIME_INIT=1' + \
            ' -DLIB_PICO_STANDARD_BINARY_INFO=1' + \
            ' -DLIB_PICO_STANDARD_LINK=1' + \
            ' -DLIB_PICO_STDIO=1' + \
            ' -DLIB_PICO_STDLIB=1' + \
            ' -DLIB_PICO_SYNC=1' + \
            ' -DLIB_PICO_SYNC_CRITICAL_SECTION=1' + \
            ' -DLIB_PICO_SYNC_MUTEX=1' + \
            ' -DLIB_PICO_SYNC_SEM=1' + \
            ' -DLIB_PICO_TIME=1' + \
            ' -DLIB_PICO_TIME_ADAPTER=1' + \
            ' -DLIB_PICO_UNIQUE_ID=1' + \
            ' -DLIB_PICO_UTIL=1' + \
            ' -DLIB_PICO_TIME_ADAPTER=1' + \
            ' -DLIB_PICO_STDIO_UART=1' + \
            ' -DLIB_PICO_STDIO_USB=0'
            

        # RP2040 Specific additions 
        sdkboot = os.path.join( abs_sdk_root, 'src', mcu_part_num, 'boot_stage2' )
        mcu      = '-mcpu=cortex-m0plus'
        platform = '-DPICO_RP2040=1 -DPICO_PLATFORM=rp2040'
        self._uf2_family = 'rp2040'
        if mcu_part_num == 'rp2040':
            pass
            
        # RP2350 Specific additions
        elif mcu_part_num.lower() == 'rp2350':
            mcu      = '-mcpu=cortex-m33 '
            platform = '-mcmse -march=armv8-m.main+fp+dsp -DPICO_RP2350=1 -DPICO_PLATFORM=rp2350'
            self._uf2_family = 'rp2350-arm-s'

        # Error
        else:
            raise Exception( f"Unsupported MCU type '{mcu}' specified for ARM GCC RP2xxx toolchain." )  
        
        #
        common_flags                    = f' -O3 -mthumb -ffunction-sections -fdata-sections -Wno-array-bounds -Wno-stringop-truncation'
        common_flags                    = f' {common_flags} -DPICO_TARGET_NAME=\\"{exename}\\" -DPICO_BOARD=\\"{board}\\"'
        common_flags                    = f' {mcu} {common_flags} {skd_libopts}'
        self._base_release.cflags       = f' {self._base_release.cflags} {common_flags}'
        self._base_release.c_only_flags = f' {self._base_release.c_only_flags} -std=gnu11'
        common_cpp_flags                = ' -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess -fno-threadsafe-statics -fno-rtti -fno-exceptions'
        common_cpp_flags                = f' {common_cpp_flags} -fno-unwind-tables -fno-use-cxa-atexit -Wno-restrict -Wno-address-of-packed-member -Wno-class-memaccess'
        self._base_release.cppflags     = f' {self._base_release.cppflags} {common_cpp_flags}'
        self._base_release.asmflags     = self._base_release.cflags 
        self._base_release.asminc       = f' {self._base_release.asminc} {self._base_release.inc} -I {sdk_src_path}/{mcu_part_num}/boot_stage2/asminclude'
        
        wrapper_funcs                   = ' -Wl,--wrap=__ctzdi2 -Wl,--wrap=__aeabi_dadd -Wl,--wrap=__aeabi_ddiv -Wl,--wrap=__aeabi_dmul' + \
                                          ' -Wl,--wrap=__aeabi_drsub -Wl,--wrap=__aeabi_dsub -Wl,--wrap=__aeabi_cdcmpeq -Wl,--wrap=__aeabi_cdrcmple' + \
                                          ' -Wl,--wrap=__aeabi_cdcmple -Wl,--wrap=__aeabi_dcmpeq -Wl,--wrap=__aeabi_dcmplt -Wl,--wrap=__aeabi_dcmple' + \
                                          ' -Wl,--wrap=__aeabi_dcmpge -Wl,--wrap=__aeabi_dcmpgt -Wl,--wrap=__aeabi_dcmpun -Wl,--wrap=__aeabi_i2d' + \
                                          ' -Wl,--wrap=__aeabi_l2d -Wl,--wrap=__aeabi_ui2d -Wl,--wrap=__aeabi_ul2d -Wl,--wrap=__aeabi_d2iz' + \
                                          ' -Wl,--wrap=__aeabi_d2lz -Wl,--wrap=__aeabi_d2uiz -Wl,--wrap=__aeabi_d2ulz -Wl,--wrap=__aeabi_d2f' + \
                                          ' -Wl,--wrap=sqrt -Wl,--wrap=cos -Wl,--wrap=sin -Wl,--wrap=tan -Wl,--wrap=atan2 -Wl,--wrap=exp -Wl,--wrap=log' + \
                                          ' -Wl,--wrap=ldexp -Wl,--wrap=copysign -Wl,--wrap=trunc -Wl,--wrap=floor -Wl,--wrap=ceil -Wl,--wrap=round' + \
                                          ' -Wl,--wrap=sincos -Wl,--wrap=asin -Wl,--wrap=acos -Wl,--wrap=atan -Wl,--wrap=sinh -Wl,--wrap=cosh' + \
                                          ' -Wl,--wrap=tanh -Wl,--wrap=asinh -Wl,--wrap=acosh -Wl,--wrap=atanh -Wl,--wrap=exp2 -Wl,--wrap=log2' + \
                                          ' -Wl,--wrap=exp10 -Wl,--wrap=log10 -Wl,--wrap=pow -Wl,--wrap=powint -Wl,--wrap=hypot -Wl,--wrap=cbrt' + \
                                          ' -Wl,--wrap=fmod -Wl,--wrap=drem -Wl,--wrap=remainder -Wl,--wrap=remquo -Wl,--wrap=expm1 -Wl,--wrap=log1p' + \
                                          ' -Wl,--wrap=fma -Wl,--wrap=__aeabi_l2f -Wl,--wrap=__aeabi_ul2f -Wl,--wrap=__aeabi_f2lz -Wl,--wrap=__aeabi_f2ulz' + \
                                          ' -Wl,--wrap=cosf -Wl,--wrap=sinf -Wl,--wrap=tanf -Wl,--wrap=atan2f -Wl,--wrap=expf -Wl,--wrap=logf' + \
                                          ' -Wl,--wrap=sincosf -Wl,--wrap=ldexpf -Wl,--wrap=copysignf -Wl,--wrap=truncf -Wl,--wrap=floorf -Wl,--wrap=ceilf' + \
                                          ' -Wl,--wrap=roundf -Wl,--wrap=asinf -Wl,--wrap=acosf -Wl,--wrap=atanf -Wl,--wrap=sinhf -Wl,--wrap=coshf' + \
                                          ' -Wl,--wrap=tanhf -Wl,--wrap=asinhf -Wl,--wrap=acoshf -Wl,--wrap=atanhf -Wl,--wrap=exp2f -Wl,--wrap=log2f' + \
                                          ' -Wl,--wrap=exp10f -Wl,--wrap=log10f -Wl,--wrap=powf -Wl,--wrap=powintf -Wl,--wrap=hypotf -Wl,--wrap=cbrtf' + \
                                          ' -Wl,--wrap=fmodf -Wl,--wrap=dremf -Wl,--wrap=remainderf -Wl,--wrap=remquof -Wl,--wrap=expm1f -Wl,--wrap=log1pf' + \
                                          ' -Wl,--wrap=fmaf -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=free' + \
                                          ' -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsnprintf -Wl,--wrap=printf -Wl,--wrap=vprintf' + \
                                          ' -Wl,--wrap=puts -Wl,--wrap=putchar -Wl,--wrap=getchar'
        self._base_release.linkflags    = f' -nostartfiles {mcu} -mthumb -Wl,--build-id=none --specs=nosys.specs -Wl,-z,max-page-size=4096 -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit -Wl,--gc-sections {wrapper_funcs} -Wl,-L{bsp_src_path} -Wl,--script={linker_script} bs2_default_padded_checksummed.S -Wl,--no-warn-rwx-segments -Wl,-Map={exename}.elf.map' 
                                        
        boot_linker_script              = os.path.join( abs_sdk_root, "src", mcu_part_num, "boot_stage2", "boot_stage2.ld" )
        self._boot_obj                  = f'xpkgs/pico-sdk/src/{mcu_part_num}/boot_stage2/compile_time_choice.o'
        self._bootloader_link_flags     = f'{mcu} -O3 -DNDEBUG -Wl,--build-id=none --specs=nosys.specs -nostartfiles -Wl,--script={boot_linker_script} -Wl,-Map=bs2_default.elf.map'   
        

        # 
        # CRITICAL: pico_runtime_init and pico_unique_id MUST be linked as object files (not from libraries)
        # because they contain __attribute__((constructor)) functions in .preinit_array sections.
        # When linked from static libraries, the linker excludes these objects because constructors
        # have no explicit symbol references - causing hardware initialization to be skipped.
        self._base_release.firstobjs    = f' _BUILT_DIR_.xpkgs/pico-sdk/src/rp2_common/pico_crt0' + \
                                          f' _BUILT_DIR_.xpkgs/pico-sdk/src/{mcu_part_num}/pico_platform' + \
                                          f' _BUILT_DIR_.xpkgs/pico-sdk/src/rp2_common/pico_runtime_init' + \
                                          f' _BUILT_DIR_.xpkgs/pico-sdk/src/rp2_common/pico_unique_id'


        # Optimized options, flags, etc.
        self._optimized_release.cflags     = self._optimized_release.cflags + r' -DNDEBUG -DPICO_CMAKE_BUILD_TYPE=\"Release\"'
        self._optimized_release.linkflags  = self._optimized_release.linkflags + ' -DNDEBUG'

        # Debug options, flags, etc.
        self._debug_release.cflags     = self._debug_release.cflags + r' -DDEBUG -DPICO_CMAKE_BUILD_TYPE=\"Debug\"'
        self._debug_release.linkflags  = self._debug_release.linkflags + ' -DDEBUG'
        

   #--------------------------------------------------------------------------
    def link( self, arguments, inf, local_external_setting, variant ):
        # Finish creating the second state boot loader
        self._ninja_writer.build(
               outputs    = 'bs2_default.elf',
               rule       = 'generic_cmd',
               inputs     = self._boot_obj ,
               variables  = {"generic_cmd":self._ld, "generic_cmd_opts":self._bootloader_link_flags, "generic_cmd_opts_out":'-o'} )
        self._ninja_writer.newline()
        self._ninja_writer.build(
               outputs    = 'bs2_default.bin',
               rule       = 'objcpy_rule',
               inputs     = 'bs2_default.elf' ,
               variables  = {"objcpy_opts":'-O binary'} )
        self._ninja_writer.newline()
        self._ninja_writer.build(
               outputs    = 'bs2_default.dis',
               rule       = 'objdmp_2stage_rule',
               inputs     = 'bs2_default.elf',
               variables  = {"objdmp_opts1":'-h', "objdmp_opts2":'-d'} )
        self._ninja_writer.newline()
        self._ninja_writer.build(
               outputs    = 'bs2_default_padded_checksummed.S',
               rule       = 'generic_cmd',
               inputs     = 'bs2_default.bin' ,
               variables  = {"generic_cmd":f'python {self._pad_chksum}', "generic_cmd_opts":"-s 0xffffffff"} )
        self._ninja_writer.newline()

        # Run the linker
        base.ToolChain.link(self, arguments, inf, local_external_setting, variant, ['bs2_default_padded_checksummed.S'], outname=self._final_output_name + ".elf" )


        # Generate the .BIN file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".bin",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf" ,
               variables  = {"objcpy_opts":'-O binary'} )
        self._ninja_writer.newline()

        # Generate the .HEX file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".hex",
               rule       = 'objcpy_rule',
               inputs     = self._final_output_name + ".elf",
               variables  = {"objcpy_opts":'-O ihex'} )
        self._ninja_writer.newline()
 
        # Generate disassembly file
        self._ninja_writer.build(
               outputs    = self._final_output_name + ".dis",
               rule       = 'objdmp_2stage_rule',
               inputs     = self._final_output_name + ".elf",
               variables  = {"objdmp_opts1":'-h', "objdmp_opts2":'-d'} )
        self._ninja_writer.newline()

        # FIXME: Need a solution to host specific picotool executable
        #        The workaround is to use picotool to load ELF files into the board directly (e.g. picotool load -fx myprog.elf)
        # Generate the UF2file
        # self._ninja_writer.rule( 
        #     name = 'picotool_uf2', 
        #     command = f'$shell {self._picotool} uf2 convert --quiet $in $out --family {self._uf2_family} --abs-block', 
        #     description = "Creating UF2: $out" )
        # self._ninja_writer.build(
        #        outputs    = self._final_output_name + ".uf2",
        #        rule       = 'picotool_uf2',
        #        inputs     = self._final_output_name + ".elf" )
        # self._ninja_writer.newline()

        # Run the 'size' command
        self._ninja_writer.rule( 
            name = 'print_size', 
            command = f'$shell {self._printsz} --format=berkeley {self._final_output_name+ ".elf"}', 
            description = "Generic Command: $cmd" )
        self._create_always_build_statments( "print_size", "dummy_printsize", impilicit_list=self._final_output_name+ ".elf" )
 
        return None
 
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        # FIXME: # self._ninja_writer.default( [self._final_output_name + ".uf2", self._final_output_name + ".dis", "bs2_default.dis", "dummy_printsize_final"] )
        self._ninja_writer.default( [self._final_output_name + ".dis", "bs2_default.dis", "dummy_printsize_final"] )


    #--------------------------------------------------------------------------
    def get_asm_extensions(self):
        extlist = [ self._asm_ext, self._asm_ext2 ]
        return extlist