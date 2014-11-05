#include "blur_gaussian.h"

BlurGaussian::BlurGaussian(PNM* img) :
    Convolution(img)
{
}

BlurGaussian::BlurGaussian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurGaussian::transform()
{
    emit message("Blurring...");

    int size = getParameter("size").toInt();
    radius = (size/2)+1;
    sigma = getParameter("sigma").toDouble();

    return convolute(getMask(size, Normalize), RepeatEdge);
}

math::matrix<float> BlurGaussian::getMask(int size, Mode)
{
	math::matrix<float> mask(size, size);
	Mode mode = RepeatEdge;
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			mask(i, j) = getGauss(i, j, sigma);
		}
	}
	convolute(mask, mode);
	return mask;
}

float BlurGaussian::getGauss(int x, int y, float sig)
{    
	float pi = 3.14f;
	float e = 2.718f;

	float g = (1 / (2 * pi*pow(sig, 2))) * (pow(e, -(pow(x, 2) + pow(y, 2)) / (2 * pow(sig, 2))));

	//qDebug() << Q_FUNC_INFO << g;
	//qDebug() << Q_FUNC_INFO << "----------------------";

	return g;
}

