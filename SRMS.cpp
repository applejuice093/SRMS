#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdlib>

using namespace std;

// Reads a number from stdin, re-prompting until the input is valid.
// Exits cleanly if stdin is closed so the program never spins forever.
template <typename T>
T readNumber(const string &retryPrompt)
{
    T value;
    while (!(cin >> value))
    {
        if (cin.eof())
        {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << retryPrompt;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

    void input(const vector<Student> &existing)
    {
        cout << "\n--- Enter Student Details ---\n";

        cout << "Enter ID (number): ";
        id = readNumber<int>("Invalid input! Enter numeric ID: ");
        for (bool clash = true; clash;)
        {
            clash = false;
            for (const auto &other : existing)
                if (other.id == id)
                    clash = true;
            if (clash)
                {
                cout << "That ID already exists. Enter a different ID: ";
                id = readNumber<int>("Invalid! Enter a number: ");
            }
        }

        cout << "Enter Name: ";
        getline(cin, name);
        while (name.empty() || name.find('|') != string::npos)
        {
            cout << "Name must be non-empty and cannot contain '|': ";
            getline(cin, name);
        }

        cout << "Enter Age (number): ";
        age = readNumber<int>("Invalid! Enter numeric age: ");
        while (age <= 0 || age > 120)
            {
                cout << "Age must be between 1 and 120: ";
                age = readNumber<int>("Invalid! Enter a number: ");
            }

        cout << "Enter Major: ";
        getline(cin, major);
        while (major.empty() || major.find('|') != string::npos)
        {
            cout << "Major must be non-empty and cannot contain '|': ";
            getline(cin, major);
        }

        cout << "Enter GPA (number): ";
        gpa = readNumber<double>("Invalid! Enter numeric GPA: ");
        while (gpa < 0.0 || gpa > 10.0)
            {
                cout << "GPA must be between 0 and 10: ";
                gpa = readNumber<double>("Invalid! Enter a number: ");
            }
    }

    void display() const
    {
        cout << left << setw(6) << id
             << setw(20) << name
             << setw(6) << age
             << setw(15) << major
             << setw(6) << fixed << setprecision(2) << gpa << "\n";
    }

    string toLine() const
    {
        ostringstream oss;
        oss << id << '|' << name << '|' << age << '|' << major << '|' << gpa;
        return oss.str();
    }

    // Parses one "id|name|age|major|gpa" line; returns false if malformed.
    static bool fromLine(const string &line, Student &s)
    {
        stringstream ss(line);
        string part;
        vector<string> parts;
        while (getline(ss, part, '|'))
            parts.push_back(part);
        if (parts.size() != 5)
            return false;
        try
        {
            s.id = stoi(parts[0]);
            s.name = parts[1];
            s.age = stoi(parts[2]);
            s.major = parts[3];
            s.gpa = stod(parts[4]);
        }
        catch (...)
        {
            return false;
        }
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
    // Rewrites students.txt from the in-memory list (one record per line,
    // fields separated by '|': id|name|age|major|gpa).
    void saveAllToFile() const
    {
        ofstream out("students.txt", ios::trunc);
        if (!out)
        {
            cout << "Error: cannot open students.txt for writing.\n";
            return;
        }
        for (const auto &s : list)
            out << s.toLine() << '\n';
    }

public:
    void loadFromFile()
    {
        list.clear();
        ifstream in("students.txt");
        if (!in)
        {
            return;
        }
        string line;
        while (getline(in, line))
        {
            if (line.empty())
                continue;
            Student s;
            if (Student::fromLine(line, s))
                list.push_back(s);
        }
    }

    void addStudent()
    {
        Student s;
        s.input(list);

        list.push_back(s);

        ofstream out("students.txt", ios::app);
        if (!out)
        {
            cout << "Error: cannot open students.txt to append. Student added in memory only.\n";
            return;
        }
        out << s.toLine() << '\n';
        cout << "Student added and saved to file.\n";
    }

    void showAll() const
    {
        if (list.empty())
        {
            cout << "No records available.\n";
            return;
        }
        cout << "\n--- Student Records ---\n";
        cout << left << setw(6) << "ID"
             << setw(20) << "Name"
             << setw(6) << "Age"
             << setw(15) << "Major"
             << "GPA" << "\n";
        cout << "---------------------------------------------------------------\n";
        for (const auto &s : list)
            s.display();
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

        int ch;
        if (!(cin >> ch))
        {
            if (cin.eof())
            {
                M.saveOnExit();
                cout << "\nInput closed. Saved and exiting.\n";
                return 0;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice!\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
