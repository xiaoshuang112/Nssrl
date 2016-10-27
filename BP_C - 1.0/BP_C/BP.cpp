#include "stdafx.h"
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <windows.h>
#include <math.h>
#include <omp.h>
#include <string> 
using namespace std; 

//#include <features2d/features2d.hpp>
//
//#include <nonfree/nonfree.hpp>

#include "opencv2/opencv.hpp"
//#include "opencv2/objdetect/objdetect.hpp"

#include "BP.h"
#include "VLMain.h"



namespace ANN {

BP::BP()
{
	weight1 = NULL;
	weight2 = NULL;
	threshold1 = NULL;
	threshold2 = NULL;
	output_hiddenLayer = NULL;
	output_outputLayer = NULL;
	adjust_error_outputLayer = NULL;
	adjust_error_hiddenLayer = NULL;

	data_input_train = NULL;
	data_output_train = NULL;
	data_input_test = NULL;
	data_output_test = NULL;
	m_trainNum = 0;
	m_testNum = 0;
}

BP::~BP()
{
	release();
}

void BP::release()
{
	if (weight1) {
		delete[] weight1;
	}
	if (weight2) {
		delete[] weight2;
	}
	if (threshold1) {
		delete[] threshold1;
	}	
	if (threshold2) {
		delete[] threshold2;
	}
	if (output_hiddenLayer) {
		delete[] output_hiddenLayer;
	}	
	if (output_outputLayer) {
		delete[] output_outputLayer;
	}
	if (adjust_error_outputLayer) {
		delete[] adjust_error_outputLayer;
	}	
	if (adjust_error_hiddenLayer) {
		delete[] adjust_error_hiddenLayer;
	}
	if (data_input_train) {
		delete[] data_input_train;
	}
	if (data_output_train) {
		delete[] data_output_train;
	}
	if (data_input_test) {
		delete[] data_input_test;
	}
	if (data_output_test) {
		delete[] data_output_test;
	}
}

bool BP::initWeightThreshold()
{
	srand(time(0) + rand());

	for (int i = 0; i < num_node_input_BP; i++) {
		for (int j = 0; j < num_node_hidden_BP; j++) {
			weight1[i*num_node_hidden_BP+j] = -1 + 2 * ((float)rand()) / RAND_MAX; //[-1, 1]
		}
	}

	for (int i = 0; i < num_node_hidden_BP; i++) {
		for (int j = 0; j < num_node_output_BP; j++) {
			weight2[i*num_node_output_BP+j] = -1 + 2 * ((float)rand()) / RAND_MAX;
		}
	}

	for (int i = 0; i < num_node_hidden_BP; i++) {
		threshold1[i] = -1 + 2 * ((float)rand()) / RAND_MAX;
	}

	for (int i = 0; i < num_node_output_BP; i++) {
		threshold2[i] = -1 + 2 * ((float)rand()) / RAND_MAX;
	}

	return true;
}

static int reverseInt(int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = i & 255;
	ch2 = (i >> 8) & 255;
	ch3 = (i >> 16) & 255;
	ch4 = (i >> 24) & 255;
	return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

static void readMnistImages(std::string filename, int* data_dst, int num_image)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	assert(file.is_open());

	int magic_number = 0;
	int number_of_images = 0;
	int n_rows = 0;
	int n_cols = 0;
	file.read((char*)&magic_number, sizeof(magic_number));
	magic_number = reverseInt(magic_number);
	file.read((char*)&number_of_images, sizeof(number_of_images));
	number_of_images = reverseInt(number_of_images);
	assert(number_of_images == num_image);
	file.read((char*)&n_rows, sizeof(n_rows));
	n_rows = reverseInt(n_rows);
	file.read((char*)&n_cols, sizeof(n_cols));
	n_cols = reverseInt(n_cols);
	assert(n_rows == height_image_BP && n_cols == width_image_BP);



	for (int i = 0; i < number_of_images; ++i) {

		//IplImage *pTemp = NULL;
		//pTemp = cvCreateImage(cvSize(width_image_BP,height_image_BP),8,1);

		for (int r = 0; r < n_rows; ++r) {
			for (int c = 0; c < n_cols; ++c) {
				unsigned char temp = 0;
				file.read((char*)&temp, sizeof(temp));
				//data_dst[i * num_node_input_BP + r * n_cols + c] = (int)temp; //formula[1]
				if (temp > 128) {
					data_dst[i * num_node_input_BP + r * n_cols + c] = 1;
					//pTemp->imageData[r * n_cols + c] = 255;
				} else {
					data_dst[i * num_node_input_BP + r * n_cols + c] = 0;
					//pTemp->imageData[r * n_cols + c] = 0;
				}
			}
		}

		//cvSaveImage("Bina.jpg",pTemp);
		//cvReleaseImage(&pTemp);
	}
}

static void readMnistLabels(std::string filename, int* data_dst, int num_image)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	assert(file.is_open());

