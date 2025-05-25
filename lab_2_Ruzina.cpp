#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <iomanip>
#include <locale.h>
#include <random>
#include "Teacher_math.h"
using namespace std;

Equation::Equation(double a, double b, double c) : a(a), b(b), c(c) {}
vector<double> Equation::solve() {
        double D = b * b - 4 * a * c;
        if (D < 0) return {};
        if (D == 0) return { -b / (2 * a) };
        return { (-b + sqrt(D)) / (2 * a), (-b - sqrt(D)) / (2 * a) };
}

Student::Student(const string& name) : name(name) {}
string Student::getName() { return name; }
const vector<vector<double>> Student::getAnswers() const { return answers; }
Student::~Student() {}


void GoodStudent::SolveEq(vector<Equation> eq) {
    for (int i = 0; i < eq.size(); i++) {
        answers.push_back(eq[i].solve());
    }
}


AverageStudent::AverageStudent(const string& name)
    : Student(name), rng(random_device{}()), dist(0, 1) {};
void AverageStudent::SolveEq(vector<Equation> eq) {
    for (int i = 0; i < eq.size(); i++) {
        bool correct = dist(rng);
        if (correct) { answers.push_back(eq[i].solve()); }
        else { answers.push_back({ 0.0 }); }
    }
}



void BadStudent::SolveEq(vector<Equation> eq) {
    for (int i = 0; i < eq.size(); i++) {
        answers.push_back({ 0.0 });
    }
}


void Teacher::printRes(const vector<StudentResult>& results, const int& size) {
    cout << left << setw(20) << "ФИО" << " | " << "Правильных ответов\n";
    cout << string(45, '-') << '\n';

    for (const auto& r : results) {
        cout << left << setw(20) << r.name << " | " << r.correctAnswers << "/" << size << '\n';
    }
}

void Teacher::SolveEqT() {
    for (auto& e : eq) {
        solutions.push_back(e.solve());
    }
}
vector<StudentResult> Teacher::CheckTest(const vector<Student*>& namesStudents) {
    vector<StudentResult> fin;
    for (auto n : namesStudents) {
        int counter = 0;
        const auto& answers = n->getAnswers();
        for (int i = 0; i < answers.size(); i++) {
            if (solutions[i] == answers[i]) {
                counter++;
            }
        }
        fin.push_back({ n->getName(), counter });
    }
    return fin;
}
const vector<vector<double>> Teacher::getSolutions() { return solutions; };
const void Teacher::setEq(vector<Equation> equ) { eq = equ; }
const vector<Equation> Teacher::getEq() { return eq; }

vector<Equation> ReadFileEq(const string& name)
{
    ifstream file(name);
    string line;
    double a, b, c;
    vector<Equation> equations;
    if (!file.is_open())
    {
        cout << "Ошибка открытия файла" << endl;
        exit(0);
    }
    while (getline(file, line)) {
        istringstream iss(line);
        if (iss >> a >> b >> c) {
            Equation eq(a, b, c);
            equations.push_back(eq);
        }
        
    }
    file.close();
    return equations;
}

int main(void) {
    setlocale(LC_ALL, "Ru");
    vector<Equation> equations = ReadFileEq("equ.txt");
    Teacher teach;
    teach.setEq(equations);
    teach.SolveEqT();
    vector<Student*> NameStu;
    NameStu.push_back(new GoodStudent("Абышов Микаил"));
    NameStu.push_back(new GoodStudent("Волгина Анастасия"));
    NameStu.push_back(new AverageStudent("Лукашёв Артём"));
    NameStu.push_back(new BadStudent("Мургин Алексей"));
    NameStu.push_back(new AverageStudent("Родионов Александр"));
    NameStu.push_back(new AverageStudent("Кузема Арина"));

    for (auto& st : NameStu) {
        st->SolveEq(teach.getEq());
    }

    vector<StudentResult> res = teach.CheckTest(NameStu);
    teach.printRes(res, teach.getEq().size());

    return 0;
}