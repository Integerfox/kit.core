"""
 
Calculates differences between a local package and a remote package
===============================================================================
usage: orc [common-opts] diff [options] <repo> summary [<id>]
       orc [common-opts] diff [options] <repo> file <fname> [<id>]
       orc [common-opts] diff [options] show
       orc [common-opts] diff [options] clean

       
Arguments:
    summary          Display's a summary of what is different between the two
                     packages.
    show             Displays the package name/version of the currently 
                     downloaded remote package/repo
    clean            Removes the working copy of a previously downloaded 
                     remote package/repo

    <repo>           Name of the package's repository to compare (with the
                     remote package/repository).
    <fname>          Local package file name to compare against the remote
                     package (the path is relative to the package root)
    <id>             Label/Tag/Hash/Version of remote package to compare. If
                     not specified, the previously compared/download remote
                     package snapshot is used.
                     
    
Options:
    -f               Force a 'quick' file comparison vs the default of
                     comparing the file contents (i.e. uses the file attributes)
                     are different (slower compare).
    -p PKGNAME       Specifies the Remote Package name if different from the 
                     <repo> name
    -b BRANCH        Specifies the source branch in <repo>.  The use/need
                     of this option in dependent on the <repo> SCM type.
    -o REMOTEORIGN   Overrides the local's package 'origin' reference when
                     downloading the remote package.
    -n NUMCONEXT     Number of context lines for a unified diff [Default: 5]
    -h, --help       Display help for this command

Common Options:
    See 'orc --help'
    
Notes:
    1. Capturing the initial remote snapshot of package can be time consuming,
       i.e. it has to pull down the entire remote repo.
           
Examples:
    # Compares the adopted colony.core pkg to the remote version OUTCAST2-main-4.4.0
    orc.py diff summary colony.core OUTCAST2-main-4.4.0


"""

# local is missing dir
# local is missing files
# local is extra files
# local file is different

import os, sys, json
import utils
from docopt.docopt import docopt
from difflib import unified_diff
from my_globals import TEMP_DIFF_DIR_NAME
from my_globals import PACKAGE_INFO_DIR
from my_globals import PACKAGE_ROOT

REMOTE_PKG_INFO_FNAME = "_snapshot_remote_pkg_info_"

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'adopt', "Compares a local package with a remote package" ))
    
#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Verbose option for subcommand
    vopt = ' -v ' if common_args['-v'] else ''

    # Default Package name
    pkg = args['<repo>']
    if ( args['-p'] ):
        pkg = args['-p']

    # default branch option
    branch_opt = ""
    if ( args['-b'] ):
        branch_opt = '-b ' + args['-b']

    # Remote package snapshot
    rpkg_dir  = os.path.join( PACKAGE_ROOT(), TEMP_DIFF_DIR_NAME() )
    rpkg_info = have_remote_package( rpkg_dir )
    
    # Show option
    if ( args['show'] ):
        show_snapshot_info( rpkg_info, rpkg_dir )
        exit(0)
        
    # Clean option
    if ( args['clean'] ):
        utils.remove_tree(rpkg_dir)
        print("Removed remote snapshot")
        exit(0)
        
    # Does the local package exist?
    json_dict = utils.load_package_file()
    pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, pkg )
    if ( pkgobj == None ):
        sys.exit( f'Package {pkg} is not a local package (i.e. has not been adopted)' );
    
    # Remote repo, origin, etc.
    remote_repo   = pkgobj['repo']['name']
    remote_origin = pkgobj['repo']['origin']
    if ( args['-o'] ):
        remote_origin = args['-o']
        
    # Reuse existing snapshot
    have_snapshot = False
    if ( not args["<id>"] ):
        if ( rpkg_info == None ):
           sys.exit( "ERROR: No currently download remote pkg - you specify a remote <id>")
        if ( rpkg_info['pkgname'] == pkgobj['pkgname'] ):
            have_snapshot = True    
        
    # Get a local snapshot
    if ( not have_snapshot ):
        # Have snapshot - but is the 'wrong' package
        utils.remove_tree(rpkg_dir)
        #cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy --force -p {pkg} {branch_opt} {tmpdst} {args['<repo>']} {args['<origin>']} {args['<id>']}"
        
        cmd = f"evie.py {vopt} --scm {common_args['--scm']} copy --force -p {pkg} {branch_opt} {rpkg_dir} {remote_repo} {remote_origin} {args['<id>']}"
        t   = utils.run_shell( cmd, common_args['-v'] )
        utils.check_results( t, f"ERROR: Failed to make a copy of the remote repo: {remote_repo}", 'copy', 'get-error-msg', common_args['--scm']  )
        
        # Create snapshot info
        rpkg_info = {}
        rpkg_info["pkgname"] = pkgobj['pkgname']
        rpkg_info["pkgtype"] = pkgobj['pkgtype']
        rpkg_info["id"]      = args["<id>"]
        rpkg_info["origin"]  = remote_origin
        data  = json.dumps( rpkg_info, indent=2 )
        fname = os.path.join(rpkg_dir, REMOTE_PKG_INFO_FNAME )
        try:
            with open( fname, "w+" ) as file:
                file.write( data )
        except Exception as e:
            sys.exit(f"ERROR: Unable to save remote-pkg information ({e})")

    # Summary
    if ( args['summary'] ):
        if ( rpkg_info['pkgtype'] == 'overlay' ):
            summary_overlay( pkgobj, rpkg_info, rpkg_dir, args['-f'] )
        else:            
            summary_not_overlay( pkgobj, rpkg_info, rpkg_dir, args['-f'] )
        
    # File compare
    if ( args['file'] ):
        print_unified_diff( pkgobj, rpkg_info, rpkg_dir, args['<fname>'], int(args['-n']), args['-f'])
        
        
