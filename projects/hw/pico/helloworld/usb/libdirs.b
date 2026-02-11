# USB stdio
xpkgs/pico-sdk/src/rp2_common/pico_stdio_usb

# TinyUSB support
xpkgs/pico-sdk/lib/tinyusb/src
xpkgs/pico-sdk/lib/tinyusb/src/common < tusb_fifo.c
xpkgs/pico-sdk/lib/tinyusb/src/device < usbd.c usbd_control.c
xpkgs/pico-sdk/lib/tinyusb/src/class/cdc < cdc_device.c
xpkgs/pico-sdk/lib/tinyusb/src/class/vendor < vendor_device.c
xpkgs/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040 < dcd_rp2040.c rp2040_usb.c
