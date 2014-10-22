#include "histogram_equalization.h"

#include "../histogram.h"

HistogramEqualization::HistogramEqualization(PNM* img) :
    Transformation(img)
{
}

HistogramEqualization::HistogramEqualization(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HistogramEqualization::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, image->format());

	if (image->format() == QImage::Format_Indexed8)
	{
		Histogram* histogram = image->getHistogram();
		histogram->get(Histogram::LChannel);
		QHash<int, int> channel = *histogram->get(Histogram::LChannel);

		// Probabilities
		QHash<int, int>::const_iterator iterator;
		double distribution[256] = { 0 };
		double bottom = width * height;
		iterator = channel.constBegin();
		while (iterator != channel.constEnd())
		{
			distribution[iterator.key()] += iterator.value() / bottom;
			iterator++;
		}

		// Distribution
		for (int i = 1; i<256; i++) distribution[i] += distribution[i - 1];

		// Equalization
		for (int x = 0; x<width; x++)
		{
			for (int y = 0; y<height; y++)
			{
				QRgb pixel = image->pixel(x, y);
				int q = qGray(pixel);
				int v = distribution[q] * 255;
				newImage->setPixel(x, y, v);
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
			// Probabilities
			QHash<int, int>::const_iterator iterator;
			double distribution[256] = { 0 };
			double bottom = width * height;
			iterator = channel.constBegin();
			while (iterator != channel.constEnd())
			{
				distribution[iterator.key()] += iterator.value() / bottom;
				iterator++;
			}

			// Distribution
			for (int i = 1; i<256; i++) distribution[i] += distribution[i - 1];

			// Equalization
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
					int v;
					switch (counter)
					{
					case 0:
						pixel = image->pixel(x, y);
						r = qRed(pixel);     // Get the 0-255 value of the R channel
						g = qGreen(pixel);   // Get the 0-255 value of the G channel
						b = qBlue(pixel);    // Get the 0-255 value of the B channel
						v = distribution[r] * 255;
						newPixel = QColor(v, g, b);
						break;
					case 1:
						pixel = image->pixel(x, y);
						tempPixel = newImage->pixel(x, y);
						r = qRed(tempPixel); // Get the 0-255 value of the R channel
						g = qGreen(pixel);   // Get the 0-255 value of the G channel
						b = qBlue(pixel);    // Get the 0-255 value of the B channel
						v = distribution[g] * 255;
						newPixel = QColor(r, v, b);
						break;
					case 2:
						pixel = image->pixel(x, y);
						tempPixel = newImage->pixel(x, y);
						r = qRed(tempPixel);  // Get the 0-255 value of the R channel
						g = qGreen(tempPixel);// Get the 0-255 value of the G channel
						b = qBlue(pixel);     // Get the 0-255 value of the B channel
						v = distribution[b] * 255;
						newPixel = QColor(r, g, v);
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

