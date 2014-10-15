#include "conversion_grayscale.h"
#include <iostream>
using namespace std;

ConversionGrayscale::ConversionGrayscale(PNM* img) :
    Transformation(img)
{
}

ConversionGrayscale::ConversionGrayscale(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* ConversionGrayscale::transform()
{
    int width = image->width();
    int height = image->height();

	cout << "test" << endl;

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

    if (image->format() == QImage::Format_Mono)
    {
		for (int x = 0; x<width; x++)
			for (int y = 0; y < height; y++)
			{
				QColor color = QColor::fromRgb(image->pixel(x, y));
				if (color == Qt::black)
				{
					newImage->setPixel(x, y, PIXEL_VAL_MAX);
				}
				else
				{
					newImage->setPixel(x, y, PIXEL_VAL_MIN);
				}
			}
    }
    else // if (image->format() == QImage::Format_RGB32)
    {
		for (int x = 0; x<width; x++)
			for (int y = 0; y < height; y++)
			{
				QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value
				int r = qRed(pixel) * 0.3;
				int g = qGreen(pixel) * 0.6;
				int b = qBlue(pixel) * 0.1;
				int sum = r + g + b;
				newImage->setPixel(x, y, sum);
			}
    }

    return newImage;
}
