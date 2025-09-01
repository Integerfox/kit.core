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

    # Success Msg
    if ( args['get-success-msg'] ):
        print( "Repo mounted and committed to your repo" )
        return

    # Error Msg
    if ( args['get-error-msg'] ):
        print( "" ) # No message
        return

    # Check if there are pending repo changes
    cmd = f'git diff-index HEAD --exit-code --quiet'
    t = utils.run_shell( cmd, False )
    cmd = f'git diff-index --cached HEAD --exit-code --quiet'
    t2 = utils.run_shell( cmd, False )
    utils.check_results( t,  "ERROR: Your local repo has pending tree modification (i.e. need to do a commit/revert)." )
    utils.check_results( t2, "ERROR: Your local repo has pending index modification (i.e. need to do a commit/revert)." )

    # -b option is not supported/needed
    if ( args['-b'] != None ):
        sys.exit( "The '-b' option is not supported/needed.  Use a 'remote-ref' as the <id> argument" )

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # Make sure the Parent destination directory exists
    dst = args['<dst>'] 
    utils.mkdirs( dst )
    
    # Set directory for the subtree directory
    dst = os.path.join( dst, pkg )
    dst = utils.force_unix_dir_sep(dst)
    utils.print_verbose( f"Destination for the copy: {dst}" )

    # Create a 'subtree'
    cmd = f'git subtree add --prefix {dst} {args["<origin>"]}/{args["<repo>"]}.git {args["<id>"]} --squash'
    t = utils.run_shell( cmd, common_args['-v'] )
    if ( utils.is_error(t) ):   # Clean-up dst dir if there was failure
        utils.remove_tree( dst ) 
    utils.check_results( t, "ERROR: Failed to create a subtree for the specified package/repository." )
