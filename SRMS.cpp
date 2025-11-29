#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

class Student
{
public:
    int id = 0;
    string name;
    int age = 0;
    string major;
    double gpa = 0.0;

    void input()
    {
        cout << "\n--- Enter Student Details ---\n";

        cout << "Enter ID (number): ";
        while (!(cin >> id))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Enter numeric ID: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Name: ";
        getline(cin, name);

        cout << "Enter Age (number): ";
        while (!(cin >> age))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid! Enter numeric age: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Major: ";
        getline(cin, major);

        cout << "Enter GPA (number): ";
        while (!(cin >> gpa))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid! Enter numeric GPA: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

    int findIndex(int id) const
    {
        for (int i = 0; i < (int)list.size(); ++i)
            if (list[i].id == id)
                return i;
        return -1;
    }
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
        s.input();

        if (findIndex(s.id) != -1)
        {
            cout << "ID already exists. Add aborted.\n";
            return;
        }

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
             << setw(6) << "GPA\n";
        cout << "---------------------------------------------------------------\n";
        for (const auto &s : list)
            s.display();
    }

    void searchStudent() const
    {
        cout << "Enter ID to search: ";
        int id;
        while (!(cin >> id))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid! Enter numeric ID: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
        int id;
        while (!(cin >> id))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid! Enter numeric ID: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
        cout << "5. Delete Student\n";
        cout << "8. Exit\n";
        cout << "Choose option: ";

        int ch;
        if (!(cin >> ch))
        {
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
        case 5:
            M.deleteStudent();
            break;
        case 8:
            M.saveOnExit();
            cout << "Saved and exiting.\n";
            return 0;
        default:
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}
