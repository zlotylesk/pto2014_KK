#include "bin_iterbimodal.h"

BinarizationIterBimodal::BinarizationIterBimodal(PNM* img) :
    Transformation(img)
{
}

BinarizationIterBimodal::BinarizationIterBimodal(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* BinarizationIterBimodal::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Mono);

    qDebug() << Q_FUNC_INFO << "Not implemented yet!";

    return newImage;
}



