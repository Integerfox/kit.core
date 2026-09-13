# NV Flash Design Review Response

## Question 1: Logical-to-Physical Address Mapping

### Scenario

```
1. User writes 10 bytes (01 02 03 04 05 06 07 08 09 0A) at offset N
2. User writes 2 bytes (B2 C4) at offset N
3. User reads 10 bytes from offset N

Expected result: B2 C4 03 04 05 06 07 08 09 0A
```

### 1a. Physical Flash Offsets

#### Operation A: Initial Write

When the user writes 10 bytes starting at offset N (assume N=0):

```
Logical Request: Write 10 bytes at offset 0
  +-- Maps to -> Logical Page 0 (determined by offset/pageSize)
     +-- Allocated to -> Physical address 0x000000 (first available erased location)
        +-- Stored as -> [Header + Data] = [24 bytes + 256 bytes page] at 0x000000

Physical Flash Layout (after Operation A):
  Address 0x000000-0x000017: Page Header
    - Magic marker (valid page indicator)
    - Sequence number (1)
    - Logical page reference (page 0)
    - CRC for integrity
  Address 0x000018-0x000117: Page Data (256 bytes)
    - [0x01, 0x02, 0x03, ..., 0x0A, 0xFF (padding), ...]
    - Bytes 0-9: User data
    - Bytes 10-255: Erased state (0xFF) or previous data
```

#### Operation B: Second Write (Read-Modify-Write)

When the user writes 2 bytes at the same offset:

```
Logical Request: Write 2 bytes at offset 0
  +-- Maps to -> Logical Page 0 (same page as before)
     +-- Action -> Read existing page, merge new data, write to new location
        +-- Stored as -> [Header + Data] = [24 bytes + 256 bytes page] at 0x000118 (new location)

Physical Flash Layout (after Operation B):

  OLD PAGE (marked obsolete):
    Address 0x000000-0x000017: Page Header
      - Magic marker
      - Sequence number (1)
      - Status: INVALID (marked obsolete)
    Address 0x000018-0x000117: Page Data
      - [0x01, 0x02, 0x03, ..., 0x0A, 0xFF, ...] <- BECOMES OBSOLETE

  NEW PAGE (current valid version):
    Address 0x000118-0x00012F: Page Header
      - Magic marker
      - Sequence number (2) <- Incremented, indicates newer version
      - Status: VALID
    Address 0x000130-0x00022F: Page Data
      - [0xB2, 0xC4, 0x03, 0x04, ..., 0x0A, 0xFF, ...]
      - Bytes 0-1: New data (0xB2, 0xC4)
      - Bytes 2-9: PRESERVED from previous version (0x03-0x0A)
      - Bytes 10-255: Erased or padding
```

**Key Insight:** The implementation does NOT modify the data in place. Instead, it:

- Stores the old version at 0x000000 (marked invalid)
- Stores the new version at 0x000118 (marked valid)
- Allows both versions to coexist in flash until the old is reclaimed

#### Operation C: Read

When the user reads 10 bytes from offset 0:

```
Logical Request: Read 10 bytes from offset 0
  +-- Maps to -> Logical Page 0
     +-- Lookup -> Page Map table: "Page 0 is at physical address 0x000118"
        +-- Read from -> Physical address 0x000130 (0x000118 + header offset)
           +-- Returns -> [0xB2, 0xC4, 0x03, 0x04, ..., 0x0A]
```

---

### 1b. Approach: Logical-to-Physical Translation

We would be using a **three-layer translation mechanism** to bridge the gap:

#### Layer 1: Logical Offset to Logical Page Index

```
User provides: A byte offset within NV storage (0 to nvTotalSize-1)

Translation:
  pageIndex = offset / pageSize
  offsetInPage = offset % pageSize

Example (with pageSize=256):
  offset=0    -> pageIndex=0, offsetInPage=0
  offset=100  -> pageIndex=0, offsetInPage=100
  offset=256  -> pageIndex=1, offsetInPage=0
  offset=257  -> pageIndex=1, offsetInPage=1
```

**Characteristic:** O(1) calculation, no lookup table needed.

#### Layer 2: Logical Page Index to Physical Address (Page Map Indirection)

