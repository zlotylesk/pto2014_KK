#include "map_horizon.h"

#include "map_height.h"

MapHorizon::MapHorizon(PNM* img) :
    Transformation(img)
{
}

MapHorizon::MapHorizon(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* MapHorizon::transform()
{
    int width  = image->width(),
        height = image->height();

    double scale     = getParameter("scale").toDouble();
    int    sun_alpha = getParameter("alpha").toInt();
    int dx, dy;

    switch (getParameter("direction").toInt())
    {
    case NORTH: dx = 0; dy = -1; break;
    case SOUTH: dx = 0; dy = 1; break;
    case EAST:  dx = 1; dy = 0; break;
    case WEST:  dx = -1; dy = 0; break;
    default:
        qWarning() << "Unknown direction!";
        dx =  0;
        dy = -1;
    }

    PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

	PNM* temporaryImage = MapHeight(image).transform();
	double alpha;
	double r;
	double ralpha;
	double delta;
	int pocz, skok;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			alpha = 0.0;
			int currentHeight = qRed(temporaryImage->pixel(x, y));
			pocz = x + dx;
			for (int pocz; pocz < width && skok < height && pocz >= 0 && skok >= 0; pocz += dx, skok += dy)
			{
				int rayh = qRed(temporaryImage->pixel(pocz, skok));
				if (currentHeight < rayh)
				{
					r = sqrt(pow(pocz - x, 2.0) + pow(skok - y, 2.0)) * scale;
					ralpha = atan((rayh - currentHeight) / r);
					if (ralpha > alpha){
						alpha = ralpha;
					}
				}
			}

			delta = alpha - sun_alpha * 3.14 / 180.;
			if (delta > 0){
				newImage->setPixel(x, y, cos(delta) * 255);
			}
			else {
				newImage->setPixel(x, y, 255);
			}
		}
	}

    return newImage;
}