	int magic_number = 0;
	int number_of_images = 0;
	file.read((char*)&magic_number, sizeof(magic_number));
	magic_number = reverseInt(magic_number);
	file.read((char*)&number_of_images, sizeof(number_of_images));
	number_of_images = reverseInt(number_of_images);
	assert(number_of_images == num_image);

	for (int i = 0; i < number_of_images; ++i) {
		unsigned char temp = 0;
		file.read((char*)&temp, sizeof(temp));
		data_dst[i * num_node_output_BP + temp] = 1; //formula[2]
	}
}


int BP::ReadFile(char *path, float* data_dst,int* Label_dst)  
{  

	FILE *fp = NULL;
	fopen_s(&fp,path, "rb");
	if (fp == NULL) 
	{
		printf("Cannot open file.\n");
		exit(1);
	}

	int i = 0;
	 

	if(fp==NULL)
	{
		return 0;
	}

	int p =0;
	while (fscanf_s(fp, "%f %f %d", data_dst+2*i, data_dst+2*i+1, &p) != EOF) 
	{
		*(Label_dst+2*i + p) = 1;
		i++;
	}


	fflush(fp);
	fclose(fp);

	return i;
} 



bool BP::getSrcData(const char* initrain,const char* initest)
{
	if (initrain==NULL||initest==NULL)
	{
		return false;
	}
	sprintf_s(m_traindatainipath,"%s",initrain);
	sprintf_s(m_testdatainipath,"%s",initest);

	//��txt�ļ���ȡͼ�����ݼ���ǩ
	m_trainNum =  ReadFile(m_traindatainipath,data_input_train,data_output_train);
	m_testNum =  ReadFile(m_testdatainipath,data_input_test,data_output_test);

	return true;
}

void BP::init(const char* initrain,const char* initest)
{
	weight1 = new float[num_node_input_BP * num_node_hidden_BP];
	memset(weight1,0,sizeof(float)*num_node_input_BP * num_node_hidden_BP);
	weight2 = new float[num_node_hidden_BP * num_node_output_BP];
	memset(weight2,0,sizeof(float)*num_node_hidden_BP * num_node_output_BP);

	threshold1 = new float[num_node_hidden_BP];
	memset(threshold1,0,sizeof(float)*num_node_hidden_BP);

	threshold2 = new float[num_node_output_BP];
	memset(threshold2,0,sizeof(float)*num_node_output_BP);
	output_hiddenLayer = new float[num_node_hidden_BP];
	memset(output_hiddenLayer,0,sizeof(float)*num_node_hidden_BP);
	output_outputLayer = new float[num_node_output_BP];
	memset(output_outputLayer,0,sizeof(float)*num_node_output_BP);


	if (initrain&&initest)
	{
		adjust_error_outputLayer = new float[num_node_output_BP];
		memset(adjust_error_outputLayer,0,sizeof(float)*num_node_output_BP);
		adjust_error_hiddenLayer = new float[num_node_hidden_BP];
		memset(adjust_error_hiddenLayer,0,sizeof(float)*num_node_hidden_BP);

		data_input_train = new float[patterns_train_BP * num_node_input_BP];
		if (NULL==data_input_train)
		{
			return;
		}
		memset(data_input_train, 0, sizeof(float) * patterns_train_BP * num_node_input_BP);
		data_output_train = new int[patterns_train_BP * num_node_output_BP];

		memset(data_output_train, 0, sizeof(int) * patterns_train_BP * num_node_output_BP);
		data_input_test = new float[patterns_test_BP * num_node_input_BP];
		memset(data_input_test, 0, sizeof(float) * patterns_test_BP * num_node_input_BP);
		data_output_test = new int[patterns_test_BP * num_node_output_BP];
		memset(data_output_test, 0, sizeof(int) * patterns_test_BP * num_node_output_BP);

		initWeightThreshold();
		getSrcData(initrain,initest);
	}
}



float BP::calcActivationFunction(float x)
{
	return 1.0 / (1.0 + exp(-x)); //formula[4] formula[5] formula[7]
}

void BP::calcHiddenLayer(const float* data)
{
	#pragma omp parallel for
	for (int i = 0; i < num_node_hidden_BP; i++) {
		float tmp = 0;
		#pragma omp parallel for
		for (int j = 0; j < num_node_input_BP; j++) {
			tmp += data[j] * weight1[j*num_node_hidden_BP+i];
		}

		tmp -= threshold1[i]; //formula[3]
		output_hiddenLayer[i] = calcActivationFunction(tmp);
	}
}

