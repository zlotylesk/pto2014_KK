#include "bin_gradient.h"

BinarizationGradient::BinarizationGradient(PNM* img) :
    Transformation(img)
{
}

BinarizationGradient::BinarizationGradient(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationGradient::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

	Mode mode = RepeatEdge;
	int licz = 0, mian = 0;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int value_pixel, G, Gx, Gy;
			QRgb pixel = getPixel(x, y, mode);
			value_pixel = qGray(pixel);
			Gx = qGray(this->getPixel(x + 1, y, mode)) - qGray(this->getPixel(x - 1, y, mode));
			Gy = qGray(this->getPixel(x, y + 1, mode)) - qGray(this->getPixel(x, y - 1, mode));
			if (Gx > Gy) {
				G = Gx;
			}
			else{
				G = Gy;
			}
			licz += value_pixel * G;
			mian += G;
		}
	}
	int threshold = licz / mian;
	
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			QRgb pixel = image->pixel(x, y);
			int value_pixel = qGray(pixel);
			if (value_pixel < threshold){
				newImage->setPixel(x, y, 0);
			}
			else{
				newImage->setPixel(x, y, 1);
			}
		}
	}

    return newImage;
}


