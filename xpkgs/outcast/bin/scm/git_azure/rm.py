""
import os, sys
import utils
from docopt.docopt import docopt
import scm.rm 
import scm.git.umount

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.rm.display_summary()

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.rm.USAGE, argv=cmd_argv)

    # Use the umount command so as to have consistent pre/post GIT behavior with adopting non-integrated packages
    cmd_argv[0] = 'umount'
    scm.git.umount.run( common_args, cmd_argv )