void BP::calcOutputLayer()
{
	#pragma omp parallel for
	for (int i = 0; i < num_node_output_BP; i++) {
		float tmp = 0;
		#pragma omp parallel for
		for (int j = 0; j < num_node_hidden_BP; j++) {
			tmp += output_hiddenLayer[j] * weight2[j*num_node_output_BP+i];
		}

		tmp -= threshold2[i]; //formula[6]
		output_outputLayer[i] = calcActivationFunction(tmp);
		//output_outputLayer[i] = calcActivationFunction_outlayer(tmp);
	}
}

void BP::calcAdjuctOutputLayer(const int* data)
{
	for (int i = 0; i < num_node_output_BP; i++) {
		adjust_error_outputLayer[i] = (data[i] - output_outputLayer[i]) *
			output_outputLayer[i] * (1.0 - output_outputLayer[i]); //formula[8], f'(x)= f(x)*(1. - f(x))
	}
}

void BP::calcAdjuctHiddenLayer()
{
	#pragma omp parallel for
	for (int i = 0; i < num_node_hidden_BP; i++) {
		float tmp = 0;
		#pragma omp parallel for
		for (int j = 0; j < num_node_output_BP; j++) {
			tmp += weight2[i*num_node_output_BP+j] * adjust_error_outputLayer[j];
		}

		adjust_error_hiddenLayer[i] = tmp * (output_hiddenLayer[i] * (1.0 - output_hiddenLayer[i])); //formula[9]
	}
}

void BP::updateWeightThresholdOutputLayer()
{
	for (int i = 0; i < num_node_output_BP; i++) {
		for (int j = 0; j < num_node_hidden_BP; j++) {
			weight2[j*num_node_output_BP+i] += (alpha_learning_BP * adjust_error_outputLayer[i] * output_hiddenLayer[j]); //formula[10]
		}

		threshold2[i] += (alpha_learning_BP * adjust_error_outputLayer[i]); //formula[11]
	}
}

void BP::updateWeightThresholdHiddenLayer(const float* data)
{
	#pragma omp parallel for
	for (int i = 0; i < num_node_hidden_BP; i++) {
		#pragma omp parallel for
		for (int j = 0; j < num_node_input_BP; j++) {
			weight1[j*num_node_hidden_BP+i] += (beta_learning_BP * adjust_error_hiddenLayer[i] * data[j]); //formula[12]
		}

		threshold1[i] += (beta_learning_BP * adjust_error_hiddenLayer[i]); //formula[13]
	}
}



float BP::test()
{
	int count_accuracy = 0;

	for (int num = 0; num < m_testNum; num++) {
		float* p1 =  data_input_test + num * num_node_input_BP ;
		calcHiddenLayer(p1);
		calcOutputLayer();

		float max_value = -9999;
		int pos = -1;

		for (int i = 0; i < num_node_output_BP; i++) {
			if (output_outputLayer[i] > max_value) {
				max_value = output_outputLayer[i];
				pos = i;
			}
		}

		int* p2 = data_output_test + num * num_node_output_BP;
		if (p2[pos] == 1) {
			count_accuracy++;
		}
		Sleep(1);
	}

	return (count_accuracy * 1.0 / m_testNum);
}

