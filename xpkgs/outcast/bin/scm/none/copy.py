import os
import utils
from docopt.docopt import docopt
import scm.copy 

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.copy.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.copy.USAGE, argv=cmd_argv)

    # Return 'error' since this is just a stub
    exit(1)
