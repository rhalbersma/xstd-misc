#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Checks that what this library copies from the C++ working draft still reads as the draft does.

Each header below cites its source in a comment of the form `N5054 [stable.name]/7`. The script checks that the
checkout is that draft, that the stable name and paragraph exist there and hold the text, and that the code matches
it token for token once the namespaces, the alternative tokens and the exposition-only spellings are set aside.

    tools/check-draft-wording.py --docno           the one draft the headers cite, such as n5054
    tools/check-draft-wording.py <draft checkout>  the check, against a checkout of github.com/cplusplus/draft
"""

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent

# The header, then what it copies: an exposition-only concept by its name, or a Returns element, with the draft's
# template parameters renamed to the header's.
CHECKS = [
    ("include/xstd/misc/concepts/simple_allocator.hpp", {"concept": "simple-allocator"}),
    ("include/xstd/misc/concepts/container_compatible_range.hpp", {"concept": "container-compatible-range"}),
    ("include/xstd/misc/utility/to_underlying.hpp", {"returns": "to_underlying", "rename": {"T": "Enum"}}),
]

CITATION = re.compile(r"//.*\b(N\d{4}) \[([a-z.]+)\]/(\d+)")


def citation(header):
    text = (ROOT / header).read_text()
    found = CITATION.search(text)
    if not found:
        sys.exit(f"{header}: no citation of the form `N5054 [stable.name]/7`")
    return found.group(1), found.group(2), int(found.group(3)), text


def paragraph(draft, stable, number):
    for tex in sorted((draft / "source").glob("*.tex")):
        lines = tex.read_text().splitlines()
        heads = [i for i, line in enumerate(lines) if line.startswith("\\rSec")]
        for i in heads:
            if f"[{stable}]" in lines[i]:
                end = next((j for j in heads if j > i), len(lines))
                section = lines[i:end]
                starts = [k for k, line in enumerate(section) if line.strip() == "\\pnum"]
                if number > len(starts):
                    return None
                stop = starts[number] if number < len(starts) else len(section)
                return "\n".join(section[starts[number - 1]:stop])
    return None


def detex(text):
    text = re.sub(r"@\\(?:def)?(?:expos|lib)concept\{([^}]*)\}@", r"\1", text)
    text = re.sub(r"//\s*\\expos", "", text)
    text = re.sub(r"\\tcode\{(.*)\}", r"\1", text)
    return text.replace("@", "")


def tokens(text, rename=None):
    text = re.sub(r"//[^\n]*", "", text)
    text = re.sub(r"\b(?:x?std)::", "", text)
    text = re.sub(r"(?<=[a-z])-(?=[a-z])", "_", text)
    found = re.findall(r"[A-Za-z_]\w*|::|->|&&|\|\||\S", text)
    spelled = {"&&": "and", "||": "or", "!": "not"}
    rename = rename or {}
    return [rename.get(t, spelled.get(t, t)) for t in found]


def definition(text, name):
    found = re.search(r"template\s*<[^;]*?\bconcept\s+" + re.escape(name) + r"\b", text)
    if not found:
        return None
    depth = 0
    for i in range(found.end(), len(text)):
        depth += {"{": 1, "(": 1, "}": -1, ")": -1}.get(text[i], 0)
        if text[i] == ";" and depth == 0:
            return text[found.start():i + 1]
    return None


def returns(text):
    found = re.search(r"\\returns\s*(.*?)\.\s*$", text, re.S | re.M)
    return found.group(1) if found else None


def check(draft, docno):
    failures = 0
    for header, what in CHECKS:
        cited, stable, number, code = citation(header)
        where = f"{header}: {cited} [{stable}]/{number}"
        text = paragraph(draft, stable, number)
        if cited != docno:
            print(f"FAIL {where}: the checkout is {docno}")
        elif text is None:
            print(f"FAIL {where}: no such paragraph")
        elif "concept" in what:
            name = what["concept"]
            ours = definition(code, name.replace("-", "_"))
            theirs = definition(detex(text), name)
            if theirs is None:
                print(f"FAIL {where}: the paragraph does not define {name}")
            elif ours is None or tokens(ours) != tokens(theirs):
                print(f"FAIL {where}: {name} reads\n  {' '.join(tokens(theirs))}\nthere, and\n  {' '.join(tokens(ours or ''))}\nhere")
            else:
                print(f"pass {where}: {name}")
                continue
        else:
            theirs = returns(detex(text))
            ours = re.search(r"\breturn\s+([^;]*);", code)
            if theirs is None:
                print(f"FAIL {where}: the paragraph has no Returns element")
            elif ours is None or tokens(ours.group(1)) != tokens(theirs, what["rename"]):
                print(f"FAIL {where}: {what['returns']} returns\n  {theirs}\nthere, and\n  {ours.group(1) if ours else ''}\nhere")
            else:
                print(f"pass {where}: {what['returns']}'s Returns")
                continue
        failures += 1
    return failures


def main():
    cited = {citation(header)[0] for header, _ in CHECKS}
    if len(cited) != 1:
        sys.exit(f"the headers cite more than one draft: {', '.join(sorted(cited))}")
    if sys.argv[1:] == ["--docno"]:
        print(cited.pop().lower())
        return 0
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    draft = pathlib.Path(sys.argv[1])
    found = re.search(r"\\newcommand\{\\docno\}\{(N\d+)\}", (draft / "source" / "config.tex").read_text())
    if not found:
        sys.exit(f"{draft}: no \\docno in source/config.tex")
    return 1 if check(draft, found.group(1)) else 0


if __name__ == "__main__":
    sys.exit(main())
