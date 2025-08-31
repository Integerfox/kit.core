import os
import utils
from docopt.docopt import docopt
import scm.umount 

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.umount.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.umount.USAGE, argv=cmd_argv)

    # Return 'error' since this is just a stub
    exit(1)
