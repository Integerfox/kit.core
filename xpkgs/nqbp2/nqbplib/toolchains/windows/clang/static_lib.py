#------------------------------------------------------------------------------
# TOOLCHAIN
#
#   Host:       Windows
#   Compiler:   Clang (LLVM-MinGW)
#   Target:     Windows
#   Output:     Static Library
#------------------------------------------------------------------------------

import sys
from nqbplib import utils
from . import console_exe

class ToolChain( console_exe.ToolChain ): #base.ToolChain ):
    def __init__( self, exename, prjdir, build_variants, default_variant='release' ):
        console_exe.ToolChain.__init__( self, exename, prjdir, build_variants, default_variant )
 
    #--------------------------------------------------------------------------
    def link( self, arguments, builtlibs, local_external_setting, variant ):

        # Build list of libraries
        libs = []
        for item in builtlibs:
            libs.append( item[0] )

        # Generate ninja build statement to merge libraries using MRI script
        # (instead of thin archive which doesn't work with ld.lld)
        self._ninja_writer.build( 
            outputs = self._final_output_name + ".a",
            rule = 'armerge',
            inputs = libs,
            variables = {"arout":self._final_output_name + ".a"} )
        self._ninja_writer.newline()
        return None
 
    def finalize( self, arguments, builtlibs, objfiles, local_external_setting, linkout=None ):
        self._ninja_writer.default( self._final_output_name + ".a" )

    #--------------------------------------------------------------------------
    def _build_arlibs_rule( self ):
        # Build rule to merge multiple .a files into a single flat archive using MRI script
        # Ninja will execute this command for each library merge
        # The $in variable contains all input .a files, $arout is the output file
        # Note: In ninja rules, % is literal (not doubled), $$ escapes to $
        mri_cmd = '(echo CREATE $arout && for %L in ($in) do @echo ADDLIB %L && echo SAVE && echo END) | $ar -M'
        self._ninja_writer.rule(
            name = 'armerge',
            command = f'cmd.exe /C "$rm $arout 1>nul 2>nul & {mri_cmd}"',
            description = "Creating Library: $out"
        )
        self._ninja_writer.newline()
