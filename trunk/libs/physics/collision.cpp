#include "collision.h"

#include "inertia.h"

#include <cmath>

using std::vector;

void impulseCollision(RigidBody &a, RigidBody &b, const Vector3D &point, const Vector3D &normal, double restitution, double friction)
{
    Vector3D rA = point - a.getPosition();
    Vector3D rB = point - b.getPosition();

    Matrix3x3 igiA = getGlobalInertia(a.getOrientation(), a.getInverseInertia());
    Matrix3x3 igiB = getGlobalInertia(b.getOrientation(), b.getInverseInertia());

    double inertiaDenomA = normal * ((igiA * (rA ^ normal)) ^ rA);
    double inertiaDenomB = normal * ((igiB * (rB ^ normal)) ^ rB);

    Vector3D vR = a.getVelocity() + ((a.getOrientation()).rotate(a.getAngularVelocity()) ^ rA) - b.getVelocity() - ((b.getOrientation()).rotate(b.getAngularVelocity()) ^ rB);
    double vRN = normal * vR;

    double f = -(1.0 + restitution) * vRN / (a.getInverseMass() + b.getInverseMass() + inertiaDenomA + inertiaDenomB);
    /*  NEED TO ADD FRICTION HERE
    Vector tangent = (normal ^ vR) ^ normal;
    tangent.normalise();
    double friction
    */
    Vector3D impulse = f * normal;

    a.applyGlobalImpulse(impulse, rA);
    b.applyGlobalImpulse(-impulse, rB);
}

void impulseCollisionB(RigidBody &a, RigidBody &b, const Vector3D &point, const Vector3D &normal, double restitution, double friction)
{
    Vector3D rA = point - a.getPosition();
    Vector3D rB = point - b.getPosition();

    Vector3D cA = rA ^ normal;
    Vector3D cB = rB ^ normal;

    Vector3D uA = getGlobalInertia(a.getOrientation(), a.getInverseInertia()) * cA;
    Vector3D uB = getGlobalInertia(b.getOrientation(), b.getInverseInertia()) * cB;

    Vector3D wA = a.getOrientation().rotate(a.getAngularVelocity());
    Vector3D wB = b.getOrientation().rotate(b.getAngularVelocity());

    double numerator = -(1.0 + restitution) * (normal * (a.getVelocity() - b.getVelocity()) + (wA * cA) - (wB * cB));
    double denominator = a.getInverseMass() + b.getInverseMass() + (cA * uA) + (cB * uB);

    double f = numerator / denominator;
    Vector3D impulse = f * normal;

    a.applyGlobalImpulse(impulse, point);
    b.applyGlobalImpulse(-impulse, point);
}

Vector3D GJK(const SupportMappable &A, const SupportMappable &B, const Vector3D &seed, vector<double> &lambdas, vector<int> &aCoordsIndex, vector<int> &bCoordsIndex)
{
    Simplex simplex;
    int aIndex, bIndex;
    Vector3D v, w;
    bool closeEnough = false;
    double mu = 0.0;
    double vLength = 0.0;

    if ((seed * seed) < 0.00000001)
        SupportMappable::getMinkowskiSupportPoint(w, aIndex, bIndex, A, B, Vector3D(1.0, 0.0, 0.0));
    else
        SupportMappable::getMinkowskiSupportPoint(w, aIndex, bIndex, A, B, seed);

    simplex.addVertex(w, aIndex, bIndex);
    v = simplex.subsimplex();
    vLength = sqrt(v * v);

    while (!closeEnough && (vLength > 0.0000001))
    {
        SupportMappable::getMinkowskiSupportPoint(w, aIndex, bIndex, A, B, -v);
        double v2 = v * v;
        vLength = sqrt(v2);
        double sigma = v * w / vLength;
        if (sigma > mu) mu = sigma;
        closeEnough = ((vLength - mu) <= 0.0000001);

        if (!closeEnough)
        {
            if (!(simplex.addVertex(w, aIndex, bIndex)))
                break;

            v = simplex.subsimplex();
        }
    }

    simplex.getClosestPointsInfo(lambdas, aCoordsIndex, bCoordsIndex);

    return v;
}