```
Decision: Maintain an in-memory "page map" array

  m_pageMap[pageIndex] = physicalAddress

Lookup:
  physicalAddr = m_pageMap[pageIndex]

Example:
  m_pageMap[0] = 0x000118  <- Page 0 data is at physical 0x000118
  m_pageMap[1] = 0xFFFFFFFF <- Page 1 has no data yet
  m_pageMap[2] = 0x000230  <- Page 2 data is at physical 0x000230
```

**Characteristic:** O(1) array lookup, fast read performance. Trade-off: Small in-memory overhead (typically 64 bytes for 16 pages).

#### Layer 3: Physical Address to Flash Operations (Read-Modify-Write Pattern)

```
For WRITES:
  1. Logical page index determined (Layer 1)
  2. Physical address looked up (Layer 2)
  3. If page exists: Read current data from that address
  4. Merge new data with existing data (preserving unchanged bytes)
  5. Find new free physical location in flash
  6. Write merged data to new location
  7. Mark old location as obsolete (via header flag)
  8. Update page map to point to new location

For READS:
  1. Logical page index determined (Layer 1)
  2. Physical address looked up (Layer 2) <- Direct lookup, no scanning!
  3. Read from that address
```

#### Key Decision: Read-Modify-Write

Since flash cannot be overwritten in place, on every write operation:

- Preserve all unmodified bytes from the current version
- Write the merged data to a new physical location
- Maintain a sequence number to identify the most recent version

This ensures:

```
a. Byte-level write granularity (user sees EEPROM-like behavior)
b. No loss of unmodified data (bytes 2-9 from Ops A preserved in Op B)
c. Wear leveling (physical locations spread over time)
d. Power-loss tolerance (old and new versions coexist until old is verified obsolete)
```

---

## Question 2: Performance Concerns

### 2a. Startup Scan Time

#### Approach: Building the Page Map

On startup, the implementation would reconstruct the page map by scanning flash:

```
Algorithm Outline:
  1. Scan each physical page in the flash region
  2. For each page with valid header:
     a. Extract logical page reference and sequence number
     b. If this is the first version of this logical page -> Record in page map
     c. If a newer version exists -> Mark older version as obsolete
  3. Identify the highest sequence number (for next writes)
  4. Page map is now ready for fast lookups
```

#### Timing Characteristics

```
Configuration Example (W25Q128):
  - Flash region: 128KB dedicated to NV storage
  - Sector size: 4KB
  - Physical page size: 280 bytes (24-byte header + 256-byte data)
  - Total pages to scan: ~463
  - SPI clock: 10 MHz (typical)

Sequential header read (per page):
  - Command + address: 4 bytes
  - Header: 24 bytes
  - SPI transfer time: (4 + 24) x 8 bits / 10 MHz ~ 22.4 us
  - Overhead: ~5-10 us
  - Total per page: ~30-35 us

Startup scan timeline:
  - First pass (scan headers): 463 pages x 35 us ~ 16 ms
  - CRC validation (valid pages): Depends on write count
  - Sequence number tracking: Minimal overhead

  Total startup time: ~16-25 ms for 128KB region
```

#### Scalability with Flash Size

The key characteristics are:

```
LINEAR SCALING: Startup time scales linearly with flash region size
  - Doubling flash size -> Approximately doubles startup time
  - Why: Sequential scan of all pages is O(totalPages)

DOMINANCE OF SPI READS: The sequential SPI read operations account for
  ~80-90% of total startup time
  - Optimization opportunity: Higher SPI clock speeds reduce this
  - Limitation: We would read headers sequentially

Timeline Examples:
  - 32KB flash:   ~5 ms startup
  - 128KB flash:  ~16-25 ms startup
  - 512KB flash:  ~64-100 ms startup
  - 1MB flash:    ~130-200 ms startup
```

#### Trade-offs

**Problem:** Larger flash region = longer startup.

**Solutions Available** (unimplemented):

1. **Burst Header Reading:** Read multiple headers in one SPI transaction
   - Reduces overhead per page
   - Requires larger buffers
2. **Configurable Scan Windows:** Scan only recently-used regions
   - Faster startup for stable systems
   - Requires tracking of access patterns
