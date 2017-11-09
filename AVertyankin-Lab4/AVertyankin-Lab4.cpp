/*
На Марсе решили побороть коррупцию.
В Министерство ежедневно собирают информацию о суммах взяток и откатов.
Данные за день нужно обработать и отсортировать определенным образом и разослать обработанные данные по разным адресам.
Информацию 1 – подготовить к передаче по факсу в прокуратуру (поместить в файл fax.txt);
Информацию 2 – подготовить к печати в «бумажном» виде (поместить в файл print.txt);
Информацию 3 – вывести на консоль оператора (на экран или по требованию - в файл screen.txt).
В связи с тем, что на Марсе много коррупционеров, обработка сообщений выполняется несколькими служащими министерства (потоками).
Кколичество потоков k задается при запуске программы.
Суммы взяток в местной валюте находятся в массиве а;
Суммы откатов в местной валюте находятся в массиве b.
За основу работы берутся массивы а и в, инициализированные в лабораторной работе 3 в соответствии с заданием на нее.
Примечание: если значения, полученные в ЛР1, не подходят для выполнения задания, выражения для их вычисления можно скорректировать.
Дополнительно:
Для проверки правильности работы значения, хранящиеся в массивах а и b, должны быть доступны для просмотра (на консоли или в файле).
Сообщение должно содержать:
личный номер служащего (номер потока), номер сообщения и собственно сумму (значение).
Порядок следования сообщений в файле (на консоли) может быть произвольным.
Программа должна работать в двух режимах:
- проверки, когда на экран и в файл выводится вся информация;
- тестирования производительности, когда вывод на консоль не выполняется, но засекается время выполнения программы
Информация 1 (факс в прокуратуру fax.txt) - (2 mod 5) + 1 = 2 + 1 = 3
Вариант 3. - Все сообщения о суммах, присвоенных взяточниками (a[i]-b[i]), ко-
торые лежат в интервале [max*0.9, max], где max – наибольшая сумма
Информация 2 (файл для печати - print.txt) - ((nvar + nst) mod 4) +1 = ((2 + 031627972) mod 4) + 1= 2+1=3
Вариант 3. a[i], при условии b[i]<>0;
Информация 3 (вывод на консоль - screen.txt) - 1
Построить и вывести на экран гистограмму распределения величин.
Для этого:
определить значения max и min – это будет диапазон значений величины;
разделить диапазон на m отрезков (m – вводимый параметр);
подсчитать, сколько значений попадает в каждый интервал.
Вариант 1. гистограмма суммы взяток a[i];
*/
#include <iostream>
#include <omp.h>
#include <fstream>
#include <ctime>
using namespace std;


int max(int* A, int n)
{
	int max_val = DBL_MIN;
	for(int i = 0; i < n; i++) max_val = A[i] > max_val ? A[i] : max_val;
	return max_val;
}

int min(int* A, int n)
{
	int min_val = DBL_MAX;
	for(int i = 0; i < n; i++) min_val = A[i] < min_val ? A[i] : min_val;
	return min_val;
}

int main()
{
	srand(time(0)); // рандом
	setlocale(LC_ALL, "Russian");
	int thread_amount, n, x, segment_amount;
	cout << "Num threads,array size,x, segment count: ";
	cin >> thread_amount >> n >> x >> segment_amount;
	int* a = new int[n]; // взятки (в реальном мире это "сумма сделки")
	int* b = new int[n]; // откаты
	int* c = new int[n]; // присвоенные суммы
	int* counter = new int[segment_amount](); // счётчик значений в промежутках

	ofstream a_out("a.txt");
	ofstream b_out("b.txt");
	ofstream c_out("c.txt");

	ofstream fax("fax.txt");
	ofstream print("print.txt");
	ofstream histogram("histogram.txt");

	a[0] = x * x;
	b[0] = x;
	c[0] = a[0] - b[0];
#pragma omp parallel for
	for(int i = 1; i < n; i++)
	{
		b[i] = i / x + rand() % 100;
		a[i] = b[i] * exp(rand() % 10);
		c[i] = a[i] - b[i];
	}
	int a_min = min(a, n);
	int a_max = max(a, n);
	int c_max = max(c, n);
	int segLength = abs((a_max - a_min) / segment_amount);
	histogram << "Количество интервалов " << segment_amount << endl
		<< "Диапозон значений от " << a_min
		<< " до " << a_max << endl
		<< "Длина интервала " << segLength << endl;

	for(int i = 0; i < n; i++)
	{
		a_out << a[i] << " ";
		b_out << b[i] << " ";
		c_out << c[i] << " ";
	}
	a_out.close();
	b_out.close();
	c_out.close();

	fax << "[max*0.9, max] = [" << c_max*0.9 << " , " << c_max << "]"
		<< endl << "число служащих : " << (thread_amount > n ? n : thread_amount) << endl;
#pragma omp parallel num_threads(k> n ? n: k) shared(avg_perc)
	{
		int segSize = ceil(n / static_cast<int>(omp_get_num_threads()));
		int begin = segSize * omp_get_thread_num();
		int end = begin + segSize > n ? n : begin + segSize;
		if(omp_get_thread_num() == 0)
		{
			begin = 0;
			end = segSize;
		}
		for(int i = begin; i < end; i++)
		{
			if(c[i]>= c_max*0.9 && c[i] <= c_max)
#pragma  omp critical
			{
				fax << "служащий " << omp_get_thread_num() << " сообщение " << i << " сумма " << c[i] << endl;
			}
			thread_amount = (i == 0) ? 1 : (i == n) ? n - 1 : i;
			if(b[thread_amount] != 0)
#pragma  omp critical
			{
				print << "служащий " << omp_get_thread_num() << " сообщение " << thread_amount << " сумма " << a[thread_amount] << endl;
			}
			for(int j = 0; j < segment_amount; j++)
			{
				int segEnd = segment_amount - 1 == j ? a_max : a_min + (j + 1) * segLength;
				if(a[i] >= a_min + (j * segLength) && a[i] <= +segEnd)
#pragma omp atomic
					counter[j]++;
			}
		}
	}
	fax.close();
	print.close();
	for(int j = 0; j < segment_amount; j++)
	{
		int segEnd = segment_amount - 1 == j ? a_max : a_min + (j + 1) * segLength;
		histogram << a_min + j * segLength << " – " << segEnd << "   " << counter[j] << endl;
	}
	histogram.close();
	delete[] a;
	delete[] b;
	delete[] c;
	delete[] counter;
	system("pause.exe");
}
