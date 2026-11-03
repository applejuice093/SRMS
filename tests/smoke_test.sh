#!/usr/bin/env bash
# End-to-end smoke test: drives the menu through stdin in a temp directory.
set -euo pipefail
BIN="$(cd "$(dirname "$0")/.." && pwd)/SRMS"
WORK="$(mktemp -d)"; trap 'rm -rf "$WORK"' EXIT
cd "$WORK"

# Add two students, try a duplicate ID, search, delete one, exit.
printf '1\n101\nAsha Rao\n20\nCS\n8.7\n1\n102\nRavi|K\nRavi K\n21\nEE\n11\n7.9\n1\n101\n103\nMeera\n19\nME\n9.1\n3\n102\n4\n101\ny\n6\n' \
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

# --- Input edge cases ---------------------------------------------------
rm -f students.txt

# EOF in the middle of adding a student exits instead of looping forever.
printf '1\n55\n' | timeout 5 "$BIN" > /dev/null

# Trailing junk is rejected, non-positive IDs too, and text is trimmed.
printf '1\n12abc\n0\n12\n   \n  Asha Rao  \n20x\n20\n CS \n9.5\n2x\n6\n' | "$BIN" > out.txt
grep -q "Invalid input! Enter numeric ID" out.txt
grep -q "ID must be a positive number" out.txt
grep -q "Invalid! Enter numeric age" out.txt
grep -q "Invalid choice!" out.txt
grep -qx '12|Asha Rao|20|CS|9.5' students.txt

# Update keeps blank fields and validates new ones.
printf '5\n12\n\n150\n21\n\n8.25\n6\n' | "$BIN" > out.txt
grep -q "Age must be a number between 1 and 120" out.txt
grep -qx '12|Asha Rao|21|CS|8.25' students.txt

# --- Loading files -----------------------------------------------------
# CRLF endings load; bad and duplicate lines are skipped and backed up.
printf '1|A|20|CS|9.4\r\n2|B|2x|CS|9\n1|Dup|20|CS|8\n3|C|20|CS|42\n' > students.txt
printf '6\n' | "$BIN" > out.txt
grep -q "skipped 3 invalid or duplicate line(s)" out.txt
grep -q '^2|B|2x|CS|9' students.txt.bak
[ "$(cat students.txt)" = '1|A|20|CS|9.4' ] || { echo "unexpected students.txt"; cat students.txt; exit 1; }
rm -f students.txt.bak

# Adding to a file without a trailing newline must not merge two lines.
printf '1|A|20|CS|9.4' > students.txt
(printf '1\n2\nB\n20\nCS\n8\n'; sleep 1) | "$BIN" > /dev/null &
sleep 0.5
grep -qx '2|B|20|CS|8' students.txt || { echo "record merged after add"; cat students.txt; exit 1; }
wait
[ ! -e students.txt.tmp ] || { echo "temp file left behind"; exit 1; }

# Long names are shortened in the table so columns stay aligned.
printf '7|Bartholomew Alexander Fitzgerald|20|CS|9\n' > students.txt
printf '2\n6\n' | "$BIN" > out.txt
grep -q '^7     Bartholomew Alex\.\.\. 20' out.txt

echo "smoke test passed"
