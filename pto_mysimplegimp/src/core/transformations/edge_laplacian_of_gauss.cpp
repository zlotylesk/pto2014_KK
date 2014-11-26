#include "edge_laplacian_of_gauss.h"

#include "blur_gaussian.h"

EdgeLaplaceOfGauss::EdgeLaplaceOfGauss(PNM* img) :
    Convolution(img)
{
}

EdgeLaplaceOfGauss::EdgeLaplaceOfGauss(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

math::matrix<float> EdgeLaplaceOfGauss::getMask(int, Mode)
{
	size = getParameter("size").toInt();
	float sigma = getParameter("sigma").toFloat();

	math::matrix<float> mask(size, size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int x = i - (size / 2);
			int y = j - (size / 2);
			mask(i, j) = getLoG(x, y, sigma);
		}
	}

	return mask;
}

float EdgeLaplaceOfGauss::getLoG(int x, int y, float s)
{
	float raz = (pow(x, 2) + pow(y, 2) - 2) / pow(s, 2);
	float dwa = BlurGaussian::getGauss(x, y, s);
	return raz * dwa;
}

int EdgeLaplaceOfGauss::getSize()
{
    return size;
}
