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
 *Вариант 5. Инициализировать массивы A и B в соответствии с выражением:
a[0]=x; a[i]=sin(x*i) + x2+i/N, i=1..N
b[0]=x2; b[i]=(b[i-1]+x)/i, i=1..N
Выполнить вычисления:
c[i]=(a[i]-b[N])/(N-i) , i=0..N
b[i]=a[i]*c[N-i]/2, i=0..N
 *
 */

#include <iostream>
#include <omp.h>
#include <fstream>
using namespace std;
void calculate(int n,int x,int k) {
	double  * a = new double[n];
	double * b = new double[n];
	double * c = new double[n];
	int start_time = omp_get_wtime();
	a[0] = x;
	b[0] = x*x;
#pragma omp parallel num_threads(k)
	{
	//	cout << "Parallel section for calculate A and B, num of threads - " << omp_get_num_threads() << endl;
		for(int i = 1; i < n; i++)
		{
			a[i] = sin(x*i) + (x*x + i) / n;
			b[i] = (b[i - 1] + x) / i;
		}
	}
#pragma omp parallel num_threads(k)
	{
	//	cout << "Parallel section for calculate C and recalculate B, num of threads - " << omp_get_num_threads() << endl;
		for(int i = 0; i < n; i++)
		{
			c[i] = (a[i] - b[n]) / (n - i);
			b[i] = a[i] * c[n - i] / 2;
		}
	}
	ofstream out("out.txt", ios::app);
	out << n << ":"  << omp_get_wtime() - start_time << endl;
	delete[] a;
	delete[] b;
	delete[] c;
}
int main() {
	int n, x, k;
	//cout << "Size, x, num of threads:" <<endl;
	//cin >> n >> x >> k;
	for(int i = 0; i < 1000*100; i+=50)
	{
		calculate(i, rand() % 10, 1);
	}
	system("pause.exe");
}
