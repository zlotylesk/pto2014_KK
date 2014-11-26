#include "edge_gradient.h"

EdgeGradient::EdgeGradient(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

EdgeGradient::EdgeGradient(PNM* img) :
    Convolution(img)
{
}

PNM* EdgeGradient::verticalDetection()
{
	return convolute(g_y, RepeatEdge);
}

PNM* EdgeGradient::horizontalDetection()
{
    return convolute(g_x, RepeatEdge);
}

int ch(int col)
{
	if (col > 255)
		return 255;
	if (col < 0)
		return 0;
	return col;
}

PNM* EdgeGradient::transform()
{
	PNM* imageX = horizontalDetection();
	PNM* imageY = verticalDetection();

	int width = image->width();
	int height = image->height();
	QImage::Format format = image->format();

	PNM* newImage = new PNM(width, height, format);

	if (image->format() == QImage::Format_Indexed8)
	{
		int q;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				int inew = (int)sqrt(pow(qGray(imageX->pixel(x, y)), 2.0) + pow(qGray(imageY->pixel(x, y)), 2.0));
				newImage->setPixel(x, y, inew);
			}
		}
	}
	else
	{
		int r, g, b;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				r = (int)sqrt(pow(qRed(imageX->pixel(x, y)), 2.0) + pow(qRed(imageY->pixel(x, y)), 2.0));
				g = (int)sqrt(pow(qGreen(imageX->pixel(x, y)), 2.0) + pow(qGreen(imageY->pixel(x, y)), 2.0));
				b = (int)sqrt(pow(qBlue(imageX->pixel(x, y)), 2.0) + pow(qBlue(imageY->pixel(x, y)), 2.0));
				newImage->setPixel(x, y, QColor(ch(r), ch(g), ch(b)).rgb());
			}
		}
	}

	return newImage;
}

