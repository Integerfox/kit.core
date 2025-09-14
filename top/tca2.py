#!/usr/bin/python3
"""Invokes NQBP's tca2_base.py script.  The Current working directory is used as the base for all relative paths. """

from __future__ import absolute_import
import os
import sys

# MAIN
if __name__ == '__main__':
    # Make sure the environment is properly set
    NQBP_BIN = os.environ.get('NQBP_BIN')
    if ( NQBP_BIN == None ):
        sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
    sys.path.append( NQBP_BIN )

    # Find the Package & Workspace root
    from nqbplib import utils
    utils.set_pkg_and_wrkspace_roots(__file__)
    here = os.getcwd()

    # Find the Package & Workspace root
    from other import tca2_base
    tca2_base.run( here, sys.argv )

