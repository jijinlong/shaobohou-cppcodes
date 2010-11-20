#ifndef BACKGROUND_MODEL
#define BACKGROUND_MODEL

// eigenbackground better?

#include "AnyImage.h"

template <typename T>
class BackgroundModel
{
    public:
        BackgroundModel()
        : the_count(0)
        {
        }

        BackgroundModel(const int width, const int height, const int pixel_size)
        : the_count(0),
          mean_image(width, height, pixel_size),
          vars_image(width, height, pixel_size),
          sum_image(width, height, pixel_size),
          sum_image2(width, height, pixel_size)
        {
        }

        BackgroundModel(const AnyImage<T> &first_image)
        : the_count(0),
          mean_image(first_image.width(), first_image.height(), first_image.pixel_size()),
          vars_image(first_image.width(), first_image.height(), first_image.pixel_size()),
          sum_image(first_image.width(), first_image.height(), first_image.pixel_size()),
          sum_image2(first_image.width(), first_image.height(), first_image.pixel_size())
        {
            addImage(first_image);
        }

        void addImage(const AnyImage<T> &new_image)
        {
            the_count++;

            sum_image += new_image;
//            sum_image2 += dotmult(new_image, new_image);

            mean_image = sum_image/static_cast<double>(the_count);
//            vars_image = sum_image2/static_cast<double>(the_count) - dotmult(mean_image, mean_image);
        }

        AnyImage<T> evaluate(const AnyImage<T> &test_image) const
        {
            assert(test_image.width() == mean_image.width());
            assert(test_image.height() == mean_image.height());
            assert(test_image.pixel_size() == mean_image.pixel_size());

            AnyImage<T> diff_rgb_mat = test_image - mean_image;
            AnyImage<T> diff_gray_mat;
            if(test_image.pixel_size() == 1)
                diff_gray_mat = fabs(diff_rgb_mat);
            else
                diff_gray_mat = sum3(fabs(diff_rgb_mat))/static_cast<double>(test_image.pixel_size());

            return diff_gray_mat;
        }

//    private:
        int the_count;

        AnyImage<T> mean_image;
        AnyImage<T> vars_image;

        AnyImage<T> sum_image;
        AnyImage<T> sum_image2;
};

#endif

