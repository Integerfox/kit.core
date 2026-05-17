# Kit::Driver::NV::Flash
@brief namespace description for Kit::Driver::NV::Flash
@namespace Kit::Driver::NV::Flash @brief

The Flash namespace provides a concrete implementation of the NV::IApi
interface backed by SPI NOR flash storage.  The implementation features:

- Wear leveling via log-structured NV Record rotation
- In-memory page map for O(1) read lookup
- Read-modify-write to preserve unmodified bytes
- CRC32 integrity checking on record headers
- Global sequence numbering for write ordering
- Two-phase sector reclamation (scan-then-erase)

Supports configurable NV page sizes and total storage sizes.  See the
design documentation in docs/manual/nvflash/ for detailed descriptions
of the NV Record structure, sector layout, and wear leveling algorithms.
