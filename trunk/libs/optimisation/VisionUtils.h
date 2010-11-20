#ifndef VISION_UTILS_H
#define VISION_UTILS_H

#include "LinearAlgebra.h"

#include <vector>

template <typename T>
T median(const std::vector<T> &values)
{
    std::vector<T> vals = values;
    sort(vals.begin(), vals.end());

    if((vals.size() % 2) == 0)
        return (vals[static_cast<unsigned int>(floor(vals.size()/2))] + vals[static_cast<unsigned int>(ceil(vals.size()/2))]) / 2;
    else
        return vals[static_cast<unsigned int>(floor(vals.size()/2))];
}

Array2D<double> weightLinearLeastSquareFit(const Array2D<double> &q, const Array2D<double> &X, const Array2D<double> &Y, const Array2D<double> &weights);

void fitRobustTranslation(Array2D<double> &translation, std::vector<unsigned int> &outliers, const Array2D<double> &from_points, const Array2D<double> &to_points);
std::vector<unsigned int> detectTranslationOutliers(const Array2D<double> &translation, const Array2D<double> &from_points, const Array2D<double> &to_points);

#endif
