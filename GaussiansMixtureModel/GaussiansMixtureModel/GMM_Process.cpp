#include "stdafx.h"
#include "GaussiansMixtureModel.h"
#include "GMM_Process.h"
#include "math.h"


GMM_Process::GMM_Process(void)
	: Image_Width(0)
	, Image_Height(0)
	, Gaussian_Pixel_1(NULL)
	, Gaussian_Pixel_2(NULL)
	, Gaussian_Pixel_3(NULL)
	, INIT_FLAG(false)
{
}

GMM_Process::~GMM_Process(void)
{
}

void GMM_Process::OnInitGMM(int Width, int Height)
{
	if(!INIT_FLAG)
	{
		Image_Width = Width;
		Image_Height = Height;

		Gaussian_Pixel_1 = new PIXEL[Image_Width * Image_Height];
		Gaussian_Pixel_2 = new PIXEL[Image_Width * Image_Height];
		Gaussian_Pixel_3 = new PIXEL[Image_Width * Image_Height];

		ZeroMemory(Gaussian_Pixel_1, sizeof(PIXEL) * (Image_Width * Image_Height));
		ZeroMemory(Gaussian_Pixel_2, sizeof(PIXEL) * (Image_Width * Image_Height));
		ZeroMemory(Gaussian_Pixel_3, sizeof(PIXEL) * (Image_Width * Image_Height));
	
		for(int i=0;i<(Width * Height);i++)
		{
			Gaussian_Pixel_1[i].EMPTY_FLAG = true;
			Gaussian_Pixel_2[i].EMPTY_FLAG = true;
			Gaussian_Pixel_3[i].EMPTY_FLAG = true;
		}

		INIT_FLAG = true;
	}
	else
	{
	}
}