bool BP::saveModelFile(const char* name)
{

	/************************************************************************/
	/* print  weight1                                              */
	/************************************************************************/
	char node_input_BP[256] = "weight1.xls";
	FILE  *fp  = NULL;
	freopen_s(&fp,node_input_BP,"at",stdout);
	if(fp==NULL)
	{
		return 0;
	}

	long fileSizeTemp=ftell(fp);
	fseek(fp,0,SEEK_END );
	fileSizeTemp=ftell(fp);
	if(fileSizeTemp==0)
	{
		for (int i = 0; i < num_node_input_BP;i++)
		{
			for (int j  =0; j < num_node_hidden_BP; j++)
			{
				printf("%f,\t",weight1[num_node_hidden_BP*i + j]);
			}
			printf("\n");
		}
	}
	fflush(fp);
	fclose(fp);

	/************************************************************************/
	/*                   print      threshold1                    */
	/************************************************************************/
	char threshold1_bp[256]= "threshold1.xls";
	FILE  *fp1  = NULL;
	freopen_s(&fp1,threshold1_bp,"at",stdout);
	if(fp1==NULL)
	{
		return 0;
	}

	fileSizeTemp=ftell(fp1);
	fseek(fp1,0,SEEK_END );
	fileSizeTemp=ftell(fp1);
	if(fileSizeTemp==0)
	{
		for (int i = 0; i < num_node_hidden_BP;i++)
		{
		    printf("%f,\t",threshold1[i]);
			printf("\n");
		}
	}
	fflush(fp1);
	fclose(fp1);

	/************************************************************************/
	/*                   print      weight2                    */
	/************************************************************************/
	char weight2_bp[256]= "weight2.xls";
	FILE  *fp2  = NULL;
	freopen_s(&fp2,weight2_bp,"at",stdout);
	if(fp2==NULL)
	{
		return 0;
	}

	fileSizeTemp=ftell(fp2);
	fseek(fp2,0,SEEK_END );
	fileSizeTemp=ftell(fp2);
	if(fileSizeTemp==0)
	{
		for (int i = 0; i < num_node_hidden_BP*num_node_output_BP;i++)
		{
			printf("%f,\t",weight2[i]);
			printf("\n");
		}
	}
	fflush(fp2);
	fclose(fp2);

	/************************************************************************/
	/*                   print      threshold2                    */
	/************************************************************************/
	char threshold2_bp[256]= "threshold2.xls";
	FILE  *fp3 = NULL;
	freopen_s(&fp3,threshold2_bp,"at",stdout);
	if(fp3==NULL)
	{
		return 0;
	}

	fileSizeTemp=ftell(fp3);
	fseek(fp3,0,SEEK_END );
	fileSizeTemp=ftell(fp3);
	if(fileSizeTemp==0)
	{
		for (int i = 0; i < num_node_output_BP;i++)
		{
			printf("%f,\t",threshold2[i]);
			printf("\n");
		}
	}
	fflush(fp3);
	fclose(fp3);


	FILE* Fp = NULL;
	fopen_s(&Fp,"BP.model", "wb");
	if (Fp == NULL) {
		return false;
	}

	int num_node_input = num_node_input_BP;
	int num_node_hidden = num_node_hidden_BP;
	int num_node_output = num_node_output_BP;
	fwrite(&num_node_input, sizeof(int), 1,  Fp);
	fwrite(&num_node_hidden, sizeof(int), 1, Fp);
	fwrite(&num_node_output, sizeof(int), 1, Fp);
	fwrite(weight1, sizeof(float)*num_node_input_BP*num_node_hidden_BP, 1, Fp);
	fwrite(threshold1, sizeof(float)*num_node_hidden_BP, 1, Fp);
	fwrite(weight2, sizeof(float)*num_node_hidden_BP*num_node_output_BP, 1, Fp);
	fwrite(threshold2, sizeof(float)*num_node_output_BP, 1, Fp);

	fflush(Fp);
	fclose(Fp);

	return true;
}

bool BP::readModelFile(const char* name)
{
	FILE* fp = NULL;
	fopen_s(&fp, name, "rb");
	if (fp == NULL) {
		return false;
	}

	int num_node_input, num_node_hidden, num_node_output;

	fread(&num_node_input, sizeof(int), 1, fp);
	assert(num_node_input == num_node_input_BP);
	fread(&num_node_hidden, sizeof(int), 1, fp);
	assert(num_node_hidden == num_node_hidden_BP);
	fread(&num_node_output, sizeof(int), 1, fp);
	assert(num_node_output == num_node_output_BP);
	fread(weight1, sizeof(float)*num_node_input_BP*num_node_hidden_BP, 1, fp);
	fread(threshold1, sizeof(float)*num_node_hidden_BP, 1, fp);
	fread(weight2, sizeof(float)*num_node_hidden_BP*num_node_output_BP, 1, fp);
	fread(threshold2, sizeof(float)*num_node_output_BP, 1, fp);

	fflush(fp);
	fclose(fp);

	return true;
}

int BP::predict(const float* data, int width, int height)
{
	//assert(data && width == width_image_BP && height == height_image_BP);

	const float* p = data;
	calcHiddenLayer(p);
	calcOutputLayer();

	float max_value = -9999;
	int ret = -1;

	for (int i = 0; i < num_node_output_BP; i++) {
		if (output_outputLayer[i] > max_value) {
			max_value = output_outputLayer[i];
			ret = i;
		}
	}

	return ret;
}

bool BP::train()
{
	int i = 0;
	for (i = 0; i < iterations_BP; i++) {
		std::cout << "iterations : " << i;

		float accuracyRate = test();
		std::cout << ",    accuray rate: " << accuracyRate << std::endl;
		if (accuracyRate > accuracy_rate_BP) {
			saveModelFile("bp.xls");
			std::cout << "generate bp model" << std::endl;
			break;
		}

		for (int j = 0; j < m_trainNum; j++) {
			float* p1 = data_input_train + j * num_node_input_BP;
			calcHiddenLayer(p1);
			calcOutputLayer();

			int* p2 = data_output_train + j * num_node_output_BP;
			calcAdjuctOutputLayer(p2);
			calcAdjuctHiddenLayer();

			updateWeightThresholdOutputLayer();
			float* p3 = data_input_train + j * num_node_input_BP;
			updateWeightThresholdHiddenLayer(p3);
		}
	}

	if (i == iterations_BP) {
		saveModelFile("bp.xls");
		std::cout << "generate bp model" << std::endl;
	}

	return true;
}

}
