# Kit::Driver::Flash::W25Q
@brief namespace description for Kit::Driver::Flash::W25Q
@namespace Kit::Driver::Flash::W25Q @brief

The W25Q namespace provides concrete implementations of the Flash::IApi
interface for Winbond W25Q series SPI NOR flash devices.  The StdSpi24 driver
supports standard (single-lane) SPI operation with 3-byte (24-bit) addressing
for W25Q16 through W25Q128 variants.  The implementation handles page-boundary
crossing for write operations and polls the BUSY status after each write/erase.
Additional drivers (e.g. QSPI or 4-byte addressing) can co-exist in this
directory.
