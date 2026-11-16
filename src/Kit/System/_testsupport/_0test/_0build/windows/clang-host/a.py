#!/usr/bin/python3
"""
Script to run unittests (leverages NQBP python code base)

The test application is run once per scenario.  Each run prints its expected
exit code (EXPECTED_EXIT_CODE=<n>) and then forces the application to exit via
Kit::System::ShutdownUnitTesting.  The test passes when the actual exit code
matches the expected exit code (and is NOT the 'success' exit code).
"""

import os
import re
import sys
import subprocess

EXE_NAME  = 'b.exe'
SCENARIOS = [ 'failure', 'success', 'setexitcode' ]

#------------------------------------------------------------------------------
def run_test( subdir ):
    utils.push_dir( subdir )
    print( "Running unit test dir: {} ...".format( subdir ) )
    for scenario in SCENARIOS:
        p = subprocess.run( [ os.path.join( '.', EXE_NAME ), scenario ], capture_output=True, text=True )
        m = re.search( r'EXPECTED_EXIT_CODE=(-?\d+)', p.stdout )
        if ( m is None ):
            utils.pop_dir()
            exit( f"FAILED test - scenario '{scenario}': no expected exit code (exit code={p.returncode})\n{p.stdout}{p.stderr}" )
        expected = int( m.group( 1 ) )
        if ( p.returncode != expected or p.returncode == 0 ):
            utils.pop_dir()
            exit( f"FAILED test - scenario '{scenario}': exit code={p.returncode}, expected={expected}" )
        print( f"  {scenario}: exit code={p.returncode} (expected={expected})" )

    print( "ALL Tests passed." )
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

if ( os.path.isdir( '_win64' ) ):
    run_test( '_win64' )
    print()
