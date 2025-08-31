"""
 
Displays information about the specified package
===============================================================================
usage: orc [common-opts] cat [options] 
       orc [common-opts] cat [options] <adoptedpkg>

Arguments:
    <adoptedpkg>        Name of an adopted package to inspect
    
Options:
    --indent NUM        Number of spaces when indenting JSON output.
                        [Default: 2]
    -v                  Enable verbose output
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o If  <adoptedpkg> is not specified, then the repository's package 
      information is displayed.
    
"""
import os, sys
import utils
from docopt.docopt import docopt
from my_globals import PACKAGE_INFO_DIR
from my_globals import OVERLAY_PKGS_DIR
from my_globals import PACKAGE_FILE


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'cat', 'Displays information for a package' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Display my package info
    if ( not args['<adoptedpkg>'] ):
        utils.cat_package_file(int(args['--indent']), verbose=args['-v'])

    # Display an adopted package
    else:
        # Check if the adopted package is actually adopted
        json_dict = utils.load_package_file()
        pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, args['<adoptedpkg>'] )
        if ( pkgobj == None ):
            sys.exit( f"ERROR: The package - {args['<adoptedpkg>']} is NOT an adopted package" )

        # OVERLAY package
        if ( pkgobj['pkgtype'] == 'overlay' ):
            utils.cat_package_file( int(args['--indent']), path=os.path.join( OVERLAY_PKGS_DIR(), args['<adoptedpkg>'], PACKAGE_INFO_DIR() ), verbose=args['-v'] )

        # Readonly/Foreign Packages
        else:
            if ( pkgobj['parentDir'] == None ):
                sys.exit( f"ERROR: the {PACKAGE_FILE()} file is corrupt. There is no parent directory for the package: {args['<adoptedpkg>']}" )
            json_dict = utils.cat_package_file( int(args['--indent']), path=os.path.join( pkgobj['parentDir'], args['<adoptedpkg>'], PACKAGE_INFO_DIR() ), verbose=args['-v'] )
            if ( json_dict == None ):
                sys.exit( f"ERROR: No package information is available for the Readonly/Foreign package: {args['<adoptedpkg>']}" )



        

