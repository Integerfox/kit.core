import os, sys
import utils
from docopt.docopt import docopt
import scm.umount

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.umount.display_summary()
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.umount.USAGE, argv=cmd_argv)

    # Success Msg
    if ( args['get-success-msg'] ):
        print( "Repo unmount.  You will need to perform a 'git add/rm' to remove the deleted files" )
        return

    # Error Msg
    if ( args['get-error-msg'] ):
        print( "" ) # No addition info
        return

    # -b option is not supported/needed
    if ( args['-b'] != None ):
        sys.exit( "The '-b' option is not supported/needed.  Use a 'remote-ref' as the <id> argument" )

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # Set the foreign package directory to be deleted
    dst = os.path.join( args['<dst>'] , pkg )
    if ( not os.path.isdir(dst) ):
        sys.exit( f"ERROR: The Package/Directory - {dst} - does not exist." )
    utils.print_verbose( f"Package/directory being removed: {dst}" )

    # The is no 'git subtree rm' command -->we just simply delete the package directory
    utils.set_tree_readonly( dst, False )
    utils.remove_tree( dst )
