#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <cstdio>

using namespace std;

// Reads one line from stdin. Exits cleanly if stdin is closed, so prompts
// that loop until they get valid text can never spin forever on EOF.
string readLine()
{
    string line;
    if (!getline(cin, line))
    {
        cout << "\nInput closed. Exiting.\n";
        exit(0);
    }
    return line;
}

// Returns `text` without leading or trailing spaces, tabs or CR/LF.
string trim(const string &text)
{
    const char *space = " \t\r\n";
    size_t start = text.find_first_not_of(space);
    if (start == string::npos)
        return "";
    size_t end = text.find_last_not_of(space);
    return text.substr(start, end - start + 1);
}

// Reads a non-empty text field that is safe to store in the '|'-separated
// file. Surrounding whitespace is trimmed so "  Asha  " is saved as "Asha".
string readTextField(const string &fieldName)
{
    string value = trim(readLine());
    while (value.empty() || value.find('|') != string::npos)
    {
        cout << fieldName << " must be non-empty and cannot contain '|': ";
        value = trim(readLine());
    }
    return value;
}

// Parses the whole of `text` as a T, allowing surrounding spaces only.
// "12abc", "1.5" (for an int) and "" are rejected instead of half-read.
template <typename T>
bool parseNumber(const string &text, T &value)
{
    istringstream iss(text);
    T parsed;
    if (!(iss >> parsed))
        return false;
    iss >> ws;
    if (!iss.eof())
        return false;
    value = parsed;
    return true;
}

// Reads a number from stdin, re-prompting until a full line is a valid
// number. Exits cleanly if stdin is closed so the program never spins forever.
template <typename T>
T readNumber(const string &retryPrompt)
{
    T value;
    while (!parseNumber(readLine(), value))
        cout << retryPrompt;
    return value;
}

class Student
{
public:
    int id = 0;
    string name;
    int age = 0;
    string major;
    double gpa = 0.0;

    static bool idExists(const vector<Student> &students, int id)
    {
        for (const auto &other : students)
            if (other.id == id)
                return true;
        return false;
    }

    void input(const vector<Student> &existing)
    {
        cout << "\n--- Enter Student Details ---\n";

        cout << "Enter ID (number): ";
        id = readNumber<int>("Invalid input! Enter numeric ID: ");
        while (true)
        {
            if (id <= 0)
                cout << "ID must be a positive number: ";
            else if (idExists(existing, id))
                cout << "That ID already exists. Enter a different ID: ";
            else
                break;
            id = readNumber<int>("Invalid! Enter a number: ");
        }

        cout << "Enter Name: ";
        name = readTextField("Name");

        cout << "Enter Age (number): ";
        age = readNumber<int>("Invalid! Enter numeric age: ");
        while (age <= 0 || age > 120)
            {
                cout << "Age must be between 1 and 120: ";
                age = readNumber<int>("Invalid! Enter a number: ");
            }

        cout << "Enter Major: ";
        major = readTextField("Major");

        cout << "Enter GPA (number): ";
        gpa = readNumber<double>("Invalid! Enter numeric GPA: ");
        while (gpa < 0.0 || gpa > 10.0)
            {
                cout << "GPA must be between 0 and 10: ";
                gpa = readNumber<double>("Invalid! Enter a number: ");
            }
    }

    // Prints one table row. Long text is shortened with "..." so it can't
    // push the following columns out of line, and the GPA is formatted in a
    // local stream so cout's own number formatting is left untouched.
    void display() const
    {
        ostringstream gpaText;
        gpaText << fixed << setprecision(2) << gpa;
        cout << left << setw(6) << id
             << setw(20) << fitColumn(name, 19)
             << setw(6) << age
             << setw(15) << fitColumn(major, 14)
             << gpaText.str() << "\n";
    }

    static string fitColumn(const string &text, size_t width)
    {
        if (text.size() <= width)
            return text;
        return text.substr(0, width - 3) + "...";
    }

    string toLine() const
    {
        ostringstream oss;
        oss << id << '|' << name << '|' << age << '|' << major << '|' << gpa;
        return oss.str();
    }

    // Parses one "id|name|age|major|gpa" line; returns false if malformed.
    // Applies the same rules as interactive input, so a hand-edited file
    // can't load records the menu would have refused (e.g. GPA 42, age "2x").
    // A trailing '\r' from files saved on Windows is ignored.
    static bool fromLine(const string &rawLine, Student &s)
    {
        string line = rawLine;
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        stringstream ss(line);
        string part;
        vector<string> parts;
        while (getline(ss, part, '|'))
            parts.push_back(part);
        if (parts.size() != 5)
            return false;

        Student parsed;
        parsed.name = trim(parts[1]);
        parsed.major = trim(parts[3]);
        if (!parseNumber(parts[0], parsed.id) || parsed.id <= 0)
            return false;
        if (!parseNumber(parts[2], parsed.age) || parsed.age <= 0 || parsed.age > 120)
            return false;
        if (!parseNumber(parts[4], parsed.gpa) || parsed.gpa < 0.0 || parsed.gpa > 10.0)
            return false;
        if (parsed.name.empty() || parsed.major.empty())
            return false;
        s = parsed;
        return true;
    }
};

class StudentManager
{
    vector<Student> list;

