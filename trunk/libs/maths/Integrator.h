#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <vector>

//interface for integratable objects
class Integratable
{
    public:
        Integratable();
        virtual ~Integratable();

        virtual std::vector<double> derive(double t, const std::vector<double> &y) const = 0;

        virtual std::vector<double> state2array() const = 0;
        virtual void array2state(const std::vector<double> &y) = 0;

        virtual void revertState() = 0;
};

class Integrator
{
    public:
        Integrator();
        Integrator(unsigned int n);

        // ODE integration functions
        std::vector<double> integrateByEuler(double t, double dt, const Integratable &integ);
        std::vector<double> integrateByMidPoint(double t, double dt, const Integratable &integ);
        std::vector<double> integrateByRungeKutta(double t, double dt, const Integratable &integ);

        unsigned int size() const;
        void resize(unsigned int n);

    private:
        std::vector<double> k1, k2, k3, k4; //intermediate integration dy values
        std::vector<double> dydt;           //derivative dy/dt for y at t
        std::vector<double> yt;             //temporary y values
};

#endif