3. **Persistent Page Map:** Store page map in flash itself
   - Eliminates startup scan entirely
   - Adds complexity for update coordination
   - Introduces new wear concerns

---

### 2b. Flash Erase Cycles During Writes

**Does the application stall waiting for a flash erase every time it writes data?**

**Proposed Answer: No.** We prevent this through a **two-phase approach**.

#### Approach: Deferred Erasure

```
Phase 1: Find Currently Erased Pages (COMMON PATH)
  When looking for space to write:
    Scan flash for pages that have already been erased
    If found: Use immediately, no erase needed

Phase 2: Erase Sectors Only When Necessary (RARE PATH)
  If no already-erased pages exist:
    Identify candidates for reclamation (sectors with only invalid pages)
    Erase the sector
    Use the freed space
```

We implement this two-phase strategy to defer sector erasure until it becomes necessary.

#### Typical Write Sequence

```
Initial state: W25Q128 with 128KB dedicated to NV
  - Total erased capacity: ~463 physical pages
  - All available for immediate use

Write sequence (representative timeline):

  Write #1-463:    Use pre-erased pages (no erase needed)
                   Each write: ~110 ms (read-modify-write only)

  Write #464:      No erased pages remaining
                   Trigger Phase 2: Erase sector 0
                   One-time cost: sector erase ~215 ms
                   Total: ~325 ms (3x normal)

  Write #465-477:  Use freshly erased pages from sector 0
                   Each write: ~110 ms

  Write #478:      Erase sector 1
                   One-time cost: ~215 ms
                   Total: ~325 ms

  Write #479-491:  Continue with new sector pages
                   Each write: ~110 ms
```

#### Erase Frequency Analysis

```
For 128KB W25Q128 configuration:

Average erase frequency: 1 erase per 10-14 writes

Why not every write?
  - Flash region: 128KB = ~463 physical pages
  - Logical NV size: 4KB = 16 logical pages
  - Over-provisioning factor: 32x (128KB / 4KB)

  Implication:
    - Many physical locations available
    - Sector reclamation is infrequent
    - Only triggered when all erased pages consumed
```

#### Performance Impact When Erase Occurs

```
Normal write (no erase):
  Read existing data:      ~9 ms
  Merge data:              <1 ms
  Write new page:          ~79 ms
  Mark old page invalid:   ~5 ms
  ---------------------------------
  Total:                   ~94-100 ms

Write requiring sector erase:
  Find no free page:       ~17 ms
  Erase sector:            ~215 ms
  Read existing data:      ~9 ms
  Merge data:              <1 ms
  Write new page:          ~79 ms
  Mark old page invalid:   ~5 ms
  ---------------------------------
  Total:                   ~325 ms
```

#### Trade-offs

**Problem:** Sector erase adds significant latency (~215 ms).

**Solutions Available** (unimplemented):

1. **Asynchronous Erase:** Background task handles erasure
   - Application doesn't stall
   - Requires careful coordination
2. **Eager Reclamation:** Proactively erase sectors before needed
   - Spreads erase cost over many writes
   - Requires idle-time detection
3. **Aggressive Over-provisioning:** Allocate more flash
   - 256KB instead of 128KB reduces erase to 1 per 20 writes
   - Trades memory cost for latency
4. **Smaller NV Regions:** Use less total flash
   - Tighter packing, more frequent erases
   - Might be acceptable for low-write-frequency apps

---

## Summary

| Question | Answer |
|----------|--------|
| **How is data stored physically?** | Via read-modify-write: new location each time, old version marked invalid, page map tracks current location |
| **How is logical offset mapped?** | Three layers: (1) offset-to-page calculation, (2) page-to-physical lookup via array, (3) physical-to-flash operations |
| **What about data preservation?** | Read-modify-write preserves unmodified bytes when updating a page |
| **Startup time?** | ~20ms for 128KB (linear scaling with size: +1.4ms per 10KB) |
| **Erase latency?** | No per-write erase; sector erase happens ~1 per 10-14 writes (not blocking) |

By maintaining a small page map in RAM and accepting that writes create new copies rather than modifying in place, we believe we could achieve EEPROM semantics on a SPI NOR flash.
