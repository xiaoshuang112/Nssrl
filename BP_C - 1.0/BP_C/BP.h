#ifndef _BP_HPP_
#define _BP_HPP_

#include <windows.h>
namespace ANN {

#define num_node_input_BP	2//756//3780//784 //�����ڵ���
#define width_image_BP		64//64//28 //��һ��ͼ���
#define height_image_BP		64//32//28 //��һ��ͼ���
#define num_node_hidden_BP	10//160//120 //������ڵ���
#define num_node_output_BP	2 //�����ڵ���
#define alpha_learning_BP	0.8//0.8 //�������������ѧϰ��
#define beta_learning_BP	0.6//0.6 //�������������ѧϰ��
#define patterns_train_BP	60000//60000 //ѵ��ģʽ����(����)
#define patterns_test_BP	10000//10000 //����ģʽ����(����)
#define iterations_BP		500   //10000 //���ѵ������
#define accuracy_rate_BP	0.91 //Ҫ��ﵽ��׼ȷ��

	class BP {
	public:
		BP();
		~BP();

		void init(const char* initrain,const char* initest); //��ʼ��������ռ�
		bool train(); //ѵ��
		int predict(const float* data, int width, int height); //Ԥ��
		bool readModelFile(const char* name); //��ȡ��ѵ���õ�BP model
		

	protected:
		void release(); //�ͷ�����Ŀռ�
		bool saveModelFile(const char* name); //��ѵ���õ�model������������������Ľڵ�����Ȩֵ����ֵ
		bool initWeightThreshold(); //��ʼ��������[-1, 1]֮������С��
		int  ReadFile(char *path, float* data_dst,int* Label_dst); 
		bool getSrcData(const char* initrain,const char* initest); //��ȡJPG��BMPͼƬ

		void calcHiddenLayer(const float* data); //�������������
		void calcOutputLayer(); //������������
		void calcAdjuctOutputLayer(const int* data); //���������У�����
		void calcAdjuctHiddenLayer(); //����������У�����

		float calcActivationFunction(float x); //���㼤���������S�κ���
	//	float calcActivationFunction_outlayer(float x); //������������

		void updateWeightThresholdOutputLayer(); //�����������������Ȩֵ����ֵ
		void updateWeightThresholdHiddenLayer(const float* data); //�����������������Ȩֵ����ֵ
		float test(); //ѵ����һ�μ���һ��׼ȷ��

	private:
		float* weight1/*[num_node_input_BP][num_node_hidden_BP]*/; //�����������������Ȩֵ
		float* weight2/*[num_node_hidden_BP][num_node_output_BP]*/; //�����������������Ȩֵ
		float* threshold1/*[num_node_hidden_BP]*/; //��������ֵ
		float* threshold2/*[num_node_output_BP]*/; //�������ֵ
		float* output_hiddenLayer/*[num_node_hidden_BP]*/; //˳���������������ֵ
		float* output_outputLayer/*[num_node_output_BP]*/; //˳��������������ֵ
		float* adjust_error_outputLayer/*[num_node_output_BP]*/; //�洫���������У�����
		float* adjust_error_hiddenLayer/*[num_node_hidden_BP]*/; //�洫����������У�����

		float* data_input_train; //ԭʼ��׼�������ݣ�ѵ��
		int* data_output_train; //ԭʼ��׼���������ѵ��
		float* data_input_test; //ԭʼ��׼�������ݣ�����
		int* data_output_test; //ԭʼ��׼�������������
		char m_traindatainipath[MAX_PATH];   //ѵ���������ݼ�¼�ļ�ini·��
		char m_testdatainipath[MAX_PATH];    //���Լ����ݼ�¼�ļ�ini·��

		int  m_trainNum;  //ʵ��ѵ��������
		int  m_testNum;   //ʵ�ʲ���������
	};

}

#endif //_BP_HPP_