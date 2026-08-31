#!/usr/bin/env python3
"""
Generate PNG diagrams from PlantUML source files.

This script generates class diagram PNGs from the .puml files in this directory.
Run this script whenever the PlantUML source files are updated.

Usage:
    python3 generate-diagram.py
    or
    ./generate-diagram.py
"""

import subprocess
import sys
import os
from pathlib import Path


def generate_png( script_dir, puml_basename, png_basename ):
    """Generate a single PNG from a PlantUML file."""
    puml_file = os.path.join( script_dir, puml_basename )
    png_file = os.path.join( script_dir, png_basename )

    if not os.path.exists( puml_file ):
        print( f"Error: {puml_file} not found", file=sys.stderr )
        return False

    print( f"Generating PNG from {puml_basename}..." )

    try:
        result = subprocess.run(
            ["plantuml", "-tpng", str( puml_file )],
            cwd=str( script_dir ),
            capture_output=True,
            text=True
        )

        if os.path.exists( png_file ):
            print( f"  Successfully generated {png_basename}" )
            return True
        else:
            print( f"Error: Failed to generate {png_basename}", file=sys.stderr )
            if result.stdout:
                print( "STDOUT:", result.stdout, file=sys.stderr )
            if result.stderr:
                print( "STDERR:", result.stderr, file=sys.stderr )
            return False

    except FileNotFoundError:
        print( "Error: plantuml command not found. Please install PlantUML:", file=sys.stderr )
        print( "  - Ubuntu/Debian: sudo apt-get install plantuml", file=sys.stderr )
        print( "  - macOS: brew install plantuml", file=sys.stderr )
        print( "  - Or download from: https://plantuml.com/download", file=sys.stderr )
        return False
    except Exception as e:
        print( f"Error: {e}", file=sys.stderr )
        return False


def main():
    # Get the directory containing this script
    script_dir = Path( __file__ ).parent.absolute()

    # Define the diagrams to generate (puml source -> png output)
    # NOTE: PNG filenames must be globally/repo unique (Doxygen constraint)
    diagrams = [
        ( "class-diagram-flash.puml", "class-diagram-flash_kitdriverflash.png" ),
    ]

    success_count = 0
    fail_count = 0

    for puml_basename, png_basename in diagrams:
        if generate_png( str( script_dir ), puml_basename, png_basename ):
            success_count += 1
        else:
            fail_count += 1

    print( f"\nResults: {success_count} generated, {fail_count} failed" )
    return 0 if fail_count == 0 else 1


if __name__ == "__main__":
    sys.exit( main() )
