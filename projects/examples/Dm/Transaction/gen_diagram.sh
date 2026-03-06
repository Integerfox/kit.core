#!/bin/bash
# Generates a PNG from the PlantUML diagram.
# Output: ClassDiagram.png (same directory as this script)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PUML_FILE="$SCRIPT_DIR/diagram.puml"
OUT_DIR="$SCRIPT_DIR"

plantuml -tpng -o "$OUT_DIR" "$PUML_FILE" && echo "Generated: $OUT_DIR/ClassDiagram.png"
