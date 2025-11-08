""
import os, sys
import utils
from docopt.docopt import docopt
import scm.copy 
import scm.git.mount

#---------------------------------------------------------------------------------------------------------
def display_summary():
    scm.copy.display_summary()

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(scm.copy.USAGE, argv=cmd_argv)

    # The '--force' option does an recursive clone
    clone_args = ''
    if ( args['--force'] ):
        clone_args = '--recurse-submodules'

    # -b option is not supported/needed
    if ( args['-b'] != None ):
        sys.exit( "The '-b' option is not supported/needed.  Use a 'remote-ref' as the <id> argument" )

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # Make sure the destination directory exists
    dst = os.path.join( os.getcwd(), args['<dst>'] )
    utils.print_verbose( f"Destination for the copy: {dst}" )
    utils.mkdirs( dst )

    # Create a clone of the repo
    # NOTE: I hate cloning the entire repo - but I have not found a way to get JUST a snapshot by a remote-ref
    cmd = f'git clone {clone_args} --branch {args["<id>"]} --depth=1 {args["<origin>"]}/{args["<repo>"]}.git {pkg}'
    utils.push_dir( dst )
    t = utils.run_shell( cmd, common_args['-v'] )
    if ( utils.is_error(t) ):   # Clean-up attempted cloned dir if there was failure
        utils.remove_tree( {args["<repo>"]} ) 
    utils.pop_dir()    
    utils.check_results( t, f"ERROR: Failed the retreive/clone the specified package/repository. Note: the <id> ({args['<id>']}) MUST be a git TAG." )

    # Remove the .git directory since this is a non-tracked copy
    gitdir = os.path.join( dst, pkg, ".git" )
    utils.remove_tree( gitdir, warn_msg="Not able to remove the .git directory for local copy" )