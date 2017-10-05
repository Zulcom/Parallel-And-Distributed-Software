#include <algorithm> // место жительства swap()
#include <iostream> // будем использовать потоковый ввод-вывод
#include <vector> // матрицы оформим как векторы
#include "omp.h" // используем openMP
#include <mpi.h> // используем MPI

using namespace std; // чтобы не дописывать пространство имён к векторам, свопам и консоли
					 /* Поиск максимального элемента в столбце
					 * vector<vector<int> > &matrix матирца
					 *  int col столбец
					 *   int n размер
					 *   возвращает позицию максимального элемента в столбце
					 */
int ProcNum, ProcRank;

/* Решение СЛАУ методом Крамера
* vector<vector<int> > &matrix - матрица неизвестых
*  vector<int> &free_term_column - слобец свободных членов
* int n - размерность
* Возвращает вектор, содержащий решения к матрице
*/

int * cramer_solving(int ** matrix, int * free_term_column, int n)
{
	int mainDet =10; // вычисляем определитель матрицы неизвестных
	if(abs(mainDet) < 0.0001) // если он не подходит под решение по правилу Крамера
		return  nullptr; //кидаем исключение
	int * solution = new int[n]; // создаем вектор решений
	if(ProcRank == 0)
	{
		int ** private_matrix = new int*[n];
		for(int i = 0; i < n; ++i) // делаем копию матрицы, чтобы избежать работы в критической секции и эффекта гонки
		{
			private_matrix[i] = new int[n];
			for(int j = 0; j < n; j++)
				private_matrix[i][j] = matrix[i][j];
		}

		for(int i = 1; i < ProcNum; ++i) //бежим по всем столбцам
		{
			swap(private_matrix[i], free_term_column); // заменяем столбец неизвестных и столбец i в матрице
			MPI_Send(&private_matrix, sizeof(int)*n*n, MPI_INT, i, 0, MPI_COMM_WORLD); // Отправляем в другой процесс ( посылаемое,количество, тип, куда,тэг, коммунитатор	)	
			swap(private_matrix[i], free_term_column);// заменяем обратно чтобы не путаться
			MPI_Status status;
			MPI_Recv(&solution[i], sizeof(int), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); // получаем из другого процесса ответ в матрицу ответов
			cout << "Ответ получен:" << solution[i]<<endl;
		}
	}
	else
	{
		MPI_Status status;
		int ** private_matrix = new int*[n]; // сюда будем писать получаемую матрицу
		MPI_Recv(&private_matrix, sizeof(int)*n*n, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); // получаем матрицу (получаемое, колчиестово, тип, откуда, тег, коомутатор, статус)
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < n; j++)
			{
				cout << private_matrix[i][j] << " ";
			}
			cout << endl;
		}
		MPI_Send(&ProcRank, sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD); // отправляем этот ответ обратно в главный процесс
	}
	return solution; // возвращаем решения
}
int main(int argc, char *argv[]) // точка входа
{

	int ProcNum, ProcRank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	if(ProcRank == 0)
	{
		for(int n = 50; n < 350; n += 50) // основной цикл
		{
			int ** matrix = new int*[n];// матрица целочисленная на указателях
			for(int i = 0; i < n; ++i) // заполнение массива - строки
			{
				matrix[i] = new int[n];
				for(int j = 0; j < n; ++j) // заполнение массива - столбцы
					matrix[i][j] = rand() % 10; // заполяем элемент M_ij случайным числом меньшим 10
			}
			int * column = new int[n]; // создаем столбец свободных членов
			for(int j = 0; j < n; ++j) //заполняем его - строки
				column[j] = rand() % 10; // заполняем элемент Column_j случайным числом
			double start_time = MPI_Wtime(); //запоминаем время
			int * solution = cramer_solving(matrix, column, n); // запоминаем вектор корней СЛАУ, полученный из автоматически оптимизированной функции cramer_solving,
																//которая принимает матрицу для поиска, столбец свободных членов, размерность матрицы (без столбца св. членов)
			cout << n << " " << MPI_Wtime() - start_time << endl; // выводим в консоль размерность и затраченное на вычисление время
			for(int i = 0; i < n; i++)
			{
				cout << solution[0] << " ";
			}
			cout << endl;
			for(int i = 0; i < n; i++)	delete [] matrix[i] ;
			delete [] matrix;
			delete[] column;
		}
		system("pause.exe");
	}

	MPI_Finalize();
	return 0;
}
