#!/usr/bin/env python3
"""
Generate PNG diagram from PlantUML source file.

This script generates the class diagram PNG from the class-diagram.puml file.
Run this script whenever the PlantUML source file is updated.

Usage:
    python3 generate-diagram.py
    or
    ./generate-diagram.py
"""

import subprocess
import sys
import os
from pathlib import Path


def main():
    # Get the directory containing this script
    script_dir = Path(__file__).parent.absolute()
    
    # Define the input and output files
    puml_file = os.path.join( script_dir , "class-diagram.puml")
    png_file = os.path.join( script_dir , "class-diagram.png")
    
    # Check if the input file exists
    if not os.path.exists(puml_file):
        print(f"Error: {puml_file} not found", file=sys.stderr)
        return 1
    
    print(f"Generating PNG from {os.path.basename(puml_file)}...")
    
    # Run plantuml to generate the PNG
    try:
        result = subprocess.run(
            ["plantuml", "-tpng", str(puml_file)],
            cwd=str(script_dir),
            capture_output=True,
            text=True
        )

        # PlantUML sometimes returns non-zero even on success, check if file was created
        if os.path.exists(png_file):
            # Use ASCII for Windows compatibility
            print(f"✓ Successfully generated {os.path.basename(png_file)}")
            return 0
        else:
            print(f"Error: Failed to generate PNG file", file=sys.stderr)
            if result.stdout:
                print("STDOUT:", result.stdout, file=sys.stderr)
            if result.stderr:
                print("STDERR:", result.stderr, file=sys.stderr)
            return 1

    except FileNotFoundError:
        print("Error: plantuml command not found. Please install PlantUML:", file=sys.stderr)
        print("  - Ubuntu/Debian: sudo apt-get install plantuml", file=sys.stderr)
        print("  - macOS: brew install plantuml", file=sys.stderr)
        print("  - Or download from: https://plantuml.com/download", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
