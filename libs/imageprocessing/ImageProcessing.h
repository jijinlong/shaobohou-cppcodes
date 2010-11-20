#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include "LinearAlgebra.h"

#include <vector>
#include <fstream>

using std::vector;


template <typename T>
T computeIntegral(const Array2D<T> &cumsum_vals, const vector<vector<int> > &xcoords)
{
    assert(cumsum_vals.dim1() == static_cast<int>(xcoords.size()));

    T total(0);
    for(unsigned int i = 0; i < xcoords.size(); i++)
    {
        int y = i;
        for(unsigned int j = 0; j < xcoords[i].size(); j+=2)
        {
            if((j+1) >= xcoords[i].size()) continue;

            int x0 = xcoords[i][j+0];
            int x1 = xcoords[i][j+1];

            total += cumsum_vals(y, x1) - cumsum_vals(y, x0);
        }
    }

    return total;
}

// scan line

template <typename T>
vector<vector<int> > scanLines(const vector<T> &lines_points, int &ymin_int, int &ymax_int, bool do_sort=true)
{
    if(lines_points.size() < 2)
        return vector<vector<int> >();

    // find min and max y coords
    double ymin = lines_points[0][1];
    double ymax = lines_points[0][1];
    for(unsigned int i = 0; i < lines_points.size(); i++)
    {
        if(lines_points[i][1] < ymin) ymin = lines_points[i][1];
        if(lines_points[i][1] > ymax) ymax = lines_points[i][1];
    }

    double ymin_real = static_cast<int>(ymin_int);
    double ymax_real = static_cast<int>(ymax_int);
    ymin = std::min(std::max(ymin, ymin_real), ymax);
    ymax = std::max(std::min(ymax, ymax_real), ymin);
    ymin_int = static_cast<int>(ymin);
    ymax_int = static_cast<int>(ymax);
    assert(ymin <= ymax);

    // generate scanlines
    vector<vector<int> > xcoords(ymax_int-ymin_int+1);
    for(unsigned int i = 0; i < lines_points.size(); i+=2)
    {
        double dxdy = (lines_points[i+1][0]-lines_points[i+0][0])/(lines_points[i+1][1]-lines_points[i+0][1]);
        double y0 = lines_points[i+0][1];
        double y1 = lines_points[i+1][1];

        if(static_cast<int>(round(y0)) == static_cast<int>(round(y1))) continue; // ignore horizontal lines
        if(((y0 < ymin) && (y1 < ymin)) || ((y0 > ymax) && (y1 > ymax))) continue; // ignore if line is completely out of view

        // clamp y values
        y0 = std::min(std::max(ymin, y0), ymax);
        y1 = std::min(std::max(ymin, y1), ymax);

        // compute x0 (important) and x1 based clamped values;
        double ratio0 = (y0-lines_points[i+1][1]) / (lines_points[i+0][1]-lines_points[i+1][1]);
//         double ratio1 = (y1-lines_points[i+1][1]) / (lines_points[i+0][1]-lines_points[i+1][1]);
        double x0 = lines_points[i+1][0] + (lines_points[i+0][0]-lines_points[i+1][0])*ratio0;
//         double x1 = lines_points[i+1][0] + (lines_points[i+0][0]-lines_points[i+1][0])*ratio1;

        if((y1-y0) >= 1.0)
        {
            int y_start = static_cast<int>(y0-ymin);
            int y_end = static_cast<int>(y1-ymin);
            for(int j = y_start; j <= y_end; j++)
            {
                xcoords[j].push_back(static_cast<int>(x0));
                x0 += dxdy;
            }
        }
        else if((y0-y1) >= 1.0)
        {
            int y_start = static_cast<int>(y0-ymin);
            int y_end = static_cast<int>(y1-ymin);
            for(int j = y_start; j >= y_end; j--)
            {
                xcoords[j].push_back(static_cast<int>(x0));
                x0 -= dxdy;
            }
        }
    }

    if(do_sort)
        for(unsigned int i = 0; i < xcoords.size(); i++)
            if(xcoords[i].size() > 1)
                sort(xcoords[i].begin(), xcoords[i].end());

    return xcoords;
}



