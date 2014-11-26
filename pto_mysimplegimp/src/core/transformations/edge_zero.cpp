#include "edge_zero.h"

#include "edge_laplacian_of_gauss.h"

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img) :
    Convolution(img)
{
}

EdgeZeroCrossing::EdgeZeroCrossing(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* EdgeZeroCrossing::transform()
{
	int width = image->width();
	int height = image->height();
	int size = getParameter("size").toInt();
	double sigma = getParameter("sigma").toDouble();
	int threshold = getParameter("threshold").toInt();
	PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);
	EdgeLaplaceOfGauss log(image, getSupervisor());
	log.setParameter("size", getParameter("size"));
	log.setParameter("sigma", getParameter("sigma"));
	Transformation laplace(log.transform());
	if (image->format() == QImage::Format_Indexed8)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				math::matrix<float> window = laplace.getWindow(x, y, size, LChannel, RepeatEdge);
				int min = PIXEL_VAL_MAX;
				int max = 0;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if (window(i, j) > max)
						{
							max = window(i, j);
						}
						if (window(i, j) < min)
						{
							min = window(i, j);
						}
					}
				}
				if (min < (128 - threshold) && max > (128 + threshold))
				{
					newImage->setPixel(x, y, window(size / 2, size / 2));
				}
				else
				{
					newImage->setPixel(x, y, 0);
				}
			}
		}
	}
	else
	{
		for (int x = 0; x<width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				math::matrix<float> windowR = laplace.getWindow(x, y, size, RChannel, RepeatEdge);
				math::matrix<float> windowG = laplace.getWindow(x, y, size, GChannel, RepeatEdge);
				math::matrix<float> windowB = laplace.getWindow(x, y, size, BChannel, RepeatEdge);
				int minR = PIXEL_VAL_MAX;
				int minG = PIXEL_VAL_MAX;
				int minB = PIXEL_VAL_MAX;
				int maxR = 0;
				int maxG = 0;
				int maxB = 0;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						if (windowR(i, j) > maxR)
						{
							maxR = windowR(i, j);
						}
						if (windowR(i, j) < minR)
						{
							minR = windowR(i, j);
						}
						if (windowG(i, j) > maxG)
						{
							maxG = windowG(i, j);
						}
						if (windowG(i, j) < minG)
						{
							minG = windowG(i, j);
						}
						if (windowB(i, j) > maxB)
						{
							maxB = windowB(i, j);
						}
						if (windowB(i, j) < minB)
						{
							minB = windowB(i, j);
						}
					}
				}
				if (minR < (128 - threshold) && maxR > (128 + threshold))
				{
					newImage->setPixel(x, y, windowR(size / 2, size / 2));
				}
				else if (minG < (128 - threshold) && maxG > (128 + threshold))
				{
					newImage->setPixel(x, y, windowG(size / 2, size / 2));
				}
				else if (minB < (128 - threshold) && maxB > (128 + threshold))
				{
					newImage->setPixel(x, y, windowB(size / 2, size / 2));
				}
				else
				{
					newImage->setPixel(x, y, 0);
				}
			}
		}
	}
	return newImage;
}
