#include "histogram_stretching.h"

#include "../histogram.h"

HistogramStretching::HistogramStretching(PNM* img) :
    Transformation(img)
{
}

HistogramStretching::HistogramStretching(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramStretching::transform()
{
    int width  = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());
	if (image->format() == QImage::Format_Indexed8)
	{
		Histogram* histogram = image->getHistogram();
		histogram->get(Histogram::LChannel);
		QHash<int, int> channel = *histogram->get(Histogram::LChannel);

		// Find min & max
		QHash<int, int>::const_iterator iterator;
		iterator = channel.constBegin();
		int minimum = 255;
		int maximum = 0;
		while (iterator != channel.constEnd())
		{
			int r = iterator.key();
			if (r > 0)
			{
				if (r < minimum) minimum = r;
				if (r > maximum) maximum = r;
			}
			iterator++;
		}

		// Calculate stretching rate
		double top = 255.0;
		double bottom = maximum - minimum;
		double stretchingRate = top / bottom;

		// Stretching
		for (int x = 0; x<width; x++)
		{
			for (int y = 0; y<height; y++)
			{
				QRgb pixel = image->pixel(x, y);
				int q = qGray(pixel);
				double v = stretchingRate * (q - minimum);
				newImage->setPixel(x, y, (int)v);
			}
		}
	}
	else
	{
		Histogram* histogram = image->getHistogram();
		histogram->get(Histogram::RChannel);
		QList<QHash<int, int>> channels;
		QHash<int, int> Rchannel = *histogram->get(Histogram::RChannel);
		QHash<int, int> Gchannel = *histogram->get(Histogram::GChannel);
		QHash<int, int> Bchannel = *histogram->get(Histogram::BChannel);
		channels.append(Rchannel);
		channels.append(Gchannel);
		channels.append(Bchannel);
		int counter = 0;
		for (QHash<int, int> channel : channels)
		{
			// Find min & max
			QHash<int, int>::const_iterator iterator;
			iterator = channel.constBegin();
			int minimum = 255;
			int maximum = 0;
			while (iterator != channel.constEnd())
			{
				int r = iterator.key();
				if (r > 0)
				{
					if (r < minimum) minimum = r;
					if (r > maximum) maximum = r;
				}
				iterator++;
			}

			// Calculate stretching rate
			double top = 255.0;
			double bottom = maximum - minimum;
			double stretchingRate = top / bottom;

			// Stretching
			for (int x = 0; x<width; x++)
			{
				for (int y = 0; y<height; y++)
				{
					QRgb pixel;
					QColor newPixel;
					QRgb tempPixel;
					int r;
					int g;
					int b;
					double v;
					switch (counter)
					{
					case 0:
						pixel = image->pixel(x, y);
						r = qRed(pixel);     // Get the 0-255 value of the R channel
						g = qGreen(pixel);   // Get the 0-255 value of the G channel
						b = qBlue(pixel);    // Get the 0-255 value of the B channel
						v = stretchingRate * (r - minimum);
						newPixel = QColor((int)v, g, b);
						break;
					case 1:
						pixel = image->pixel(x, y);
						tempPixel = newImage->pixel(x, y);
						r = qRed(tempPixel); // Get the 0-255 value of the R channel
						g = qGreen(pixel);   // Get the 0-255 value of the G channel
						b = qBlue(pixel);    // Get the 0-255 value of the B channel
						v = stretchingRate * (g - minimum);
						newPixel = QColor(r, (int)v, b);
						break;
					case 2:
						pixel = image->pixel(x, y);
						tempPixel = newImage->pixel(x, y);
						r = qRed(tempPixel);  // Get the 0-255 value of the R channel
						g = qGreen(tempPixel);// Get the 0-255 value of the G channel
						b = qBlue(pixel);     // Get the 0-255 value of the B channel
						v = stretchingRate * (b - minimum);
						newPixel = QColor(r, g, (int)v);
						break;
					default:
						break;
					}
					newImage->setPixel(x, y, newPixel.rgb());
				}
			}
			counter++;
		}
	}
    
    return newImage;
}


