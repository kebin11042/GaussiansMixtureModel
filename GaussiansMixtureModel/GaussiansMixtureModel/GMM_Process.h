#pragma once

#include "GaussiansMixtureModelDlg.h"

#define	LAMDA		2.5
#define ALPHA		0.005
#define THRESHOLD	0.7
#define WEIGHT		0.05
#define VARIANCE	10

typedef struct PIXEL
{
	double	weight;		//가중치
	double	mean;		//평균
	double	variance;	//분산
	double	probability;	//p
	bool	EMPTY_FLAG;
}PIXEL;

class GMM_Process
{
public:
	GMM_Process(void);
	~GMM_Process(void);
	
	void Gaussian_Initialize(BYTE *Input, BYTE *Foreground_Image, BYTE *Background_Image);
	void OnInitGMM(int Width, int Height);
	void GenerateGaussian(PIXEL *Gaussian_Pixel, BYTE input, double Weight);
	void Gaussian_Update(PIXEL *Gaussian_Pixel, BYTE input);
	void Gaussian_DropWeight(PIXEL *Gaussian_Pixel);
	void Gaussian_Sort(PIXEL *GP_1, PIXEL *GP_2, PIXEL *GP_3);
	void Segment_Foreground(int index, BYTE input, BYTE *Foreground_Image, BYTE *Background_Image);

	int Image_Width;
	int Image_Height;

	PIXEL *Gaussian_Pixel_1;
	PIXEL *Gaussian_Pixel_2;
	PIXEL *Gaussian_Pixel_3;
	bool INIT_FLAG;
};
