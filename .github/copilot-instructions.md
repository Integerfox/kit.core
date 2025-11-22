# Kit.Core C++ Library - AI Coding Assistant Instructions

## Architecture Overview

Kit.Core is a **platform-agnostic embedded C++ middleware library** designed for microcontroller applications. Key architectural principles:

- **Zero dynamic allocation** after startup - uses intrusive containers and static memory management
- **Platform abstraction** through BSP (Board Support Package) layers enabling the same code to run on simulators, Linux, Windows, and microcontrollers
- **Message-passing ITC** (Inter-Thread Communication) using ownership semantics instead of mutex-heavy designs
- **Layered namespace structure**: `Kit::System`, `Kit::Container`, `Kit::Itc`, `Kit::EventQueue`, `Kit::Bsp`, etc.

## Build System (NQBP)

Uses **NQBP2** build system (not CMake/Make). Key files and patterns:

- **Environment setup**: Always run `. env.sh <compiler>` first (sets NQBP_* variables)
- **Build files**: 
  - `libdirs.b` - defines library dependencies and include paths
  - `sources.b` - explicitly lists source files (optional, auto-discovered otherwise) 
  - Platform-specific builds under `_0build/<os>/<compiler>/`
- **Commands**:
  - `nqbp.py` - builds individual executables (applications or unit tests)
  - `bob -v` - CI helper that recursively finds directories with `nqbp.py` scripts and launches them with provided arguments
  - `chuck` - CI helper that recursively finds and executes unit tests
  - `tca2.py` - test coverage analysis
  - `vcc` - view code coverage in browser

## Development Workflows

### Setting Up Environment
```bash
cd kit.core
. env.sh          # List available compilers
. env.sh 1        # Set GCC host compiler
```

### Building and Testing
```bash
cd src/Kit/<Module>/_0test/_0build/linux/gcc-host
nqbp.py          # Build unit test executable
nqbp.py -g       # Build unit test with debug symbols
_posix/a.out     # Run the unit test executable. Example assumes POSIX target
vcc              # Executes the tca2.py to generate code coverage metrics and then launch the browser to view the metrics
```

### Running Specific Tests
Navigate to `src/Kit/<Module>/_0test/_0build/<platform>/<compiler>/` and run `nqbp.py`.

## Code Conventions

### Memory Management
- **No `new`/`delete`** after initialization
- Most containers use **intrusive linking** - items must inherit from `Kit::Container::Item`
  - Items can only exist in **one container at a time**
- Static memory pools along with placement new can be used for dynamic-like allocation

### Header Structure & LHeader/LConfig Patterns
```cpp
#include "kit_config.h"  // Always first (so all subsequent headers see this) - LConfig pattern
#include "kit_map.h"     // LHeader pattern  


/// Namespace structure
namespace Kit {
namespace ModuleName {
class ClassName {
    // Implementation
};
}}
```

**LHeader Pattern (`kit_map.h`)**: Provides compile-time interface resolution without `#ifdef` blocks:
- **No header latch** in `kit_map.h` files (intentional for cyclic dependency prevention)
- Maps platform-specific implementations: `#define KitSystemMutex_T_MAP pthread_mutex_t` 
- Different per build directory: `_0build/<platform>/<compiler>/kit_map.h`
- Examples: `Kit/System/Posix/mappings.h`, `Kit/System/FreeRTOS/mappings.h`

**LConfig Pattern (`kit_config.h`)**: Project-specific configuration overrides:
- Controls conditional compilation: `#ifdef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY`
- Sets magic constants: `#define OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD`

### ITC Message Patterns
- **Client-server model** with ownership transfer
- Messages inherit from `Kit::Itc::Message` types
- **Async/sync variants**: Use `AsyncReturnHandler` or `SyncReturnHandler`
- Clients own payloads → send transfers ownership → server processes → return transfers back

### Testing Patterns
- Tests in `_0test/` directories use **Catch2** framework
- Test files: `*.cpp` (no separate `_test.cpp` suffix)
- Mock implementations in `src/Kit/_support/`
- Hardware loopback tests in `_0test/_hw_*` subdirectories

## Key Components

### Containers (`Kit::Container`)
Intrusive, zero-allocation containers:
- `SList<T>`, `DList<T>` - linked lists.  Intrusive linkage.
- `Map<KEY,ITEM>` - key-value mapping. Intrusive linkage. 
- `RingBuffer<T>` - circular buffer. Contains copies of items.
- All items placed in Intrusive containers must inherit from `Kit::Container::Item`

### Event System (`Kit::EventQueue`)
- `Server` - processes all events, this include software timers, event flags, ITC messages and data model change notifications
  - For application that do not use Data Model change notifications - use the Compile-time configuration via `USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY` not to include Data Model change notification processing in the event loops
- Event loops can be extended with a periodic scheduler


### Platform Abstraction (`Kit::System`, `Kit::Bsp`)
- **`Kit::System`** - OSAL (Operating System Abstraction Layer) for underlying OS (threads, mutexes, timers, watchdog) and bare-metal targets
- **HAL (Hardware Abstraction Layer)** - Component-specific hardware access in two forms:
  - **Explicit HAL**: Component defines its own HAL interface (no current examples in Kit library)
  - **Indirect HAL**: Uses BSP-provided interfaces (e.g., `Kit/Io/Serial/ST/M232F4/StreamDriver.h` uses auto-generated MX files)
- **`Kit::Bsp`** - Board Support Packages encapsulating:
  - Microcontroller datasheet specifics
  - Target board schematic details  
  - Compiler toolchain configuration, pragmas, and keywords
  - MCU vendor SDK integration (e.g., ST CubeMX auto-generated files in `MX/` directories)
- **BSP Principles**:
  - Only code specific to a particular compiler
  - Independent of applications (applications depend on BSP via LHeader pattern)
  - Minimal business rules because BSPs are unique to a given target board
  - Each board revision requires its own BSP
- Platform implementations: `Posix/`, `Win32/`, `FreeRTOS/`, `ST/` (STM32)

## External Dependencies

Managed via **Outcast** package manager (see `pkg.info/package.json`):
- `nqbp2` - build system
- `catch2` - unit testing framework  
- `freertos-v10` - RTOS (when not bare-metal)
- `stm32f4-sdk` - STM32 hardware support

## Common Gotchas

- **Include order matters**: `kit_config.h` must be first, followed by `kit_map.h`
- **LHeader pattern**: `kit_map.h` has **no header latch** (prevents cyclic dependencies)
- **Build-specific mappings**: Each `_0build/<platform>/<compiler>/` directory has its own `kit_map.h`
- **Environment not set**: Run `. env.sh <compiler>` before building
- **Container ownership**: For intrusive containers - items can't be in multiple containers simultaneously
- **Platform paths**: Use forward slashes in `libdirs.b` even on Windows
- **Unit Test Code**: Source code for unit tests are placed in the `_0test` directories
- **UnitTest Build directories**: The build directory for each unit test (manual or automated) reside in a `_0build` directory under the `_0test` directory of each module

## Documentation Generation

- **Doxygen**: `top/run_doxygen.py` generates API docs. The script must be run from the `top/` directory. The output will be placed in `docs/publish/doxygen/`.
- **Coverage**: `tca2.py` generates test coverage reports
- **Docs location**: The non-source documentation for the Doxygen output is located in the `docs/manual/` directory.