#include "map_height.h"
#include "conversion_grayscale.h"


MapHeight::MapHeight(PNM* img) :
    Transformation(img)
{
}

MapHeight::MapHeight(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHeight::transform()
{
    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

	if (image->format == QImage::Format_Mono)
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				QColor color = QColor::fromRgb(image->pixel(w, h));
				newImage->setPixel(w, h, color == Qt::white ? PIXEL_VAL_MIN : PIXEL_VAL_MAX);
			}
		}
	} else {
		for (int w = 0; w < width; w++)
			for (int h = 0; h < height; h++)
			{
			QRgb pixel = image->pixel(width, height);

			int r = qRed(pixel);
			int g = qGreen(pixel);
			int b = qBlue(pixel);

			newImage->setPixel(w, h, (0.3*r + 0, 6 * g + 0, 1 * b));
			}
	}

    return newImage;
}
