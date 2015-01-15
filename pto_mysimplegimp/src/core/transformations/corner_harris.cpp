#include "corner_harris.h"

#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

CornerHarris::CornerHarris(PNM* img) :
    Convolution(img)
{
}

CornerHarris::CornerHarris(PNM* img, ImageViewer* iv) :
    Convolution(img, iv)
{
}

PNM* CornerHarris::transform()
{
	int    threshold = 30000000;
	double sigma = 1.0;
	double sigma_weight = 0.78;
	double k_param = 0.05;

    int width  = image->width(),
        height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

	math::matrix<double>
		imx(width, height),
		imy(width, height),
		imxy(width, height),
		corncan(width, height),
		cornnonsup(width, height);
	PNM* tImage = ConversionGrayscale(image).transform();
	BlurGaussian blurGauss(tImage);
	blurGauss.setParameter("size", 3);
	blurGauss.setParameter("sigma", 3.6);
	tImage = blurGauss.transform();
	EdgeSobel esob(tImage);
	math::matrix<float>* xgra = esob.rawHorizontalDetection();
	math::matrix<float>* ygra = esob.rawVerticalDetection();
	for (int w = 0; w < width; w++){
		for (int h = 0; h < height; h++){
			double xgr = (*xgra)(w, h), ygr = (*ygra)(w, h);
			imx(w, h) = xgr * xgr;
			imy(w, h) = ygr * ygr;
			imxy(w, h) = xgr * ygr;
		}
	}
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			if (w == 0 || w == width - 1 || h == 0 || h == height - 1)
			{
				corncan(w, h) = 0;
				cornnonsup(w, h) = 0;
			}
			else
			{
				double sx = 0, sy = 0, sxy = 0;
				for (int k = -1; k < 2; k++){
					for (int j = -1; j < 2; j++){
						double g_val = BlurGaussian::getGauss(k, j, sigma);
						sx += imx(w + k, h + j)*g_val;
						sy += imy(w + k, h + j)*g_val;
						sxy += imxy(w + k, h + j)*g_val;
					}
				}
				sx /= sigma_weight;
				sy /= sigma_weight;
				sxy /= sigma_weight;
				math::matrix<double> H(2, 2);
				H(0, 0) = sx; H(1, 1) = sy; H(0, 1) = sxy; H(1, 0) = sxy;
				double r = H(0, 0)*H(1, 1) - H(0, 1)*H(1, 0) - k_param*pow(H(0, 0) + H(1, 1), 2);
				corncan(w, h) = (r > threshold) ? r : 0;
			}
		}
	}
	bool maknonsup = true;
	while (maknonsup)
	{
		maknonsup = false;
		for (int w = 1; w < width - 1; w++)
		{
			for (int h = 1; h < height - 1; h++)
			{
				double val = corncan(w, h);
				if (val > corncan(w - 1, h - 1) && val > corncan(w - 1, h) && val > corncan(w - 1, h + 1) && val > corncan(w, h - 1) && val > corncan(w, h + 1) && val > corncan(w + 1, h - 1) && val > corncan(w + 1, h) && val > corncan(w + 1, h + 1))
				{
					cornnonsup(w, h) = val;
				}
				else
				{
					if (val > 0)
					{
						maknonsup = true;
					}
					cornnonsup(w, h) = 0;
				}
			}
		}
		corncan = cornnonsup;
	}
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			if (corncan(w, h) == 0){
				newImage->setPixel(w, h, 0);
			}
			else
			{
				newImage->setPixel(w, h, 1);
			}
		}
	}

    return newImage;
}
