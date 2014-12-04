#include "map_normal.h"

#include "edge_sobel.h"
#include "map_height.h"

MapNormal::MapNormal(PNM* img) :
Convolution(img)
{
}

MapNormal::MapNormal(PNM* img, ImageViewer* iv) :
Convolution(img, iv)
{
}

PNM* MapNormal::transform()
{
	int width = image->width(),
		height = image->height();

	double strength = getParameter("strength").toDouble();

	PNM* newImage = new PNM(width, height, image->format());

	PNM* tempImage = MapHeight(image).transform();

	EdgeSobel sobel(tempImage);
	math::matrix<float>* gx = sobel.rawHorizontalDetection();
	math::matrix<float>* gy = sobel.rawVerticalDetection();

	newImage = new PNM(width, height, QImage::Format_RGB32);
	float dzStart = (float)(1.0 / strength);

	float dx, dy, dz, len;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			dx = (*gx)(x, y) / 255.0;
			dy = (*gy)(x, y) / 255.0;
			dz = dzStart;

			len = sqrt(pow(dx, 2.0) + pow(dy, 2.0) + pow(dz, 2.0));

			dx /= len;
			dy /= len;
			dz /= len;

			dx = (dx + 1.0) * (255.0 / 2.0);
			dy = (dy + 1.0) * (255.0 / 2.0);
			dz = (dz + 1.0) * (255.0 / 2.0);

			newImage->setPixel(x, y, qRgb(dx, dy, dz));
		}
	}

	return newImage;
}