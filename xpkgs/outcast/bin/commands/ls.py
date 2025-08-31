"""
 
Lists the adopted packages
===============================================================================
usage: orc [common-opts] ls [options] [<wildcard>]

Arguments:
    <wildcard>          Filters the list agains the package names by specified 
                        <wildcard>.  The notation for <wildcard> is Python's 
                        Unix filename pattern matching.
    
Options:
    -v                  Display version info
    -r                  Display repo info
    -l                  Displays all details (i.e. -vr)
    -w                  Display only weak dependencies
    -s                  Display only strong dependencies
    -x                  Suppress column headers
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    
"""
import os, sys
import utils
import fnmatch
from docopt.docopt import docopt

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'ls', "Lists the adopted packages" ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)
    if ( args['<wildcard>'] == None ):
        args['<wildcard>'] = '*'

    # Get the list of adopted packages
    json_dict = utils.load_package_file()

    # Get Dependencies
    pkgs = utils.get_dependency_list( json_dict, include_immediate=not args['-w'], include_weak=not args['-s'] )

    # Sort the list
    pkgs = sorted( pkgs, key = lambda i: i['pkgname'] )
    if ( not args['-x'] ):
        header  = "PkgName          DV PkType    AdoptedDate               ParentDir        "
        rheader = "RepoName         RepoType RepoOrigin                               "
        vheader = "SemVer   Branch           Tag"
        if ( args['-l'] or args['-r'] ):
            header = header + rheader
        if ( args['-l'] or args['-v'] ):
            header = header + vheader
        print( header )

    # display the list
    for p in pkgs:
        if ( fnmatch.fnmatch(p['pkgname'], args['<wildcard>']) ):
            virtual = ' '
            if ( 'virtualAdoption' in p and p['virtualAdoption'] == True ):
                virtual = '*'
            info = f"{p['pkgname']:<16} {p['depType']}{virtual} {p['pkgtype']:<8}  {p['adoptedDate']}  {utils.json_get_dep_parentdir(p):<16}"
        
            # Repo info
            if ( args['-l'] or args['-r'] ):
                info = info + f" {utils.json_get_dep_repo_name(p):<16} {utils.json_get_dep_repo_type(p):<8} {utils.json_get_dep_repo_origin(p):40}"

            # Version info
            if ( args['-l'] or args['-v'] ):
                info = info + f" {utils.json_get_dep_semver(p):<8} {utils.json_get_dep_branch(p):<16} {utils.json_get_dep_tag(p)}"

            # display output
            print( info )  


