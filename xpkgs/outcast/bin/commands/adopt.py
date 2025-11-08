"""
 
Adopts the specified Repo/Package package
===============================================================================
usage: orc [common-opts] adopt [options] readonly <dst> <repo> <origin> <id>
       orc [common-opts] adopt [options] foreign  <dst> <repo> <origin> <id>               
       orc [common-opts] adopt [options] overlay  <repo> <origin> <id>
       orc [common-opts] adopt [options] update  <repo> <id>
       orc [common-opts] adopt [options] clean 

Arguments:
    readonly         Adopts the specified package as a readonly package
    foreign          Adopts the specified package as a foreign package
    overlay          Adopts the specified package as a overlay package
    update           Updates the repo to the specific new <id>, i.e. it performs
                     an 'orc rm <repo>' and 'orc adopt' commands.
    clean            Cleans up after a fail adoption of a OVERLAY package.
    <dst>            Parent directory for where the adopted package is placed. 
                     The directory is specified as a relative path to the root
                     of primary repository.    
    <repo>           Name of the repository to adopt
    <origin>         Path/URL to the repository
    <id>             Label/Tag/Hash/Version of code to be adopted
    
Options:
    --weak           Adopt the package as 'weak' dependencies.  The default is
                     to adopt as an 'strong' dependency. A 'weak' dependency 
                     is not propagated when determining transitive dependencies.
    --virtual        The adoption is 'virtual' so as to satisfy dependencies, 
                     but no actual files (other than package meta data) is 
                     brought in to the repo. This is usefully when you are 
                     adopting a package that has 'baggage' you are not planning 
                     to use but don't want to deal with failed dependencies 
                     checks. Note: this option forces the --weak option to be 
                     set.
    --semver VER     Specifies the semantic version info for the package
                     being adopted. This information is not required, but
                     recommended if it is available. NOTE: If the package being
                     adopted has an Outcast package.json file - then the adopt
                     command will attempt to use the adoptee's published
                     semantic version info.
    -p PKGNAME       Specifies the Package name if different from the <repo> 
                     name
    -b BRANCH        Specifies the source branch in <repo>.  The use/need
                     of this option in dependent on the <repo> SCM type.
    --nowarn         Ignores warnings
    -h, --help       Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:

"""
import os, sys, time, json
import utils
import uuid
from docopt.docopt import docopt
from my_globals import OVERLAY_PKGS_DIR
from my_globals import PACKAGE_INFO_DIR
from my_globals import TEMP_DIR_NAME
from my_globals import PACKAGE_FILE
from my_globals import PKG_DIRS_FILE
from my_globals import IGNORE_DIRS_FILE
from my_globals import PACKAGE_ROOT


