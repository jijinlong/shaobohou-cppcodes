#ifndef EXPONENTIAL_MAP_H
#define EXPONENTIAL_MAP_H

#include "Vector3D.h"
#include "Quaternion.h"
// #include "LinearAlgebra.h"

Quaternion exp(const Vector3D &v);
Vector3D log(const Quaternion &q);
bool reparameterise(Vector3D &v);

// NRMatrix<double> dRPdQ(const Vector3D &point, const Quaternion &rotation);
// NRMatrix<double> dQdE(const Vector3D &em);

#endif
