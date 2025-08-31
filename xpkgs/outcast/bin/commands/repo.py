"""
 
Collection of operations performed directly on a package's repository
===============================================================================
usage: orc [common-opts] repo [options] <repo> list-tag [<tfilter>]
       orc [common-opts] repo [options] <repo> <origin> list-tag [<tfilter>]
       

Arguments:
    <repo>           Name of the targeted repository
    <origin>         Path/URL to the repository (Used when the <repo> has not 
                     yet been adopted)
    <tfilter>        Tag filter, e.g. OUTCAST2
    
Options:
    -v                  Enable verbose output
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
"""
import os, sys
import utils
from docopt.docopt import docopt
from my_globals import PACKAGE_INFO_DIR
from my_globals import OVERLAY_PKGS_DIR
from my_globals import PACKAGE_FILE


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'repo', "Operations performed directly on a package repository" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Verbose option for subcommand
    vopt = ' -v ' if common_args['-v'] else ''

    # Display tag information
    if args['list-tag']:
        if args['<origin>'] == None:
            json_dict = utils.load_package_file()
            pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, args['<repo>'] )
            args['<origin>']        = pkgobj['repo']['origin']

        cmd = f"evie.py {vopt} --scm {common_args['--scm']} meta {args['<repo>']} {args['<origin>']} tag"
        if args['<tfilter>']:
            cmd += f" {args['<tfilter>']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to retrieve the tag(s)"  )
        print( t[1] )


        

