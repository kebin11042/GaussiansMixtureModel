#pragma once

#define GRAY_CNT 256

class Histogram_Normalization
{
public:
	Histogram_Normalization(void);
	~Histogram_Normalization(void);
	void OnInitHistogram(int W, int H, BYTE *_input);
	
	int Image_W;
	int Image_H;
	BYTE *input;
	void Histogram_Normalization_Apply(void);
};

