#include "histogram.h"

#include <QDebug>
#include <QPainter>

#include <cmath>

Histogram::Histogram(QImage* image)
{
    R = new QHash<int, int>;
    G = new QHash<int, int>;
    B = new QHash<int, int>;
    L = new QHash<int, int>;
    generate(image);
}

Histogram::~Histogram()
{
    delete R;
    delete G;
    delete B;
    delete L;
}

void Histogram::generate(QImage* image)
{
	int width = image->width();
	int height = image->height();

	if (image->format() == QImage::Format_Indexed8)
	{
		for (int w = 0; w  < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				QRgb pixel = image->pixel(w, h);
				QHash<int, int>::iterator i;

				// L channel
				int r = qRed(pixel);    // Get the 0-255 value of the R channel
				int g = qGreen(pixel);  // Get the 0-255 value of the G channel
				int b = qBlue(pixel);   // Get the 0-255 value of the B channel
				int grayscale = qGray(r, g, b);
				i = L->find(grayscale);
				if (i != L->end())
				{
					i.value() += 1;
				}
				else L->insert(grayscale, 1);
			}
		}
	}
	else
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				QRgb pixel = image->pixel(w, h);
				QColor pColor = QColor(pixel);
				QHash<int, int>::iterator i;

				i= R->find(pColor.red());
				
				if (i!= R->end()){
					i.value() += 1;
				}
				else R->insert(pColor.red(), 1);

				i = G->find(pColor.green());
				if (i != G->end()){
					i.value() += 1; }
				else G->insert(pColor.green(), 1);

				i = B->find(pColor.blue());
				if (i != B->end()){ 
					i.value() += 1; }
				else B->insert(pColor.blue(), 1);

				int r = qRed(pixel);
				int g = qGreen(pixel);
				int b = qBlue(pixel);

				int grayscale = qGray(r, g, b);

				i = L->find(grayscale);
				if (i != L->end()) i.value() += 1;
				else L->insert(grayscale, 1);
			}
		}
	}
}

/** Returns the maximal value of the histogram in the given channel */
int Histogram::maximumValue(Channel selectedChannel = RGB)
{
	QHash<int, int>* channel = Histogram::get(selectedChannel);
	QHash<int, int>::const_iterator i;
	int maximumValue = 0;
	if (selectedChannel == RGB)
	{
		maximumValue = Histogram::maximumValue(RChannel);
		int temp = Histogram::maximumValue(GChannel);
		if (temp > maximumValue) maximumValue = temp; 
		temp = Histogram::maximumValue(BChannel); 
		if (temp > maximumValue) maximumValue = temp;
	}
	else
	{
		for (i = channel->begin(); i != channel->end(); i++)
		{
			if (i.value() > maximumValue) maximumValue = i.value();
		}
	}

	return maximumValue;
}


/** Returns a pointer to the given channel QHash<int, int> */
QHash<int, int>* Histogram::get(Channel channel = LChannel)
{
    if (channel==LChannel) return L;
    if (channel==RChannel) return R;
    if (channel==GChannel) return G;
    if (channel==BChannel) return B;

    return 0;
}

/**
 *  Returns a 255 by 100 QImage containing a Histogram for the given channel.
 *  The background is transparent (Alpha 0, RGB=255) */
QImage Histogram::getImage(Channel channel = LChannel, QBrush pen = Qt::gray)
{
    // Create blank QImage and fill it with transparent background:
    QImage histImage(255, 100, QImage::Format_ARGB32);
    histImage.fill(0);
    QPainter painter(&histImage);
    painter.setBrush(Qt::transparent); 
    painter.setPen(Qt::transparent);
    painter.drawRect(0,0,255,100);

    // Calculate the aspect ratio using the maximal value of the color histograms
    int maximum = (channel == LChannel ? maximumValue(LChannel) :  maximumValue(RGB));
    float ratio = 100.0/float(maximum);

    // Preparing the painter:
    painter.setBrush(pen);
    painter.setPen(pen.color());

    int h;
    // Draw histogram
    QHash<int, int>* hist = get(channel);
    QHash<int, int>::const_iterator cit = hist->begin();

    while (cit != hist->end())
    {
        h = 100 - floor(ratio*cit.value());
        painter.drawLine(cit.key(), h, cit.key(), 100);
        ++cit;
    }

    return histImage;
}
