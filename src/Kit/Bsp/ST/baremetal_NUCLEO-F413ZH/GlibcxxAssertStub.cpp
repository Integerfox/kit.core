/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 
 * 
 * Workaround for GCC 14.x libstdc++ bug where std::atomic operations
 * call __glibcxx_assert_fail even when _GLIBCXX_ASSERTIONS=0 is defined.
 * 
 * This function is not provided by newlib (embedded C library), causing
 * linker errors with "Unknown destination type (ARM/Thumb)" on embedded targets.
 * 
 * This stub prevents the linker error. Since _GLIBCXX_ASSERTIONS=0 is set,
 * this function should never actually be called at runtime, but it must
 * exist for linking to succeed.
 */

 namespace std {

// Stub implementation for missing GCC 14.x libstdc++ assertion function
// Mark with ARM/Thumb attributes for embedded targets
__attribute__((used))
void __glibcxx_assert_fail(const char* /* file */, 
                          int /* line */,
                          const char* /* function */, 
                          const char* /* condition */)
{
    // If this is ever called, it indicates a serious bug in libstdc++
    // since _GLIBCXX_ASSERTIONS=0 should disable all assertion checks.
    // On embedded systems without exception support, trigger a fault.
    volatile int* crash = nullptr;
    *crash = 0;
    
    // Infinite loop as fallback if memory write doesn't fault
    while(1) {}
}

} // namespace std
