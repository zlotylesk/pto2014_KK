#include "hough_rectangles.h"
#include "corner_harris.h"

HoughRectangles::HoughRectangles(PNM* img) :
    Transformation(img)
{
}

HoughRectangles::HoughRectangles(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* remove(int x, int y, int newx, int newy, PNM* img)
{
	QColor color = QColor::fromRgb(img->pixel(newx, newy));
	if (newx != x && newy != y)
	{
		img->setPixel(newx, newy, Qt::white);
	}
	if (QColor::fromRgb(img->pixel(newx - 1, newy)).black()!=255)
	{
		img = remove(x, y, newx - 1, newy, img);
	}
	if (QColor::fromRgb(img->pixel(newx + 1, newy)).black() != 255)
	{
		img = remove(x, y, newx + 1, newy, img);
	}
	if (QColor::fromRgb(img->pixel(newx, newy - 1)).black() != 255)
	{
		img = remove(x, y, newx, newy - 1, img);
	}
	if (QColor::fromRgb(img->pixel(newx, newy + 1)).black() != 255)
	{
		img = remove(x, y, newx, newy + 1, img);
	}
	return img;
}

std::vector<std::pair<int, int>> check_quadrat(std::pair<int, int> point, PNM* img)
{
	int stage = 0;

	int startx = 0;
	int starty = 0;

	std::vector<std::pair<int, int>> points;
	points.push_back(std::make_pair(point.first, point.second));

	if (stage == 0)
	{
		for (int i = point.first+1; i < img->width(); i++)
		{
			for (int j = point.second - 2; j < point.second + 2; j++)
			{
				if (stage != 0)
					continue;
				if (QColor::fromRgb(img->pixel(i, j)).black() != 255)
				{
					stage = 1;
					startx = i;
					starty = j;
					points.push_back(std::make_pair(i, j));
				}
			}
		}
	}

	if (stage == 1)
	{
		for (int i = startx-2; i < startx+2; i++)
		{
			for (int j = starty+1; j < img->height(); j++)
			{
				if (stage != 1)
					continue;
				if (QColor::fromRgb(img->pixel(i, j)).black() != 255)
				{
					stage = 2;
					startx = i;
					starty = j;
					points.push_back(std::make_pair(i, j));
				}
			}
		}
	}

	if (stage == 2)
	{
		for (int i = startx-1; i >= 0; i--)
		{
			for (int j = starty - 2; j < starty + 2; j++)
			{
				if (stage != 2)
					continue;
				if (QColor::fromRgb(img->pixel(i, j)).black() != 255)
				{
					stage = 3;
					startx = i;
					starty = j;
					points.push_back(std::make_pair(i, j));
				}
			}
		}
	}

	return points;
}

PNM* drawQuadrat(std::vector<std::pair<int, int>> corners, PNM* img)
{
	for (int i = corners.at(0).first; i < corners.at(1).first; i++)
	{
		for (int j = corners.at(0).second - 2; j < corners.at(0).second + 2; j++)
		{
			img->setPixel(i, j, 1);
		}
	}

	for (int i = corners.at(3).first; i < corners.at(2).first; i++)
	{
		for (int j = corners.at(2).second - 2; j < corners.at(2).second + 2; j++)
		{
			img->setPixel(i, j, 1);
		}
	}

	for (int i = corners.at(0).second; i < corners.at(3).second; i++)
	{
		for (int j = corners.at(0).first - 2; j < corners.at(0).first + 2; j++)
		{
			img->setPixel(j, i, 1);
		}
	}

	for (int i = corners.at(1).second; i < corners.at(2).second; i++)
	{
		for (int j = corners.at(2).first - 2; j < corners.at(2).first + 2; j++)
		{
			img->setPixel(j, i, 1);
		}
	}

	return img;
}

PNM* HoughRectangles::transform()
{
	PNM* img = CornerHarris(image).transform();

	for (int i = 0; i < img->width(); i++)
	{
		for (int j = 0; j < img->height(); j++)
		{
			QColor color = QColor::fromRgb(img->pixel(i, j));
			if (color.black() != 255)
			{
				img = remove(i, j, i, j, img);
			}
		}
	}

	std::vector<std::pair<int, int>> points;

	for (int i = 0; i < img->width(); i++)
	{
		for (int j = 0; j < img->height(); j++)
		{
			QColor color = QColor::fromRgb(img->pixel(i, j));
			if (color.black() != 255)
			{
				points.push_back(std::make_pair(i, j));
			}
		}
	}

	for (int i = 0; i < points.size(); i++)
	{
		std::vector<std::pair<int, int>> corners = check_quadrat(points.at(i), img);
		if (corners.size() == 4)
		{
			img = drawQuadrat(corners, img);
		}
	}

    return img;
}

