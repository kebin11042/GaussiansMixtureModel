#include "stdafx.h"
#include "Median_Filter.h"


Median_Filter::Median_Filter(void)
	: Image_W(0)
	, Image_H(0)
	, nWinSize(0)
	, input(NULL)
{
}


Median_Filter::~Median_Filter(void)
{
}


void Median_Filter::OnInitMedian_Filter(int W, int H, int _nWinSize, BYTE *_input)
{
	Image_W = W;	//영상 폭
	Image_H = H;	//영상 높이
	nWinSize = _nWinSize;	//마스크 사이즈
	input = _input;	//입력 영상

	Median_Filter_Apply();
}


void Median_Filter::Median_Filter_Apply(void)
{
	BYTE *Mask;

	Mask = new BYTE[nWinSize * nWinSize];

	for(int h=1;h<Image_H - 1;h++)
	{
		for(int w=1;w<Image_W - 1;w++)
		{
			Median_Filter_Mask(Mask, w, h);

			Median_Filter_MaskSort(Mask);

			input[Image_W*h + w] = Mask[nWinSize*nWinSize / 2];
		}
	}

	delete [] Mask;
}


void Median_Filter::Median_Filter_Mask(BYTE *Mask, int w, int h)
{
	for(int i=0;i<nWinSize;i++)
	{
		for(int j=0;j<nWinSize;j++)
		{
			Mask[(i*nWinSize) + j] = input[(Image_W*(h+i-1)) + (w+j-1)];
		}
	}
}


void Median_Filter::Median_Filter_MaskSort(BYTE *Mask)
{
	BYTE Temp;

	for(int i=0;i<nWinSize*nWinSize-1;i++)
	{
		for(int j=i+1;j<nWinSize*nWinSize;j++)
		{
			if(Mask[i] < Mask[j])
			{
				Temp = Mask[i];
				Mask[i] = Mask[j];
				Mask[j] = Temp;
			}
		}
	}
}
