#include <algorithm>
#include <iostream>
#include <vector>
#include "omp.h"
template <typename T>
int col_max(const std::vector<std::vector<T> > &matrix, int col, int n)
{
	T max = std::abs(matrix[col][col]);
	int maxPos = col;
#pragma omp parallel
	{
		T loc_max = max;
		T loc_max_pos = maxPos;
#pragma omp for
		for(int i = col + 1; i < n; ++i)
		{
			T element = std::abs(matrix[i][col]);
			if(element > loc_max)
			{
				loc_max = element;
				loc_max_pos = i;
			}
		}
#pragma omp critical
		{
			if(max < loc_max)
			{
				max = loc_max;
				maxPos = loc_max_pos;
			}
		}
	}
	return maxPos;
}
template <typename T>
int triangulation(std::vector<std::vector<T> > &matrix, int n)
{
	unsigned int swapCount = 0;
	if(0 == n)
		return swapCount;
	const int num_cols = matrix[0].size();
	for(int i = 0; i < n - 1; ++i)
	{
		unsigned int imax = col_max(matrix, i, n);
		if(i != imax)
		{
			swap(matrix[i], matrix[imax]);
			++swapCount;
		}
		for(int j = i + 1; j < n; ++j)
		{
			T mul = -matrix[j][i] / matrix[i][i];
			for(int k = i; k < num_cols; ++k)
			{
				matrix[j][k] += matrix[i][k] * mul;
			}
		}
	}
	return swapCount;
}
template <typename T>
T gauss_determinant(std::vector<std::vector<T> > &matrix, int n)
{
	unsigned int swapCount = triangulation(matrix, n);
	T determinanit = 1;
	if(swapCount % 2 == 1)
		determinanit = -1;
	for(int i = 0; i < n; ++i)
	{
		determinanit *= matrix[i][i];
	}
	return determinanit;
}

template <typename T>
std::vector<T> cramer_solving(std::vector<std::vector<T> > &matrix,
	std::vector<T> &free_term_column, int n)
{
	T mainDet = gauss_determinant(matrix, n);
	if(std::abs(mainDet) < 0.0001)
		throw 20;
	std::vector<T> solution(n);
#pragma omp parallel
	{
		std::vector<std::vector<T> > private_matrix = matrix;
#pragma omp for
		for(int i = 0; i < n; ++i)
		{
			std::swap(matrix[i], free_term_column);
			solution[i] = gauss_determinant(private_matrix, n) / mainDet;
			std::swap(matrix[i], free_term_column);
		}
	}
	return solution;
}
int main()
{
	for(int n = 50; n < 350; n += 50)
	{
		std::vector<std::vector<double> > matrix(n);
		for(int i = 0; i < n; ++i)
		{
			matrix[i].resize(n);
			for(int j = 0; j < n; ++j)
				matrix[i][j] = rand();
		}
		std::vector<double> column(n);
		for(int j = 0; j < n; ++j)
			column[j] = rand();
		double start_time = omp_get_wtime();
		std::vector<double> solution = cramer_solving(matrix, column, n);
		std::cout << n << " " << omp_get_wtime() - start_time << std::endl;
	}
}