#pragma once
class Median_Filter
{
public:
	Median_Filter(void);
	~Median_Filter(void);
	
	void OnInitMedian_Filter(int W, int H, int _nWinSize, BYTE *_input);
	void Median_Filter_Apply(void);
	void Median_Filter_Mask(BYTE *Mask, int w, int h);
	void Median_Filter_MaskSort(BYTE *Mask);

	int Image_W;
	int Image_H;
	
	int nWinSize;
	BYTE *input;
};

