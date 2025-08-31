"""
 
Checks and manages a package dependencies
===============================================================================
usage: orc [common-opts] deps [options]
       orc [common-opts] deps [options] check
       orc [common-opts] deps [options] ls [<adoptedpkg>]
       orc [common-opts] deps [options] mv <adoptedpkg> 

Arguments:
    check               Checks for any missing or cyclical dependencies
    ls                  Displays the dependencies incurred by the specified 
                        adopted package. If no adoptedpkg is specified then the
                        dependency tree for the current package is displayed.
    mv                  Moves an adopted package dependency from immeidate
                        to weak and vice-versus
    <adoptedpkg>        Name of a adopted package

Options:
    --noweak            Skip processing/checking weak dependencies
    --nostrong          Skip processing/checking strong dependencies
    -h, --help          Display help for this command

Common Options:
    See 'orc --help'
    
    
Notes:
    
"""
import os, json, sys
import utils
from docopt.docopt import docopt

#---------------------------------------------------------------------------------------------------------
def display_summary():
    print("{:<13}{}".format( 'deps', 'Checks and manages a package dependencies' ))
    

#------------------------------------------------------------------------------
def run( common_args, cmd_argv ):
    args = docopt(__doc__, argv=cmd_argv)

    # Housekeeping
    exit_code = 0;
    
    # Load my package info
    json_dict = utils.load_package_file()

    # Build dependency tree. 
    root = utils.Node( (json_dict, "ROOT") )
    build_tree( root, json_dict )

    # Get generation lists
    children = root.get_children().copy()
    grand_children = []
    for c in children:
        grand_children.extend( c.get_children() )

    # MOVE a dependency
    if ( args['mv'] ):
        # Look up the details of the package to be moved
        pkgobj, deptype, pkgidx = utils.json_find_dependency( json_dict, args['<adoptedpkg>']  )
        if ( pkgobj == None ):
            sys.exit( f"Cannot find the package - {args['<adoptedpkg>'] } - in the list of adopted packages" );
       
        # A Virtual adopted package can NOT be made a strong dependency
        if ( deptype == 'weak' and 'virtualAdoption' in json_dict['dependencies']['weak'][pkgidx] and json_dict['dependencies']['weak'][pkgidx]['virtualAdoption'] == True ):
            sys.exit(f"Package - {args['<adoptedpkg>']} - is virtual adopted and can NOT be a strong dependency" )

        # Remove then add the package from the deps list
        now_is_weak = True if deptype == 'strong' else False
        json_dict['dependencies'][deptype].pop(pkgidx)
        utils.json_update_package_file_with_new_dep_entry( json_dict, pkgobj, now_is_weak )
        print( f"Package - {args['<adoptedpkg>']} is now a {'weak' if now_is_weak else 'strong'} dependency " )
        sys.exit( 0 )

    # CHECK dependencies
    if ( args['check'] ):
        print( f"Checking dependencies for package: {utils.json_get_package_name(json_dict)}" )

        # Perform checks
        missing_list   = check_missing( root, children, grand_children )
        noncompat_list = check_compatibility( root, children, grand_children ) 
        cyclical_list  = check_cylical( missing_list, root.get_pkgname() )

        # Cyclical deps
        if ( not args['--noweak'] ):
            cyclical = filter_for_weak_dependency( cyclical_list )
            print_cyclical_list( cyclical, "Warning", "weak" )

        if ( not args['--nostrong'] ):
            cyclical = filter_for_strong_dependency( cyclical_list )
            print_cyclical_list( cyclical, "ERROR", "strong" )
            if ( len(cyclical) > 0 ):
                exit_code = 1
        
        # Missing deps
        if ( not args['--noweak'] ):
            missing = filter_for_weak_dependency( missing_list )
            print_missing_list( missing, "Warning", "weak" )

        if ( not args['--nostrong'] ):
            missing = filter_for_strong_dependency( missing_list )
            print_missing_list( missing, "ERROR", "strong" )
            if ( len(missing) > 0 ):
                exit_code = 1

        # Compatible check
        if ( not args['--noweak'] ):
            noncompat = filter_for_weak_dependency( noncompat_list )
            print_noncompat_list( noncompat, "Warning", "weak", children )

        if ( not args['--nostrong'] ):
            noncompat = filter_for_strong_dependency( noncompat_list )
            print_noncompat_list( noncompat, "ERROR", "strong", children )
            if ( len(noncompat) > 0 ):
                exit_code = 1

        print(f"All dependency checks completed (errors={'YES' if exit_code != 0 else 'no'})." )
        sys.exit( exit_code )


    # SHOW dependencies
    if ( not args['<adoptedpkg>'] ):
        print( root )
        sys.exit( 0 )

    # Filter the tree for a specific adopted package
    for c in children:
        if ( c.get_pkgname() != args['<adoptedpkg>'] ):
            root.remove_child_node( c )
    print( root )
    sys.exit( 0 )


