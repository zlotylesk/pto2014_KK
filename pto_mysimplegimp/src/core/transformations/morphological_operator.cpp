#include "morphological_operator.h"

MorphologicalOperator::MorphologicalOperator(PNM* img) :
    Transformation(img)
{
}

MorphologicalOperator::MorphologicalOperator(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

// abstract
const int MorphologicalOperator::morph(math::matrix<float>, math::matrix<bool>)
{
    return 0;
}

math::matrix<bool> MorphologicalOperator::getSE(int size, SE shape)
{
    switch (shape)
    {
    case Square:    return seSquare(size);
    case Cross:     return seCross(size);
    case XCross:    return seXCross(size);
    case VLine:     return seVLine(size);
    case HLine:     return seHLine(size);
    default:        return seSquare(size);
    }
}


math::matrix<bool> MorphologicalOperator::seSquare(int size)
{
    math::matrix<bool> ret(size, size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			ret(i, j) = true;
		}
	}

    return ret;
}

math::matrix<bool> MorphologicalOperator::seCross(int size)
{
    math::matrix<bool> ret(size, size);

	int halfSize = (size / 2) + 1;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (i == halfSize || j == halfSize)
			{
				ret(i, j) = true;
			}
			else
			{
				ret(i, j) = false;
			}
		}
	}

    return ret;
}

math::matrix<bool> MorphologicalOperator::seXCross(int size)
{
    math::matrix<bool> ret(size, size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (i == j || i == size - j)
			{
				ret(i, j) = true;
			}
			else
			{
				ret(i, j) = false;
			}
		}
	}

    return ret;
}

math::matrix<bool> MorphologicalOperator::seVLine(int size)
{
    math::matrix<bool> ret(size, size);
	int halfSize = (size / 2) + 1;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (i == halfSize)
			{
				ret(i, j) = true;
			}
			else
			{
				ret(i, j) = false;
			}
		}
	}

    return ret;
}

math::matrix<bool> MorphologicalOperator::seHLine(int size)
{
    math::matrix<bool> ret(size, size);
	int halfSize = (size / 2) + 1;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (j == halfSize)
			{
				ret(i, j) = true;
			}
			else
			{
				ret(i, j) = false;
			}
		}
	}

    return ret;
}

PNM* MorphologicalOperator::transform()
{  
    int size  = getParameter("size").toInt();
    SE  shape = (MorphologicalOperator::SE) getParameter("shape").toInt();

    PNM* newImage = new PNM(image->width(), image->height(), QImage::Format_RGB32);

	int w = image->width();
	int h = image->height();

	math::matrix<bool> newShape = getSE(size, shape);

	if (image->format() == QImage::Format_RGB32) 
	{
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{

				QRgb pixel = getPixel(x, y, RepeatEdge);
				math::matrix<float> windowR = getWindow(x, y, size, RChannel, RepeatEdge);
				math::matrix<float> windowG = getWindow(x, y, size, GChannel, RepeatEdge);
				math::matrix<float> windowB = getWindow(x, y, size, BChannel, RepeatEdge);

				int r = morph(windowR, newShape);
				int g = morph(windowG, newShape);
				int b = morph(windowB, newShape);

				QColor newPixel = QColor(r, g, b);
				newImage->setPixel(x, y, newPixel.rgb());
			}
		}
		if (image->format() == QImage::Format_Mono) 
			{
				for (int x = 0; x < w; x++) 
				{
					for (int y = 0; y < h; y++)
					{
						QRgb pixel = getPixel(x, y, RepeatEdge);
						math::matrix<float> window = getWindow(x, y, size, LChannel, RepeatEdge);
						double v = morph(window, newShape);
						newImage->setPixel(x, y, v);
					}
				}
			}
	}
    return newImage;
}
