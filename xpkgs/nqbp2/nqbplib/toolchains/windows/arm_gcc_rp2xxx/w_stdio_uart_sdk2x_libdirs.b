# UART stdio
xpkgs/pico-sdk/src/rp2_common/pico_stdio_uart

# SDK-Multicore
xpkgs/pico-sdk/src/rp2_common/pico_multicore

# Board specific
xpkgs/pico-sdk/src/rp2_common/pico_status_led

# RP2350 specific 
xpkgs/pico-sdk/src/rp2350/pico_platform
xpkgs/pico-sdk/src/rp2350/boot_stage2 < compile_time_choice.S
xpkgs/pico-sdk/src/rp2_common/hardware_divider < divider.c
xpkgs/pico-sdk/src/rp2_common/pico_divider < divider_compiler.c
xpkgs/pico-sdk/src/rp2_common/pico_double < double_math.c double_conv_m33.S double_sci_m33.S double_aeabi_dcp.S double_fma_dcp.S
xpkgs/pico-sdk/src/rp2_common/pico_float < float_math.c float_conv32_vfp.S float_sci_m33.S float_common_m33.S

# Core SDK
xpkgs/pico-sdk/src/rp2_common/pico_crt0 < crt0.S
xpkgs/pico-sdk/src/rp2_common/pico_clib_interface < newlib_interface.c
xpkgs/pico-sdk/src/common/pico_sync
xpkgs/pico-sdk/src/common/pico_time
xpkgs/pico-sdk/src/common/pico_util
xpkgs/pico-sdk/src/common/hardware_claim
xpkgs/pico-sdk/src/rp2_common/pico_stdlib
xpkgs/pico-sdk/src/rp2_common/hardware_gpio
xpkgs/pico-sdk/src/rp2_common/pico_platform_common
xpkgs/pico-sdk/src/rp2_common/pico_platform_compiler
xpkgs/pico-sdk/src/rp2_common/pico_platform_panic
xpkgs/pico-sdk/src/rp2_common/pico_platform_sections
xpkgs/pico-sdk/src/rp2_common/hardware_sync
xpkgs/pico-sdk/src/rp2_common/hardware_sync_spin_lock
xpkgs/pico-sdk/src/rp2_common/hardware_irq
xpkgs/pico-sdk/src/rp2_common/hardware_spi
xpkgs/pico-sdk/src/rp2_common/hardware_dma
xpkgs/pico-sdk/src/rp2_common/hardware_timer
xpkgs/pico-sdk/src/rp2_common/hardware_ticks
xpkgs/pico-sdk/src/rp2_common/hardware_uart
xpkgs/pico-sdk/src/rp2_common/hardware_pio

xpkgs/pico-sdk/src/rp2_common/pico_async_context < async_context_threadsafe_background.c async_context_base.c
xpkgs/pico-sdk/src/rp2_common/pico_runtime
xpkgs/pico-sdk/src/rp2_common/pico_runtime_init
xpkgs/pico-sdk/src/rp2_common/pico_atomic
xpkgs/pico-sdk/src/rp2_common/pico_standard_binary_info
xpkgs/pico-sdk/src/rp2_common/hardware_flash
xpkgs/pico-sdk/src/rp2_common/hardware_xip_cache
xpkgs/pico-sdk/src/rp2_common/hardware_clocks
xpkgs/pico-sdk/src/rp2_common/hardware_pll
xpkgs/pico-sdk/src/rp2_common/hardware_vreg
xpkgs/pico-sdk/src/rp2_common/hardware_watchdog
xpkgs/pico-sdk/src/rp2_common/hardware_xosc
xpkgs/pico-sdk/src/rp2_common/hardware_boot_lock
xpkgs/pico-sdk/src/rp2_common/pico_time_adapter
xpkgs/pico-sdk/src/rp2_common/pico_printf < printf.c
xpkgs/pico-sdk/src/rp2_common/pico_bit_ops
xpkgs/pico-sdk/src/rp2_common/pico_bootrom
xpkgs/pico-sdk/src/rp2_common/pico_int64_ops
xpkgs/pico-sdk/src/rp2_common/pico_malloc
xpkgs/pico-sdk/src/rp2_common/pico_mem_ops
xpkgs/pico-sdk/src/rp2_common/pico_standard_link
xpkgs/pico-sdk/src/rp2_common/pico_stdio
xpkgs/pico-sdk/src/rp2_common/pico_unique_id
xpkgs/pico-sdk/src/rp2_common/pico_cxx_options

# Minimum CYW43 support (e.g. access to the LED)
xpkgs/pico-sdk/src/rp2_common/pico_cyw43_arch < cyw43_arch.c cyw43_arch_threadsafe_background.c
xpkgs/pico-sdk/src/rp2_common/pico_cyw43_driver > btstack_chipset_cyw43.c btstack_cyw43.c btstack_hci_transport_cyw43.c wifi_firmware.S 
xpkgs/pico-sdk/lib/cyw43-driver/src < cyw43_ll.c cyw43_ctrl.c