#include "Matrix.h"
#include <omp.h>
#include <chrono>
void Multiply(Matrix*, Matrix*, Matrix*);
void MultiplyLinear(Matrix*, Matrix*, Matrix*);


int main()
{
	Matrix *firstMatrix, *secondMatrix;

	Matrix* resultMatrix;


	firstMatrix = new Matrix();
	resultMatrix = new Matrix();
	secondMatrix = new Matrix();



	firstMatrix->GetRandomMatrixToFile(3, 3, "../11.txt",10);
	secondMatrix->GetRandomMatrixToFile(3, 3, "../22.txt",10);






#pragma omp parallel sections
	{
#pragma omp section
		{
			firstMatrix->GetMatrixFromFile("../11.txt");  // Ввод исходных матриц
		}
#pragma omp section
		{
			secondMatrix->GetMatrixFromFile("../22.txt");
		}
	}


	MultiplyLinear(firstMatrix, secondMatrix,resultMatrix);
	resultMatrix->DebugOutpurMatrix();

	resultMatrix->WriteToFile("../result.txt");
	//firstMatrix.GetMatrixFromFile("S:\\11.txt");
	//secondMatrix.GetMatrixFromFile("S:\\22.txt");
	/*if (resultMatrix = Multiply(firstMatrix, secondMatrix))
	{
		//resultMatrix->DebugOutpurMatrix(); 
		
		resultMatrix->WriteToFile("../result.txt");  // Вывод результата
	}*/
	/*else
	{
		std::cout << "Failed...";
	}*/




	


std:getchar();
	return 0;
}

void Multiply(Matrix* firstMatrix, Matrix* secondMatrix,Matrix* resultMatrix)
{
	try
	{
		std::cout << "Paralel:\n";
		auto  start = std::chrono::system_clock::now();
		if (firstMatrix->N != secondMatrix->M)  throw "can't multiply n1 not equal m2";
		resultMatrix->Init(firstMatrix->M, secondMatrix->N);


		int str;
		int stolb;
		int k;
		int i;



#pragma omp parallel
		{
		
#pragma omp for private(str, stolb, k, i) schedule(static)

			for (i = 0; i < resultMatrix->M * resultMatrix->N; i++)
			{
				str = i % resultMatrix->M;
				stolb = i / resultMatrix->M;


				for (k = 0; k < firstMatrix->N; k++)
				{
					resultMatrix->matrix[str][stolb] += firstMatrix->matrix[str][k] * secondMatrix->matrix[k][stolb];
				}
			}
		}
		std::chrono::duration<double>  time = std::chrono::system_clock::now() - start;
		std::cout << "time:" << time.count() << std::endl;

	}
	catch (const char* str)
	{
		std::cout << str << std::endl;
		return;
	}

}


void MultiplyLinear(Matrix* firstMatrix, Matrix* secondMatrix,Matrix *resultMatrix)
{

	try
	{
		std::cout << "Linear:\n";
		auto  start = std::chrono::system_clock::now();
		if (firstMatrix->N != secondMatrix->M)  throw "can't multiply n1 not equal m2";
		resultMatrix->Init(firstMatrix->M, secondMatrix->N);

		int str;
		int stolb;
		int k;
		int i;




			for (i = 0; i < resultMatrix->M * resultMatrix->N; i++)
			{
				str = i / resultMatrix->M;
				stolb = i % resultMatrix->M;

				for (k = 0; k < firstMatrix->N; k++)
				{
					std::cout << firstMatrix->matrix[str][k]<<"+"<<secondMatrix->matrix[k][stolb];
					resultMatrix->matrix[str][stolb] += firstMatrix->matrix[str][k] * secondMatrix->matrix[k][stolb];
					std::cout <<"===" <<resultMatrix->matrix[str][stolb] << std::endl;
					//resultMatrix->matrix[str][stolb] = k;
				}
			}
			std::chrono::duration<double>  time = std::chrono::system_clock::now() - start;
			std::cout << "time2:" << time.count() << std::endl;

			
	}
	catch (const char* str)
	{
		std::cout << str << std::endl;
		return;
	}

}




