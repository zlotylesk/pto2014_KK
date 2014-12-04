#include "edge_sobel.h"

EdgeSobel::EdgeSobel(PNM* img, ImageViewer* iv) :
    EdgeGradient(img, iv)
{
    prepareMatrices();
}

EdgeSobel::EdgeSobel(PNM* img) :
    EdgeGradient(img)
{
    prepareMatrices();
}

void EdgeSobel::prepareMatrices()
{
	math::matrix<float> gx(3, 3);
	math::matrix<float> gy(3, 3);

	g_x = gx;
	g_y = gy;

	g_x(0, 0) = -1;
	g_x(0, 1) = 0;
	g_x(0, 2) = 1;
	g_x(1, 0) = -2;
	g_x(1, 1) = 0;
	g_x(1, 2) = 2;
	g_x(2, 0) = -1;
	g_x(2, 1) = 0;
	g_x(2, 2) = 1;

	g_y(0, 0) = -1;
	g_y(0, 1) = -2;
	g_y(0, 2) = -1;
	g_y(1, 0) = 0;
	g_y(1, 1) = 0;
	g_y(1, 2) = 0;
	g_y(2, 0) = 1;
	g_y(2, 1) = 2;
	g_y(2, 2) = 1;
}

math::matrix<float>* EdgeSobel::rawHorizontalDetection()
{
    math::matrix<float>* x_gradient = new math::matrix<float>(this->image->width(), this->image->height());

	int width = x_gradient->rowno();
	int height = x_gradient->colno();
	for (int w = 0; w < width; w++){
		for (int h = 0; h < height; h++){
			math::matrix<float> window = getWindow(w, h, 3, LChannel, NullEdge);
			(*x_gradient)(w, h) = Convolution::sum(Convolution::join(g_x, window));
		}
	}
    return x_gradient;
}

math::matrix<float>* EdgeSobel::rawVerticalDetection()
{
    math::matrix<float>* y_gradient = new  math::matrix<float>(this->image->width(), this->image->height());

	int width = y_gradient->rowno();
	int height = y_gradient->colno();
	for (int unsigned w = 0; w < width; w++){
		for (int unsigned h = 0; h < height; h++){
			math::matrix<float> window = getWindow(w, h, 3, LChannel, RepeatEdge);
			float suma = sum(join(g_y, window));
			(*y_gradient)(w, h) = Convolution::sum(Convolution::join(g_y, window));
		}
	}
    return y_gradient;
}
