#ifndef ANY_IMAGE_H
#define ANY_IMAGE_H

#include "LinearAlgebra.h"

#include <string>
#include <vector>

template <typename T>
class AnyImage : public Array3D<T>
{
    public:
        AnyImage()
        {}

        AnyImage(int width, int height, int pixel_size)
        : Array3D<T>(width, pixel_size, height)
        {
        }

        AnyImage(int width, int height, int pixel_size, const T &val)
        : Array3D<T>(width, pixel_size, height, val)
        {
        }

        AnyImage(const Array3D<T> &rhs)
        : Array3D<T>(rhs)
        {
        }

        AnyImage<T>& operator=(const Array3D<T> &rhs)
        {
            Array3D<T>::operator=(rhs);
            return *this;
        }

        AnyImage<T>& operator=(const T &a)
        {
            Array3D<T>::operator=(a);
            return *this;
        }

        bool isSameSize(const AnyImage<T> &other) const
        {
            return (width() == other.width()) && (height() == other.height()) && (pixel_size() == other.pixel_size());
        }

        int width() const
        {
            return Array3D<T>::dim1();
        }

        int height() const
        {
            return Array3D<T>::dim3();
        }

        int pixel_size() const
        {
            return Array3D<T>::dim2();
        }

        void set(const T &val)
        {
            Array3D<T>::operator=(val);
        }

        void set(const int &x, const int &y, const int &p, const T &val)
        {
            Array3D<T>::operator()(x, p, y) = val;
        }

        const T &clampedGet(const int &x, const int &y, const int &p) const
        {
            return Array3D<T>::operator()(std::max(0, std::min(x, width()-1)), p, std::max(0, std::min(y, height()-1)));
        }

        void copy2image(AnyImage &other) const
        {
            assert(other.width() >= width());
            assert(other.height() >= height());
            assert(other.pixel_size() == pixel_size());

            unsigned char *psource = this->vals();
            unsigned char *pdest = other.vals();

            for(int y = 0; y < height(); y++)
            {
                memcpy(pdest, psource, width()*pixel_size()*sizeof(T));
                psource += width()*pixel_size();
                pdest += other.width()*other.pixel_size();
            }
        }

        bool copy2buffer(unsigned char *buffer, const int buffer_width, const int buffer_height, const int buffer_pixel_size) const
        {
            assert(buffer_width >= width());
            assert(buffer_height >= height());
            assert(buffer_pixel_size == pixel_size());

            unsigned char *psource = this->vals();
            unsigned char *pdest = buffer;

            for(int y = 0; y < height(); y++)
            {
                memcpy(pdest, psource, width()*pixel_size()*sizeof(unsigned char));
                psource += width()*pixel_size();
                pdest += buffer_width*buffer_pixel_size;
            }

            return true;
        }
};


template <typename T>
void rgb2gray(const AnyImage<T> &rgb_image, AnyImage<double> &gray_image)
{
    assert(rgb_image.width() == gray_image.width());
    assert(rgb_image.height() == gray_image.height());
    assert(rgb_image.pixel_size() == 3);
    assert(gray_image.pixel_size() == 1);

    const int n = rgb_image.width()*rgb_image.height();
    T *psource = rgb_image.vals();
    double *ptarget = gray_image.vals();
    for(int i = 0; i < n; i++)
    {
        *ptarget = psource[0]*0.299 + psource[1]*0.587 + psource[2]*0.114;
        ptarget++;
        psource += 3;
    }
}

template <typename T>
void threshold(AnyImage<unsigned char> &mask_image, const AnyImage<T> &test_image, const T &tol)
{
    assert(mask_image.dim1() == test_image.dim1());
    assert(mask_image.dim2() == test_image.dim2());
    assert(mask_image.dim3() == test_image.dim3());

    mask_image.set(255);
    const int n = test_image.width()*test_image.height()*test_image.pixel_size();
    T *psource = test_image.vals();
    unsigned char *ptarget = mask_image.vals();
    for(int i = 0; i < n; i++)
    {
        if(*psource < tol)
            *ptarget = 0;
        ptarget++;
        psource ++;
    }
}

template <typename T>
std::vector<int> extractForeground(Array2D<double> &points, const AnyImage<T> &fore_mask)
{
    assert(fore_mask.pixel_size() == 1);

    const int w = fore_mask.width();
    const int h = fore_mask.height();
    const int n = fore_mask.size();

    std::vector<int> inds;
    for(int i = 0; i < n; i++)
        if(fore_mask(i) > 0)
            inds.push_back(i);
//    std::cout << inds.size() << std::endl;

    points = Array2D<double>(inds.size(), 2);
    for(int i = 0; i < points.dim1(); i++)
    {
        points(i, 0) =   inds[i]%w;
        points(i, 1) = h-inds[i]/w;
    }

    return inds;
    
}

#endif
