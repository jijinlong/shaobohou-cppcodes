#ifndef OPTIMISE_H
#define OPTIMISE_H

#include <ostream>

#include "LinearAlgebra.h"

class OptimisationOptions
{
    public:
        unsigned int numberOfIterations;
        double parametersPrecision;
        double objectivePrecision;
        bool printInfo;

        OptimisationOptions();
        OptimisationOptions(unsigned int numberOfIterations, double parameterPrecision, double objectivePrecision, bool printInfo);
        virtual ~OptimisationOptions();
};

class Optimisation
{
    public:
        Optimisation();
        virtual ~Optimisation();

        virtual double computeObjective(const Array1D<double> &parameters) const = 0;
        virtual Array1D<double> computeGradient(const Array1D<double> &parameters) const = 0;

    protected:
        static Array1D<double> SCG(const Optimisation &optimisation, const Array1D<double> &parameters, const OptimisationOptions &options, std::ostream& out);
};

#endif
