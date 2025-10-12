This directory contains the Catch2 user configuration header file for the
[Catch2 framework](https://github.com/catchorg/Catch2).  This header is typically is created when the Catch2 framework is installed on a host.  However the KIT library does not 'install' the Catch2 framework, rather it builds the library on demand.  This avoid host architecture
and toolchain compatibility issues with respect to Catch2 and KIT unit test
binaries.