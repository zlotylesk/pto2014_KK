#include "blur_uniform.h"

BlurUniform::BlurUniform(PNM* img) :
    Convolution(img)
{
}

BlurUniform::BlurUniform(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

math::matrix<float> BlurUniform::getMask(int size, Mode m)
{
	//Nale¿y dokonaæ splotu obrazu z mask¹ wype³nion¹ jedynkami
	math::matrix<float> mask(size, size);
	for (int x = 0; x<size; x++)
	{
		for (int y = 0; y<size; y++)
		{
			mask(x, y) = 1;
		}
	}
	convolute(mask, m = RepeatEdge);
	return mask;
}
