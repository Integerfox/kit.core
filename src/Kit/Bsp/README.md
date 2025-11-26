# Kit::Bsp
@brief namespace description for Kit::Bsp
@namespace Kit::Bsp @brief

The Bsp namespace is the root name space for microcontroller Board Support Packages (BSPs).

BSP are responsible for encapsuling the MCU data sheet, board schematic, 
cross compiler, and MCU Vendor supplied SDK/generated code.  In BSP are/required:

- Support a the common/generic interface defined in this root namespace (i.e
  `Api.h`)
- In general BSP are not intended to be reused across multiple physical boards.
  In fact each revision of a PCBA - that impact on firmware behavior - should
  have its own BSP instance in the namespace.

NOTE: A microcontroller BSP is similar in concept to a Linux BSP, but a
      microcontroller BSP is significantly different in practice.  The BSPs in
      this namespace should be as *small/thin* as possible and do not contain
      any application business logic.  In addition, BSPs should not contain
      *drivers* per say, but provide the lowest level lego blocks for building
      drivers.  For example, do **not** put an SPI or I2C driver implementation
      in your BSP.  Instead put under the `src/Kit/Driver` tree.  It is okay to
      have Hardware/Chip specific drivers in the Driver tree - but these drivers
      must go through BSP interfaces for access HW Registers, etc. (i.e. can
      not contain any MCU/compiler specific code).  

TODO: Add link to future blog article on the *Indirect BSP* pattern
