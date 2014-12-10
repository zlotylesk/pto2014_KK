#include "edge_canny.h"
#include "blur_gaussian.h"
#include "conversion_grayscale.h"
#include "edge_sobel.h"

int kos[361];
math::matrix<float> m;
math::matrix<float> o;

void init(PNM* img)
{
	int width = img->width();
	int height = img->height();

	m = math::matrix<float>(width, height);
	o = math::matrix<float>(width, height);
	for (int i = 0; i < 361; i++)
	{
		// sprawdŸ, w którym z 4 mo¿liwych kierunków skierowany jest gradient piksela
		if ((i >= 0 && i < 23) || (i >= 158 && i < 203) || (i >= 338 && i < 361)) {
			kos[i] = 0;
		} else if ((i >= 23 && i < 68) || (i >= 203 && i < 248)) {
			kos[i] = 1;
		} else if ((i >= 68 && i < 113) || (i >= 248 && i < 293)) {
			kos[i] = 2;
		} else if ((i >= 113 && i < 158) || (i >= 293 && i < 338)) {
			kos[i] = 3;
		}
	}
}

EdgeCanny::EdgeCanny(PNM* img) : Convolution(img)
{
	init(img);
}

EdgeCanny::EdgeCanny(PNM* img, ImageViewer* iv) : Convolution(img, iv)
{
	init(img);
}

