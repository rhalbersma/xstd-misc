#!/usr/bin/env python3
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

"""Deletes each clause of each constraint in turn, and reports the deletions no test notices.

A concept or a requires-clause emits no code for a mutation tester to change, so its mutants are made here instead:
for every concept definition and requires-clause in a public header, each operand of an `and` or an `or`, each
`not`, and each requirement of a requires-expression is deleted on its own. The test sources are then compiled,
without being run, on each compiler named. A mutant is killed when one of them no longer compiles; one that every
compiler accepts survives, and is either answered by a test with an existing type or accepted in
tools/delete-clauses-survivors.toml, with the reason no existing type fails that clause alone. The run fails on a
survivor the file does not accept, and on an accepted one that a test now kills or that no longer exists.

    tools/delete-clauses.py                         every header, on g++-15 and on clang++-22 with libc++
    tools/delete-clauses.py include/xstd/misc/concepts/proxy_reference.hpp
"""

import concurrent.futures
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import tomllib

ROOT = pathlib.Path(__file__).resolve().parent.parent

COMPILERS = [
    ["g++-15", "-std=c++2c"],
    ["clang++-22", "-std=c++2c", "-stdlib=libc++"],
]

OPEN, CLOSE = "({[", ")}]"

SURVIVORS = ROOT / "tools" / "delete-clauses-survivors.toml"


def blank_comments(text):
    """The text with each comment's characters turned to spaces, so offsets into it are offsets into the original."""
    return re.sub(r"//[^\n]*", lambda found: " " * len(found.group(0)), text)


def constraints(text):
    """The spans of every concept definition's right-hand side and every requires-clause's expression."""
    for found in re.finditer(r"\bconcept\s+\w+\s*=", text):
        yield found.end(), statement_end(text, found.end())
    # A requires-clause opens the line after a template head; a requires-expression opens anywhere else.
    for found in re.finditer(r"^[ \t]*template\s*<[^\n]*>[ \t]*\n[ \t]*requires\b", text, re.M):
        start = found.end()
        yield start, text.index("\n", start)


def statement_end(text, start):
    depth = 0
    for i in range(start, len(text)):
        depth += (text[i] in OPEN) - (text[i] in CLOSE)
        if text[i] == ";" and depth == 0:
            return i
    raise ValueError("unterminated constraint")


def operands(text, start, end):
    """Each deletable piece of text[start:end], as the span whose removal is the mutant."""
    depth, pieces, last = 0, [], start
    i = start
    while i < end:
        c = text[i]
        if c in OPEN:
            depth += 1
        elif c in CLOSE:
            depth -= 1
        elif depth == 0:
            word = re.match(r"\b(and|or)\b", text[i:end])
            if word and (i == 0 or not (text[i - 1].isalnum() or text[i - 1] == "_")):
                pieces.append((last, i))
                last = i + len(word.group(1))
                i = last
                continue
        i += 1
    pieces.append((last, end))
    if len(pieces) > 1:
        for k, (s, e) in enumerate(pieces):
            # The operand goes, with the operator that joins it to its neighbour.
            yield (s, pieces[k + 1][0]) if k + 1 < len(pieces) else (pieces[k - 1][1], e)
    for s, e in pieces:
        yield from inner(text, s, e)


def inner(text, start, end):
    """The mutants inside one operand: its negation, its parenthesized subexpressions, its requirements."""
    stripped = re.match(r"\s*", text[start:end]).end() + start
    negation = re.match(r"not\b\s*", text[stripped:end])
    if negation:
        yield stripped, stripped + negation.end()
        stripped += negation.end()
    i = stripped
    while i < end:
        if text[i] == "(" and not re.search(r"(requires|\w|>)\s*$", text[stripped:i]):
            close = matching(text, i)
            yield from operands(text, i + 1, close)
            i = close + 1
            continue
        if text[i] == "{" and re.search(r"requires\s*(\([^)]*\))?\s*$", text[start:i]):
            close = matching(text, i)
            yield from requirements(text, i + 1, close)
            i = close + 1
            continue
        i += 1


def requirements(text, start, end):
    depth, last = 0, start
    for i in range(start, end):
        depth += (text[i] in OPEN) - (text[i] in CLOSE)
        if text[i] == ";" and depth == 0:
            if text[last:i + 1].strip():
                yield last, i + 1
            last = i + 1


