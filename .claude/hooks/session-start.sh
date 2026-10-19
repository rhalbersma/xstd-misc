#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# A Claude Code cloud session's SessionStart hook: installs what CI builds with, through tools/setup-toolchain.sh, so
# the session starts on the stable rung rather than on noble's GCC 13. The script is idempotent, and a container that
# already ran it redoes little more than the GCC trunk snapshot. Its output goes to a log, not into the session.
set -euo pipefail

if [ "${CLAUDE_CODE_REMOTE:-}" != "true" ]; then
        exit 0
fi

log=/tmp/xstd-setup-toolchain.log
if ! "${CLAUDE_PROJECT_DIR}/tools/setup-toolchain.sh" > "${log}" 2>&1; then
        echo "tools/setup-toolchain.sh failed; the last lines of ${log}:"
        tail -n 20 "${log}"
        exit 1
fi

# CMake, gcovr and the unversioned clang-format come from PyPI into ~/.local/bin.
if [ -n "${CLAUDE_ENV_FILE:-}" ]; then
        echo "export PATH=\"${HOME:-/root}/.local/bin:\${PATH}\"" >> "${CLAUDE_ENV_FILE}"
fi
echo "The toolchain is installed (log: ${log}); tools/ci-local.sh runs CI's Linux legs on it."
