#ifndef CUBIC_SPLINE_H
#define CUBIC_SPLINE_H

#include "LinearAlgebra.h"

#include <vector>

class CubicSpline
{
    public:
        CubicSpline();
        CubicSpline(const Array2D<double> &parameters);
        ~CubicSpline();

        unsigned int numberOfSegments() const;
        double evaluate(double t) const;
        double evaluate(int index, double t) const;

        static CubicSpline computeNaturalCubicSpline(const std::vector<double> &x);

    protected:
        Array2D<double> parameters;
};

#endif
