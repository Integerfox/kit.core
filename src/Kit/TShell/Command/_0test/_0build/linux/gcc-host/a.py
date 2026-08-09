#!/usr/bin/python3
"""
Script to run unittests (leverages NQBP python code base)

"""

import filecmp
import os
import sys
import shutil
import subprocess

#------------------------------------------------------------------------------
def run_test( subdir ):
    utils.push_dir( subdir )
    basesrc = os.path.join( my_globals.NQBP_PKG_ROOT(), 'src', 'Kit', 'TShell', 'Command', '_0test' )
    src     = os.path.join( basesrc, 'testinput.txt' )
    golden  = os.path.join( basesrc, 'expectedoutput.txt' )
    shutil.copy( src, '.' ); 
    print( "Running unit test dir: {} ...".format( subdir ) )
    p = subprocess.Popen( f'cat {src}| b.out > out.txt', shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        utils.pop_dir()
        exit( "FAILED test" )
    post_process_output( golden )
    print( "ALL Tests passed." )
    utils.pop_dir()
        

def post_process_output( golden ):
    # Filter out line with runtime specific values
    # NOTE: We open with newline='' and then write with '\n' to ensure the 
    #       output file always has Unix-style line endings (\n), which 
    #       matches the 'golden' file checked into the repo.
    with open( 'out.txt', 'r', newline='' ) as inf:
        with open( 'temp.out.txt', "w", newline='\n') as outf:  
            for line in inf:
                if ( 'LAST CHECKED LINE' in line ):
                    break
                outf.write( line.rstrip('\r\n') + '\n' )

    if ( not filecmp.cmp(golden,'temp.out.txt') ):
        utils.pop_dir()
        exit( "FAILED test - output does NOT match the expected values" )


#------------------------------------------------------------------------------
# Make sure the environment is properly set
NQBP_BIN = os.environ.get('NQBP_BIN')
if ( NQBP_BIN is None ):
    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
sys.path.append( NQBP_BIN )

# Find the Package & Workspace root
from nqbplib import utils
from nqbplib import my_globals
utils.set_pkg_and_wrkspace_roots(__file__)


#------------------------------------------------------------------------------
# MAIN ENTRY POINT....
#

if ( os.path.isdir( '_posix' ) ):
    run_test( '_posix' )
    print()

if ( os.path.isdir( '_posix64' ) ):
    run_test( '_posix64' )
    print()