template <typename T>
std::vector<Array1D<T> > split(const Array2D<T> &mat, const std::string &dim="rows")
{
    std::vector<Array1D<T> > res;

    if(dim == "rows")
    {
        for(int i = 0; i < mat.dim1(); i++)
        {
            res.push_back(Array1D<T>(mat.dim2()));
            for(int j = 0; j < mat.dim2(); j++)
                res[i][j] = mat(i, j);
        }
    }
    else if(dim == "cols")
    {
        for(int i = 0; i < mat.dim2(); i++)
        {
            res.push_back(Array1D<T>(mat.dim1()));
            for(int j = 0; j < mat.dim1(); j++)
                res[i][j] = mat(j, i);
        }
    }
    else
        assert(false);

    return res;
}

template <typename T>
Array1D<T> cumsum(const Array1D<T> &v)
{
    assert (v.dim() > 0);

    Array1D<T> res(v.dim());
    res[0] = v[0];
    for(int i = 1; i < res.dim(); i++)
        res[i] = res[i-1]+v[i];

    return res;
}

template <typename T>
Array1D<T> gradient(const Array1D<T> &v)
{
    assert(v.dim() > 0);

    Array1D<T> g(v.dim(), T(0));
    if(v.dim() > 1)
    {
        g[0] = v[1]-v[0];
        for(int i = 1; i < v.dim()-1; i++)
            g[i] = (v[i+1]-v[i-1])/2;
        g[v.dim()-1] = v[v.dim()-1]-v[v.dim()-2];
    }

    return g;
}

template <typename T>
Array1D<T> conv(const Array1D<T> &v, const Array1D<T> &kern, bool repeat_boundary=false, bool same=false)
{
    int offset = kern.dim()-1;

    Array1D<T> res(v.dim() + offset, T(0));
    for(int i = -offset; i < v.dim(); i++)
        for(int j = 0; j < kern.dim(); j++)
        {
            if((i+j) < 0)
            {
                if(repeat_boundary)
                    res[i+offset] += v[0]*kern[j];
            }
            else if((i+j) >= v.dim())
            {
                if(repeat_boundary)
                    res[i+offset] += v[v.dim()-1]*kern[j];
            }
            else
                res[i+offset] += v[i+j]*kern[j];
        }

    if(same)
        res = res.subarray(offset/2, res.dim()-offset/2-1);

    return res;
}

template <typename T>
Array1D<T> generate1DGaussianKernel(const T &sigma, const T &tol)
{
    T sigma2(sigma*sigma);
    T pi(3.1415926);

    int dist = -1;
    T temp_prob(0.0);
    do
    {
        dist++;
        temp_prob = exp(-0.5*dist*dist/sigma2)/sqrt(2.0*pi*sigma2);
    }
    while(temp_prob > tol);

    Array1D<T> kernel(dist*2+1);
    for(int i = 0; i <= dist; i++)
    {
        kernel[dist-i] = exp(-0.5*i*i/sigma2)/sqrt(2.0*pi*sigma2);
        kernel[dist+i] = kernel[dist-i];
    }
    kernel = kernel/sum(kernel);

    return kernel;
}

template <typename T>
Array2D<T> diff(const Array2D<T> &X, const std::string &dim="rows")
{
    Array2D<T> res;

    if(dim == "rows")
    {
        assert(X.dim1() > 1);
        Array2D<T> tempX = X;
        res = tempX.subarray(1, X.dim1()-1, 0, X.dim2()-1) - tempX.subarray(0, X.dim1()-2, 0, X.dim2()-1);
    }
    else if(dim == "cols")
    {
        assert(X.dim2() > 1);
        Array2D<T> tempX = X;
        res = tempX.subarray(0, X.dim1()-1, 1, X.dim2()-1) - tempX.subarray(0, X.dim1()-1, 0, X.dim2()-2);
    }
    else
        assert(false);

    return res;
}

