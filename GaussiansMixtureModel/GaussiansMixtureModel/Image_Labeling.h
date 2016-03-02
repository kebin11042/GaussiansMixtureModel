#pragma once
#include "GaussiansMixtureModel.h"

class Image_Labeling
{
public:
	Image_Labeling(void);
	~Image_Labeling(void);

	int OnInitLabeling(BYTE *_input, int **_Labeling_Image, int W, int H, int _nConnNumThre);
	int Labeling_Apply(void);

	int Image_W;
	int Image_H;
	BYTE *input;
	BYTE *Labeling_Image;
	BYTE **ImageGray;
	int **ImageLabel;
	int nConnNumThre;
	BYTE ReturnLabelImage(void);
};

