#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   Clang (LLVM-MinGW)
#   Target:     Windows
#   Output:     64 bit Console application
#------------------------------------------------------------------------------

import sys
from nqbplib import base
from nqbplib import utils


class ToolChain( base.ToolChain ):
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        base.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
        self._ccname = 'Clang (Windows)'
        self._cc     = 'clang'
        self._ld     = 'clang'
        self._ar     = 'llvm-ar'
        
        # more stuff to clean
        self._clean_list.extend( ['xml'] )

        # Configure for MSVC-compatible linker when using Clang on Windows
        # This handles the case where Clang uses MSVC as the backend
        self._linker_libgroup_start = ''
        self._linker_libgroup_end   = ''
        
        # Use MSVC-style libraries when Clang is using MSVC backend
        self._base_release.linklibs = ''
        
        # Turn off ALL optimization on the debug build
        self._debug_release.cflags    = self._debug_release.cflags + ' -O0'
        self._debug_release.linkflags = self._debug_release.linkflags + ' ' + self._debug_release.cflags
 
    #--------------------------------------------------------------------------
    def validate_cc( self ):
       t = base.ToolChain.validate_cc(self)
       if ( not '64' in t[1].decode() ):
           utils.output( "ERROR: Incorrect build of Clang (target does NOT equal 64Bit version)" )
           sys.exit(1)

       return t

    #--------------------------------------------------------------------------
    def _build_ar_rule( self ):
        self._win32_build_ar_rule()
