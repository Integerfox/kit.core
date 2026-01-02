#!/usr/bin/python3
r"""
General purpose utility for integrating NQBP into other tools, e.g. VSCode
===============================================================================
usage: sancho [options] buildirs PATH
       sancho [options] compiler PATH

Arguments:
    buildirs    Generates a list of nqbp build project directories that might
                build/use the specified file or directory. The build directories
                are listed in the 'most relevant' order. NOTE: The xpkgs/
                directory at the repository root is excluded from searches.
    compiler    Looks up the compiler option number for env.bat/env.sh based
                on the build directory path.  NOTE: PATH must be a build
                directory that contains an nqbp.py file.
    PATH        Path to file or directory (absolute or relative to 
                repository root directory).

Options:
    -v                   Verbose output
    -h, --help           Display help

Examples:
    ; Find build projects for a specific file
    sancho.py buildirs src/Kit/System/Api.h
    
    ; Find compiler option for a build directory
    sancho.py compiler src/Kit/Container/_0test/_0build/windows/msvc
    
"""

import os
import sys
import platform
from pathlib import Path

sys.path.append( os.path.dirname(__file__) + os.sep + ".." )
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from nqbplib.my_globals import NQBP_PKG_ROOT

SANCHO_VERSION = '1.0'

#------------------------------------------------------------------------------
def find_nqbp_files_recursive(directory, repo_root=None):
    """
    Recursively search for nqbp.py files in a directory tree.
    
    Args:
        directory: Path to search
        repo_root: Repository root path (to skip xpkgs at repo root level)
        
    Returns:
        List of directories containing nqbp.py
    """
    results = []
    directory = Path(directory).resolve()
    
    if not directory.exists() or not directory.is_dir():
        return results
    
    # Determine xpkgs path to skip
    xpkgs_path = None
    if repo_root:
        xpkgs_path = Path(repo_root) / 'xpkgs'
    
    try:
        for root, dirs, files in os.walk(directory):
            root_path = Path(root)
            
            # Skip if current root is inside xpkgs directory
            if xpkgs_path and (root_path == xpkgs_path or xpkgs_path in root_path.parents):
                dirs[:] = []  # Don't recurse into any subdirectories
                continue
            
            # Skip hidden directories and common ignore patterns
            dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['node_modules', '__pycache__']]
            
            # Remove xpkgs from dirs list to prevent descending into it
            if xpkgs_path and 'xpkgs' in dirs and root_path == repo_root:
                dirs.remove('xpkgs')
            
            # Sort directories alphabetically for consistent traversal order
            dirs.sort()
            
            if 'nqbp.py' in files:
                results.append(root_path)
    except PermissionError:
        pass  # Skip directories we can't read
    
    # Sort results alphabetically for consistent output
    results.sort()
    
    return results


def find_build_projects(file_path, repo_root):
    """
    Find all nqbp.py build projects in the repository.
    
    Walks the entire repository (excluding xpkgs/) and generates a list of directories 
    containing nqbp.py, separated into platform matches and non-matches. Both lists are
    then sorted by proximity to the input file/directory.
    
    Args:
        file_path: Path to file or directory to use as reference for proximity sorting
        repo_root: Repository root path
        
    Returns:
        Tuple of (platform_matches, non_matches) - both are proximity-sorted lists of Path objects
    """
    repo_root = Path(repo_root).resolve()
    file_path = Path(file_path).resolve()
    
    # If file_path is a file, use its directory
    if file_path.is_file():
        input_dir = file_path.parent
    else:
        input_dir = file_path
    
    # Find all nqbp.py directories in the entire repo (excluding xpkgs)
    all_nqbp_dirs = find_nqbp_files_recursive(repo_root, repo_root)
    
    # Separate into platform matches and non-matches
    platform_matches, non_matches = separate_platform_matches(all_nqbp_dirs)
    
    # Sort both lists by proximity to input directory
    platform_matches = sort_by_proximity(platform_matches, input_dir)
    non_matches = sort_by_proximity(non_matches, input_dir)
    
    # Return as two separate lists
    return platform_matches, non_matches