    // Returns the position of the student with this ID, or -1 if absent.
    int findIndex(int id) const
    {
        for (int i = 0; i < (int)list.size(); ++i)
            if (list[i].id == id)
                return i;
        return -1;
    }
    static void printTableHeader()
    {
        cout << left << setw(6) << "ID"
             << setw(20) << "Name"
             << setw(6) << "Age"
             << setw(15) << "Major"
             << "GPA" << "\n";
        cout << "---------------------------------------------------------------\n";
    }

    // Rewrites students.txt from the in-memory list (one record per line,
    // fields separated by '|': id|name|age|major|gpa).
    // Writes to students.txt.tmp first and then renames it over the real
    // file, so a failed or interrupted write can't leave a half-written file.
    bool saveAllToFile() const
    {
        const char *tmpName = "students.txt.tmp";
        {
            ofstream out(tmpName, ios::trunc);
            if (!out)
            {
                cout << "Error: cannot open " << tmpName << " for writing.\n";
                return false;
            }
            for (const auto &s : list)
                out << s.toLine() << '\n';
            out.flush();
            if (!out)
            {
                cout << "Error: failed while writing student records.\n";
                remove(tmpName);
                return false;
            }
        }
        // POSIX rename() replaces the target atomically. On Windows it fails
        // if the target exists, so fall back to removing it and retrying.
        if (rename(tmpName, "students.txt") != 0 &&
            (remove("students.txt"), rename(tmpName, "students.txt") != 0))
        {
            cout << "Error: cannot replace students.txt (data kept in " << tmpName << ").\n";
            return false;
        }
        return true;
    }

public:
    // Loads students.txt. Lines that are malformed or repeat an earlier ID
    // are skipped with a warning; because the next save rewrites the file,
    // the original is first copied to students.txt.bak so nothing is lost.
    void loadFromFile()
    {
        list.clear();
        ifstream in("students.txt");
        if (!in)
        {
            return;
        }
        string line;
        vector<int> skipped;
        int lineNo = 0;
        while (getline(in, line))
        {
            ++lineNo;
            if (trim(line).empty())
                continue;
            Student s;
            if (Student::fromLine(line, s) && findIndex(s.id) == -1)
                list.push_back(s);
            else
                skipped.push_back(lineNo);
        }
        in.close();

        if (!skipped.empty())
        {
            cout << "Warning: skipped " << skipped.size()
                 << " invalid or duplicate line(s) in students.txt (line numbers:";
            for (int n : skipped)
                cout << ' ' << n;
            cout << ").\n";
            ifstream src("students.txt", ios::binary);
            ofstream bak("students.txt.bak", ios::binary | ios::trunc);
            if (src && bak && (bak << src.rdbuf()))
                cout << "The original file was copied to students.txt.bak.\n";
            else
                cout << "Warning: could not create students.txt.bak.\n";
        }
    }

    void addStudent()
    {
        Student s;
        s.input(list);

        list.push_back(s);
        // Rewrite the whole file rather than appending: appending to a file
        // whose last line has no newline glues two records together.
        if (saveAllToFile())
            cout << "Student added and saved to file.\n";
        else
            cout << "Student added in memory only; it will be saved again on exit.\n";
    }

    void showAll() const
    {
        if (list.empty())
        {
            cout << "No records available.\n";
            return;
        }
        cout << "\n--- Student Records ---\n";
        printTableHeader();
        for (const auto &s : list)
            s.display();
        cout << list.size() << " record(s).\n";
    }

    void searchStudent() const
    {
        cout << "Enter ID to search: ";
        int id = readNumber<int>("Invalid! Enter numeric ID: ");

        int idx = findIndex(id);
        if (idx == -1)
        {
            cout << "Student not found.\n";
        }
        else
        {
            cout << "\n--- Student Found ---\n";
            printTableHeader();
            list[idx].display();
        }
    }

    void deleteStudent()
    {
        cout << "Enter ID to delete: ";
        int id = readNumber<int>("Invalid! Enter numeric ID: ");

        int idx = findIndex(id);
        if (idx == -1)
        {
            cout << "Student not found.\n";
            return;
        }
        cout << "Are you sure you want to delete student '" << list[idx].name << "' (y/n)? ";
        string ans;
        getline(cin, ans);
        if (ans.empty() || (ans[0] != 'y' && ans[0] != 'Y'))
        {
            cout << "Delete cancelled.\n";
            return;
        }

        list.erase(list.begin() + idx);
        saveAllToFile();
        cout << "Student deleted and file updated.\n";
    }

    void saveOnExit() const
    {
        saveAllToFile();
    }
};

int main()
{
    StudentManager M;
    M.loadFromFile();

    while (true)
    {
        cout << "\n======= STUDENT RECORD SYSTEM =======\n";
        cout << "1. Add Student\n";
        cout << "2. Show All Students\n";
        cout << "3. Search Student\n";
        cout << "4. Delete Student\n";
        cout << "5. Exit\n";
        cout << "Choose option: ";

        string choice;
        if (!getline(cin, choice))
        {
            M.saveOnExit();
            cout << "\nInput closed. Saved and exiting.\n";
            return 0;
        }
        int ch = 0;
        if (!parseNumber(choice, ch))
        {
            cout << "Invalid choice!\n";
            continue;
        }

        switch (ch)
        {
        case 1:
            M.addStudent();
            break;
        case 2:
            M.showAll();
            break;
        case 3:
            M.searchStudent();
            break;
        case 4:
            M.deleteStudent();
            break;
        case 5:
            M.saveOnExit();
            cout << "Saved and exiting.\n";
            return 0;
        default:
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}
