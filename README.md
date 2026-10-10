# SRMS: Student Record Management System

A small C++ console app for keeping student records (ID, name, age, major, GPA).
Records are stored in a plain text file, `students.txt`, in the folder you run it from.

## Features

- Add a student, with validation for numbers, ranges (age 1-120, GPA 0-10) and duplicate IDs
- List all students in a table
- Search by ID
- Delete by ID, with a confirmation prompt
- Saves automatically, and exits cleanly if input is closed

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

## Tests

```bash
make test
```

This runs `tests/smoke_test.sh`, which drives the menu through stdin in a temporary
folder and checks the saved file. GitHub Actions runs it on every push.

## Project files

- `SRMS.cpp`: the whole program
- `PROJECT REPORT.pdf`, `Student Record System.pptx`, `Report`: project write-up and slides