#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'adopt', "Adopts an external package" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Verbose option for subcommand
    vopt = ' -v ' if common_args['-v'] else ''

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # CLEAN-UP (for a failed overlay adoption)
    if ( args['clean'] ):
        tmpdst = os.path.join( PACKAGE_ROOT(), PACKAGE_INFO_DIR(), TEMP_DIR_NAME() )
        utils.remove_tree( tmpdst, "error", "warn" )
        sys.exit(0)

    # default branch option
    branch_opt = ""
    if ( args['-b'] ):
        branch_opt = '-b ' + args['-b']

    # Get the current time
    dt_string = time.asctime(time.gmtime())

    # check for already adopted
    json_dict = utils.load_package_file()
    pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, pkg )
    if ( pkgobj != None and not args["update"]):
        sys.exit( f'Package {pkg} already has been adopted as {deptype} dependency' );
        
    # double check if the package has already been adopted (i.e. there was manual edits to the package.json file)
    dstpkg = ''
    if ( not args['overlay'] and not args["update"]):
        dstpkg = os.path.join( args['<dst>'], pkg)
        if ( os.path.exists( dstpkg ) ):
            sys.exit( f"ERROR: The destination - {dstpkg} - already exists" )

    # Refresh operation -->get the current repo's 'type' and 'origin'
    if ( args["update"] ):
        if ( pkgobj == None ):
            sys.exit( f'ERROR: Refresh - package {pkg} does NOT exist' );

        # Remove the current package version
        cmd = f"orc.py {vopt} --primary-scm {common_args['--primary-scm']} --scm {common_args['--scm']} rm {pkg}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to remove original package: {args['<repo>']}" )
        json_dict = utils.load_package_file()
     
        # setup for adopting a newer/different version
        args['readonly']        = None
        args['foreign']         = None
        args['overlay']         = None
        args['update']         = None
        args[pkgobj['pkgtype']] = True
        args['<origin>']        = pkgobj['repo']['origin']
        args['--weak']          = True if deptype == 'weak' else False
        try:                    
            args['--virtual']   = pkgobj['virtualAdoption']  # older 'adoptions' do not have the 'virtualAdoption' KVP
        except:
            args['--virtual']   = False
        args['<dst>']           = None if pkgobj['pkgtype'] == 'overlay' else pkgobj['parentDir']
        
    # The virtual adoption option forces a weak dependency
    if ( args['--virtual'] ):
        args['--weak'] = True

    #
    # Adopt: Foreign
    # 
    if ( args['foreign'] ):
        # Copy the FO package
        # Note: do a brute force clone/copy of the repo because it has 'better commit' semantics (i.e. works with pending commit changes) for the repo doing the adopting
        #cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy -p {pkg} {branch_opt} {args['<dst>']} {args['<repo>']} {args['<origin>']} {args['<id>']}"
        cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy --force -p {pkg} {branch_opt} {args['<dst>']} {args['<repo>']} {args['<origin>']} {args['<id>']}"
      
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to make a copy of the repo: {args['<repo>']}", 'copy', 'get-error-msg', common_args['--scm']  )

        # update the package.json file
        dst_pkg_info    = os.path.join( args['<dst>'], pkg, PACKAGE_INFO_DIR() )
        incoming_semver = utils.get_adopted_semver( dst_pkg_info, args['--semver'], pkg, not args['--nowarn'] )
        d = utils.json_create_dep_entry( pkg, "foreign", args['<dst>'], dt_string, incoming_semver, args['-b'], args['<id>'], args['<repo>'], common_args['--scm'], args['<origin>'], args['--virtual'] )

        # Verify there is package file for package being adopted
        if ( check_for_package_file( d, args ) ):

            # Check for cyclical deps
            if ( check_cyclical_deps( json_dict, d, args) == False ):
                # Remove the package
                cmd = f"evie.py {vopt} --scm {d['repo']['type']} rm -p {d['pkgname']} {branch_opt} {d['parentDir']} {d['repo']['name']} {d['repo']['origin']} {d['version']['tag']}"
                t   = utils.run_shell( cmd, common_args['-v'] )
                utils.check_results( t, f"ERROR: Failed to remove the package: {d['repo']['name']}, 'rm', 'get-error-msg', common_args['--scm']" )
            
                # Display parting message (if there is one)
                print("Adoption was 'reverted'")
                utils.display_scm_message( 'rm', 'get-success-msg', common_args['--scm'] )
                sys.exit(1)

        # Save changes
        utils.json_update_package_file_with_new_dep_entry( json_dict, d, args['--weak'] )
        
        # Delete unwanted files 
        if ( args['--virtual'] ):
            utils.delete_tree_except( os.path.join( args['<dst>'], pkg ), PACKAGE_INFO_DIR() )
            print( f"Package - {pkg} - adopted as a virtual FOREIGN package. Remember to commit changes to your SCM" )
        else:
            print( f"Package - {pkg} - adopted as an FOREIGN package. Remember to add the new files to your SCM" )

        # Display parting message (if there is one)
        #utils.display_scm_message( 'copy', 'get-success-msg', common_args['--scm'] )

    #
    # Adopt: ReadOnly 
    # 
    elif ( args['readonly'] ):
        # Mount the RO package
        # Note: do a brute force clone/copy of the repo because it has 'better commit' semantics (i.e. works with pending commit changes) for the repo doing the adopting
        #cmd = f"evie.py {vopt} --scm {common_args['--scm']} mount -p {pkg} {branch_opt} {args['<dst>']} {args['<repo>']} {args['<origin>']} {args['<id>']}"
        cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy --force -p {pkg} {branch_opt} {args['<dst>']} {args['<repo>']} {args['<origin>']} {args['<id>']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        #utils.check_results( t, f"ERROR: Failed to mount the repo: {args['<repo>']}", 'mount', 'get-error-msg', common_args['--scm'] )
        utils.check_results( t, f"ERROR: Failed to make a copy of the repo: {args['<repo>']}", 'copy', 'get-error-msg', common_args['--scm']  )

        # update the package.json file
        dst_pkg_info    = os.path.join( args['<dst>'], pkg, PACKAGE_INFO_DIR() )
        incoming_semver = utils.get_adopted_semver( dst_pkg_info, args['--semver'], pkg, not args['--nowarn'] )
        d = utils.json_create_dep_entry( pkg, "readonly", args['<dst>'], dt_string, incoming_semver, args['-b'], args['<id>'], args['<repo>'], common_args['--scm'], args['<origin>'], args['--virtual'] )

        # Verify there is package file for package being adopted
        if ( check_for_package_file( d, args ) ):

            # Check for cyclical deps
            if ( check_cyclical_deps( json_dict, d, args) == False ):
                # Remove the package
                cmd = f"evie.py {vopt} --scm {d['repo']['type']} rm -p {d['pkgname']} {branch_opt} {d['parentDir']} {d['repo']['name']} {d['repo']['origin']} {d['version']['tag']}"
                t   = utils.run_shell( cmd, common_args['-v'] )
                utils.check_results( t, f"ERROR: Failed to remove the package: {d['repo']['name']}, 'rm', 'get-error-msg', common_args['--scm']" )
            
                # Display parting message (if there is one)
                print("Adoption was 'reverted'")
                utils.display_scm_message( 'rm', 'get-success-msg', common_args['--scm'] )
                sys.exit(1)


        # Save changes
        utils.json_update_package_file_with_new_dep_entry( json_dict, d, args['--weak'] )

        # Delete unwanted files 
        if ( args['--virtual'] ):
            utils.delete_tree_except( os.path.join( args['<dst>'], pkg ), PACKAGE_INFO_DIR() )
            print("Virtual Adoption completed." )
            #utils.display_scm_message( 'umount', 'get-success-msg', common_args['--scm'] )

        # Mark files as readonly
        utils.set_tree_readonly( dstpkg )

        # Display parting message (if there is one)
        #utils.display_scm_message( 'mount', 'get-success-msg', common_args['--scm'] )
        print( f"Package - {pkg} - adopted as an READONLY package. Remember to add the new files to your SCM" )
        
    #
    # Adopt: overlay
    # 
    else:
        # Get a temporary copy of the OV package
        tmpdst = os.path.join( PACKAGE_INFO_DIR(), TEMP_DIR_NAME() )
        cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy --force -p {pkg} {branch_opt} {tmpdst} {args['<repo>']} {args['<origin>']} {args['<id>']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to make a copy of the repo: {args['<repo>']}", 'copy', 'get-error-msg', common_args['--scm']  )

        # Fail if missing outcast info
        src_pkg      = os.path.join( tmpdst, pkg )
        dst_pkg      = os.path.join( OVERLAY_PKGS_DIR(), pkg )
        dst_pkg_info = os.path.join( dst_pkg, PACKAGE_INFO_DIR() )
        src_pkg_info = os.path.join( src_pkg, PACKAGE_INFO_DIR() )
        if ( not os.path.isfile( os.path.join(src_pkg_info, PACKAGE_FILE() ) ) ):
            utils.remove_tree(tmpdst)
            sys.exit( f"ERROR: Package - {pkg} - does NOT have {PACKAGE_FILE()} file")
        if ( not os.path.isfile( os.path.join(src_pkg_info, PKG_DIRS_FILE() ) )):
            utils.remove_tree(tmpdst)
            sys.exit( f"ERROR: Package - {pkg} - does NOT have {PKG_DIRS_FILE()} file")
        if ( not os.path.isfile( os.path.join(src_pkg_info, IGNORE_DIRS_FILE() ) )):
            utils.remove_tree(tmpdst)
            sys.exit( f"ERROR: Package - {pkg} - does NOT have {IGNORE_DIRS_FILE()} file")

        # Copy the adoptee's package info directory
        utils.copy_pkg_info_dir( dst_pkg_info, src_pkg_info )

        # Create the dependency entry for the adopted package
        incoming_semver = utils.get_adopted_semver( dst_pkg_info, args['--semver'], pkg, not args['--nowarn'] )
        d = utils.json_create_dep_entry( pkg, "overlay", args['<dst>'], dt_string, incoming_semver, args['-b'], args['<id>'], args['<repo>'], common_args['--scm'], args['<origin>'], args['--virtual'] )

        # Check for cyclical deps
        if ( check_cyclical_deps( json_dict, d, args) == False ):
            # Remove the the package from the overlaid directory
            utils.remove_tree( dst_pkg )
            sys.exit("Adoption was 'reverted'")


        if ( not args['--virtual'] ):
            # Copy the adoptee's extra info directories
            utils.copy_extra_dirs( dst_pkg, src_pkg )

            # Get list of directories to copy/overlay
            dirs = utils.get_adoptee_owned_dirs( os.path.join( tmpdst, pkg, PACKAGE_INFO_DIR()), tmpdst )
            if ( dirs != None ):
                for dir in dirs:
                    src = os.path.join( src_pkg, dir )
                    dst = os.path.join( PACKAGE_ROOT(), dir )
                    utils.copy_files( src, dst )

        # Clean-up
        utils.remove_tree( tmpdst )
        
        # Save changes
        utils.json_update_package_file_with_new_dep_entry( json_dict, d, args['--weak'] )
        if ( not args['--virtual'] ):
            print( f"Package - {pkg} - adopted as an OVERLAY package. Remember to add the new files to your SCM" )
        else:
            print( f"Package - {pkg} - adopted as a virtual OVERLAY package. Remember to commit changes to your SCM" )


def check_cyclical_deps( json_dict, dep_pkg, args):
    # Enforce NO cyclical  deps
    cyc_strong, cyc_weak = utils.check_cyclical_deps( utils.json_get_package_name(json_dict), dep_pkg, suppress_warnings=args['--nowarn'])
    if ( len(cyc_strong) > 0 ):
        print( f"ERROR: One or more cyclical dependencies would be created with the adoption of: {dep_pkg['pkgname']}" )
        print_dep_list(cyc_strong)
        return False

    if ( len(cyc_weak) > 0 and args['--nowarn'] == False ):
        print( f"Warning: One or more cyclical WEAK dependencies would be created with the adoption of: {dep_pkg['pkgname']}" )
        print_dep_list(cyc_weak)
        inp = input( "Do you wish to continue? (Y/n)" )
        if ( not 'y' in inp.lower() ):
            return False

    return True

def check_for_package_file( dep_pkg, args ):
    # Get Dependencies info
    if ( None == utils.load_dependent_package_file( dep_pkg ) and not args['--nowar'] ):
        print( f"Warning: Package = {dep_pkg['pkgname']} does NOT have {PACKAGE_FILE()} file.  Cannot verify dependencies" )
        return False
    return True

def print_dep_list( deplist ):
    for d in deplist:
        print( json.dumps(d, indent=2) )

