#pragma once
#include "Utils.h"
#include <ctime>

class Matrix
{
public:
	
	int** matrix = nullptr;
	int M, N;
	void GetMatrixFromFile(std::string FilePath);
	void DebugOutpurMatrix();
	void GetRandomMatrixToFile(int, int, std::string,int maxCount);
	void WriteToFile(std::string);
	void Init(int M, int N);
	
};