def sort_by_proximity(project_dirs, input_dir):
    """
    Sort build directories by their distance from the input directory.
    
    Distance is measured by the minimum number of 'cd' steps needed to navigate
    from the input directory to the build directory.
    
    Within the same distance, directories are sorted alphabetically.
    
    Args:
        project_dirs: List of Path objects to build directories
        input_dir: Reference directory for distance calculation
        
    Returns:
        Sorted list of Path objects, closest first
    """
    def calculate_distance(build_dir, ref_dir):
        """
        Calculate the number of directory steps between two paths.
        Returns the sum of steps up to common ancestor + steps down to target.
        """
        # Find common ancestor
        build_parts = build_dir.parts
        ref_parts = ref_dir.parts
        
        # Find the length of the common prefix
        common_length = 0
        for i in range(min(len(build_parts), len(ref_parts))):
            if build_parts[i] == ref_parts[i]:
                common_length = i + 1
            else:
                break
        
        # Steps up from ref_dir to common ancestor
        steps_up = len(ref_parts) - common_length
        
        # Steps down from common ancestor to build_dir
        steps_down = len(build_parts) - common_length
        
        # Total distance
        return steps_up + steps_down
    
    # Calculate distance for each directory and pair it with the directory
    dirs_with_distance = [(calculate_distance(project_dir, input_dir), project_dir) 
                          for project_dir in project_dirs]
    
    # Sort by distance first, then alphabetically by path
    dirs_with_distance.sort(key=lambda x: (x[0], x[1]))
    
    # Return just the directories
    return [d[1] for d in dirs_with_distance]


def separate_platform_matches(project_dirs):
    """
    Separate project directories into platform matches and non-matches.
    Both groups are sorted alphabetically.
    
    Args:
        project_dirs: List of Path objects to build directories
        
    Returns:
        Tuple of (platform_matches, non_matches), both sorted alphabetically
    """
    current_platform = platform.system().lower()
    platform_matches = []
    non_matches = []
    
    for project_dir in project_dirs:
        # Get all directory components in the path
        parts = [p.lower() for p in project_dir.parts]
        
        # Check if platform name appears in the path
        if current_platform in parts:
            platform_matches.append(project_dir)
        else:
            non_matches.append(project_dir)
    
    # Sort both lists alphabetically
    platform_matches.sort()
    non_matches.sort()
    
    return platform_matches, non_matches


def parse_build_naming_file(repo_root):
    """
    Parse the .nqbp-build-naming.txt file to get compiler mappings.
    
    The file uses INI-style syntax with platform sections like [windows], [linux].
    Under each section are lines in the format:
    compiler_number = partial_path_pattern
    
    Args:
        repo_root: Repository root path
        
    Returns:
        Dictionary mapping platform -> list of (compiler_number, pattern) tuples
    """
    naming_file = Path(repo_root) / '.nqbp-build-naming.txt'
    
    if not naming_file.exists():
        return {}
    
    mappings = {}
    current_platform = None
    
    with open(naming_file, 'r') as f:
        for line in f:
            line = line.strip()
            
            # Skip empty lines and comments
            if not line or line.startswith('#') or line.startswith(';'):
                continue
            
            # Check for section header [platform]
            if line.startswith('[') and line.endswith(']'):
                current_platform = line[1:-1].strip().lower()
                if current_platform not in mappings:
                    mappings[current_platform] = []
                continue
            
            # Parse compiler mapping: number = pattern
            if current_platform and '=' in line:
                parts = line.split('=', 1)
                compiler_num = parts[0].strip()
                pattern = parts[1].strip()
                mappings[current_platform].append((compiler_num, pattern))
    
    return mappings


