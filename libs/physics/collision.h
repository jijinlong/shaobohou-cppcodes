#ifndef COLLISION_H
#define COLLISION_H

#include <vector>

#include "RigidBody.h"
#include "GraphicsMaths.h"
#include "SupportMappable.h"

//impulse based collision response, based on discontinuous change of state due to a collisioo
void impulseCollision(RigidBody &a, RigidBody &b, const Vector3D &point, const Vector3D &normal, double restitution, double friction);
void impulseCollisionB(RigidBody &a, RigidBody &b, const Vector3D &point, const Vector3D &normal, double restitution, double friction);

//GJK distance algorithm, returns squared closest distance
//seed is initial direction, if 0a default one will be used
//lambdas, aCoordsIndex and bCoordsIndex should be same size
Vector3D GJK(const SupportMappable &A, const SupportMappable &B, const Vector3D &seed, std::vector<double> &lambdas, std::vector<int> &aCoordsIndex, std::vector<int> &bCoordsIndex);

#endif