template <typename T>
Array1D<T> computeArcLength(const Array2D<T> &X)
{
    Array2D<T> diffs = diff(X);
    return sqrt(sum(diffs*diffs, "cols"));
}

template <typename T>
Array2D<T> linearInterpolation(const Array2D<T> &X, unsigned int nsamples)
{
    assert(nsamples > 1);

    Array1D<T> segments = computeArcLength(X);
    segments = cumsum(segments/sum(segments));

    Array1D<T> samples(nsamples);
    for(unsigned int i = 0; i < nsamples; i++)
        samples[i] = T(i)/T(nsamples-1);

    int ind = 0;
    Array2D<T> sample_X(nsamples, X.dim2());
    for(int i = 0; i < samples.dim(); i++)
    {
        if(ind == 0)
        {
            if(samples[i] <= segments[0])
            {
                T ratio = samples[i]/segments[0];
                for(int j = 0; j < X.dim2(); j++)
                    sample_X(i, j) = X(0, j) + (X(1, j) - X(0, j))*ratio;
                continue;
            }
            else
                ind++;
        }

        while(ind < X.dim1())
        {
            if(samples[i] <= segments[ind])              // if less than start of segment
            {
                for(int j = 0; j < X.dim2(); j++)
                    sample_X(i, j) = X(ind, j);
                break;
            }
            else if((ind+1) >= X.dim1())                // if not less than start of segment but ind is the last point
            {
                for(int j = 0; j < X.dim2(); j++)
                    sample_X(i, j) = X(X.dim1()-1, j);

                break;
            }
            else if((samples[i] >= segments[ind]) && (samples[i] <= segments[ind+1]))   // within the segment
            {
                T ratio = (samples[i]-segments[ind])/(segments[ind+1]-segments[ind]);
                for(int j = 0; j < X.dim2(); j++)
                    sample_X(i, j) = X(ind+1, j) + (X(ind+2, j)-X(ind+1, j))*ratio;     // there is an offset of 1 between segments and X

                break;
            }
            else
                ind++;
        }

    }

    return sample_X;
}

template <typename T>
Array1D<T> computeCSSImage(const Array1D<T> &x, const Array1D<T> &y, const T &sigma)
{
    Array1D<T> kern = generate1DGaussianKernel(sigma, 0.001);
    Array1D<T> kern_d = gradient(kern);
    Array1D<T> kern_d2 = gradient(kern_d);

//     Array1D<T> X = conv(x, kern, true, true);
    Array1D<T> Xu = conv(x, kern_d, true, true);
    Array1D<T> Xuu = conv(x, kern_d2, true, true);
//     Array1D<T> Y = conv(y, kern, true, true);
    Array1D<T> Yu = conv(y, kern_d, true, true);
    Array1D<T> Yuu = conv(y, kern_d2, true, true);

    Array1D<T> den = sqrt(Xu*Xu + Yu*Yu);
    den = den*den*den;
    Array1D<T> css = (Xu*Yuu - Xuu*Yu) / den;

    return css;
}

template <typename T>
Array1D<int> thresholdAbove(const Array1D<T> &v, const T &tol)
{
    Array1D<int> inds(v.dim(), 0);
    for(int i = 0; i < v.dim(); i++)
        if(v[i] > tol)
            inds[i] = 1;

    return inds;
}


template <typename T>
Array1D<int> findMaxima(const Array1D<T> &v)
{
    Array1D<int> inds(v.dim(), 0);
    for(int i = 0; i < v.dim(); i++)
        if((v[i] > v[i-1]) && (v[i] > v[i+1]))
            inds[i] = 1;

    return inds;
}

template <typename T>
Array1D<int> findMinima(const Array1D<T> &v)
{
    Array1D<int> inds(v.dim(), 0);
    for(int i = 0; i < v.dim(); i++)
        if((v[i] < v[i-1]) && (v[i] < v[i+1]))
            inds[i] = 1;

    return inds;
}

#endif
