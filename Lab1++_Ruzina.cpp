#include <fstream>
#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

vector<double> ReadFile(string name)
{
    ifstream file(name);
    double coefficient;
    vector<double> coefficients;
    if (!file.is_open())
    {
        cout << "Ошибка открытия файла" << endl;
        exit(0);
    }
    while (file >> coefficient)
    {
        coefficients.push_back(coefficient);
    }
    if (coefficients.size() != 3)
    {
        cout << "Некорректные данные в файле" << endl;
        exit(0);
    }
    file.close();
    return coefficients;
}

vector<double> solution_Polynom(vector<double> coefficients)
{
    vector<double> result;
    double D = coefficients[1] * coefficients[1] - 4 * coefficients[0] * coefficients[2];
    if (D >= 0)
    {
        if (D == 0)
        {
            result.push_back((0 - coefficients[1] + sqrt(D)) / (2 * coefficients[0]));
        }
        else
        {
            result.push_back((0 - coefficients[1] + sqrt(D)) / (2 * coefficients[0]));
            result.push_back((0 - coefficients[1] - sqrt(D)) / (2 * coefficients[0]));
        }
    }
    return result;
}

void WriteResulInFile(vector<double> result, string name)
{
    ofstream file(name);
    if (!file.is_open())
    {
        cout << "Ошибка открытия файла для записи ответа." << endl;
        exit(0);
    }
    switch (result.size())
    {
    case 0: file << "Нет действительных корней" << endl;
        break;
    case 1: file << "Корень уравнения:" << endl;
        break;
    case 2: file << "Корни уравнения:" << endl;
        break;
    }
    for (int i = 0; i < result.size(); i++)
    {
        file << result[i] << " ";
    }
    return;
}

int main()
{
    setlocale(LC_ALL, "ru");
    vector<double> coef = ReadFile("test.txt");
    vector<double> result = solution_Polynom(coef);
    WriteResulInFile(result, "final.txt");
	return 0;
}