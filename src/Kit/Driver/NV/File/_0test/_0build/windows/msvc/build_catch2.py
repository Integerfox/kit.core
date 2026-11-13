#!/usr/bin/python3

import os
import sys

# MAIN
if __name__ == '__main__':
    # Get the xpackage root directory from environment variable
    xpackage_root  = os.environ.get('NQBP_XPKGS_ROOT')

    # Run the "real" script
    script = os.path.join( xpackage_root, "nqbp2", "other", "compile_catch2_base.py" )
    cmd = script + " " + " ".join(sys.argv[1:])
    rc = os.system( cmd )
    sys.exit( rc )
