"""
 
Manages the package's 'Owned' dirs
===============================================================================
usage: orc [common-opts] dirs [-s|-o]
       orc [common-opts] dirs [options] ls
       orc [common-opts] dirs [options] pls
       orc [common-opts] dirs [options] xls
       orc [common-opts] dirs [options] set <primary>...
       orc [common-opts] dirs [options] rm  <primary>...
       orc [common-opts] dirs [options] xset <adoptedExtra>...
       orc [common-opts] dirs [options] xrm  <adoptedExtra>...

Arguments:
    ls                  Calculates the package's owned directories
    pls                 Dislays the list of 'primary' directories
    xls                 Displays the list of 'extra' (when being adopted)
                        directories
    set                 Sets (in the package.json file) top-level 'primary' 
                        directories that will be overlaid when the package is 
                        adopted (as an overlay pacakge).
    rm                  Removes (from the package.json file) top-level 'primary'
                        dirtectories that will be 'overlaid' when the package
                        is adopted (as an overlay pacakge).
    xset                Sets (in the package.json file) 'extra' directories 
                        that will be copied as part of the adopted package's
                        information.
    xrm                 Removes (from the package.json file) 'extra'
                        dirtectories that will be copied as part of the adopted 
                        package's information.
    <primary>           One or more top-level 'primary' directories 
    <adoptedExtra>      One or more directories that 'extra' adopted directories.
                        The directory name/path is relative to the package's
                        root directory.
    
Options:
    -u                  Update the 'pkg-dirs.lst' file with the 'ls' results
    -s                  Displays the current 'pkg-dirs.lst' instead of
                        deriving the package's owned directories
    -o                  Displays the current list of 'overlay' directories
                        defined for the package.
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Examples:
    orc dirs ls
    orc dirs set src tests
    orc dirs rm tests
    
"""
import os, sys
import utils
from docopt.docopt import docopt
from my_globals import PKG_DIRS_FILE
from my_globals import PACKAGE_ROOT

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'dirs', "Manages my package's 'Owned' directories" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    
    # Show existing dirs file
    if ( args['-s'] ):
        dirs = utils.load_dirs_list_file()
        if ( dirs != None ):
            for d in dirs:
                print(d)
        
        sys.exit(0)

    # Show the currently defined overlay directories
    if ( args['-o'] ):
        show_primary_dirs()
        sys.exit(0)

    # Set directories
    if ( args['set'] ):
        newdirs = args['<primary>']
        newdirs = check_dirs( newdirs )
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_primary_dirs(package_json) 
        if ( pdirs == None ):
            utils.json_update_package_file_with_new_primary_dirs( package_json, newdirs )
        else:
            # Filter out would-be-duplicates
            for d in newdirs:
                if ( not d in pdirs ):
                    pdirs.append( d )

            # Sort the list and update the file
            pdirs.sort()
            utils.json_update_package_file_with_new_primary_dirs( package_json, pdirs )

        # Show update list
        show_primary_dirs()
        sys.exit(0)

    # Remove directories
    if ( args['rm'] ):
        rmdirs = standardize_input_dirs( args['<primary>'] )
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_primary_dirs(package_json) 
        if ( pdirs == None ):
            pass
        else:
            # Delete dirs
            newlist = []
            for d in pdirs:
                if ( not d in rmdirs ):
                    newlist.append( d )

            # Sort the list and update the file
            newlist.sort()
            utils.json_update_package_file_with_new_primary_dirs( package_json, newlist )

        # Show update list
        show_primary_dirs()
        sys.exit(0)

    # derive directories
    if ( args['ls'] ):
        owndirs = utils.get_owned_dirs( PACKAGE_ROOT() )
        for d in owndirs:
            print(d)

        # Update the directory list file
        if ( args['-u'] ):
            utils.save_dirs_list_file( owndirs )

        sys.exit(0)

    # Set 'extra' directories
    if ( args['xset'] ):
        newdirs = args['<adoptedExtra>']
        newdirs = check_dirs( newdirs )
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_extra_dirs(package_json) 
        if ( pdirs == None ):
            utils.json_update_package_file_with_new_extra_dirs( package_json, newdirs )
        else:
            # Filter out would-be-duplicates
            for d in newdirs:
                if ( not d in pdirs ):
                    pdirs.append( d )

            # Sort the list and update the file
            pdirs.sort()
            utils.json_update_package_file_with_new_extra_dirs( package_json, pdirs )

        # Show update list
        show_extra_dirs()
        sys.exit(0)

    # Remove directories
    if ( args['xrm'] ):
        rmdirs = standardize_input_dirs( args['<adoptedExtra>'] )
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_extra_dirs(package_json) 
        if ( pdirs == None ):
            pass
        else:
            # Delete dirs
            newlist = []
            for d in pdirs:
                if ( not d in rmdirs ):
                    newlist.append( d )

            # Sort the list and update the file
            newlist.sort()
            utils.json_update_package_file_with_new_extra_dirs( package_json, newlist )

        # Show update list
        show_extra_dirs()
        sys.exit(0)

    # List extra directories
    if ( args['xls'] ):
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_extra_dirs(package_json) 
        for d in pdirs:
            print(d)
        sys.exit(0)

    # List extra directories
    if ( args['pls'] ):
        package_json = utils.load_package_file();
        pdirs = utils.json_get_package_primary_dirs(package_json) 
        for d in pdirs:
            print(d)
        sys.exit(0)

#
def show_primary_dirs():
    package_json = utils.load_package_file();
    pdirs = utils.json_get_package_primary_dirs(package_json) 
    if ( pdirs == None ):
        print( "No primary directories are currently set for the package" )
    else:
        for d in pdirs:
            print( d )

def show_extra_dirs():
    package_json = utils.load_package_file();
    pdirs = utils.json_get_package_extra_dirs(package_json) 
    if ( pdirs == None ):
        print( "No adopted 'extra' directories are currently set for the package" )
    else:
        for d in pdirs:
            print( d )

def standardize_input_dirs( list_of_dirs ):
    stdlist = []
    for d in list_of_dirs:
        stdlist.append( utils.force_unix_dir_sep( d ) )
    return stdlist
#
def check_dirs( dirlist ):
    stdlist = []
    for d in dirlist:
        path = os.path.join( PACKAGE_ROOT(), d )
        stdlist.append( utils.force_unix_dir_sep( d ) )
        if ( not os.path.isdir( d ) ):
            sys.exit( f"ERROR: Directory {d}{os.sep} does not exists" )

    return stdlist
