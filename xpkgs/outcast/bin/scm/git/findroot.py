import os
import utils
from docopt.docopt import docopt
import scm.findroot

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.findroot.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.findroot.USAGE, argv=cmd_argv)

    # Update the 'subtree'
    cmd = f'git rev-parse --show-toplevel'
    t = utils.run_shell( cmd, common_args['-v'] )
    utils.check_results( t, "ERROR: Failed find to find the root directory of local repository." )
    print( utils.standardize_dir_sep(t[1]) )
