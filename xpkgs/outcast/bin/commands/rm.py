"""
 
Removes an adopted package
===============================================================================
usage: orc [common-opts] rm [options] <adoptedpkg>

Arguments:
    <adoptedpkg>        Name of an adopted package to be removed
    
Options:
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    o If  <adoptedpkg> is not speficied, then the repository's primary 
      package information is displayed.
    
"""
import os, sys
import utils
from docopt.docopt import docopt
from my_globals import PACKAGE_FILE
from my_globals import OVERLAY_PKGS_DIR


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'rm', 'Removes an adopted package' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Verbose option for subcommand
    vopt = ' -v ' if common_args['-v'] else ''

    # Look up the details of the package to be removed
    pkg = args['<adoptedpkg>']
    json_dict = utils.load_package_file()
    pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, pkg )
    if ( pkgobj == None ):
        sys.exit( f'Cannot find the package - {pkg} - in the list of adopted packages' );

    # default branch option
    branch_opt = ""
    if ( pkgobj['version']['branch'] != None ):
        branch_opt = '-b ' + args['-b']

    # READONLY Package
    if ( pkgobj['pkgtype'] == 'readonly' ):
        if ( pkgobj['parentDir'] == None ):
            sys.exit( f"ERROR: the {PACKAGE_FILE()} file is corrupt - there is no parent directory for the package" )

        # Remove the package
        #cmd = f"evie.py {vopt} --scm {pkgobj['repo']['type']} umount -p {pkgobj['pkgname']} {branch_opt} {pkgobj['parentDir']} {pkgobj['repo']['name']} {pkgobj['repo']['origin']} {pkgobj['version']['tag']}"
        cmd = f"evie.py {vopt} --scm {pkgobj['repo']['type']} rm -p {pkgobj['pkgname']} {branch_opt} {pkgobj['parentDir']} {pkgobj['repo']['name']} {pkgobj['repo']['origin']} {pkgobj['version']['tag']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to umount the repo: {pkgobj['repo']['name']}, 'umount', 'get-error-msg', common_args['--scm']" )
        
        # Remove the package from the deps list
        json_dict['dependencies'][deptype].pop(pkgidx)
        utils.write_package_file( json_dict )

        # Display parting message (if there is one)
        utils.display_scm_message( 'rm', 'get-success-msg', common_args['--scm'] )

    # FOREIGN Package
    elif ( pkgobj['pkgtype'] == 'foreign' ):
        if ( pkgobj['parentDir'] == None ):
            sys.exit( f"ERROR: the {PACKAGE_FILE()} file is corrupt - there is no parent directory for the package" )

        # Remove the package
        cmd = f"evie.py {vopt} --scm {pkgobj['repo']['type']} rm -p {pkgobj['pkgname']} {branch_opt} {pkgobj['parentDir']} {pkgobj['repo']['name']} {pkgobj['repo']['origin']} {pkgobj['version']['tag']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to remove the package: {pkgobj['repo']['name']}, 'rm', 'get-error-msg', common_args['--scm']" )
        
        # Remove the package from the deps list
        json_dict['dependencies'][deptype].pop(pkgidx)
        utils.write_package_file( json_dict )

        # Display parting message (if there is one)
        utils.display_scm_message( 'rm', 'get-success-msg', common_args['--scm'] )

    # OVERALY Package
    elif ( pkgobj['pkgtype'] == 'overlay' ):
        dstpkgpath = os.path.join( OVERLAY_PKGS_DIR(), pkg )
        if ( not os.path.isdir(dstpkgpath) ):
            sys.exit( f"ERROR: The {pkg} does not exist under the {OVERLAY_PKGS_DIR()}/ directory" )

        # Remove the overlaid directories/files
        dirs = utils.load_overlaid_dirs_list_file( pkg )
        try:
            for d in dirs:
                utils.delete_directory_files( d )
        except Exception as e:
            sys.exit(f"ERROR: Failed to remove overlaid directories [{e}]" )
            
        # Remove 'overlaid' directory
        utils.remove_tree( dstpkgpath )

        # Remove the package from the deps list
        json_dict['dependencies'][deptype].pop(pkgidx)
        utils.write_package_file( json_dict )

        # Remove the pkgs.overlaid dir if there are no more overlaid packages
        try:
            os.rmdir( OVERLAY_PKGS_DIR() )
        except:
            pass

        # Display parting message 
        print(f"Overlay package - {pkg} - removed" )
        utils.display_scm_message( 'rm', 'get-success-msg', common_args['--scm'] )


    # Unsupported package type
    else:
        sys.exit( f"ERROR: Unsupport package type: {pkgobj['pkgtype']}. The {PACKAGE_FILE()} file has been corrupted" )

