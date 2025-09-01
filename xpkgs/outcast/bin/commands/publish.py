"""
 
Publishes the Primary Package
===============================================================================
usage: orc [common-opts] publish [options] <semver> <comments>
       orc [common-opts] publish [options] ls 
       orc [common-opts] publish [options] help
       orc [common-opts] publish [options] 

Arguments:
    new             Publishs a new version of the package
    <semver>        The Semantic version information for the Published version.
    <comments>      Publish comments.
    ls              Outputs the 'change file' contents.  The default operation 
                    is to output the changes forthe latest published version.
    help            Display the recommended steps/process for publishing a
                    package

Options:
    -c FILE         Specifies a text file that contained details about 
                    individual changes.
    --edit          Updates the 'current' publish information in place, i.e.
                    does not create a new version.  The date/time fields are
                    NOT updated.
    --edithist      Updates a version in the History array.  Which version is
                    specified by the '--hist' option.
    --hist N        Selects the 'N' entry in the History array. N is zero
                    based index. This option can be used editing a history
                    entry as well as when displaying changes.
    -w              Suppress warning about missing files
    --nodirs        Skip updating the pkg-dirs.lst when publishing
    -a              Displays the 'changes' for all versions
    --
    -v              Enable verbose output
    -h, --help      Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o Publishing simply associates a comment/date/semantic-version with
      package at a point in time (i.e. only the package.json file is updated).
    o The user is responsible for archiving/labeling/tagging the package
      in its native SCM environment.
    o Publishing is not require for package management. However, life is
      better with respect to managing dependencies if packages are published.

"""
import os, json, sys
import utils
from docopt.docopt import docopt
from my_globals import PACKAGE_INFO_DIR
from my_globals import PACKAGE_ROOT
from my_globals import PKG_DIRS_FILE
from my_globals import IGNORE_DIRS_FILE

help_text = \
""" The following are the recommend steps for publishing an Outcast2 package:

  1  The source-code/changes are completed and ready to submit a pull request.

 [2] Use the 'orc info' command to set the package's basic information.  
     NOTE: this step only needs to be performed ONCE during the life of the 
     package (or on change of the package's information).

 [3] If the package is intended to be adopted as an 'overlay' package, use 
     the 'orc dirs set' command to set the package's primary directories. 
     NOTE: this step only needs to be performed ONCE during the life of the 
     package (or on change of the package's primary directories ).

 [4] If the package is intended to be adopted as an 'overlay' package, use 
     the 'orc dirs xset' command to set the package's 'adopted-extra' dirs. 
     NOTE: this step only needs to be performed ONCE during the life of the 
     package (or on change of the package's adopted-extra directories ).

 [5] If the package is intended to be adopted as an 'overlay' package, edit
     (or create) the package's 'ignore-dirs.lst' file.  Populate the file as
     needed.
     NOTE: this step only needs to be performed ONCE during the life of the 
     package (or on change of the package's ignore directories ).

  6  Run the 'orc publish' command to specify the semantic version information
     and brief description of what is being published.

  7  Commit and push changes to the package's Outcast files.

  8  Create the pull request for publishing the source-code/changes.

  9  Complete the pull request process and merge the PR into its parent
     branch.

[10] If the parent branch is not a 'release' branch, then propagate the merged
     changes to the 'release' branch

 11  Create a Tag/label - with the same semantic version information from 
     step 6 - and apply the tag/label to the 'release' branch from step 
     9 (or 10).
     Note: If using 'git' make sure that the tag gets pushed back to the 
           package's origin (e.g. git push origin MY-TAG-Maj.Min.Patch)
"""

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'publish', 'Publishes the Primary package' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Help on the publish sequence
    if ( args['help'] ):
        print( help_text )
        sys.exit(0)

    # Get the package data
    json_dict = utils.load_package_file()

    # Display changes
    if ( args['ls'] ):
        p = utils.json_get_published( json_dict )

        # Display all changes
        if ( args['-a'] ):
            utils.show_version_changes( p['current'], args['-v'] )
            for idx in range(0,len(p['history'])):
                utils.show_version_changes( p['history'][idx], args['-v'] )

        # Display the current version's changes
        elif ( args['--hist'] == None ):
            utils.show_version_changes( p['current'], args['-v'] )
            
        # Display the a history entry's changes
        elif ( args['--hist'] != None ):
            idx = int(args['--hist'])
            if ( idx < len(p['history']) ):
                utils.show_version_changes( p['history'][idx], args['-v'] )

        sys.exit(0)


    # Edit in place
    if ( args['--edit'] ):
        prev = utils.json_get_current_version( json_dict )
        v = utils.json_create_version_entry( args['<comments>'], args['<semver>'], args['-c'], prev['date'] )
        utils.json_update_current_version( json_dict, v )

    # Edit a history entry
    elif ( args['--edithist'] ):
        utils.json_update_history_version( json_dict, args['--hist'],  args['<comments>'], args['<semver>'], args['-c'] )

    # Create new entry
    elif ( args['<semver>'] ):
        v = utils.json_create_version_entry( args['<comments>'], args['<semver>'], args['-c'] )
        utils.json_add_new_version( json_dict, v )
        
    # Important files...
    dirs_file   = os.path.join(PACKAGE_INFO_DIR(), PKG_DIRS_FILE() )
    ignore_file = os.path.join(PACKAGE_INFO_DIR(), IGNORE_DIRS_FILE() )

    # Ensure the 'dirs list' is up to date
    if ( not args['--nodirs'] and args['<semver>'] ):
        if ( os.path.isfile( ignore_file ) ):
            owndirs = utils.get_owned_dirs( PACKAGE_ROOT() )
            utils.save_dirs_list_file( owndirs )

    # display publish info
    p = utils.json_get_published( json_dict )
    if ( args['-v'] == False ):
        p = utils.filter_changes_in_publish_dict(p)
    p = utils.flip_history_order(p)
    print( "HISTORY" ) 
    print( json.dumps(p['history'], indent=2) )
    print( "CURRENT" ) 
    print( json.dumps(p['current'], indent=2) )

    # Display warnings
    if ( not args['-w'] ):
        warning = False
        if ( not os.path.isfile( dirs_file )):
            print( f"Warning: NO {dirs_file} file has been created for the package.  See the 'orc dirs' command")
            warning = True
        if ( not os.path.isfile( ignore_file )):
            print( f"Warning: NO {ignore_file} file has been created for the package. Create using a text editor. The file has same semantics as a .gitignore file.")
            warning = True
        if ( warning ):
            print()
            print( f"The above warning(s) can be ignored if the package is NOT intended to be to be adopted as an 'overlay' package." )
    
