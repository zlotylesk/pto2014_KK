#include "blur_linear.h"

BlurLinear::BlurLinear(PNM* img) :
    Convolution(img)
{
}

BlurLinear::BlurLinear(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* BlurLinear::transform()
{
	int maskSize = getParameter("size").toInt();
	QList<QVariant> tmpMask = getParameter("mask").toList();
	bool normalize = getParameter("normalize").toBool();
	math::matrix<float> mask(maskSize, maskSize);
	int i = 0;
	for (int x = 0; x<maskSize; x++)
	{
		for (int y = 0; y<maskSize; y++)
		{
			qDebug() << Q_FUNC_INFO << tmpMask.at(i).toDouble();
			mask(x, y) = tmpMask.at(i).toDouble();
			i++;
			if (normalize == 1)
			{
				mask(x, y) = mask(x, y) / sum(mask);
			}
		}
	}

	return convolute(mask, RepeatEdge);
}
