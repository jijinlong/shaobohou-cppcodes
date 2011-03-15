#include "Integrator.h"

#include <cassert>

using std::vector;

Integratable::Integratable()
{
}

Integratable::~Integratable()
{
}

Integrator::Integrator()
{
}

Integrator::Integrator(unsigned int n)
:k1(n), k2(n), k3(n), k4(n), dydt(n), yt(n)
{
}

//Euler's Integration
//dy = dt * f(t, y)
vector<double> Integrator::integrateByEuler(double t, double dt, const Integratable &integ)
{
    vector<double> y = integ.state2array();
    assert(y.size() == size());
    vector<double> dy(size());

    dydt = integ.derive(t, y);
    for (unsigned int i = 0; i < size(); i++) dy[i] = dydt[i] * dt;

    return dy;
}

//MidPoint or RK2 Integration
//k1 = dt * f(t, y)
//dy = dt * f(t + dt / 2.0, y + k1 / 2.0)
vector<double> Integrator::integrateByMidPoint(double t, double dt, const Integratable &integ)
{
    vector<double> y = integ.state2array();
    assert(y.size() == size());
    vector<double> dy(size());

    dydt = integ.derive(t, y);
    for (unsigned int i = 0; i < size(); i++) k1[i] = dt * dydt[i];

    for (unsigned int i = 0; i < size(); i++) yt[i] = y[i] + k1[i]/2.0;
    dydt = integ.derive(t + dt/2.0, yt);
    for (unsigned int i = 0; i < size(); i++) dy[i] = dt * dydt[i];

    return dy;
}

//Runge-Kutta Integration
//k1 = dt * f(t, y)
//k2 = dt * f(t + dt / 2.0, y + k1 / 2.0)
//k3 = dt * f(t + dt / 2.0, y + k2 / 2.0)
//k4 = dt * f(t + dt, y + k3)
//dy = (k1 + k2 * 2.0 + k3 * 2.0 + k4) / 6.0
vector<double> Integrator::integrateByRungeKutta(double t, double dt, const Integratable &integ)
{
    vector<double> y = integ.state2array();
    assert(y.size() == size());
    vector<double> dy(size());

    dydt = integ.derive(t, y);
    for (unsigned int i = 0; i < y.size(); i++) k1[i] = dt * dydt[i];

    for (unsigned int i = 0; i < y.size(); i++) yt[i] = y[i] + k1[i]/2.0;
    dydt = integ.derive(t + dt/2.0, yt);
    for (unsigned int i = 0; i < y.size(); i++) k2[i] = dt * dydt[i];

    for (unsigned int i = 0; i < y.size(); i++) yt[i] = y[i] + k2[i]/2.0;
    dydt = integ.derive(t + dt/2.0, yt);
    for (unsigned int i = 0; i < y.size(); i++) k3[i] = dt * dydt[i];

    for (unsigned int i = 0; i < y.size(); i++) yt[i] = y[i] + k3[i];
    dydt = integ.derive(t + dt, yt);
    for (unsigned int i = 0; i < y.size(); i++) k4[i] = dt * dydt[i];

    for (unsigned int i = 0; i < y.size(); i++) dy[i] = (k1[i] + (k2[i]*2.0) + (k3[i]*2.0) + k4[i]) / 6.0;

    return dy;
}

unsigned int Integrator::size() const
{
    return dydt.size();
}

void Integrator::resize(unsigned int n)
{
    k1.resize(n);
    k2.resize(n);
    k3.resize(n);
    k4.resize(n);
    dydt.resize(n);
    yt.resize(n);
}
