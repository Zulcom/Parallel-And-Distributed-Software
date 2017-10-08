/*
*Входные данные:
N – размер массивов,
x – вводимый параметр, для параллельной программы
k – количество потоков.
Задание:
инициализировать массивы A и B в соответствии с выражением (см. вариант);
выполнить вычисление массива С и переопределение массива В;
по требованию выполнить вывод всех массивов на экран (в файл);
предусмотреть возможность исполнения некоторых параллельных секций числомпотоков, задаваемых переменной k;
при входе в параллельную секцию на экран выводить краткое описание секции число исполняющих нитей
Вариант 18. Инициализировать массивы A и B в соответствии с выражением:
a[0]=x; a[i]=sin(x*i) + x 2 /i, i=1..N
b[0]=x^2 ; b[i]=(b[i-1]*x)/i, i=1..N
Выполнить вычисления:
c[i]=a[i]-b[N-i] , i=0..N
b[i]=(a[i]*c[i])^1/2 , i=1..N
*/

#include <iostream>
#include <omp.h>
#include <fstream>
using namespace std;
double bi(int i, int x, int n)
{
	return i == 0 ? x *x : (bi(i - 1, x, n) *x) / i;
}
void calculate(int n, int x, int k, bool needPrint)
{
	double * a = new double[n];
	double * b = new double[n];
	double * c = new double[n];
	double start_time = omp_get_wtime();
	a[0] = x;
	b[0] = x*x;
	ofstream out("out.txt");
#pragma omp parallel num_threads(k> n ? n: k)
	{
		
		int m = ceil(n / static_cast<double>(omp_get_num_threads()));
		int begin = m*omp_get_thread_num() == 0 ? 1 : m*omp_get_thread_num();
		int end = begin + m > n ? n : begin + m;
		for(int i = begin; i < end; i++)
		{
			a[i] = sin(x*i) + x *x / i;
			b[i] = (b[i - 1] * x) / i;
			//cout << bi(i, x, n) <<" " << b[i] <<endl;
		}
	}

//	out << endl << "b: ";
	for (int i = 0; i < n; i++)	out << b[i] << " ";
#pragma omp parallel num_threads(k> n ? n: k)
	{
		int m = ceil(n / static_cast<double>(omp_get_num_threads()));
		int begin = m*omp_get_thread_num();
		int end = begin + m > n ? n : begin + m;
		for(int i = begin; i < end; i++)
		{
			c[i] = a[i] - bi(n-1-i,x,n);
			b[i] = sqrt(abs(a[i] * c[i]));
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
