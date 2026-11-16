#!/usr/bin/env python3
"""
Checks that every C/C++ file a change touches is formatted per the repo-root
.clang-format file (KIT coding standard: touched files shall be formatted).

A 'touched' file is one that was added, copied, modified, or renamed relative
to the merge-base with the base branch.  Uncommitted and untracked files are
included so the script can be run locally before committing.  Third-party and
auto-generated files (see EXCLUDED_PATTERNS) are never checked.

Usage: check_clang_format.py [--base <ref>] [--fix] [--all]

Options:
    --base <ref>   Branch/ref to compare against [default: origin/main]
    --fix          Reformat the touched files in place instead of checking
    --all          Check all KIT-owned C/C++ files, not just touched files

Exit code is 0 when all checked files are formatted, 1 otherwise.
"""

import argparse
import fnmatch
import os
import re
import shutil
import subprocess
import sys

MIN_CLANG_FORMAT_MAJOR = 19
SOURCE_EXTENSIONS      = ( '.h', '.hh', '.hpp', '.c', '.cc', '.cpp', '.cxx' )

# Third-party and auto-generated code (fnmatch patterns, '*' also matches '/')
EXCLUDED_PATTERNS = (
    'xpkgs/*',
    'projects/xpkgs/*',
    'src/Kit/Bsp/*/MX/*',                           # ST CubeMX generated
    'src/Kit/Bsp/ST/*/freertos-*.c',                # ST CubeMX generated
    'src/Kit/Bsp/*/SeggerSysView/*',                # SEGGER SystemView
    'src/Kit/Bsp/*.pio.h',                          # pioasm generated
    'src/Kit/Checksum/md5_aladdin_.c',              # Aladdin MD5
    'src/Kit/Checksum/_0test/_aladdin/*',           # Aladdin MD5
    'src/Kit/Json/Arduino.h',                       # ArduinoJson
    'src/Kit/Type/BetterEnum*',                     # Better Enums source/generated
    'src/Kit/_support/statesmith/_0test/MyFsm.*',   # StateSmith generated
)


#------------------------------------------------------------------------------
def git( *args ):
    r = subprocess.run( [ 'git' ] + list( args ), capture_output=True, text=True )
    if r.returncode != 0:
        sys.exit( f"ERROR: git {' '.join( args )} failed:\n{r.stderr.strip()}" )
    return [ l for l in r.stdout.splitlines() if l.strip() ]


def is_candidate( path ):
    path = path.replace( '\\', '/' )
    if not path.endswith( SOURCE_EXTENSIONS ) or not os.path.isfile( path ):
        return False
    return not any( fnmatch.fnmatchcase( path, p ) for p in EXCLUDED_PATTERNS )


def touched_files( base ):
    merge_base = git( 'merge-base', base, 'HEAD' )[0]
    files      = git( 'diff', '--name-only', '--diff-filter=ACMR', merge_base )
    files     += git( 'ls-files', '--others', '--exclude-standard' )
    return sorted( { f for f in files if is_candidate( f ) } )


def all_files():
    files = git( 'ls-files', '--cached', '--others', '--exclude-standard' )
    return sorted( { f for f in files if is_candidate( f ) } )


def check_clang_format_version( exe ):
    out = subprocess.run( [ exe, '--version' ], capture_output=True, text=True ).stdout
    m   = re.search( r'version\s+(\d+)\.', out )
    if not m or int( m.group( 1 ) ) < MIN_CLANG_FORMAT_MAJOR:
        sys.exit( f"ERROR: clang-format >= {MIN_CLANG_FORMAT_MAJOR} is required (found: {out.strip() or 'unknown'}).\n"
                  f"       Install with: pip install -r top/gha-requirements.txt" )
    return out.strip()


#------------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser( description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter )
    parser.add_argument( '--base', default='origin/main', help='ref to compare against (default: origin/main)' )
    parser.add_argument( '--fix', action='store_true', help='reformat the files in place' )
    parser.add_argument( '--all', action='store_true', help='check all KIT-owned C/C++ files' )
    args = parser.parse_args()

    # Paths are relative to the repository root
    os.chdir( git( 'rev-parse', '--show-toplevel' )[0] )

    exe = shutil.which( 'clang-format' )
    if exe is None:
        sys.exit( "ERROR: clang-format is not in the command path.  Install with: pip install -r top/gha-requirements.txt" )
    version = check_clang_format_version( exe )

    files = all_files() if args.all else touched_files( args.base )
    scope = 'all KIT-owned' if args.all else f'touched (vs {args.base})'
    print( f"{version}\nChecking {len( files )} {scope} C/C++ file(s)..." )
    if not files:
        return 0

    if args.fix:
        subprocess.run( [ exe, '-i', '--style=file' ] + files, check=True )
        print( "Reformatted:\n  " + "\n  ".join( files ) )
        return 0

    unformatted = []
    for f in files:
        r = subprocess.run( [ exe, '--dry-run', '--Werror', '--style=file', f ], capture_output=True, text=True )
        if r.returncode != 0:
            unformatted.append( f )

    if unformatted:
        print( "\nThe following file(s) are not clang-format clean (KIT coding standard):" )
        for f in unformatted:
            print( f"  {f}" )
        fix_opts = ( " --all" if args.all else "" ) + ( "" if args.base == 'origin/main' else f" --base {args.base}" )
        print( f"\nFix with:  python3 top/check_clang_format.py --fix{fix_opts}" )
        return 1

    print( "All checked files are clang-format clean." )
    return 0


if __name__ == '__main__':
    sys.exit( main() )
