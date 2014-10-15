#include "correction.h"
#include <math.h>

Correction::Correction(PNM* img) :
    Transformation(img)
{
}

Correction::Correction(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

int ck(int val)
{
	if (val > 255)
		return 255;
	if (val < 0)
		return 0;
	return val;
}

PNM* Correction::transform()
{
    float shift  = getParameter("shift").toFloat();
    float factor = getParameter("factor").toFloat();
    float gamma  = getParameter("gamma").toFloat();

    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	int shiftt[256] = {};
	int factort[256] = {};
	int gammat[256] = {};
	for (int i = 0; i < 256; i++)
	{
		shiftt[i] = i + shift;
		factort[i] = i * factor;
		gammat[i] = pow(i, gamma);
	}
	
	for (int x = 0; x<width; x++)
		for (int y = 0; y < height; y++)
		{
			QRgb pixel = image->pixel(x, y); // Getting the pixel(x,y) value

			int r = qRed(pixel);
			r = ck(shiftt[r]);
			r = ck(factort[r]);
			r = ck(gammat[r]);

			int g = qGreen(pixel);
			g = ck(shiftt[g]);
			g = ck(factort[g]);
			g = ck(gammat[g]);

			int b = qBlue(pixel);
			b = ck(shiftt[b]);
			b = ck(factort[b]);
			b = ck(gammat[b]);
			
			QColor newPixel = QColor(r, g, b);
			newImage->setPixel(x, y, newPixel.rgb());
		}

    qDebug() << shift << factor << gamma;

    return newImage;
}