def find_compiler_option(build_dir_path, repo_root):
    """
    Find the compiler option number for a given build directory.
    
    Args:
        build_dir_path: Path to the build directory
        repo_root: Repository root path
        
    Returns:
        Compiler option number as string, or None if not found
    """
    build_dir = Path(build_dir_path).resolve()
    repo_root = Path(repo_root).resolve()
    
    # Get relative path from repo root
    try:
        rel_path = build_dir.relative_to(repo_root)
    except ValueError:
        # Path is not relative to repo root, use absolute path
        rel_path = build_dir
    
    # Normalize path separators for matching
    rel_path_str = str(rel_path).replace('\\', '/')
    
    # Get current platform
    current_platform = platform.system().lower()
    
    # Parse the build naming file
    mappings = parse_build_naming_file(repo_root)
    
    # Check if we have mappings for current platform
    if current_platform not in mappings:
        return None
    
    # Try to match patterns
    for compiler_num, pattern in mappings[current_platform]:
        # Build full pattern: platform/pattern_value
        full_pattern = f"{current_platform}/{pattern}"
        
        # Normalize pattern separators
        full_pattern = full_pattern.replace('\\', '/')
        
        # Check if pattern is in the path
        if full_pattern in rel_path_str:
            return compiler_num
    
    return None


#------------------------------------------------------------------------------
def run( arguments ):
    """Main entry point"""
    
    # Get input path
    input_path = Path(arguments['PATH'])
    
    # If relative path, make it relative to current working directory
    if not input_path.is_absolute():
        input_path = Path.cwd() / input_path
    
    if not input_path.exists():
        print(f"Error: Path does not exist: {arguments['PATH']}", file=sys.stderr)
        return 1
    
    # Default the projects/ dir path to the current working directory
    utils.set_pkg_and_wrkspace_roots( os.getcwd() )
    repo_root = NQBP_PKG_ROOT()

    # Handle compiler subcommand
    if arguments['compiler']:
        # Verify the path is a directory
        if not input_path.is_dir():
            if arguments['-v']:
                print(f"Error: Path is not a directory: {arguments['PATH']}", file=sys.stderr)
            return 1
        
        # Verify the directory contains nqbp.py
        nqbp_file = input_path / 'nqbp.py'
        if not nqbp_file.exists():
            if arguments['-v']:
                print(f"Error: Directory does not contain nqbp.py: {arguments['PATH']}", file=sys.stderr)
            return 1
        
        compiler_num = find_compiler_option(input_path, repo_root)
        
        if compiler_num is None:
            if arguments['-v']:
                print(f"Error: No compiler mapping found for: {arguments['PATH']}", file=sys.stderr)
            return 1
        
        # Output just the compiler number
        print(compiler_num)
        return 0
    
    # Handle buildirs subcommand
    if arguments['buildirs']:
        # Find build projects
        platform_matches, non_matches = find_build_projects(input_path, repo_root)
        
        # Combine for display
        all_projects = platform_matches + non_matches
        
        if not all_projects:
            if arguments['-v']:
                print("No nqbp.py files found.", file=sys.stderr)
            return 1
        
        # Display results
        if arguments['-v']:
            # Verbose output with headers and numbering
            for i, project_dir in enumerate(all_projects, 1):
                # Show path relative to repo root if possible
                try:
                    rel_path = project_dir.relative_to(repo_root)
                    print(f"{i}. {rel_path}")
                except ValueError:
                    print(f"{i}. {project_dir}")
        else:
            # Default bare output - just directory names
            for project_dir in all_projects:
                try:
                    rel_path = project_dir.relative_to(repo_root)
                    print(rel_path)
                except ValueError:
                    print(project_dir)
        
        return 0
    
    return 1


#------------------------------------------------------------------------------
# BEGIN
if __name__ == '__main__':
    arguments = docopt(__doc__, version=SANCHO_VERSION)
    sys.exit( run(arguments) )
