#include "stdafx.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Utils.h"
#include "Matrix.h"

#define SIZE 3
using namespace std;
void getFreeCoef(Matrix& inputMatrix, myArray& freeMatrix);
int main(int argc, char *argv[])
{
	int rank, size;     // for storing this process' rank, and the number of processes
	int *sendcounts;    // array describing how many elements to send to each process
	int *displs;        // array describing the displacements where each segment begins
	int n_part;
	int part_size;
	

	Matrix mattrix;
	myArray *matrixArr = new myArray();
	myArray *array= new myArray();
	myArray *free= new myArray();
	myArray *answerArr= new myArray();

	int n = 0;
	double error = 0.1;
	

	double* answer=0;
	double *rec_buf=0;       
	double* mattrixToScaterrv = 0;
	double* freeToScaterrv = 0;
	double* priblizhToScaterrv = 0;
	double totalTime=0;
	double all_totalTime = 0;



	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	double t_start = 0, t_end = 0;
	
	sendcounts = (int *) malloc(sizeof(int)*size);
	displs = (int *)malloc(sizeof(int)*size);
	
		
	if (rank == 0)	{				answer = new double[size];		//Считываем данные в конервом процессе	
		t_start = MPI_Wtime();	
		//array->GetRandomArrayToFile(3, "D:\\arrayRandom.txt");
		array->GetArrayFromFile("D:\\arrayRandom.txt");
		//array->GetArrayFromFile("C:\\array1.txt");
		//mattrix.GetRandomMatrixToFile(3, 4, "D:\\matrixRand.txt");
		mattrix.GetMatrixFromFile("D:\\matrixRand.txt");
		//mattrix.GetMatrixFromFile("C:\\matrix1.txt");
		getFreeCoef(mattrix, *free);
		t_end = MPI_Wtime();
		totalTime += t_end - t_start;		printf("Time to readFile: %f sec\n", t_end - t_start);	
		n = array->size;
		mattrixToScaterrv = new double[n*n];			
		mattrix.MatrixToArray(*matrixArr);
		mattrixToScaterrv = matrixArr->data;

	}
	
	t_start = MPI_Wtime();
	MPI_Bcast(&n, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
	priblizhToScaterrv = new double[n];
	freeToScaterrv = new double[n];
	
	if (rank == 0)
	{
		freeToScaterrv = free->data;
		priblizhToScaterrv = array->data;
	}
	
	MPI_Bcast(&error, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	MPI_Bcast(priblizhToScaterrv, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	MPI_Bcast(freeToScaterrv, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	   
	n_part = (n / size) + (n%size > rank ? 1 : 0);
	//printf("rank:%d part: %d\n", rank, n_part);

	part_size = n * n_part;	//printf("rank:%d partsize: %d\n", rank, part_size);

	MPI_Allgather(&part_size, 1, MPI_INT, sendcounts, 1, MPI_INT, MPI_COMM_WORLD);
	displs[0] = 0;
	for (int i = 1; i < size; i++)	{	displs[i] = displs[i - 1] + sendcounts[i - 1];	}

	 	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	t_end = MPI_Wtime();
	totalTime+= t_end - t_start;
	printf("Time to preapre calc for rank:%d : %f sec\n",rank, t_end - t_start);
	


	if (part_size != 0)
	{
		t_start = MPI_Wtime();
		rec_buf = new double[part_size];
		MPI_Scatterv(mattrixToScaterrv, sendcounts, displs, MPI_DOUBLE, rec_buf, part_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);


		if (rank == 0)
		{			for (int i = 0; i < size; i++)			{				sendcounts[i] = sendcounts[i] / n;				displs[i] = displs[i] / n;			}
		}

		int num_start = 0;

		for (int i = 0; i < rank; i++)
		{
			num_start += sendcounts[i];
		}
		num_start = num_start / n;
		

		double * result = new double[n_part];
	
		double norm = 0;
		int count = 0;
		do
		{
			count++;
			int countStr = 0;

			for (int str = num_start; str < num_start + n_part; str++)
			{

				result[str - num_start] = freeToScaterrv[str];
				for (int i = 0; i < n; i++)
				{
					if (i != str)
					{
						result[str - num_start] -= rec_buf[i + countStr * n] * priblizhToScaterrv[i];

					}
				}
				result[str - num_start] /= rec_buf[str + countStr * n];
				countStr++;
			}
			norm = fabs(priblizhToScaterrv[0] - result[0]);

			for (int h = 0; h < n_part; h++)
			{
				if (fabs(priblizhToScaterrv[h] - result[h]) > norm)
				{		
					norm = fabs(priblizhToScaterrv[h] - result[h]);
				}
				priblizhToScaterrv[h] = result[h];
			}
			
		} while (count<5);
		
		MPI_Gatherv(result, n_part, MPI_DOUBLE, answer, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		t_end = MPI_Wtime();
		totalTime += t_end - t_start;
		printf("Time to Jacobi calc for %d str, on rank:%d : %f sec\n", n_part,rank, t_end - t_start);
	}
	MPI_Reduce(&totalTime, &all_totalTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	if (rank == 0)
	{
		printf("all total time: %f", all_totalTime);
		answerArr->SetData(answer,n);
		answerArr->WriteToFile("D:\\answer.txt");
	}
	
	
MPI_Finalize();

	return 0;
}


void getFreeCoef(Matrix &inputMatrix, myArray &freeMatrix)
{
	freeMatrix.size= inputMatrix.M;
	freeMatrix.data = new double[freeMatrix.size];
	for (int i = 0; i < freeMatrix.size; i++)
	{
		freeMatrix.data[i] = inputMatrix.data[i][inputMatrix.N - 1];
	}
	//freeMatrix.DebugOutpurMatrix();
	inputMatrix.N--;
}