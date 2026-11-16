#!/usr/bin/python3
"""Invokes NQBP's tca2_base.py script.  The Current working directory is used as the base for all relative paths. """

from __future__ import absolute_import
import os
import sys

# Code that is NOT part of the published code coverage report, i.e. third-party
# code and the unit test code itself.  It is excluded when the coverage data is
# captured so that lcov's data consistency checks are only applied to the KIT
# code that is reported (gcov can generate coverage data for third-party code,
# e.g. ArduinoJson, that lcov rejects as 'inconsistent').
CAPTURE_EXCLUDES = [ '*/xpkgs/*',
                     '*/src/Kit/Json/Arduino.h',
                     '*/_0test/*',
                     '*/_support/*' ]

# MAIN
if __name__ == '__main__':
    # Make sure the environment is properly set
    NQBP_BIN = os.environ.get('NQBP_BIN')
    if ( NQBP_BIN is None ):
        sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
    sys.path.append( NQBP_BIN )

    # Find the Package & Workspace root
    from nqbplib import utils
    utils.set_pkg_and_wrkspace_roots(__file__)
    here = os.getcwd()

    # Find the Package & Workspace root
    from other import tca2_base

    # Apply the excludes ONLY when capturing (the later lcov commands filter the
    # captured data, i.e. the patterns would match nothing - which lcov reports
    # as an 'unused' error)
    capture_metrics = tca2_base.capture_metrics
    def capture_metrics_with_excludes( srcdir, outfile, filter_lines=None ):
        lcov_opts = tca2_base.LCOV_OPTS
        tca2_base.LCOV_OPTS = lcov_opts + ''.join( f" --exclude '{p}'" for p in CAPTURE_EXCLUDES )
        try:
            capture_metrics( srcdir, outfile, filter_lines )
        finally:
            tca2_base.LCOV_OPTS = lcov_opts
    tca2_base.capture_metrics = capture_metrics_with_excludes

    tca2_base.run( here, sys.argv )

