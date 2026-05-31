#!/usr/bin/python3
"""
Script to run unittests (leverages NQBP python code base)

"""

import os
import sys
import shutil
import subprocess

#------------------------------------------------------------------------------
def run_test( subdir ):
    utils.push_dir( subdir )
    
    print( f"Running unit test dir (in Test Declaration order): {subdir} ..." )
    p = subprocess.Popen( f'b.exe --order decl', shell=True )
    p.communicate()
    if ( p.returncode != 0 ):
        exit( "FAILED test" )
    utils.pop_dir()


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

if ( os.path.isdir( '_win32' ) ):
    run_test( '_win32' )
    print()
