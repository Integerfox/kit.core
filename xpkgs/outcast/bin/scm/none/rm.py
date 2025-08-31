import os
import utils
from docopt.docopt import docopt
import scm.rm 

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.rm.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.rm.USAGE, argv=cmd_argv)

    # Return 'error' since this is just a stub
    exit(1)
