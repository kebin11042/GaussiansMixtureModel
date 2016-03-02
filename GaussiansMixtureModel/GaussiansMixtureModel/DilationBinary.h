#pragma once
class DilationBinary
{
public:
	DilationBinary(void);
	~DilationBinary(void);
	void DilationBinary_Apply(BYTE *output);
	void Init(BYTE *input, BYTE *output, int W, int H, int _nShape, int _nSize);
	
	BYTE **ImageGray;
	BYTE **OutputGray;
	int nW;
	int nH;
	int nShape;
	int nSize;
};

