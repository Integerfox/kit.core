import os
import utils
from docopt.docopt import docopt
import scm.mount 

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.mount.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.mount.USAGE, argv=cmd_argv)

    # Return 'error' since this is just a stub
    exit(1)
