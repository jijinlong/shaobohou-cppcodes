#ifndef QUADRIC_H
#define QUADRIC_H

#include "Conic.h"

#include <string>

class Quadric
{
    public:
        Quadric();
        Quadric(const Array2D<double> &Q, const std::string &type);
        virtual ~Quadric();

        static Quadric createEllipsoid(const double &width, const double &height, const double &depth);

        const Array2D<double>& Q() const;
        const std::string& type() const;

        Quadric transform(const Array2D<double> &H) const;

        Conic computeConic() const;
        Conic computeConic(const Array2D<double> &H) const;

    private:
        Array2D<double> m_Q;
        std::string m_type;
};

#endif