void GMM_Process::Gaussian_Initialize(BYTE *Input, BYTE *Foreground_Image, BYTE *Background_Image)
{
	for(int i=0;i<(Image_Width * Image_Height);i++)
	{
		if(Gaussian_Pixel_1[i].EMPTY_FLAG)		//ù��° ����þ��� ����°�
		{
			GenerateGaussian(&Gaussian_Pixel_1[i], Input[i], 1);	//ù��° ����þ� ����
		}


		else if(!Gaussian_Pixel_1[i].EMPTY_FLAG && Gaussian_Pixel_2[i].EMPTY_FLAG)	//�ι�° ����þ��� ����°�
		{
			if(fabs(Input[i] - Gaussian_Pixel_1[i].mean) < LAMDA * Gaussian_Pixel_1[i].variance)	//Input���� ù��° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_1[i], Input[i]);	// ù��° ����þ� Update
			}
			else //ù��° ����þȿ� ������ �������
			{
				GenerateGaussian(&Gaussian_Pixel_2[i], Input[i], WEIGHT);	//�ι�° ����þ� ����

				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//ù��° ����þ� ����ġ ����

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
		}


		else if(!Gaussian_Pixel_1[i].EMPTY_FLAG && !Gaussian_Pixel_2[i].EMPTY_FLAG && Gaussian_Pixel_3[i].EMPTY_FLAG)	// ����° ����þ��� ����°�
		{
			if(fabs(Input[i] - Gaussian_Pixel_1[i].mean) < LAMDA * Gaussian_Pixel_1[i].variance)	//ù��° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_1[i], Input[i]);	//ù��° ����þ� Update

				Gaussian_DropWeight(&Gaussian_Pixel_2[i]);	//�ι�° ����þ� ����ġ ����

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
			else if(fabs(Input[i] - Gaussian_Pixel_2[i].mean) < LAMDA * Gaussian_Pixel_2[i].variance)	//�ι�° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_2[i], Input[i]);	//�ι�° ����þ� Upate

				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//ù��° ����þ� ����ġ ����

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
			else //ù��°, �ι�° ����þȿ� ������ ������
			{
				GenerateGaussian(&Gaussian_Pixel_3[i], Input[i], WEIGHT);	//����° ����þ� ����

				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//ù��°, �ι�° ����þ� ����ġ ����
				Gaussian_DropWeight(&Gaussian_Pixel_2[i]);

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
		}


		else if(!Gaussian_Pixel_1[i].EMPTY_FLAG && !Gaussian_Pixel_2[i].EMPTY_FLAG && !Gaussian_Pixel_3[i].EMPTY_FLAG)	//����þ��� 3�� ��������
		{
			if(fabs(Input[i] - Gaussian_Pixel_1[i].mean) < LAMDA * Gaussian_Pixel_1[i].variance)	//ù��° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_1[i], Input[i]);	//ù��° ����þ� Update

				Gaussian_DropWeight(&Gaussian_Pixel_2[i]);	//�ι�° ����° ����þ� ����ġ ����
				Gaussian_DropWeight(&Gaussian_Pixel_3[i]);

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
			else if(fabs(Input[i] - Gaussian_Pixel_2[i].mean) < LAMDA * Gaussian_Pixel_2[i].variance)	//�ι�° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_2[i], Input[i]);	//�ι�° ����þ� Upate

				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//ù��° ����° ����þ� ����ġ ����
				Gaussian_DropWeight(&Gaussian_Pixel_3[i]);

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
			else if(fabs(Input[i] - Gaussian_Pixel_3[i].mean) < LAMDA * Gaussian_Pixel_3[i].variance)	//�ι�° ����þȿ� ���Ҷ�
			{
				Gaussian_Update(&Gaussian_Pixel_3[i], Input[i]);	//����° ����þ� Upate

				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//ù��° �ι�° ����þ� ����ġ ����
				Gaussian_DropWeight(&Gaussian_Pixel_2[i]);

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
			else  //1,2,3��° ����þȿ� ������ ������
			{
				Gaussian_DropWeight(&Gaussian_Pixel_1[i]);	//1,2,3 ��° ����þ� ����ġ ����
				Gaussian_DropWeight(&Gaussian_Pixel_2[i]);
				Gaussian_DropWeight(&Gaussian_Pixel_3[i]);

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����

				if(Gaussian_Pixel_3[i].weight < WEIGHT)	//���� ������ ����þ��� ����ġ�� ������ �ִ� ���� ���� ����þ��� ����ġ���� ������
				{
					GenerateGaussian(&Gaussian_Pixel_3[i], Input[i], WEIGHT);	//����ġ�� ���� ���� ����þ��� ���ο� ����þ����� �����
				}

				Gaussian_Sort(&Gaussian_Pixel_1[i], &Gaussian_Pixel_2[i], &Gaussian_Pixel_3[i]);//����ġ�� ���� ���� ������ ����þ� ����
			}
		}

		Segment_Foreground(i, Input[i], &Foreground_Image[i], &Background_Image[i]);
	}
}

void GMM_Process::GenerateGaussian(PIXEL *Gaussian_Pixel, BYTE input, double Weight)
{
	Gaussian_Pixel->mean = input;
	Gaussian_Pixel->weight = Weight;
	Gaussian_Pixel->variance = VARIANCE;
	Gaussian_Pixel->EMPTY_FLAG = false;
}

void GMM_Process::Gaussian_Update(PIXEL *Gaussian_Pixel, BYTE input)
{
	Gaussian_Pixel->weight = (1 - ALPHA) * Gaussian_Pixel->weight + ALPHA;
	Gaussian_Pixel->mean = (1 - Gaussian_Pixel->probability) * Gaussian_Pixel->mean + Gaussian_Pixel->probability * input;
	Gaussian_Pixel->variance = sqrt((1 - Gaussian_Pixel->probability) * pow(Gaussian_Pixel->variance, 2) + 
												Gaussian_Pixel->probability * pow(input - Gaussian_Pixel->mean, 2));
	Gaussian_Pixel->probability = ALPHA / Gaussian_Pixel->weight;
}

void GMM_Process::Gaussian_DropWeight(PIXEL *Gaussian_Pixel)
{
	Gaussian_Pixel->weight = (1 - ALPHA) * Gaussian_Pixel->weight;
}

void GMM_Process::Gaussian_Sort(PIXEL *GP_1, PIXEL *GP_2, PIXEL *GP_3)
{
	PIXEL Temp;
	PIXEL GP[3];

	GP[0] = *GP_1;
	GP[1] = *GP_2;
	GP[2] = *GP_3;

	if(GP_3->EMPTY_FLAG)	//����þ��� ���� 2���϶�
	{
		if(GP_2->weight > GP_1->weight)	//2��° ����þ��� ����ġ�� Ŭ ���
		{
			Temp = *GP_1;
			*GP_1 = *GP_2;
			*GP_2 = Temp;
		}
	}
	else //����þ��� ���� 3���϶�
	{
		for(int i=0;i<3-1;i++)
		{
			for(int j=(i+1);j<3;j++)
			{
				if(GP[i].weight < GP[j].weight)
				{
					Temp = GP[i];
					GP[i] = GP[j];
					GP[j] = Temp;
				}
			}
		}

		*GP_1 = GP[0];
		*GP_2 = GP[1];
		*GP_3 = GP[2];
	}
}

void GMM_Process::Segment_Foreground(int index, BYTE input, BYTE *Foreground_Image, BYTE *Background_Image)
{
	if(Gaussian_Pixel_1[index].weight > THRESHOLD
		|| (Gaussian_Pixel_1[index].weight + Gaussian_Pixel_2[index].weight) > THRESHOLD
		|| (Gaussian_Pixel_1[index].weight + Gaussian_Pixel_2[index].weight + Gaussian_Pixel_3[index].weight > THRESHOLD))
	{
		if(fabs(input - Gaussian_Pixel_1[index].mean) < LAMDA * Gaussian_Pixel_1[index].variance)
		{
			*Background_Image = (BYTE) Gaussian_Pixel_1[index].mean;
			*Foreground_Image = 0;
		}
		else
		{
			*Background_Image = (BYTE) Gaussian_Pixel_1[index].mean;
			*Foreground_Image = 255;
		}
	}
	else
	{
		*Background_Image = (BYTE) Gaussian_Pixel_1[index].mean;
		*Foreground_Image = 255;
	}
}
