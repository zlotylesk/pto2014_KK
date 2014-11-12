#include "noise_bilateral.h"

NoiseBilateral::NoiseBilateral(PNM* img) :
    Convolution(img)
{
}

NoiseBilateral::NoiseBilateral(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseBilateral::transform()
{
	int width = image->width();
	int height = image->height();

	PNM* returnImage = new PNM(width, height, image->format());
	sigma_d = getParameter("sigma_d").toInt();
	sigma_r = getParameter("sigma_r").toInt();
	radius = sigma_d;

	for (int x = 0; x<width; x++)
	{
		for (int y = 0; y<height; y++)
		{
			if (image->format() == QImage::Format_RGB32)
			{
				int R = calcVal(x, y, RChannel);
				int G = calcVal(x, y, GChannel);
				int B = calcVal(x, y, BChannel);
				returnImage->setPixel(x, y, QColor(R, G, B).rgb());
			}
			else
			{
				int temp = calcVal(x, y, LChannel);
				returnImage->setPixel(x, y, temp);
			}
		}
	}

	return returnImage;
}

int NoiseBilateral::calcVal(int x, int y, Channel channel)
{
	int size = radius * 2 + 1;
	int pxl = 0;

	math::matrix<float> win = getWindow(x, y, size, channel, Transformation::RepeatEdge);

	if (channel == Channel::RChannel)
	{
		pxl = qRed(image->pixel(x, y));
	}
	if (channel == Channel::GChannel)
	{
		pxl = qGreen(image->pixel(x, y));
	}
	if (channel == Channel::BChannel)
	{
		pxl = qBlue(image->pixel(x, y));
	}
	if (channel == Channel::LChannel)
	{
		pxl = qGray(image->pixel(x, y));
	}

	float sumw = 0.0f;
	float summ = 0.0f;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			float cc = colorCloseness((int)win(i, j), pxl);
			float sc = spatialCloseness(QPoint(x - radius, y - radius), QPoint(x, y));
			float mucl = cc * sc;
			sumw += (win(i, j) * mucl);
			summ += mucl;
		}
	}

	int val = 0;

	if (summ != 0)
	{
		val = (int)(sumw / summ);
	}

	if (val > 255)
		return 255;
	if (val < 0)
		return 0;

	return val;
}

float NoiseBilateral::colorCloseness(int val1, int val2)
{
	float E = 2.71;

	float m1 = pow((float)(val1 - val2), 2.0);
	float m2 = 2.0 * pow((float)sigma_r, 2.0);

	float wyn = E - m1/m2;

	return wyn;
}

float NoiseBilateral::spatialCloseness(QPoint point1, QPoint point2)
{
	float E = 2.71;

	float m1 = pow(point1.x() - point2.x(), 2.0) + pow((point1.y() - point2.y()), 2.0);
	float m2 = 2.0 * pow(sigma_d, 2.0);

	float wyn = E - m1/m2;

	return wyn;
}