PNM* EdgeCanny::transform()
{
	int width = image->width();
	int	height = image->height();

	int upper_threshold = getParameter("upper_threshold").toInt();
	int	lower_threshold = getParameter("lower_threshold").toInt();

	PNM* newImage = new PNM(width, height, QImage::Format_Indexed8);

	// Dokonaj konwersji obrazu wejœciowego na skalê odcieni szaroœci.
	PNM* tmpImage = ConversionGrayscale::ConversionGrayscale(image).transform();

	// Rozmyj otrzymany obraz filtrem Gaussa 3x3 z parametrem 1.6
	BlurGaussian blurGaussian(tmpImage);
	blurGaussian.setParameter("sigma", 1.6);
	blurGaussian.setParameter("size", 3);
	tmpImage = blurGaussian.transform();

	// Oblicz gradienty kierunkowe Gx i Gy operatorem Sobela przy pomocy metod rawHorizontalDetection() i rawVerticalDetection().
	EdgeSobel sobel(tmpImage);
	math::matrix<float>* Gx = sobel.rawHorizontalDetection();
	math::matrix<float>* Gy = sobel.rawVerticalDetection();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			// Dla ka¿dego piksela(i, j) oblicz moc m oraz kierunek o gradientu.
			newImage->setPixel(i, j, PIXEL_VAL_MIN);
			m(i, j) = sqrt(pow((*Gx)(i, j), 2) + pow((*Gy)(i, j), 2));
			o(i, j) = (int)((atan2((*Gy)(i, j), (*Gx)(i, j)) / (3.14159265359)) * 180 + 360) % 360;
		}
	}

	double ne1, ne2;
	std::list<QPoint> initialPoints;
	for (int i = 1; i < width - 1; i++)
	{
		for (int j = 1; j < height - 1; j++)
		{
			// wybierz s¹siadów piksela (i,j) bêd¹cych na linii prostopad³ej do kierunku gradientu piksela
			int pos = o(i, j);
			double m2 = m(i, j);
			int point1n = 1;
			QPoint point1;
			if (point1n == 1)
			{
				if (kos[pos] == 0)
				{
					point1 = QPoint(0, -1);
				}
				else if (kos[pos] == 1)
				{
					point1 = QPoint(1, -1);
				}
				else if (kos[pos] == 2)
				{
					point1 = QPoint(-1, 0);
				}
				else if (kos[pos] == 3)
				{
					point1 = QPoint(-1, -1);
				}
			}
			else
			{
				if (kos[pos] == 0)
				{
					point1 = QPoint(0, 1);
				}
				else if (kos[pos] == 1)
				{
					point1 = QPoint(-1, 1);
				}
				else if (kos[pos] == 2)
				{
					point1 = QPoint(1, 0);
				}
				else if (kos[pos] == 3)
				{
					point1 = QPoint(1, 1);
				}
			}
			QPoint point2;
			int point2n = 2;
			if (point2n == 1)
			{
				if (kos[pos] == 0)
				{
					point2 = QPoint(0, -1);
				}
				else if (kos[pos] == 1)
				{
					point2 = QPoint(1, -1);
				}
				else if (kos[pos] == 2)
				{
					point2 = QPoint(-1, 0);
				}
				else if (kos[pos] == 3)
				{
					point2 = QPoint(-1, -1);
				}
			}
			else
			{
				if (kos[pos] == 0)
				{
					point2 = QPoint(0, 1);
				}
				else if (kos[pos] == 1)
				{
					point2 = QPoint(-1, 1);
				}
				else if (kos[pos] == 2)
				{
					point2 = QPoint(1, 0);
				}
				else if (kos[pos] == 3)
				{
					point2 = QPoint(1, 1);
				}
			}
			ne1 = m(i + point1.x(), j + point1.y());
			ne2 = m(i + point2.x(), j + point2.y());

			// je¿eli moc gradientu piksela(i, j) jest wiêksza od mocy gradientów 
			// odpowiadaj¹cych mu s¹siadów oraz wiêksza od upper_thresh, to dodaj 
			// piksel(i, j) do pocz¹tkowego zbioru krawêdzi.
			if (m2 > ne1 && m2 > ne2 && m2 > upper_threshold)
			{
				newImage->setPixel(i, j, PIXEL_VAL_MAX);
				initialPoints.push_back(QPoint(i, j));
			}
		}
	}

	// przeprowadŸ progowanie z histerez¹
	while (!initialPoints.empty())
	{
		QPoint point = initialPoints.back();
		initialPoints.pop_back();
		int pos = o(point.x(), point.y());
		QPoint tneipoin[7] = {
			QPoint(-1, 0),
			QPoint(-1, 1),
			QPoint(0, -1),
			QPoint(0, 1),
			QPoint(1, -1),
			QPoint(1, 0),
			QPoint(1, 1),
		};
		for (int i = 1; i <= 2; i++)
		{
			QPoint dirVe;
			int point2n = 2;
			if (i == 1)
			{
				if (kos[pos] == 0)
				{
					dirVe = QPoint(0, -1);
				}
				else if (kos[pos] == 1)
				{
					dirVe = QPoint(1, -1);
				}
				else if (kos[pos] == 2)
				{
					dirVe = QPoint(-1, 0);
				}
				else if (kos[pos] == 3)
				{
					dirVe = QPoint(-1, -1);
				}
			}
			else
			{
				if (kos[pos] == 0)
				{
					dirVe = QPoint(0, 1);
				}
				else if (kos[pos] == 1)
				{
					dirVe = QPoint(-1, 1);
				}
				else if (kos[pos] == 2)
				{
					dirVe = QPoint(1, 0);
				}
				else if (kos[pos] == 3)
				{
					dirVe = QPoint(1, 1);
				}
			}
			QPoint cPoint = point;
			while (true)
			{
				QPoint npoint = QPoint(cPoint.x() + dirVe.x(), cPoint.y() + dirVe.y());
				if (npoint.x() == width - 1 || npoint.x() == 0 || npoint.y() == height - 1 || npoint.y() == 0) break;
				if (newImage->pixel(npoint.x(), npoint.y()) == PIXEL_VAL_MAX) break;
				// moc gradientu wiêksz¹ od lower_thresh
				if (m(npoint.x(), npoint.y()) < lower_threshold) break;
				// gradient skierowany w tê sam¹ stronê co piksel, z którego przyszed³eœ
				if (kos[(int)o(npoint.x(), npoint.y())] != kos[pos]) break;
				bool maxM = true;
				double mag = m(npoint.x(), npoint.y());
				for (int i = 0; i < 7; i++)
				{
					QPoint tnei(npoint.x() + tneipoin[i].x(), npoint.y() + tneipoin[i].y());
					if (tnei.x() == cPoint.x() && tnei.y() == cPoint.y()) continue;
					if (kos[(int)o(tnei.x(), tnei.y())] != kos[pos]) continue;
					if (m(tnei.x(), tnei.y()) >= mag)
					{
						maxM = false;
						break;
					}
				}
				if (!maxM) break;
				newImage->setPixel(npoint.x(), npoint.y(), PIXEL_VAL_MAX);
				cPoint = npoint;
			}
		}
	}

	return newImage;
}
