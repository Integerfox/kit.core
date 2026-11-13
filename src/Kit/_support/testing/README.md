# Unit Testing Support {#unit_testing_support}

\brief Unit testing specific infrastructure.

This directory (`src/Kit/_support/_testing`) contains helper classes, KIT library
specific reusable 'mocks', and other functionality useful for developing and
running unit tests.

| File | Contents |
|---|---|
| `helpers.h` | Generic helpers, e.g. `waitForCondition()`, Model Point observer mocks, `TestEventQueueServer` (an event-queue *thread* that can be frozen/thawed), and `AsyncOpenClose` |
| `ManualEventQueue.h` | `ManualEventQueue` - an Event Queue that is run by the test thread (no event-queue thread), so tests of event-queue servers are deterministic: no sleeps, no polling. Includes `requestOpen()`/`requestClose()`/`postAndRun()` replacements for synchronous ITC calls, and a virtual clock for software timers (`advanceTime()`). See the header for usage and examples. |
