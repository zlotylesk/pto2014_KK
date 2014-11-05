#include "convolution.h"

#include <iostream>
using namespace std;

/** Overloaded constructor */
Convolution::Convolution(PNM* img) :
    Transformation(img)
{
}

Convolution::Convolution(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

/** Returns a convoluted form of the image */
PNM* Convolution::transform()
{
    return convolute(getMask(3, Normalize), RepeatEdge);
}

/** Returns a sizeXsize matrix with the center point equal 1.0 */
math::matrix<float> Convolution::getMask(int size, Mode mode = Normalize)
{
	math::matrix<float> mask(size, size);
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			mask(x, y) = 0;
			if (x == y == (size - 1) / 2)
				mask(x, y) = 1;
		}
	}
	return mask;
}

/** Does the convolution process for all pixels using the given mask. */
PNM* Convolution::convolute(math::matrix<float> mask, Mode mode = RepeatEdge)
{
	int width = image->width(),
		height = image->height();

	PNM* newImage = new PNM(width, height, image->format());

	//obliczyæ wagê maski
	float wmask = sum(mask);

	vector<Channel> channels;
	if (image->format() == QImage::Format_Indexed8)
	{
		channels.push_back(LChannel);
	}
	else
	{
		channels.push_back(RChannel);
		channels.push_back(GChannel);
		channels.push_back(BChannel);
	}
	double result[3];

	for (int x = 0; x<width; x++)
	{
		for (int y = 0; y<height; y++)
		{
			for (int z = 0; z<channels.size(); z++)
			{
				//pobraæ okno, czyli wartoœci s¹siadów piksela
				math::matrix<float> okno = getWindow(x, y, mask.rowno(), channels[z], mode);
				//obliczyæ akumulator, czyli pomno¿yæ po wspó³rzêdnych okno z odbiciem maski
				math::matrix<float> akum = join(okno, reflection(mask));
				//obliczyæ sumê akumulatora
				result[z] = sum(akum);
				//je¿eli waga maski jest niezerowa, to nale¿y sumê akumulatora podzieliæ przez tê wagê
				if (wmask != 0)
				{
					result[z] = result[z] / wmask;
				}
				//sprawdziæ, czy suma akumulatora mieœci siê miêdzy 0 a 255
				if (result[z] < 0)
					result[z] = 0;
				else if (result[z] > 255)
					result[z] = 255;
			}
			//nadaæ pikselowi wartoœæ sumy akumulatora
			if (image->format() == QImage::Format_Indexed8) 
				newImage->setPixel(x, y, result[0]);
			else 
				newImage->setPixel(x, y, qRgb(result[0], result[1], result[2]));
		}
	}

	return newImage;
}

/** Joins to matrices by multiplying the A[i,j] with B[i,j].
  * Warning! Both Matrices must be squares with the same size!
  */
const math::matrix<float> Convolution::join(math::matrix<float> A, math::matrix<float> B)
{
	int size = A.rowno();
	math::matrix<float> C(size, size);

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			C(x, y) = A(x, y) * B(x, y);
		}
	}

	return C;
}

/** Sums all of the matrixes elements */
const float Convolution::sum(const math::matrix<float> A)
{
	float sum = 0.0;
	int rows = A.rowno();
	int cols = A.colno();
	for (int x = 0; x<rows; x++)
	{
		for (int y = 0; y<cols; y++)
		{
			sum += A(x,y);
		}
	}

	return sum;
}


/** Returns reflected version of a matrix */
const math::matrix<float> Convolution::reflection(const math::matrix<float> A)
{
    int size = A.rowno();
    math::matrix<float> C(size, size);

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			C(x, y) = A(size - 1 - x, size - 1 - y);
		}
	}

    return C;
}
