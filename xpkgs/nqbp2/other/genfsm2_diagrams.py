#!/usr/bin/env python3
"""PlantUML -> SVG generator for StateSmith FSM diagrams.

Converts every `.puml`/`.plantuml` file in a directory to an SVG. PlantUML names
each SVG after the diagram's `@startuml <name>` directive, falling back to the
source file's stem when the directive has no name - so `@startuml Fsm` inside
`Fsm.puml` yields `Fsm.svg`. A white background is injected so the diagram stays
readable on dark GitHub / VS Code themes. Only sources whose SVG(s) are missing
or older than the source are rebuilt unless `--force` is given.

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

# `@startuml`, optionally followed by the diagram name that PlantUML uses to name
# its output file
STARTUML = re.compile(r"^\s*@startuml(?:[ \t]+(\S+))?", re.MULTILINE)


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


def _svg_targets(src: Path) -> list:
    """The SVG file(s) PlantUML will emit for `src`. Must match PlantUML's naming
    rule, otherwise the staleness check below never finds them and the source is
    needlessly re-rendered on every run."""
    names = STARTUML.findall(src.read_text(encoding="utf-8", errors="replace"))
    if not names:
        return [src.with_suffix(".svg")]
    targets = []
    for index, name in enumerate(names):
        if name:
            targets.append(src.with_name(name + ".svg"))
        elif index == 0:
            targets.append(src.with_suffix(".svg"))
        else:
            targets.append(src.with_name(f"{src.stem}_{index:03d}.svg"))
    return targets


def _stale(src: Path, force: bool) -> bool:
    """True when `src` must be (re)rendered: forced, or any target SVG is missing
    or older than the source."""
    if force:
        return True
    mtime = src.stat().st_mtime
    return any(not t.exists() or mtime > t.stat().st_mtime for t in _svg_targets(src))


def generate(directory, force: bool = False) -> int:
    """Render the stale .puml/.plantuml sources in `directory` to SVG."""
    directory = Path(directory).resolve()
    sources = [
        s for s in sorted(set(directory.glob("*.puml")) | set(directory.glob("*.plantuml")))
        if _stale(s, force)
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


def main(argv) -> int:
    parser = argparse.ArgumentParser(
        description=("Convert PlantUML (.puml/.plantuml) sources to SVG. Each SVG is "
                     "named after the diagram's '@startuml <name>' directive, or the "
                     "source file's stem when the directive has no name. Only sources "
                     "whose SVG is missing or older than the source are rebuilt unless "
                     "--force is given."),
    )
    parser.add_argument(
        "-f", "--force", action="store_true",
        help="regenerate every SVG, even if it is already up to date",
    )
    parser.add_argument(
        "dirs", nargs="+", metavar="DIR",
        help="directory containing .puml/.plantuml sources to convert",
    )
    args = parser.parse_args(argv[1:])
    rc = 0
    for d in args.dirs:
        rc |= generate(Path(d), args.force)
    return rc


if __name__ == "__main__":
    sys.exit(main(sys.argv))
