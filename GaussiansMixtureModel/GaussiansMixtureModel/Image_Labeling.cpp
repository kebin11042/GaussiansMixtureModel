#include "stdafx.h"
#include "Image_Labeling.h"


Image_Labeling::Image_Labeling(void)
	: Image_W(0)
	, Image_H(0)
	, input(NULL)
	, Labeling_Image(NULL)
	, ImageGray(NULL)
	, ImageLabel(NULL)
	, nConnNumThre(0)
{
}


Image_Labeling::~Image_Labeling(void)
{
}

int Image_Labeling::OnInitLabeling(BYTE *_input, int **_Labeling_Image, int W, int H, int _nConnNumThre)
{
	Image_W = W;
	Image_H = H;
	nConnNumThre = _nConnNumThre;
	input = _input;
	ImageLabel = _Labeling_Image;
	int cnt;

	ImageGray = new BYTE*[Image_H];
	for(int y=0;y<Image_H;y++)
	{
		ImageGray[y] = new BYTE[Image_W];
	}

	for(int h=0;h<Image_H;h++)
	{
		for(int w=0;w<Image_W;w++)
		{
			ImageGray[h][w] = input[Image_W*h + w];
		}
	}

	cnt = Labeling_Apply();

	return cnt;
}


int Image_Labeling::Labeling_Apply(void)	//two-pass �˰���
{
	int *r, *area;
	int num;
	int left, top;

	r = new int[Image_W * Image_H];
	area = new int[Image_W * Image_H];

	for(int h=0;h<Image_H;h++)
	{
		for(int w=0;w<Image_W;w++)
		{
			if(ImageGray[h][w] >= 255) ImageLabel[h][w] = 0;	//���� ���� 1
			else ImageLabel[h][w] = -1;	//��� ���� -1
		}
	}

	//������ ��輱 ��ü ���� ����
	for(int x=0;x<Image_W;x++)
	{
		ImageLabel[0][x] = -1;
		ImageLabel[Image_H - 1][x] = -1;
	}
	for(int y=0;y<Image_H;y++)
	{
		ImageLabel[y][0] = -1;
		ImageLabel[y][Image_W - 1] = -1;
	}

	num = -1;
	for(int y=0;y<Image_H;y++)
	{
		for(int x=0;x<Image_W;x++)
		{
			if(y>0 && x>0)
			{
				if(ImageLabel[y][x] >= 0)
				{
					left = ImageLabel[y][x-1];
					top = ImageLabel[y-1][x];

					if(left == -1 && top != -1)	//���� ����
					{
						ImageLabel[y][x] = r[top];
					}
					else if(left != -1 && top == -1)	// ���ʿ���
					{
						ImageLabel[y][x] = r[left];
					}
					else if(left == -1 && top == -1)	//���ʰ� ���� ����
					{
						num++;

						r[num] = num;
						ImageLabel[y][x] = r[num];
					}
					else if(left != -1 && top != -1)	//���� ���� �Ѵ� ����
					{
						if(r[left] == r[top])
						{
							ImageLabel[y][x] = r[left];
						}
						else if(r[left] > r[top])
						{
							ImageLabel[y][x] = r[top];
							r[left] = r[top];
						}
						else
						{
							ImageLabel[y][x] = r[left];
							r[top] = r[left];
						}
					}
				}
			}
		}
	}

	for(int i=0;i<=num;i++)	//�ٽ��ѹ� �˻�
	{
		if(i != r[i])	//�ȼ��� num�� �ٲ���� ã�Ƽ� ���� ������ ��ü�Ѵ�
		{
			r[i] = r[r[i]];
		}
		area[i] = 0;	//ũ�� �ʱ�ȭ
	}

	for(int y=0;y<Image_H;y++)	//�ٲ� r������ ImageLable ������(�پ��ִ°��� ����)
		for(int x=0;x<Image_W;x++)
		{
			if(ImageLabel[y][x] > -1)
			{
				ImageLabel[y][x] = r[ImageLabel[y][x]];
				area[ImageLabel[y][x]]++;
			}
		}

	int cnt = 0;
	for(int i=0;i<=num;i++)
	{
		if(area[i] > nConnNumThre)
		{
			r[i] = cnt++;
		}
		else
		{
			r[i] = -1;
		}
	}

	for(int y=0;y<Image_H;y++)
		for(int x=0;x<Image_W;x++)
		{
			if(ImageLabel[y][x] >= 0)
				ImageLabel[y][x] = r[ImageLabel[y][x]];
		}

	delete [] r;
	delete [] area;

	return cnt;
}


BYTE Image_Labeling::ReturnLabelImage(void)
{
	BYTE *a = new BYTE[Image_W * Image_H];

	for(int h=0;h<Image_H;h++)
	{
		for(int w=0;w<Image_W;w++)
		{
			a[Image_W*h + w] = ImageLabel[h][w];
		}
	}

	return *a;
}
