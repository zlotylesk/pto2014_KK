#include "bin_manual.h"

BinarizationManual::BinarizationManual(PNM* img) :
    Transformation(img)
{
}

BinarizationManual::BinarizationManual(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationManual::transform()
{
    int threshold = getParameter("threshold").toInt();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

	for (int w = 0; w < width; w++){
		for (int h = 0; h < height; h++){
			int pixel = image->pixel(w, h);
			newImage->setPixel(w, h, qGray(pixel) > threshold ? Qt::color1 : Qt::color0);
	}}

    qDebug() << Q_FUNC_INFO << "Not implemented yet!";

    return newImage;
}




