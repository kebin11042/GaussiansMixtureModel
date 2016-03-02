#include "stdafx.h"
#include "Histogram_Normalization.h"


Histogram_Normalization::Histogram_Normalization(void)
	: Image_W(0)
	, Image_H(0)
	, input(NULL)
{
}


Histogram_Normalization::~Histogram_Normalization(void)
{
}


void Histogram_Normalization::OnInitHistogram(int W, int H, BYTE *_input)
{
	Image_W = W;
	Image_H = H;
	input = _input;

	Histogram_Normalization_Apply();
}


void Histogram_Normalization::Histogram_Normalization_Apply(void)
{
	BYTE *output = new BYTE[Image_W * Image_H];

	BYTE Table[GRAY_CNT];
	int CumHisto[GRAY_CNT], Histogram[GRAY_CNT] = {0};
	BYTE gray;

	//빈도수 계산
	for(int i=0;i<Image_W * Image_H;i++)
	{
		Histogram[input[i]]++;
	}

	//누적 빈도수 계산
	int nSum = 0;
	for(int i=0;i<GRAY_CNT;i++)
	{
		nSum += Histogram[i];
		CumHisto[i] = nSum;
	}

	//변환 테이블 생성
	for(int i=0;i<GRAY_CNT;i++)
	{
		Table[i] = (BYTE) ((CumHisto[i] * (GRAY_CNT-1)) / (Image_W * Image_H));
	}

	//균일화 수행
	for(int h=0;h<Image_H;h++)
	{
		for(int w=0;w<Image_W;w++)
		{
			gray = Table[input[Image_W*h + w]];
			if(gray < 0)
			{
				gray = 0;
			}
			else if(gray >= GRAY_CNT)
			{
				gray = GRAY_CNT - 1;
			}

			output[Image_W*h + w] = gray;
		}
	}

	input = output;
}
