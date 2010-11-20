#include "VisionUtils.h"

#include <cmath>
#include <limits>

#include "special.h"

using std::vector;
using std::pow;
using std::numeric_limits;

Array2D<double> weightLinearLeastSquareFit(const Array2D<double> &q, const Array2D<double> &X, const Array2D<double> &Y, const Array2D<double> &weights)
{
    Array2D<double> WX(X.dim1(), X.dim2()+1, 1.0);
    WX = getrows(X, 1, WX.dim1()-1);
    WX = dotmult(WX, repmat(weights, 1, WX.dim2()));

    Array2D<double> WY = dotmult(Y, repmat(weights, 1, WX.dim2()));

    Array2D<double> Wq(q.dim1(), X.dim2()+1, 1.0);
    Wq = getrows(q, 1, Wq.dim1()-1);

    Array2D<double> B = transpose(WX)*WX;
    B = inv(B);
    B = B*transpose(WX)*WY;

    return q*B;
}

void fitRobustTranslation(Array2D<double> &translation, vector<unsigned int> &outliers, const Array2D<double> &from_points, const Array2D<double> &to_points)
{
    assert(from_points.dim1() == to_points.dim1());

    if(from_points.dim1() < 2)
    {
        translation = to_points-from_points;
        return;
    }

    double est_outlier_rate = 0.5;
    unsigned int num_guesses = static_cast<unsigned int>(ceil(1.0 / pow((1.0-est_outlier_rate), 3.0)));
    unsigned int min_num_samples = 2;

    Array2D<double> best_T;
    vector<unsigned int> best_samples;
    double best_median = numeric_limits<double>::max();
    vector<double> best_residuals;

    for(unsigned int i = 0; i < num_guesses; i++)
    {
        vector<unsigned int> samples;
        samples.push_back(static_cast<unsigned int>(uniform_rand()*from_points.dim1()));
        samples.push_back(static_cast<unsigned int>(uniform_rand()*from_points.dim1()));

        while(samples[1] == samples[0])
            samples[1] = static_cast<unsigned int>(uniform_rand()*from_points.dim1());

        // compute mean translations
        Array2D<double> from_samples(2, 2), to_samples(2, 2);
        for(unsigned int i = 0; i < min_num_samples; i++)
        {
            setrow(from_samples, i, getrow(from_points, samples[i]));
            setrow(to_samples, i, getrow(to_points, samples[i]));
        }
        Array2D<double> T = sum(to_samples-from_samples, "rows")/static_cast<double>(to_samples.dim1());

        vector<double> residuals;
        for(int i = 0; i < to_points.dim1(); i++)
        {
            Array2D<double> res = getrow(from_points, i) + T - getrow(to_points, i);
            residuals.push_back(sqrt((res*transpose(res))(0, 0)));
        }
        double new_median = median(residuals);

        // choose least median
        if(new_median < best_median)
        {
            best_residuals = residuals;
            best_median = new_median;
            best_samples = samples;
            best_T = T;
        }
    }

    // detect outliers
    translation = best_T;
    outliers = detectTranslationOutliers(translation, from_points, to_points);
}

std::vector<unsigned int> detectTranslationOutliers(const Array2D<double> &translation, const Array2D<double> &from_points, const Array2D<double> &to_points)
{
    vector<double> residuals;
    for(int i = 0; i < to_points.dim1(); i++)
    {
        Array2D<double> res = getrow(from_points, i) + translation - getrow(to_points, i);
        residuals.push_back(sqrt((res*transpose(res))(0, 0)));
    }
    double new_median = median(residuals);
    double new_sigma = new_median*1.4826;

    // detect outliers
    vector<unsigned int> outliers;
    for(unsigned int i = 0; i < residuals.size(); i++)
        if(residuals[i] > 2.5*new_sigma)
            outliers.push_back(i);

    return outliers;
}
