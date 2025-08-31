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

    # Return 'error' since this is just a stub
    exit(1)
