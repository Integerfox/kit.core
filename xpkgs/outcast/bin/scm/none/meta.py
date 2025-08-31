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

    # Return 'error' since this is just a stub
    exit(1)
