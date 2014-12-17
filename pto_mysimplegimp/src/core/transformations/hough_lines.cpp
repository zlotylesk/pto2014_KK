#include "hough_lines.h"

#include "bin_gradient.h"
#include "edge_laplacian.h"
#include "hough.h"

#include <QPainter>

HoughLines::HoughLines(PNM* img) :
    Transformation(img)
{
}

HoughLines::HoughLines(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HoughLines::transform()
{
	// Cut of value from the image;
	int  threshold = getParameter("threshold").toInt();
	bool drawWholeLines = getParameter("draw_whole_lines").toBool();

	PNM* newImage = new PNM(image->copy());
	PNM* tempImage, *binaryImage;

	EdgeLaplacian* edges = new EdgeLaplacian(image);
	edges->setParameter("size", 3);
	tempImage = edges->transform();

	BinarizationGradient* binary = new BinarizationGradient(tempImage);
	binaryImage = binary->transform();

	Hough* hough = new Hough(binaryImage);
	hough->setParameter("theta_density", 3);
	hough->setParameter("skip_edge_detection", true);
	tempImage = hough->transform();

	QPainter* qPainter = new QPainter(newImage);
	qPainter->setPen(Qt::red);

	int x1 = 0, x2, y1, y2;
	for (int theta = 0; theta < tempImage->width(); theta++) {
		for (int rho = 0; rho < tempImage->height(); rho++) {

			QRgb pixel = tempImage->pixel(theta, rho);
			int valuePixel = qGray(pixel);
			if (valuePixel > threshold)
			{
				int real_rho = rho - tempImage->height() / 2;
				double rtheta = (theta / 3.0)*M_PI / 180.0;
				double sinTheta = sin(rtheta);
				double cosTheta = cos(rtheta);

				y1 = round(real_rho / sinTheta);
				x2 = newImage->width() - 1;
				y2 = round((real_rho - (newImage->width() - 1)*cosTheta) / sinTheta);
				qPainter->drawLine(x1, y1, x2, y2);
			}
		}
	}

	if (drawWholeLines == false)
	{
		for (int i = 0; i<newImage->width(); i++)
			for (int j = 0; j<newImage->height(); j++)
			{
			if (qRed(binaryImage->pixel(i, j)) == 0)
			{
				newImage->setPixel(i, j, image->pixel(i, j));
			}
			}
	}

	return newImage;
}