#------------------------------------------------------------------------------
def print_unified_diff( pkgobj, rpkg_info, rpkg_dir, local_file, ncontext, quick=True ):
    # Validate the specified file exists
    leftfname = os.path.join( PACKAGE_ROOT(), local_file )
    if ( not os.path.isfile(leftfname) ):
        sys.exit( f"ERROR: Invalid file name: {leftfname}")

    # Perform the comparison
    cmp = generate_compare_data( pkgobj, rpkg_info, rpkg_dir, quick )        
    
    # Find the 'file' in the different list
    result = cmp.find_difference_file( leftfname )
    if ( result == None ):
        sys.exit( f"ERROR: The specified file ({leftfname}) is not found in the 'different' list")
    
    # generate unified diff
    try:
        localdata = []
        with open(result[0], "r") as fd:
            lines = fd.readlines()
            for l in lines:
                localdata.append( l.rstrip())
        remotedata = []            
        with open(result[1], "r") as fd:
            lines = fd.readlines()
            for l in lines:
                remotedata.append( l.rstrip())
        
    except Exception as e:
        sys.exit( f"ERROR: Unable to compare the specified files ({e})")
    
    dif = unified_diff( localdata, remotedata, fromfile=leftfname, tofile="<remote>", n=ncontext)    
    for l in dif:
        print( l )        
    
def generate_compare_data( pkgobj, rpkg_info, rpkg_dir, quick=True ):
    # Overlay package
    if (pkgobj['pkgtype'] == 'overlay' ):
        # Get the list of overlayed directories
        odirs = utils.load_overlaid_dirs_list_file( pkgobj['pkgname'])
        
        # Compare overlay dirs
        cmp = utils.CompareDirs(recurse=False, quick=quick )
        for d in odirs:
            leftdir  = os.path.join( PACKAGE_ROOT(), d )
            rightdir = os.path.join( rpkg_dir, pkgobj['repo']['name'], d )
            cmp.compare_directory( leftdir, rightdir)
        return cmp
            
    # All other package types
    cmp = utils.CompareDirs(recurse=True, quick=quick  )
    leftdir  = os.path.join( PACKAGE_ROOT(), pkgobj['parentDir'], pkgobj['repo']['name'])
    rightdir = os.path.join( rpkg_dir, pkgobj['repo']['name'] )
    cmp.compare_directory( leftdir, rightdir)
    return cmp

def summary_overlay( pkgobj, rpkg_info, rpkg_dir, quick=True ):
    cmp = generate_compare_data( pkgobj, rpkg_info, rpkg_dir, quick )        
    prefix = PACKAGE_ROOT() + os.sep
    cmp.print_diff_files( "diff ", strip_prefix=prefix )
    cmp.print_left_files( "local", strip_prefix=prefix )
    cmp.print_right_files( "remote",  strip_prefix=None )
    cmp.print_error_files( "error ", strip_prefix=prefix )
    cmp.print_summary()

def summary_not_overlay( pkgobj, rpkg_info, rpkg_dir, quick=True ):
    cmp = generate_compare_data( pkgobj, rpkg_info, rpkg_dir, quick )        
    prefix = PACKAGE_ROOT() + os.sep
    cmp.print_diff_files( "diff ", strip_prefix=prefix )
    cmp.print_left_files( "local", strip_prefix=prefix )
    cmp.print_right_files( "remote",  strip_prefix=None )
    cmp.print_error_files( "error ", strip_prefix=prefix )
    cmp.print_summary()
    
def show_snapshot_info( rpkg_info, rpkg_dir ):
    if ( rpkg_info == None ):
        print("NO downloaded remote snapshot.")
    else:
        print(f"Remote snapshot: {rpkg_info["pkgname"]}")
        print(f"  type:       {rpkg_info["pkgtype"]}")
        print(f"  id:         {rpkg_info["id"]}")
        print(f"  origin:     {rpkg_info["origin"]}")
        print(f"  remote-dir: {rpkg_dir}")

def have_remote_package( dirname ):
    if ( not os.path.isdir(dirname) ):
        return None
    try:
        fname = os.path.join(dirname, REMOTE_PKG_INFO_FNAME )
        with open(fname) as fd:
            jsonobj = json.load(fd)
        return jsonobj
    except Exception as e:
        print(e)
        return None            

        
