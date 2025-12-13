#!/usr/bin/python3
import os, sys
if __name__ == '__main__':
	NQBP_BIN = os.environ.get('NQBP_BIN')
	if ( NQBP_BIN is None ):
	    sys.exit( "ERROR: The environment variable NQBP_BIN is not set!" )
	sys.path.append( NQBP_BIN )
	from nqbplib import utils
	utils.set_pkg_and_wrkspace_roots(__file__)
	import mytoolchain
	from nqbplib import mk
	mk.build( sys.argv, mytoolchain.create() )
