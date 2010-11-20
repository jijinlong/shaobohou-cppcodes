#include "CubicSpline.h"

using std::vector;

CubicSpline::CubicSpline()
{
}

CubicSpline::CubicSpline(const Array2D<double> &parameters)
{
    assert(parameters.dim2() == 4);
    this->parameters = parameters;
}

CubicSpline::~CubicSpline()
{
}

unsigned int CubicSpline::numberOfSegments() const
{
    return parameters.dim1();
}

double CubicSpline::evaluate(double t) const
{
    double time = t;
    if (time < 0.0)
        time = 0.0;
    else if (time > 1.0)
        time = 1.0;
    else
        time = time;

    double segmentLength = 1.0 / numberOfSegments();
    unsigned int segmentIndex = 0;
    while (time > segmentLength)
    {
        segmentIndex++;
        time -= segmentLength;
    }
    time /= segmentLength;
    if (segmentIndex >= numberOfSegments()) segmentIndex = numberOfSegments() - 1;

    return evaluate(segmentIndex, time);
}

double CubicSpline::evaluate(int index, double t) const
{
    assert(index < parameters.dim1());

    double a = parameters(index, 0);
    double b = parameters(index, 1);
    double c = parameters(index, 2);
    double d = parameters(index, 3);

    if (t < 0.0) return a;
    if (t > 1.0) return a + b + c + d;

    return a + b * t + c * t * t + d * t * t * t;
}

CubicSpline CubicSpline::computeNaturalCubicSpline(const vector<double> &x)
{
    unsigned int n = x.size() - 1;
    Array1D<double> gamma(n + 1, 0.0);
    Array1D<double> delta(n + 1, 0.0);
    Array1D<double> dx(n + 1, 0.0);

    //compute gamma
    gamma[0] = 1.0 / 2.0;
    for (unsigned int i = 1; i < n; i++)
        gamma[i] = 1.0 / (4 - gamma[i - 1]);
    gamma[n] = 1 / (2 - gamma[n - 1]);

    //compute delta
    delta[0] = 3.0 * (x[1] - x[0]) * gamma[0];
    for (unsigned int i = 1; i < n; i++)
        delta[i] = (3.0 * (x[i + 1] - x[i - 1]) - delta[i - 1]) * gamma[i];
    delta[n] = (3.0 * (x[n] - x[n - 1]) - delta[n - 1]) * gamma[n];

    //compute dx
    dx[n] = delta[n];
    for (int i = n - 1; i >= 0; i--)
        dx[i] = delta[i] - gamma[i] * dx[i + 1];

    Array2D<double> parameters(n, 4, 0.0);
    for (unsigned int i = 0; i < n; i++)
    {
        parameters(i, 0) = x[i];
        parameters(i, 1) = dx[i];
        parameters(i, 2) = 3.0 * (x[i + 1] - x[i]) - 2.0 * dx[i] - dx[i + 1];
        parameters(i, 3) = 2.0 * (x[i] - x[i + 1]) + dx[i] + dx[i + 1];
    }

    return CubicSpline(parameters);
}
