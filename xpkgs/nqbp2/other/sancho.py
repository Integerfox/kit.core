#!/usr/bin/python3
r"""
General purpose utility for integrating NQBP into other tools, e.g. VSCode
===============================================================================
usage: sancho [options] build-dirs PATH
       sancho [options] compiler PATH

Arguments:
    build-dirs  Generates a list of nqbp build project directories that might
                build/use the specified file or directory. The build directories
                are listed in the 'most relevant' order. 
    compiler    Looks up the compiler option number for env.bat/env.sh based
                on the build directory path.  NOTE: PATH must be a build
                directory that contains an nqbp.py file.
    PATH        Path to file or directory (absolute or relative to 
                repository root directory).

Options:
    -c CONFIG   Configuration file to use (relative to repository root)
                [default: .nqbp-configuration.json]
    -v          Verbose output
    -h, --help  Display help

Examples:
    ; Find build projects for a specific file
    sancho.py build-dirs src/Kit/System/Api.h
    
    ; Find compiler option for a build directory
    sancho.py compiler src/Kit/Container/_0test/_0build/windows/msvc
    
"""

import os
import sys
import json
import platform
from pathlib import Path

sys.path.append( os.path.dirname(__file__) + os.sep + ".." )
from nqbplib.docopt.docopt import docopt
from nqbplib import utils
from nqbplib.my_globals import NQBP_PKG_ROOT

SANCHO_VERSION = '1.0'

