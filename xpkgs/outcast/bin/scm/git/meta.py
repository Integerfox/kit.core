import os
import utils
from docopt.docopt import docopt
import scm.meta

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.meta.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.meta.USAGE, argv=cmd_argv)

    # List tag(s)
    if args['tag']:
        cmd = f'git ls-remote --refs --tags {args['<origin>']}/{args['<repo>']}'
        t = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, "ERROR: Failed to list the tags." )
        for line in t[1].splitlines():
            if args['<tfilter>'] != None:
                if args['<tfilter>'] in line:
                    print( line )
            else:
                print( line )
