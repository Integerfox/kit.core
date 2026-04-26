# Kit::Driver::Flash::W25Q
@brief namespace description for Kit::Driver::Flash::W25Q
@namespace Kit::Driver::Flash::W25Q @brief

The W25Q namespace provides a concrete implementation of the Flash::IApi
interface for Winbond W25Q series SPI NOR flash devices.  Supports W25Q16
through W25Q128 variants.  The implementation handles page-boundary crossing
for write operations and polls the BUSY status after each write/erase.
