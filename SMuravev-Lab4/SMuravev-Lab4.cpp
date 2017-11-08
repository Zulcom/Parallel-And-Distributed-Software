/*
На Марсе решили побороть коррупцию.
В Министерство ежедневно собирают информацию о суммах взяток и откатов.
Данные за день нужно обработать и отсортировать определенным образом и разослать обработанные данные по разным адресам.
Информацию 1 – подготовить к передаче по факсу в прокуратуру (поместить в файл fax.txt);
Информацию 2 – подготовить к печати в «бумажном» виде (поместить в файл print.txt);
Информацию 3 – вывести на консоль оператора (на экран или по требованию - в файл screen.txt).
В связи с тем, что на Марсе много коррупционеров, обработка сообщений выполняется несколькими служащими министерства (потоками).
Кколичество потоков k задается при запуске программы.
 Суммы взяток в местной валюте находятся в массиве а;
 Суммы откатов в местной валюте находятся в массиве b.
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
Информация 1 (факс в прокуратуру fax.txt) - (6 mod 5) + 1 = 1 + 1 = 2
Вариант 2. - все сообщения об откатах (массив b), процент которых от суммы взятки (100*b[i]/a[i]) больше среднего.
Информация 2 (файл для печати - print.txt) - ((nvar + nst) mod 4) +1 = ((6 + 031510182) mod 4) + 1= 0+1=1
Вариант 1. a[i] при условии, что a[i] > a[i-1], a[i]<a[i+1] для при i = 1..N-1;
Информация 3 (вывод на консоль - screen.txt) - 3
Построить и вывести на экран гистограмму распределения величин.
Для этого:
определить значения max и min – это будет диапазон значений величины;
разделить диапазон на m отрезков (m – вводимый параметр);
подсчитать, сколько значений попадает в каждый интервал.
Вариант 3. гистограмма присвоенных сумм a[i]-b[i];
 */
#include <iostream>
#include <omp.h>
#include <fstream>

int avgPerc(int* a, int* b, int n)
	{
	int sum = 0;
#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < n; i++) sum += (100 * b[i] / a[i]);
	return (double) sum / (double) n;
	}

int max(int* A, int n)
	{
	int max_val= DBL_MAX;
#pragma omp parallel for 
	for (int i = 0; i < n; i++) max_val = max_val > A[i] ? max_val : A[i];
	return max_val;
	}

int min(int* A, int n)
	{
	int min_val= DBL_MIN;
#pragma omp parallel for 
	for (int i = 0; i < n; i++) min_val = min_val > A[i] ? min_val : A[i];
	return min_val;
	}

int main(int argc, char* argv[]){
	int k, n, x, m;
	std::cout << "Num threads,array size,x, segment count: ";
	std::cin >> k >> n >> x >> m;
	int* a = new int[n]; // взятки
	int* b = new int[n]; // откаты
	int* c = new int[n]; // присвоенные суммы
	int* counter = new int[m](); // счётчик значений в промежутках

	std::ofstream a_out("a.txt");
	std::ofstream b_out("b.txt");
	std::ofstream c_out("c.txt");

	std::ofstream fax("fax.txt", std::ios::app);
	std::ofstream print("print.txt", std::ios::app);
	std::ofstream histogram("histogram.txt");
	a[0] = x*x;
	b[0] = x;

#pragma omp parallel for ordered
	for (int i = 1; i < n; i++)
	{
		a[i] =i*i*exp(rand()%10);
		b[i] = i/x + rand()%100 ;
	}
#pragma omp parallel for
	for (int i = 0; i < n; i++) c[i] = a[i] - b[i];

	int c_min = min(c, n);
	int c_max = max(c, n);
	int segLenght = (c_max - c_min) / m;
	histogram << "Количество интервалов " << m << std::endl
		<< "диапозон значений от" << c_min << " до " << c_max << std::endl;

	for (int i = 0; i < n; i++)
	{
		a_out << a[i] << " ";
		b_out << b[i] << " ";
		c_out << c[i] << " ";
	}
	a_out.close();
	b_out.close();
	int avg_perc = avgPerc(a, b, n);
	
	fax << "средний процент отката от суммы взятки : " << avg_perc
		<< std::endl << "число служащих : " << (k> n ? n : k) << std::endl;
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
		for(int i = begin; i < end; i++){
			if((100 * b[i] / a[i]) > avg_perc) 
#pragma  omp critical
			{
				fax << "служащий " << omp_get_thread_num() << " сообщение " << i << " сумма " << b[i] << std::endl;
			}
			k = (i == 0) ? 1 : (i == n) ? n - 1 : i;
			if (a[k] > a[i - 1] && a[k] < a[k + 1])
#pragma  omp critical
			{ print << "служащий " << omp_get_thread_num() << " сообщение " << k << " сумма " << a[k] <<std::endl; }
			for (int j = 0; j < m; j++)
				if (c[i] >= c_min + (j * segLenght) && c[i] <= c_min + (j+1 * segLenght))
#pragma omp atomic
					counter[j]++;
		}
	}
	for(int j = 0; j < m; j++)	
		histogram << (j * segLenght) << " - " << (j + 1 * segLenght) << "   " << counter[j] << std::endl;
	delete[] a;
	delete[] b;
	delete[] c;
	delete[] counter;
	system("pause.exe");
}
