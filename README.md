# SRMS: Student Record Management System

A small C++ console app for keeping student records (ID, name, age, major, GPA).
Records are stored in a plain text file, `students.txt`, in the folder you run it from.

## Features

- Add a student, with validation for numbers (the whole line must be a number),
  ranges (positive ID, age 1-120, GPA 0-10) and duplicate IDs
- List all students in a table, with long names shortened so columns line up
- Search by ID
- Update a student in place (press Enter to keep a field as it is)
- Delete by ID, with a confirmation prompt
- Saves after every change, and exits cleanly if input is closed

## Menu

```
1. Add Student
2. Show All Students
3. Search Student
4. Delete Student
5. Update Student
6. Exit
```

## Build and run

You need a C++17 compiler (g++ or clang++).

```bash
make          # builds ./SRMS
./SRMS
```

Without make: `g++ -std=c++17 -O2 -o SRMS SRMS.cpp`.
On Windows (MinGW) the same command produces `SRMS.exe`.

To start with example data, copy the sample file first:

```bash
cp sample_students.txt students.txt
```

## Data format

Each line of `students.txt` is one record, with fields separated by `|`:

```
id|name|age|major|gpa
1|Aniket Mahto|20|CSE|9.4
```

That's why names and majors can't contain the `|` character.

When the program starts it checks every line with the same rules as the Add
menu. Lines that are malformed, out of range, or repeat an earlier ID are
skipped and listed in a warning, and the original file is copied to
`students.txt.bak` before anything is saved, so you can fix those lines by hand.
Files with Windows (CRLF) line endings load fine.

Saves are written to `students.txt.tmp` and then renamed over `students.txt`,
so an interrupted save doesn't leave a half-written file.

## Tests

```bash
make test
```

This runs `tests/smoke_test.sh` (needs bash), which drives the menu through stdin
in a temporary folder and checks the saved file. It covers adding, validation,
search, update, delete, closed input, and loading bad or CRLF files.

## Project files

- `SRMS.cpp`: the whole program
- `tests/smoke_test.sh`: end-to-end test
- `sample_students.txt`: one example record
- `PROJECT REPORT.pdf`, `Student Record System.pptx`: project write-up and slides
