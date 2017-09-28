/*
*������� ������:
N � ������ ��������,
x � �������� ��������, ��� ������������ ���������
k � ���������� �������.
�������:
���������������� ������� A � B � ������������ � ���������� (��. �������);
��������� ���������� ������� � � ��������������� ������� �;
�� ���������� ��������� ����� ���� �������� �� ����� (� ����);
������������� ����������� ���������� ��������� ������������ ������ �������������, ���������� ���������� k;
��� ����� � ������������ ������ �� ����� �������� ������� �������� ������ ����� ����������� �����
������� 18. ���������������� ������� A � B � ������������ � ����������:
a[0]=x; a[i]=sin(x*i) + x 2 /i, i=1..N
b[0]=x^2 ; b[i]=(b[i-1]*x)/i, i=1..N
��������� ����������:
c[i]=a[i]-b[N-i] , i=0..N
b[i]=(a[i]*c[i])^1/2 , i=1..N
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
	b[0] = x*x;
	ofstream out("out.txt");
#pragma omp parallel num_threads(k)
	{
		out << "Parallel section for calculate A and B, num of threads - " << omp_get_num_threads() << endl;
		for(int i = 1; i < n; i++)
		{
			a[i] = sin(x*i) + x *x / i;
			b[i] = (b[i - 1] * x) / i;
		}
	}
#pragma omp parallel num_threads(k)
	{
		out << "Parallel section for calculate C and recalculate B, num of threads - " << omp_get_num_threads() << endl;
		for(int i = 0; i < n; i++)
		{
			c[i] = a[i] - b[n-1 - i];
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
