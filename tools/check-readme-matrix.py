#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Checks that the README's CI matrix names the rungs cpp-ci's toolchain table puts the ladders on.

The workflows call cpp-ci's at one commit, and its toolchain action resolves each rung from toolchains.json there.
Each row of the README's table names a toolchain family and, per rung, the release on it, or a dash for a rung the
family does not fill. Emscripten's row is left out: its version is an input of its own workflow, not a rung.

    tools/check-readme-matrix.py <toolchains.json>
"""

import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent

RUNGS = ["stable", "qualification", "development"]

# The README's (platform, compiler, standard library) row, and the family of the table it follows.
FAMILIES = {
    ("Linux", "GCC", "libstdc++"): "gcc",
    ("Windows", "MinGW", "libstdc++"): "mingw",
    ("Linux", "Clang", "libstdc++"): "clang",
    ("Linux", "Clang", "libc++"): "clang",
    ("macOS", "Apple Clang", "libc++"): "apple-clang",
    ("Windows", "Clang-CL", "MSVC"): "msvc",
    ("Windows", "MSVC", "MSVC"): "msvc",
}


def rows(readme):
    for line in readme.splitlines():
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        if len(cells) == 7 and tuple(cells[:3]) in FAMILIES:
            yield tuple(cells[:3]), dict(zip(RUNGS, cells[3:6]))


def names(cell, label):
    return re.search(r"(?<![\w.-])" + re.escape(label) + r"(?![\w.-])", cell) is not None


def libstdcxx(entry):
    return entry["gcc_version"] + ("-SVN" if entry["gcc_trunk"] == "true" else "")


def check(readme, table):
    failures = 0
    seen = set()
    for row, cells in rows(readme):
        seen.add(row)
        family = table[FAMILIES[row]]
        for rung, cell in cells.items():
            entry = family.get(rung)
            where = f"{' / '.join(row)}, {rung}: the README says {cell!r}"
            if entry is None:
                wrong = cell != "—" and f"{FAMILIES[row]} has no {rung} rung"
            elif cell == "—":
                wrong = f"{FAMILIES[row]} puts {entry['label']} there"
            elif not names(cell, entry["label"]):
                wrong = f"{FAMILIES[row]} puts {entry['label']} there"
            elif row == ("Linux", "Clang", "libstdc++") and not names(cell, f"libstdc++ {libstdcxx(entry)}"):
                wrong = f"that Clang builds on libstdc++ {libstdcxx(entry)}"
            else:
                wrong = None
            if wrong:
                print(f"FAIL {where}, and {wrong}")
                failures += 1
            else:
                print(f"pass {where}")
    for row in FAMILIES.keys() - seen:
        print(f"FAIL {' / '.join(row)}: no such row in the README's table")
        failures += 1
    return failures


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    table = json.loads(pathlib.Path(sys.argv[1]).read_text())
    return 1 if check((ROOT / "README.md").read_text(), table) else 0


if __name__ == "__main__":
    sys.exit(main())
