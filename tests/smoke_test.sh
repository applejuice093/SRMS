#!/usr/bin/env bash
# End-to-end smoke test: drives the menu through stdin in a temp directory.
set -euo pipefail
BIN="$(cd "$(dirname "$0")/.." && pwd)/SRMS"
WORK="$(mktemp -d)"; trap 'rm -rf "$WORK"' EXIT
cd "$WORK"

# Add two students, try a duplicate ID, search, delete one, exit.
printf '1\n101\nAsha Rao\n20\nCS\n8.7\n1\n102\nRavi|K\nRavi K\n21\nEE\n11\n7.9\n1\n101\n103\nMeera\n19\nME\n9.1\n3\n102\n4\n101\ny\n5\n' \
  | "$BIN" > out.txt

grep -q "Student Found" out.txt
grep -q "cannot contain '|'" out.txt
grep -q "GPA must be between 0 and 10" out.txt
grep -q "That ID already exists" out.txt
[ "$(wc -l < students.txt)" -eq 2 ] || { echo "expected 2 records"; cat students.txt; exit 1; }
! grep -q '^101|' students.txt
grep -q '^102|Ravi K|21|EE|7.9' students.txt

# Closing stdin must not hang.
timeout 5 "$BIN" < /dev/null > /dev/null
echo "smoke test passed"
