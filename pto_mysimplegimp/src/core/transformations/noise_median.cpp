#include "noise_median.h"

NoiseMedian::NoiseMedian(PNM* img) :
    Convolution(img)
{
}

NoiseMedian::NoiseMedian(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* NoiseMedian::transform()
{
	int w = image->width();
	int h = image->height();

	PNM* retImg = new PNM(w, h, image->format());

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			if (image->format() == QImage::Format_RGB32)
			{
				int R = getMedian(x, y, RChannel);
				int G = getMedian(x, y, GChannel);
				int B = getMedian(x, y, BChannel);
				retImg->setPixel(x, y, QColor(R, G, B).rgb());
			}
			else
			{
				int tempv = getMedian(x, y, LChannel);
				retImg->setPixel(x, y, tempv);
			}
		}
	}

	return retImg;
}

int NoiseMedian::getMedian(int x, int y, Channel channel)
{
	int radius = getParameter("radius").toInt();
	int pom = radius * 2 + 1;
	double tab[100];
	math::matrix<float> wind = getWindow(x, y, pom, channel, RepeatEdge);
	int cols = wind.colno();
	int rows = wind.rowno();
	int z = 0;

	for (int i = 0; i<cols; i++)
	{
		for (int j = 0; j<rows; j++)
		{
			tab[z++] = wind(i, j);
		}
	}

	std::sort(&tab[0], &tab[z]);

	int idx = z / 2;
	if (tab[idx]>255)
		tab[idx] = 255;
	if (tab[idx] < 0)
		tab[idx] = 0;

	return tab[idx];
}
