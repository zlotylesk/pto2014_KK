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
		Histogram* hist = image->getHistogram();
		hist->get(Histogram::LChannel);
		QHash<int, int> channel = *hist->get(Histogram::LChannel);

		// Probabilities
		QHash<int, int>::const_iterator i;
		double distribution[256] = { 0 };
		double down = width * height;
		i = channel.constBegin();
		while (i != channel.constEnd())
		{
			distribution[i.key()] += i.value() / down;
			i++;
		}

		// Distribution
		for (int i = 1; i < 256; i++) distribution[i] += distribution[i - 1];

		// Equalization
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				QRgb pixel = image->pixel(w, h);
				int q = qGray(pixel);
				int v = distribution[q] * 255;
				newImage->setPixel(w, h, v);
			}
		}
	}
	else
	{
		Histogram* hist = image->getHistogram();
		hist->get(Histogram::RChannel);
		QList<QHash<int, int>> channels;
		
		QHash<int, int> Rchannel = *hist->get(Histogram::RChannel);
		QHash<int, int> Gchannel = *hist->get(Histogram::GChannel);
		QHash<int, int> Bchannel = *hist->get(Histogram::BChannel);

		channels.append(Rchannel);
		channels.append(Gchannel);
		channels.append(Bchannel);

		int counter = 0;
		for (QHash<int, int> channel : channels)
		{
			QHash<int, int>::const_iterator i;
			double distribution[256] = { 0 };
			double down = width * height;
			i = channel.constBegin();
			while (i != channel.constEnd())
			{
				distribution[i.key()] += i.value() / down;
				i++;
			}

			for (int i = 1; i < 256; i++){ 
				distribution[i] += distribution[i - 1]; }
			for (int w = 0; w < width; w++)
			{
				for (int h = 0; h < height; h++)
				{
					QRgb pixel;
					QColor newPixel;
					QRgb tempPixel;
					int r, g, b, v;
					//int g;
					//int b;
					//int v;
					if (counter == 0){
						pixel = image->pixel(w, h);
						r = qRed(pixel);     // Get the 0-255 value of the R channel
						g = qGreen(pixel);   // Get the 0-255 value of the G channel
						b = qBlue(pixel);    // Get the 0-255 value of the B channel
						v = distribution[r] * 255;
						newPixel = QColor(v, g, b);
					}
					if (counter == 1){
							pixel = image->pixel(w, h);
							tempPixel = newImage->pixel(w, h);
							r = qRed(tempPixel); // Get the 0-255 value of the R channel
							g = qGreen(pixel);   // Get the 0-255 value of the G channel
							b = qBlue(pixel);    // Get the 0-255 value of the B channel
							v = distribution[g] * 255;
							newPixel = QColor(r, v, b);
					}
					if (counter == 2){
							pixel = image->pixel(w, h);
							tempPixel = newImage->pixel(w, h);
							r = qRed(tempPixel);  // Get the 0-255 value of the R channel
							g = qGreen(tempPixel);// Get the 0-255 value of the G channel
							b = qBlue(pixel);     // Get the 0-255 value of the B channel
							v = distribution[b] * 255;
							newPixel = QColor(r, g, v);
					}
					newImage->setPixel(w, h, newPixel.rgb());
				}
			}
			counter++;
		}
	}
	return newImage;
}

