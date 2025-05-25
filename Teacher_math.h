#pragma once
#include <string>
#include <vector>
#include <random>
using namespace std;

struct StudentResult {
    string name;
    int correctAnswers;
};

class Equation {
private:
    double a, b, c;
public:
    Equation(double a, double b, double c);
    vector<double> solve();
};

class Student {
protected:
    string name;
    vector<vector<double>> answers;
public:
    Student(const string& name);
    virtual void SolveEq(vector<Equation> eq) = 0;
    string getName();
    const vector<vector<double>> getAnswers() const;
    virtual ~Student();
};

class GoodStudent : public Student {
public:
    using Student::Student;
    void SolveEq(vector<Equation> eq) override;
};

class AverageStudent : public Student {
private:
    mt19937 rng;
    uniform_int_distribution<int> dist;
public:
    AverageStudent(const string& name);
    void SolveEq(vector<Equation> eq) override;
};

class BadStudent : public Student {
public:
    using Student::Student;
    void SolveEq(vector<Equation> eq) override;
};

class Teacher {
private:
    vector<Equation> eq;
    vector<vector<double>> solutions;
public:
    void printRes(const vector<StudentResult>& results, const int& size);

    void SolveEqT();
    vector<StudentResult> CheckTest(const vector<Student*>& namesStudents);
    const vector<vector<double>> getSolutions();
    const void setEq(vector<Equation> equ);
    const vector<Equation> getEq();
};

vector<Equation> ReadFileEq(const string& name);