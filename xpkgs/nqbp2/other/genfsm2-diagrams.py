#!/usr/bin/env python3
"""PlantUML -> SVG generator for StateSmith FSM diagrams.

Converts every `.puml`/`.plantuml` file in a directory to an SVG that keeps the
source file's name (e.g. `Fsm.puml` -> `Fsm.svg`), so StateSmith FSM diagrams
keep their filename. A white background is injected so the diagram stays readable
on dark GitHub / VS Code themes. Only sources whose `.svg` is missing or older
than the source are rebuilt unless `--force` is given.

`genfsm2.py` invokes this (via `generate(<dir>)`) after generating FSM code so
the diagram's SVG is kept in sync; it can also be run directly on one or more
DIRs.

Requires PlantUML (and Java). Install once:
    sudo apt-get install -y plantuml
(or set the PLANTUML_JAR env var to a plantuml.jar; needs `java`).
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

WHITE = "#ffffff"


def find_plantuml():
    """Command prefix to run PlantUML: ['plantuml'] or ['java', '-jar', <jar>].
    Exits with a clear message if neither is available."""
    exe = shutil.which("plantuml")
    if exe:
        return [exe]
    candidates = []
    if os.environ.get("PLANTUML_JAR"):
        candidates.append(os.environ["PLANTUML_JAR"])
    candidates += [
        "/usr/share/plantuml/plantuml.jar",
        "/usr/local/share/plantuml/plantuml.jar",
        str(Path.home() / "plantuml.jar"),
    ]
    java = shutil.which("java")
    for jar in candidates:
        if jar and Path(jar).is_file():
            if not java:
                sys.exit("error: found a PlantUML jar but 'java' is not installed.")
            return [java, "-jar", jar]
    sys.exit(
        "error: PlantUML not found (needed to convert .puml/.plantuml files).\n"
        "       Install it with:  sudo apt-get install -y plantuml\n"
        "       or set PLANTUML_JAR to a plantuml.jar (requires 'java')."
    )


def add_white_background(svg: Path) -> None:
    text = svg.read_text(encoding="utf-8")
    open_tag = re.search(r"<svg\b[^>]*>", text)
    if not open_tag:
        return
    vb = re.search(r'viewBox="([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)"', open_tag.group(0))
    if vb:
        x, y, w, h = vb.groups()
    else:
        x = y = "0"
        w = h = "100%"
    rect = f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{WHITE}"/>'
    at = open_tag.end()
    svg.write_text(text[:at] + rect + text[at:], encoding="utf-8")


def _stale(src: Path, target: Path, force: bool) -> bool:
    """True when `target` must be (re)generated: forced, missing, or older than src."""
    return force or not target.exists() or src.stat().st_mtime > target.stat().st_mtime


def generate(directory, force: bool = False) -> int:
    """Convert stale .puml/.plantuml sources in `directory` to SVG (same stem)."""
    directory = Path(directory).resolve()
    sources = [
        s for s in sorted(set(directory.glob("*.puml")) | set(directory.glob("*.plantuml")))
        if _stale(s, directory / (s.stem + ".svg"), force)
    ]
    if not sources:
        return 0
    cmd = find_plantuml()
    for src in sources:
        with tempfile.TemporaryDirectory() as outdir:
            out = Path(outdir)
            result = subprocess.run(
                cmd + ["-tsvg", "-o", str(out), str(src)],
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                sys.stderr.write(result.stdout + result.stderr)
                result.check_returncode()
            # PlantUML names each SVG after its @startuml name (or the source
            # file). Keep that name as-is (case preserved).
            for produced in sorted(out.glob("*.svg")):
                dest = directory / produced.name
                shutil.move(str(produced), str(dest))
                add_white_background(dest)
                print(f"generated {dest.relative_to(directory)}")
    return 0


def cli(argv, directory=None) -> int:
    """Command-line entry. Parses --force/--help (and, when `directory` is None,
    one or more DIR arguments). Per-directory wrappers pass their own directory."""
    parser = argparse.ArgumentParser(
        description=("Convert PlantUML (.puml/.plantuml) sources to SVG, keeping the "
                     "source filename. Only sources whose .svg is missing or older "
                     "than the source are rebuilt unless --force is given."),
    )
    parser.add_argument(
        "-f", "--force", action="store_true",
        help="regenerate every SVG, even if it is already up to date",
    )
    if directory is None:
        parser.add_argument(
            "dirs", nargs="+", metavar="DIR",
            help="directory containing .puml/.plantuml sources to convert",
        )
    args = parser.parse_args(argv[1:])
    directories = [Path(directory)] if directory is not None else [Path(d) for d in args.dirs]
    rc = 0
    for d in directories:
        rc |= generate(d, args.force)
    return rc


def main(argv) -> int:
    return cli(argv)


if __name__ == "__main__":
    sys.exit(main(sys.argv))
