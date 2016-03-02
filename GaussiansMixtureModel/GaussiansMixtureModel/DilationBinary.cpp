#include "stdafx.h"
#include "DilationBinary.h"


DilationBinary::DilationBinary(void)
	: ImageGray(NULL)
	, OutputGray(NULL)
	, nW(0)
	, nH(0)
	, nShape(0)
	, nSize(0)
{
}


DilationBinary::~DilationBinary(void)
{
}

void DilationBinary::Init(BYTE *input, BYTE *output, int W, int H, int _nShape, int _nSize)
{
	nW = W;
	nH = H;
	nShape = _nShape;
	nSize = _nSize;

	ImageGray = new BYTE*[nH];
	OutputGray = new BYTE*[nH];
	for(int y=0;y<nH;y++)
	{
		ImageGray[y] = new BYTE[nW];
		OutputGray[y] = new BYTE[nW];
	}

	for(int h=0;h<nH;h++)
	{
		for(int w=0;w<nW;w++)
		{
			ImageGray[h][w] = input[nW*h + w];
		}
	}

	DilationBinary_Apply(output);
}

void DilationBinary::DilationBinary_Apply(BYTE *output)
{
	int x, y;
	int dx, dy;
	int xx, yy;
	BOOL bDone;

	if(nSize <= 0) return;

	double nRange = nSize/2.;

	for(y=0;y<nH;y++)
		for(x=0;x<nW;x++)
		{
			bDone = false;
			OutputGray[y][x] = 0;

			for(dy = -(int)nRange ; dy < nRange ; dy++)
			{
				if(nShape == 1 && dy != 0) continue;

				for(dx = -(int)nRange ; dx < nRange ; dx++)
				{
					if(nShape == 2 && dx != 0) continue;

					xx = x+dx;
					yy = y+dy;

					if(xx < 0) xx = 0;
					if(xx >= nW) xx = nW - 1;
					if(yy < 0) yy = 0;
					if(yy > nH) yy = nH - 1;

					if(ImageGray[yy][xx] > 256/2)
					{
						OutputGray[y][x] = 255;
						bDone = TRUE;
						break;
					}
				}

				if(bDone) break;
			}
		}

	
	for(int h=0;h<nH;h++)
	{
		for(int w=0;w<nW;w++)
		{
			output[nW*h + w] = OutputGray[h][w];
		}
	}
}
