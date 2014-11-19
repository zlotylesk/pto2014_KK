#include "morph_dilate.h"

MorphDilate::MorphDilate(PNM* img) :
    MorphologicalOperator(img)
{
}

MorphDilate::MorphDilate(PNM* img, ImageViewer* iv) :
    MorphologicalOperator(img, iv)
{
}

const int MorphDilate::morph(math::matrix<float> window, math::matrix<bool> se)
{
    float min = PIXEL_VAL_MAX+1;
	int areaSize = window.rowno();

	for (int i = 0; i < areaSize; i++)
	{
		for (int j = 0; j < areaSize; j++)
		{
			if (se(i,j) == true)
			{
				if (window(i, j) < min ) 
				{
					min = window(i, j);
				}
			}
		}
	}

    return min;
}