def is_in_list( needle, haystack ):
    for s in haystack:
        if ( s.get_pkgname() == needle.get_pkgname() ):
            return True
    return False

    
def build_tree( node, json_dict ):
    if ( json_dict != None ):
        children = utils.get_dependency_list( json_dict )
        for c in children:
            dep_dict = utils.load_dependent_package_file( c )
            if ( utils.json_get_package_name(dep_dict) == None ):
                dep_dict = None
            cnode = node.add_child_data( (dep_dict, c['depType'], c) ) 
            build_tree( cnode, dep_dict )
                    

def print_missing_list( list, deptype_prefix,  deptype_label ):
    for p in list:
        print( f"{deptype_prefix}: Missing {deptype_label} dependency: {p[-1].get_pkgname()}" )
        print( "  PATH to dependent package:" )
        print_path( p )
        print()

def print_path( path, level=0 ):
    if ( len(path) > 0 and level <len(path) ):
        node = path[level]
        print( "{}{}-{} ({})".format( "  "*(level+2), node.get_pkgname(), node.get_semver(), node.get_dep_type() ) )
        print_path( path, level+1 )

def check_missing( tree, child_list, grand_child_list ):
    missing = []
    for gc in grand_child_list:
        if ( not is_in_list(gc, child_list ) ):
            missing.append( gc.get_path_to_me() )
    return missing                

def print_noncompat_list( list, deptype_prefix,  deptype_label, children ):
    for p in list:
        print( f"{deptype_prefix}: NON-Compatible {deptype_label} dependency: {p[-1].get_pkgname()}-{p[-1].get_semver()}" )
        have = get_child( p[-1].get_pkgname(), children )
        print( f"  HAVE: {have.get_pkgname()}-{have.get_semver()}" )
        print( "  PATH to dependent package:" )
        print_path( p )
        print()


def print_cyclical_list( list, deptype_prefix,  deptype_label ):
    for p in list:
        print( f"{deptype_prefix}: Cyclical {deptype_label} dependency: {p[-1].get_pkgname()}" )
        print( "  PATH to dependent package:" )
        print_path( p )
        print()

def get_child( pkgname, children ):
    for c in children:
        if ( pkgname == c.get_pkgname() ):
            return c
    return None

def check_compatibility( tree, child_list, grand_child_list ):
    noncompat = []
    for c in child_list:
        for gc in grand_child_list:
            if ( c.get_pkgname() == gc.get_pkgname() ):
                if ( not utils.is_semver_compatible( gc.get_semver(), c.get_semver() ) ):
                    noncompat.append( gc.get_path_to_me() )

    return noncompat
    
def check_cylical( missing_list, pkgname ):
    cyclical = []
    for path in missing_list:
        if ( path[-1].get_pkgname() == pkgname ):
            cyclical.append( path )
            missing_list.remove( path )

    return cyclical

def filter_for_weak_dependency( list_of_paths ):
    filtered = []
    if ( list_of_paths != None ):
        for path in list_of_paths:
            for n in path:
                if ( n.get_dep_type() == 'W' ):
                    filtered.append( path )
                    break

    return filtered

def filter_for_strong_dependency( list_of_paths ):
    filtered = []
    if ( list_of_paths != None ):
        for path in list_of_paths:
            weak_found = False
            for n in path:
                if ( n.get_dep_type() == 'W' ):
                    weak_found = True
                    break
            if ( not weak_found ):
                filtered.append( path )

    return filtered

