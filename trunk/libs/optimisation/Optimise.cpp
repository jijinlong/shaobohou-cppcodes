#include "Optimise.h"

using std::ostream;
using std::endl;

double epsilon = 1.0e-16;

//based on netlab scg implementation
Array1D<double> Optimisation::SCG(const Optimisation &optimisation, const Array1D<double> &parameters, const OptimisationOptions &options, ostream &out)
{
    Array1D<double> x = parameters;
    double sigma0 = 0.0001;
    double fold = optimisation.computeObjective(x);
    double fnow = fold;
    Array1D<double> gradNew = optimisation.computeGradient(x);
    Array1D<double> gradOld = gradNew;
    Array1D<double> direction = -gradNew;

    bool success = true;
    int nsuccess = 0;
    double beta = 1.0;
    double betaMin = 1.0e-15;
    double betaMax = 1.0e100;

    //main optimisation loop
    double mu = 0.0;
    double kappa = 0.0;
    double sigma = 0.0;
    Array1D<double> xplus;
    Array1D<double> gplus;
    double theta = 0.0;
    double delta = 0.0;
    double alpha = 0.0;
    Array1D<double> xnew;
    double fnew = 0.0;
    double Delta = 0.0;
    double gamma = 0.0;
    for (unsigned int i = 0; i < options.numberOfIterations; i++)
    {
        //calculate first and second directional derivarive
        if (success)
        {
            mu = dot(direction, gradNew);
            if (mu >= 0)
            {
                direction = -gradNew;
                mu = dot(direction, gradNew);
            }

            kappa = dot(direction, direction);
            if (kappa < epsilon)
            {
                if (options.printInfo)
                    out << "Terminating because kappa is less than " << epsilon << endl;
                return x;
            }

            sigma = sigma0 / sqrt(kappa);
            xplus = x + (sigma * direction);
            gplus = optimisation.computeGradient(xplus);
            theta = dot(direction, gplus-gradNew) / sigma;
        }

        // Increase effective curvature and evaluate step size alpha.
        delta = theta + beta * kappa;
        if (delta <= 0)
        {
            delta = beta * kappa;
            beta = beta - (theta / kappa);
        }
        alpha = -mu / delta;

        // Calculate the comparison ratio.
        xnew = x + alpha * direction;
        fnew = optimisation.computeObjective(xnew);
        Delta = 2.0 * (fnew - fold) / (alpha * mu);

        if (Delta  >= 0)
        {
            success = true;
            nsuccess += 1;
            x = xnew;
            fnow = fnew;
        }
        else
        {
            success = false;
            fnow = fold;
        }

        if (options.printInfo)
            out << "Cycle " << i << " Error " << fnow << " Scales " << beta << " ----------------------------- " << endl;

        if (success)
        {// Test for termination
            if ((max(fabs(alpha * direction)) < options.parametersPrecision) && (fabs(fnew - fold) < options.objectivePrecision))
            {
                if (options.printInfo)
                    out << "Terminating because both change in x and change in objective value are within their respective precision. " << endl;
                return x;
            }
            else
            {
                // Update variables for new position
                fold = fnew;
                gradOld = gradNew;
                gradNew = optimisation.computeGradient(x);
                // If the gradient is zero then we are done.
                if ((dot(gradNew, gradNew)) < epsilon)
                {
                    if (options.printInfo)
                        out << "Terminating because gradient squared magnitude is less than " << epsilon << endl;
                    return x;
                }
            }
        }

        // Adjust beta according to comparison ratio.
        if (Delta < 0.25)
        {
            if ((4.0 * beta) < betaMax)
                beta = 4.0 * beta;
            else
                beta = betaMax;
        }
        if (Delta > 0.75)
        {
            if ((0.5 * beta) > betaMin)
                beta = 0.5 * beta;
            else
                beta = betaMin;
        }

        // Update search direction using Polak-Ribiere formula, or re-start
        // in direction of negative gradient after nparams steps.
        if (nsuccess == parameters.dim())
        {
            direction = -gradNew;
            nsuccess = 0;
        }
        else
        {
            if (success)
            {
                gamma = dot(gradOld-gradNew, gradNew) / mu;
                direction = gamma * direction - gradNew;
            }
        }
    }

    if (options.printInfo)
        out << "Maximum number of iterations exceeded" << endl;

    return x;
}

//Optimisation implementation
Optimisation::Optimisation()
{
}

Optimisation::~Optimisation()
{
}


//OptimiseOptions implmentations
OptimisationOptions::OptimisationOptions()
{
    numberOfIterations = 100;
    parametersPrecision = 0.0001;
    objectivePrecision = 0.0001;
    printInfo = false;
}

OptimisationOptions::OptimisationOptions(unsigned int numberOfIterations, double parametersPrecision, double objectivePrecision, bool printInfo)
{
    this->numberOfIterations = numberOfIterations;
    this->parametersPrecision = parametersPrecision;
    this->objectivePrecision = objectivePrecision;
    this->printInfo = printInfo;
}

OptimisationOptions::~OptimisationOptions()
{
}