#------------------------------------------------------------------------------
def parse_exclude_list(repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Parse the configuration file to get exclude patterns.
    
    The file contains a JSON object with an "exclude-list" array.
    
    Args:
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
    Returns:
        List of exclude patterns (strings)
    """
    config_file = Path(repo_root) / config_file_name
    
    if not config_file.exists():
        return []
    
    try:
        with open(config_file, 'r') as f:
            config = json.load(f)
            
            # Get exclude-list array
            exclude_patterns = config.get('exclude-list', [])
            
            # Normalize path separators
            exclude_patterns = [p.replace('\\', '/') for p in exclude_patterns]
            
            return exclude_patterns
    except (json.JSONDecodeError, IOError):
        return []


def find_nqbp_files_recursive(directory, repo_root=None, exclude_patterns=None):
    """
    Recursively search for nqbp.py files in a directory tree.
    
    Args:
        directory: Path to search
        repo_root: Repository root path 
        exclude_patterns: List of path patterns to exclude
        
    Returns:
        List of directories containing nqbp.py
    """
    results = []
    directory = Path(directory).resolve()
    
    if not directory.exists() or not directory.is_dir():
        return results
    
    # Default exclude patterns to empty list if not provided
    if exclude_patterns is None:
        exclude_patterns = []
    
    try:
        for root, dirs, files in os.walk(directory):
            root_path = Path(root)
            
            # Check if current path matches any exclude pattern
            if repo_root and exclude_patterns:
                try:
                    rel_path = str(root_path.relative_to(repo_root)).replace('\\', '/')
                    should_exclude = False
                    for pattern in exclude_patterns:
                        # Check if the path starts with the exclude pattern
                        if rel_path.startswith(pattern):
                            should_exclude = True
                            break
                    
                    if should_exclude:
                        dirs[:] = []  # Don't recurse into subdirectories
                        continue
                except ValueError:
                    pass  # Path not relative to repo_root
            
            # Skip hidden directories and common ignore patterns
            dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['node_modules', '__pycache__']]
            
            # Sort directories alphabetically for consistent traversal order
            dirs.sort()
            
            if 'nqbp.py' in files:
                results.append(root_path)
    except PermissionError:
        pass  # Skip directories we can't read
    
    # Sort results alphabetically for consistent output
    results.sort()
    
    return results


def separate_test_directories(project_dirs, repo_root):
    """
    Separate project directories into test directories and non-test directories.
    Test directories are those whose path relative to repo_root starts with 'tests/'.
    
    Args:
        project_dirs: List of Path objects to build directories
        repo_root: Repository root path
        
    Returns:
        Tuple of (non_test_dirs, test_dirs)
    """
    repo_root = Path(repo_root).resolve()
    test_dirs = []
    non_test_dirs = []
    
    for project_dir in project_dirs:
        # Get relative path from repo root
        try:
            rel_path = project_dir.relative_to(repo_root)
            rel_path_str = str(rel_path).replace('\\', '/')
            
            # Check if path starts with 'tests/'
            if rel_path_str.startswith('tests/'):
                test_dirs.append(project_dir)
            else:
                non_test_dirs.append(project_dir)
        except ValueError:
            # Path is not relative to repo root, treat as non-test
            non_test_dirs.append(project_dir)
    
    return non_test_dirs, test_dirs


def parse_active_projects_file(repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Parse the configuration file to get active project patterns from JSON.
    
    The file contains a JSON object with an "active-projects" array.
    
    Args:
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
    Returns:
        List of partial path patterns (strings)
    """
    config_file = Path(repo_root) / config_file_name
    
    if not config_file.exists():
        return []
    
    try:
        with open(config_file, 'r') as f:
            config = json.load(f)
            
            # Get active-projects array
            patterns = config.get('active-projects', [])
            
            # Normalize path separators
            patterns = [p.replace('\\', '/') for p in patterns]
            
            return patterns
    except (json.JSONDecodeError, IOError):
        return []


def separate_active_projects(project_dirs, input_dir, repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Separate project directories into active projects and other projects.
    
    Active projects are build directories whose path (relative to repo root)
    starts with any of the patterns from the configuration file.
    
    Args:
        project_dirs: List of Path objects to build directories
        input_dir: The input directory being queried (unused, kept for API compatibility)
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
    Returns:
        Tuple of (active_dirs, other_dirs)
    """
    repo_root = Path(repo_root).resolve()
    
    # Parse active projects file
    active_patterns = parse_active_projects_file(repo_root, config_file_name)
    
    if not active_patterns:
        # No active projects file or empty, return all as "other"
        return [], list(project_dirs)
    
    # Separate directories based on whether they match any active pattern
    active_dirs = []
    other_dirs = []
    
    for project_dir in project_dirs:
        try:
            rel_path = str(project_dir.relative_to(repo_root)).replace('\\', '/')
            
            # Check if this build directory starts with any active pattern
            is_active = False
            for pattern in active_patterns:
                if rel_path.startswith(pattern):
                    is_active = True
                    break
            
            if is_active:
                active_dirs.append(project_dir)
            else:
                other_dirs.append(project_dir)
        except ValueError:
            other_dirs.append(project_dir)
    
    return active_dirs, other_dirs


def find_build_projects(file_path, repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Find all nqbp.py build projects in the repository.
    
    Walks the entire repository (except for the exclude list) and generates a 
    list of directories containing nqbp.py, separated into platform matches and
    non-matches. Both lists are then sorted by proximity to the input file/directory.
    each platform group.
    
    Args:
        file_path: Path to file or directory to use as reference for proximity sorting
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
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
    
    # Get exclude patterns from configuration
    exclude_patterns = parse_exclude_list(repo_root, config_file_name)
    
    # Find all nqbp.py directories in the entire repo (excluding patterns in exclude-list)
    all_nqbp_dirs = find_nqbp_files_recursive(repo_root, repo_root, exclude_patterns)
    
    # Separate into active projects (always first) and other projects
    active_dirs, other_dirs = separate_active_projects(all_nqbp_dirs, input_dir, repo_root, config_file_name)
    
    # Process other directories (apply all sorting criteria)
    non_test_dirs, test_dirs = separate_test_directories(other_dirs, repo_root)
    platform_matches_non_test, non_matches_non_test = separate_platform_matches(non_test_dirs)
    platform_matches_test, non_matches_test = separate_platform_matches(test_dirs)
    
    platform_matches_non_test = sort_by_proximity(platform_matches_non_test, input_dir, repo_root)
    non_matches_non_test = sort_by_proximity(non_matches_non_test, input_dir, repo_root)
    platform_matches_test = sort_by_proximity(platform_matches_test, input_dir, repo_root)
    non_matches_test = sort_by_proximity(non_matches_test, input_dir, repo_root)
    
    # Combine: test directories first, then non-test directories
    platform_matches = platform_matches_test + platform_matches_non_test
    non_matches = non_matches_test + non_matches_non_test
    
    # Prepend active projects to platform_matches (they always come first)
    platform_matches = active_dirs + platform_matches
    
    # Return as two separate lists
    return platform_matches, non_matches


def sort_by_proximity(project_dirs, input_dir, repo_root):
    """
    Sort build directories by how many parent directories they share with the input directory.
    
    The comparison excludes the top-level root directory (e.g., src/, tests/, projects/)
    from both paths before counting matching directory components.
    
    More matching directories = higher priority (listed first).
    Within the same match count, directories are sorted alphabetically.
    
    Args:
        project_dirs: List of Path objects to build directories
        input_dir: Reference directory for matching calculation
        repo_root: Repository root path
        
    Returns:
        Sorted list of Path objects, most matches first
    """
    def get_path_without_root(path):
        """
        Get the path components excluding the top-level root directory.
        E.g., /repo/src/Kit/Container -> ['Kit', 'Container']
        """
        try:
            rel_path = path.relative_to(repo_root)
            parts = rel_path.parts
            # Skip the first component (src/, tests/, projects/, etc.)
            return list(parts[1:]) if len(parts) > 1 else []
        except ValueError:
            # Path not relative to repo_root, return empty
            return []
    
    def count_matching_components(build_dir, ref_dir):
        """
        Count how many directory components match between the two paths,
        excluding their top-level root directories.
        """
        build_parts = get_path_without_root(build_dir)
        ref_parts = get_path_without_root(ref_dir)
        
        # Count how many components from ref_parts appear in build_parts (in order)
        matches = 0
        for ref_part in ref_parts:
            if ref_part in build_parts:
                matches += 1
        
        return matches
    
    # Calculate match count for each directory and pair it with the directory
    dirs_with_matches = [(count_matching_components(project_dir, input_dir), project_dir) 
                         for project_dir in project_dirs]
    
    # Sort by match count descending (more matches first), then alphabetically by path
    dirs_with_matches.sort(key=lambda x: (-x[0], x[1]))
    
    # Return just the directories
    return [d[1] for d in dirs_with_matches]


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


def parse_build_naming_file(repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Parse the configuration file to get compiler mappings from JSON.
    
    The file contains a JSON object with a "build-naming" array.
    Each element in the array contains platform mappings where:
    - Keys are compiler names (e.g., "msvc", "gcc-host")
    - Values are compiler numbers
    
    Args:
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
    Returns:
        Dictionary mapping platform -> dict of {pattern: compiler_number}
    """
    config_file = Path(repo_root) / config_file_name
    
    if not config_file.exists():
        return {}
    
    try:
        with open(config_file, 'r') as f:
            config = json.load(f)
            
            # Get build-naming array
            build_naming = config.get('build-naming', [])
            
            # Convert to the format expected by find_compiler_option
            # Platform -> {pattern: compiler_number}
            mappings = {}
            for platform_obj in build_naming:
                for platform, compiler_map in platform_obj.items():
                    mappings[platform.lower()] = compiler_map
            
            return mappings
    except (json.JSONDecodeError, IOError):
        return {}


def find_compiler_option(build_dir_path, repo_root, config_file_name='.nqbp-configuration.json'):
    """
    Find the compiler option number for a given build directory.
    
    Args:
        build_dir_path: Path to the build directory
        repo_root: Repository root path
        config_file_name: Name of configuration file (relative to repo_root)
        
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
    mappings = parse_build_naming_file(repo_root, config_file_name)
    
    # Check if we have mappings for current platform
    if current_platform not in mappings:
        return None
    
    # Try to match patterns
    # mappings[platform] is a dict of {compiler_name: compiler_number}
    for pattern, compiler_num in mappings[current_platform].items():
        # Build full pattern: platform/pattern_value
        full_pattern = f"{current_platform}/{pattern}"
        
        # Normalize pattern separators
        full_pattern = full_pattern.replace('\\', '/')
        
        # Check if pattern is in the path
        if full_pattern in rel_path_str:
            return str(compiler_num)
    
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
    
    # Get configuration file name
    config_file_name = arguments['-c']

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
        
        compiler_num = find_compiler_option(input_path, repo_root, config_file_name)
        
        if compiler_num is None:
            if arguments['-v']:
                print(f"Error: No compiler mapping found for: {arguments['PATH']}", file=sys.stderr)
            return 1
        
        # Output just the compiler number
        print(compiler_num)
        return 0
    
    # Handle build-dirs subcommand
    if arguments['build-dirs']:
        # Find build projects
        platform_matches, non_matches = find_build_projects(input_path, repo_root, config_file_name)
        
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
