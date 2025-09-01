"""
 
Updates a previously mounted repository
===============================================================================
usage: evie [common-opts] update [options] <dst> <repo> <origin> <id>

Arguments:
    <dst>            PARENT directory for where the copy-to-be-updated is
                     located.  The directory is specified as a relative path to
                     the root of primary repository.<pkg>            
    <repo>           Name of the repository to update
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be updated
    
Options:
    -p PKGNAME       Specifies the Package name if different from the <repo> 
                     name
    -b BRANCH        Specifies the source branch in <repo>.  The use/need
                     of this option in dependent on the <repo> SCM type.
                        
Options:
    -h, --help          Display help for this command

    
Notes:
    o The command MUST be run in the root of the primary respostiory.

"""
import os
import utils
from docopt.docopt import docopt
import scm.update

#------------------------------------------------------------------------------
def display_summary():
    scm.update.display_summary()

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # -b option is not supported/needed
    if ( args['-b'] != None ):
        sys.exit( "The '-b' option is not supported/needed.  Use a 'remote-ref' as the <id> argument" )

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # Set directory for the subtree directory
    dst = os.path.join( args['<dst>'], pkg )
    dst = utils.force_unix_dir_sep(dst)
    utils.print_verbose( f"Location of the copy being updated: {dst}" )

    # Update the 'subtree'
    cmd = f'git subtree pull --prefix {dst} {args["<origin>"]}/_git/{args["<repo>"]} {args["<id>"]} --squash'
    t = utils.run_shell( cmd, common_args['-v'] )
    utils.check_results( t, "ERROR: Failed the update a subtree for the specified package/repository." )
