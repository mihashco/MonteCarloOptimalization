// MonteCarloOptimalization.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//
#include <stdafx.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>

double f_x(double point[],int n); // funkcja zwraca wartosc f(x) dla podanego punktu
void fx_min(int iter, int n, bool test);

double fx_min_test_table[100];
int run_time_table[100];

double x_i_min = -20.0; //dolna granica dziedziny funkcji
double x_i_max = 20.0; //gorna granica dziedziny funkcji
double rand_max = RAND_MAX / 40.0; //ograniczenie zbioru losowania do podanej dziedziny

int main(int argc, char **argv)
{
	omp_set_num_threads(2);
	omp_set_nested(1);

	srand(time(NULL));
	int n;  //wymiar zadania. 
	int iter; //maksymalna liczba iteracji 

	if (argc == 1)
	{
		printf("Optymalizacja nieliniowa, szukanie prostych metoda Monte-Carlo. \n");
		printf("Podaj wymiar problemu do rozwiazania n \n");
		scanf_s("%d", &n);
		printf("Wymiar jest rowny %d \n", n);

		printf("Podaj liczbe iteracji \n");
		scanf_s("%d", &iter);
		printf("Liczba iteracji : %d\n", iter);

		fx_min(iter, n, false);
		
	}
	else if (argc == 2 && strcmp("test", argv[1]) == 0)
	{
		int i;
		iter = 1000;
		n = 2;

		for (i = 0; i < 100; i++)
		{
			system("cls");
			printf("Obliczanie... %d \n", i);
			fx_min(iter, n, true);
		}

		printf("Test \t Czas [s] \t Wynik \n");
		for (i = 0; i < 100; i++)
		{
			printf("%d \t %.3f \t %f \n", i, (double) run_time_table[i]/ (double) CLK_TCK, fx_min_test_table[i]);
		}

	}

	system("pause");
	return 0;
}

double f_x(double point[], int d)
{
	int i; //iterator tablicy
	double suma = 0.0; //czesc z funkcji  danej w zadaniu
	double iloczyn = 0.0; //czesc z funkcji danej w zadaniu
	double ret_val; 

	if (!point)
	{
		//printf("Invalid parameter in function [%s] ", __func__);
		return -1.0; // TODO: replace with macro
	}

	//w warunkach zadania iteracja od i = 1 do n.
	for (i = 1; i <= d; i++)
	{
		suma += point[i-1] * point[i-1];
	}

	double cos_val = cos(point[0] / 1); // wartoœæ cos dla pierwszej iteracji obliczeñ.
	iloczyn = cos_val; 

	for (i = 1; i <= d; i++)
	{
		iloczyn *= cos(point[i - 1] / i);
	}

	ret_val = 1 / 40 * suma + 1 - iloczyn; // funkcja z warunków zadania ma tak¹ postaæ. 
	return ret_val;
}

void fx_min(int iter, int n, bool test)
{
	double xmin;

	double start_time = clock();

	iter = (int)pow((double)iter, n);
	static int tc = 0; // numer testu, iterator tablicy testow

	double *point1 = (double *)malloc(n * sizeof * point1); // punkty losowane w kazdej iteracji.
	double *point2 = (double *)malloc(n * sizeof * point2);
	double *point3 = (double *)malloc(n * sizeof * point3);
	double *point4 = (double *)malloc(n * sizeof * point4);

	////losowanie punktu (krok 0 algorytmu)
	int i;
	for (i = 0; i < n; i++)
	{
		double val = rand() / rand_max - x_i_max;
		point1[i] = val;
	}

	xmin = f_x(point1, n);

	double xmin1 = xmin, xmin2 = xmin, xmin3 = xmin, xmin4 = xmin;

	unsigned long long int it;

#pragma omp parallel
	{
		omp_get
		iter = (int)iter;
		for (it = 0; it < iter; it++)
		{
			for (i = 0; i < n; i++)
			{
				double val1 = rand() / rand_max - 20; // przedzia³ od -20 do -10
				double val2 = rand() / rand_max - 10; // przedzia³ od -10 do 0
				double val3 = rand() / rand_max; //przedza³ od 0 do 10
				double val4 = rand() / rand_max + 10; //przedzial od 10 do 20

				point1[i] = val1;
				point2[i] = val2;
				point3[i] = val3;
				point4[i] = val4;
			}

			double temp1_xmin = f_x(point1, n);
			double temp2_xmin = f_x(point2, n);
			double temp3_xmin = f_x(point3, n);
			double temp4_xmin = f_x(point4, n);

			if (temp1_xmin < xmin1) xmin1 = temp1_xmin;
			if (temp2_xmin < xmin2) xmin2 = temp2_xmin;
			if (temp3_xmin < xmin3) xmin3 = temp3_xmin;
			if (temp4_xmin < xmin4) xmin4 = temp4_xmin;

			#pragma omp barrier
		}
	}

	double tab[] = { xmin1, xmin2, xmin3, xmin4 };
	double temp;

	xmin = tab[0];
	for (i = 1; i < 4; i++)
	{
		if (tab[i] < xmin) xmin = tab[i];
	}

	printf("MINIMUM IS %f\n ", xmin);

	if (test == true)
	{
		fx_min_test_table[tc] = xmin;
		run_time_table[tc] = clock() - start_time;
		tc++;
	}

	if (tc == 100)
	{
		tc = 0;
	}
}