#include "hough.h"

#include "conversion_grayscale.h"
#include "edge_laplacian.h"
#include "edge_prewitt.h"

Hough::Hough(PNM* img) :
    Transformation(img)
{
}

Hough::Hough(PNM* img, ImageViewer* super) :
    Transformation(img, super)
{
}

PNM* Hough::transform()
{   
    int thetaDensity = getParameter("theta_density").toInt();

	int width = image->width(),
		height = image->height();

	PNM* newImage = new PNM(image->copy());

	PNM* tempImage = new PNM(width, height, QImage::Format_Indexed8);
	int tempWidth = tempImage->width(),
		tempHeight = tempImage->height();

	for (int i = 0; i < tempWidth; i++)
		for (int j = 0; j < tempHeight; j++)
			tempImage->setPixel(i, j, qGray(image->pixel(i, j)));

	if (getParameter("skip_edge_detection").toBool() == false)
	{
		EdgeLaplacian* laplacian = new EdgeLaplacian(tempImage);
		laplacian->setParameter("size", 3);
		tempImage = laplacian->transform();
	}

	int rhoMax = sqrt(pow(width, 2) + pow(height, 2));
	int thetaSize = 180 * thetaDensity;

	PNM* outputImage = new PNM(thetaSize, rhoMax * 2 + 1, QImage::Format_Indexed8);

	math::matrix<int> hough(thetaSize, rhoMax * 2 + 1);
	for (int i = 0; i < thetaSize; i++)
		for (int j = 0; j < rhoMax * 2 + 1; j++)
			hough(i, j) = 0;

	for (int i = 0; i < tempWidth; i++)
		for (int j = 0; j < tempHeight; j++)
		{
		QRgb pixel = tempImage->pixel(i, j);

		int q = qGray(pixel);
		if (q > 0)
		{
			for (int k = 0; k < thetaSize; k++)
			{
				float theta = (k * M_PI) / (180 * thetaDensity);
				int rho = round(i * cos(theta) + j * sin(theta));
				int counter = hough(k, rho + rhoMax);
				hough(k, int(rho + rhoMax)) = counter + 1;
			}
		}
		}

	int maximum = 0;
	for (int i = 0; i < outputImage->width(); i++)
		for (int j = 0; j < outputImage->height(); j++)
		{
		int v = hough(i, j);

		if (hough(i, j) > maximum)
			maximum = v;
		}

	for (int i = 0; i < outputImage->width(); i++)
		for (int j = 0; j < outputImage->height(); j++)
			outputImage->setPixel(i, j, round(255 * hough(i, j) / maximum));

	return outputImage;
}
