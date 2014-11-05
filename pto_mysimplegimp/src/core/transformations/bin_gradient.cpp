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

	int up = 0;
	int down = 0;
	int grw = 0;
	int grh = 0;
	int gr = 0;
	int threshold = 0;

	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{

			QRgb pixel0 = image->pixel(w - 1, h);
			QRgb pixel1 = image->pixel(w + 1, h);
			grw = qGray(pixel1) - qGray(pixel0);

			pixel0 = image->pixel(w, h - 1);
			pixel1 = image->pixel(w, h + 1);
			grh = qGray(pixel1) - qGray(pixel0);

			if (grw >= grh){
				gr = grw;
			}
			else gr = grh;
			QRgb pixel = image->pixel(w, h);
			up += qGray(pixel) * gr;
			down += gr;
			threshold = up / down;

			if (qGray(pixel) >= threshold){
				newImage->setPixel(w, h, Qt::color1);
			}
			else 
				newImage->setPixel(w, h, Qt::color0);
		}
	}

    qDebug() << Q_FUNC_INFO << "Not implemented yet!";

    return newImage;
}