def matching(text, i):
    depth = 0
    for j in range(i, len(text)):
        depth += (text[j] in OPEN) - (text[j] in CLOSE)
        if depth == 0:
            return j
    raise ValueError("unbalanced")


def mutants(text):
    """Each span whose deletion is a mutant, or a span and the text that replaces it, for a constraint of one atom."""
    seen = set()
    code = blank_comments(text)
    for start, end in constraints(code):
        found = list(operands(code, start, end))
        if not found:
            # A constraint of one atom is deleted whole: the requires-clause goes, the concept becomes true.
            clause = code.rfind("requires", 0, start)
            found = [(clause, end) if code[start - 8:start] == "requires" else (start, end, " true")]
        for span in found:
            piece = code[span[0]:span[1]].strip()
            folded = code[span[1]:].lstrip().startswith("...") or piece.endswith("...")
            if span[:2] not in seen and piece and not folded:
                seen.add(span[:2])
                yield span


def compiles(compiler, source, include):
    command = [*compiler, "-fsyntax-only", f"-I{include}", f"-I{ROOT / 'test' / 'include'}", str(source)]
    return subprocess.run(command, capture_output=True).returncode == 0


def sources():
    return sorted((ROOT / "test" / "src").rglob("*.cpp"))


def verdict(header, text, span):
    with tempfile.TemporaryDirectory() as scratch:
        mirror = pathlib.Path(scratch) / "include"
        for original in (ROOT / "include").rglob("*.hpp"):
            copy = mirror / original.relative_to(ROOT / "include")
            copy.parent.mkdir(parents=True, exist_ok=True)
            copy.write_text(original.read_text())
        replacement = span[2] if len(span) > 2 else ""
        (mirror / header.relative_to(ROOT / "include")).write_text(text[:span[0]] + replacement + text[span[1]:])
        alone = pathlib.Path(scratch) / "alone.cpp"
        alone.write_text(f"#include <{header.relative_to(ROOT / 'include')}>\n")
        if not all(compiles(compiler, alone, mirror) for compiler in COMPILERS):
            return "invalid: the header itself no longer compiles"
        for compiler in COMPILERS:
            for source in sources():
                if not compiles(compiler, source, mirror):
                    return f"killed by {source.relative_to(ROOT)} on {compiler[0]}"
    return None


def accepted():
    """Each accepted survivor, as its header's path and its deleted text with whitespace collapsed."""
    with SURVIVORS.open("rb") as file:
        entries = tomllib.load(file)["survivor"]
    return {(entry["header"], " ".join(entry["deletes"].split())) for entry in entries}


def main():
    headers = [pathlib.Path(arg).resolve() for arg in sys.argv[1:]] or sorted((ROOT / "include").rglob("*.hpp"))
    expected = {key for key in accepted() if ROOT / key[0] in headers}
    for compiler in COMPILERS:
        failing = [s for s in sources() if not compiles(compiler, s, ROOT / "include")]
        if failing:
            sys.exit(f"{compiler[0]} rejects the unmutated {failing[0].relative_to(ROOT)}")
    jobs = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=os.cpu_count()) as pool:
        for header in headers:
            text = header.read_text()
            code = blank_comments(text)
            for span in mutants(text):
                line = text.count("\n", 0, span[0]) + 1
                deleted = " ".join(code[span[0]:span[1]].split())
                jobs.append((header, line, deleted, pool.submit(verdict, header, text, span)))
        survived, unaccepted = set(), 0
        for header, line, deleted, job in jobs:
            result = job.result()
            key = (str(header.relative_to(ROOT)), deleted)
            if result is None:
                survived.add(key)
                unaccepted += key not in expected
                status = "accepted" if key in expected else "SURVIVES"
            else:
                status = "INVALID" if result.startswith("invalid") else "killed"
            print(f"{status} {key[0]}:{line} deleting `{deleted}`" + (f": {result}" if result else ""))
    stale = sorted(expected - survived)
    for header, deleted in stale:
        print(f"STALE {header} deleting `{deleted}`: no longer survives, so its entry in {SURVIVORS.name} goes")
    print(f"{len(jobs)} mutants, {len(survived)} surviving, {unaccepted} of them unaccepted, {len(stale)} stale")
    return 1 if unaccepted or stale else 0


if __name__ == "__main__":
    sys.exit(main())
