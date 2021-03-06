﻿/*
*Входные данные:
N – размер массивов,
x – вводимый параметр, для параллельной программы
k – количество потоков.
Задание на ЛР1:
инициализировать массивы A и B в соответствии с выражением (см. вариант);
выполнить вычисление массива С и переопределение массива В;
по требованию выполнить вывод всех массивов на экран (в файл);
предусмотреть возможность исполнения некоторых параллельных секций числомпотоков, задаваемых переменной k;
при входе в параллельную секцию на экран выводить краткое описание секции число исполняющих нитей
1. Ознакомиться с описанием лабораторной работы.
Вариант задания совпадает с вариантом задания на лабораторную работу 1.
2. Проанализировать программу ЛР1 и определить,
какие фрагменты программы целесообразно/возможно распараллелить, используя директиву for
3. Распараллелить программу с использованием директивы #pragma omp for
(статическое планирование исполнения)
4. Убедиться, что прежняя и новая версии выдают одинаковые результаты.
5. Зафиксировать время выполнения программы при различных размерах обрабатываемых массивов.
6. Изменить способ планирования выполнения циклов на любой другой. Повторить эксперимент.

Вариант 6.
Инициализировать массивы A и B в соответствии с выражением:
a[0]=x; a[i]=(x*i) + x 2 /i, i=1..N
b[0]=x/N; b[i]=(b[i-1]+N/i)/x, i=1..N
Выполнить вычисления:
c[i]=a[i]-b[N-i] , i=0..N
b[i]=(a[i]+c[i])/2, i=0..N
*
*/

#include <iostream>
#include <omp.h>
#include <fstream>
using namespace std;
void calculate(int n, int x, int k, bool needPrint)
{
	double * a = new double[n];
	double * b = new double[n];
	double * c = new double[n];
	double start_time = omp_get_wtime();
	a[0] = x;
	b[0] = x / n;
	ofstream out("out.txt");
	
#pragma omp parallel for ordered
		for(int i = 1; i < n ; i++)
		{
			{
				a[i] = x*i + x*x / i;
#pragma omp ordered
				b[i] = (b[i - 1] + n / i) / x;
			}
		}
#pragma omp parallel for ordered
		for(int i = 1; i < n ; i++)
		{
			{
#pragma omp ordered
				c[i] = a[i] - b[n - i];
				b[i] = (a[i] + c[i]) / 2;
			}
		}

	
	if(needPrint)
	{
		out << "a: ";
		for(int i = 0; i < n; i++)	out << a[i] << " ";
		out << endl << "b: ";
		for(int i = 0; i < n; i++)	out << b[i] << " ";
		out << endl << "c: ";
		for(int i = 0; i < n; i++)	out << c[i] << " ";
		out << endl;
		out << "Time" << ":" << omp_get_wtime() - start_time << endl;
	}
	out.close();
	delete[] a;
	delete[] b;
	delete[] c;
}
int main()
{
	int n, x, k;
	cout << "Size, x, num of threads:" << endl;
	cin >> n >> x >> k;
	char needPrint;
	cout << "Print arrays to file?(y/n)";
	cin >> needPrint;
	calculate(n, x, k, needPrint == 'y');
	system("pause.exe");
}
