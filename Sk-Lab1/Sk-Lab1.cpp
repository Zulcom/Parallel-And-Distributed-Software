﻿#include <algorithm> // место жительства swap()
#include <iostream> // будем использовать потоковый ввод-вывод
#include <vector> // матрицы оформим как векторы
#include "omp.h" // используем openMP
#include <fstream>
using namespace std; // чтобы не дописывать пространство имён к векторам, свопам и консоли
/* Поиск максимального элемента в столбце
* vector<vector<T> > &matrix матирца
*  int col столбец
*   int n размер
*   возвращает позицию максимального элемента в столбце
*/
template <typename T>// шаблон функции, реальная будет создана на этапе компиляции и масимально оптимизированна
unsigned int col_max(vector<vector<T>>& matrix, const int col, const unsigned int n) {
	T max = abs(matrix[col ][ col]);// берём за минимальный центровой элемент столбца..
	unsigned int maxPos = col;// ..запомним его индекс
#pragma omp parallel// параллелим поиски
	{
		T loc_max = max;// для каждого потока будет локальный максимум..
		unsigned int loc_max_pos = maxPos; // ...и его позиция
#pragma omp for// параллельно обрабытываем разные части строки
		for (int i = 0; i < n; ++i) {
			T element = abs(matrix[i ][ col]);// запомним текущий элемент
			if (element > loc_max) {
				// если он больше предыдущего максимуму
				loc_max = element;// меняем этот максимум
				loc_max_pos = i;// и запомненную позицию
			}
		}
#pragma omp critical// потоки не должны переписывать общий максимум, иначе из-за эффекта гонки будет беда. Помечаем область критической
		{
			if (loc_max > max) {
				// проверяем, больше ли найденные максимум чем тот максимум, который был найден в других потоках
				max = loc_max;// если да, то меняем "глобальный" максимум
				maxPos = loc_max_pos;// и запомненную позицию
			}
		}
	}
	return maxPos;// вернём позицию максимального
}

/* Триангуляция матрицы (приведение к "треугольному")
 * Треуго́льная матрица — матрица, у которой все элементы, стоящие ниже (или выше) главной диагонали, равны нулю.
* vector<vector<T> > &matrix матрица для триангулцияя
* int n её размерность
* Возвращает количество совершенных перестановок
*/
template <typename T>// шаблон функции, реальная будет создана на этапе компиляции и масимально оптимизированна
unsigned int triangulation(vector<vector<T>>& matrix, const unsigned int n) {
	unsigned int swapCount = 0;// здесь будет счётчик количества перестановок. Очевидно, что он только положительный.
	for (unsigned int i = 0; i < n - 1; ++i) {
		// пробегаем по всем строкам кроме поледней, потому что мы не сможем переставить последнюю+1
		if (0 == matrix[i ][ i]) {
			// если замена нужна
			const unsigned int imax = col_max(matrix, i, n);
			// поиск номера строки содержащей максимальный по модулю элемент столбца с номером i
			if (i != imax) {
				// если текущая строка не максимальна
				swap(matrix[i], matrix[imax]);// ставим ёё его место максимальной
				++swapCount;// накидываем счётчик перестановок
			}
		}
#pragma omp parallel for // параллелим эквивалентные преобразования
		for (int j = i + 1; j < n; ++j) {
			// пробегаем по всем элементам строки
			T mul = matrix[j ][ i] / matrix[i ][ i];// вычисляем число, на которое нужно домножить
			for (unsigned int k = i; k < n; ++k)// бежим по столбцам снова
				matrix[j ][ k] -= matrix[i ][ k] * mul;// домножаем элемент на число и складыываем строки
		}
	}
	return swapCount; // возвращаем кол-во перестановок
}

/* Поиск определителя методом Гаусса
 * vector<vector<T> > &matrix матрица для поиска
 * int n её размерность
 * Возвращает определитель
 */
template <typename T>// шаблон функции, реальная будет создана на этапе компиляции и масимально оптимизированна
T gauss_determinant(vector<vector<T>>& matrix, const unsigned int n) {
	const unsigned int swapCount = triangulation(matrix, n);
	// ищем количество перестановок строк и делаем триангуляцию матрицы
	T determinanit = 1; // объявляем определитель как 1, на случай если перестановок не будет
	if (swapCount % 2 == 1) // если количество перестановок нечётное...
		determinanit = -1;
	// ..очевидно, что определитель будет отрицательный, поскольку при каждой перестановке он меняет знак
	for (unsigned int i = 0; i < n; ++i)
		// не параллелим этот цикл поскольку на быстродействии это не скажется - только память копиями забьём
	{
		determinanit *= matrix[i][ i]; // считаем произведение элементов на главной диагонали
	}
	return determinanit; // возвращаем их
}

/* Решение СЛАУ методом Крамера
 * vector<vector<T> > &matrix - матрица неизвестых
 *  vector<T> &free_term_column - слобец свободных членов
 * int n - размерность
 * Возвращает вектор, содержащий решения к матрице
 */
template <typename T> // шаблон функции, реальная будет создана на этапе компиляции и масимально оптимизированна
vector<T> cramer_solving(vector<vector<T>>& matrix, vector<T>& free_term_column, int n) {
	vector<vector<T>> private_matrix(matrix);
	T mainDet = gauss_determinant(private_matrix, n); // вычисляем определитель матрицы неизвестных
	if (abs(mainDet) < 0.0001) // если он не подходит под решение по правилу Крамера
		throw invalid_argument("Определитель главной матрицы меньше нуля!"); //кидаем исключение
	vector<T> solution(n); // создаем вектор решений
#pragma omp parallel private(private_matrix)  // просим openMP сделать этот блок параллельно
	{
		private_matrix = matrix; // делаем копию матрицы, чтобы избежать работы в критической секции и эффекта гонки
#pragma omp for //параллелим цикл
		for (int i = 0; i < n; ++i) //бежим по всем столбцам
		{
			for (int j = 0; j < n; j++) private_matrix[j][i] = free_term_column[j];
			// заменяем столбец на столбец свободных членов
			solution[i] = gauss_determinant(private_matrix, n) / mainDet;
			// изем определитель и делим на главный, результат в вектор
		}
	}
	return solution; // возвращаем решения
}

int main() // точка входа
{

	int n = 3;

	vector<vector<double>> matrix(n); // матрица, построенная на векторах цифр с плавующей точкой
	for (int i = 0; i < n; ++i) // заполнение массива - строки
	{
		matrix[i].resize(n); // задаём новый размер кажой строке
		for (int j = 0; j < n; ++j) // заполнение массива - столбцы
			matrix[i][j] = rand() % 100; // заполяем элемент M_ij случайным числом
	}
	vector<double> column(n); // создаем столбец свободных членов
	for (int j = 0; j < n; ++j) //заполняем его - строки
		column[j] = rand() % 100; // заполняем элемент Column_j случайным числом
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) { cout << matrix[i][j] << " "; }
		cout << column[i] << endl;
	}
	try {
		vector<double> solution = cramer_solving(matrix, column, n);
		// запоминаем вектор корней СЛАУ, полученный из автоматически оптимизированной функции cramer_solving,
		//которая принимает матрицу для поиска, столбец свободных членов, размерность матрицы (без столбца св. членов)
		for (double element : solution) { cout << element << " "; }
	}
	catch (exception e) { cout << e.what() << endl; }

	system("pause.exe");
}
