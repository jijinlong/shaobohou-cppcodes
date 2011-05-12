#include "CrossPlatform.h"

#include "OBB.h"

#include <cmath>


const double NEAR_ONE = 0.999999;


OBB::OBB()
{
}

void OBB::updateAxes(const Quaternion &orientation)
{
    axes[0] = orientation.rotate(Vector3D(1.0, 0.0, 0.0));
    axes[1] = orientation.rotate(Vector3D(0.0, 1.0, 0.0));
    axes[2] = orientation.rotate(Vector3D(0.0, 0.0, 1.0));
}

OBB::OBB(const Quaternion &orientationOffset, const Vector3D &centreOffset, const Vector3D &extents)
{
    this->orientationOffset = orientationOffset;
    this->centreOffset = centreOffset;
    this->extents = extents;
}

OBB::OBB(const Vector3D &position, const Quaternion &orientation, const Quaternion &orientationOffset, const Vector3D &centreOffset, const Vector3D &extents)
{
    this->orientationOffset = orientationOffset;
    this->centreOffset = centreOffset;
    this->extents = extents;
    sync(position, orientation);
}

OBB::OBB(const ConvexHull3D &hull, double scaling)
{
    Vector3D eigenvalues;              //eigenvalues of the covariance matrix
    Vector3D eigenvector[3];          //matching eigenvectors, each perpendicular to the other two
    hull.covariance().computeEigenSystem(eigenvalues, eigenvector[0], eigenvector[1], eigenvector[2]);

    //assume eigenvector[0] is +x, eigenvector[1] is +y, but eigenvector[2] may not be +z so, use cross product of eigenvector[0] and eigenvector[1]
    Vector3D v = eigenvector[0] ^ eigenvector[1];
    Matrix3x3 m(eigenvector[0][0], eigenvector[1][0], v[0],
                eigenvector[0][1], eigenvector[1][1], v[1],
                eigenvector[0][2], eigenvector[1][2], v[2]);
    this->orientationOffset = m.makeQuaternion();

    this->axes[0] = orientationOffset.rotate(Vector3D(1.0, 0.0, 0.0));
    this->axes[1] = orientationOffset.rotate(Vector3D(0.0, 1.0, 0.0));
    this->axes[2] = orientationOffset.rotate(Vector3D(0.0, 0.0, 1.0));

    Vector3D min(hull.vertex(0).position*axes[0], hull.vertex(0).position*axes[1], hull.vertex(0).position*axes[2]);
    Vector3D max(hull.vertex(0).position*axes[0], hull.vertex(0).position*axes[1], hull.vertex(0).position*axes[2]);

    for(unsigned int i = 0; i < hull.numVertices(); i++)
        for(int j = 0; j < 3; j++)
        {
            double current = hull.vertex(i).position * axes[j];

            if(current < min[j]) min[j] = current;
            if(current > max[j]) max[j] = current;
        }

    this->centreOffset = (max + min) / 2.0;
    this->extents = (max - min) / 2.0;

    // make sure that centreOffset and orientationOffset are both defined in
    // design space (body space), hence the rotation here
    this->centreOffset = orientationOffset.rotate(centreOffset);
    this->extents *= scaling;
}

OBB::~OBB()
{
}

void OBB::setExtents(const Vector3D &extents)
{
    this->extents = extents;
}

const Quaternion& OBB::getOrientation() const
{
    return orientation;
}

const Vector3D& OBB::getCentre() const
{
    return centre;
}

const Vector3D& OBB::getExtents() const
{
    return extents;
}

const Vector3D& OBB::getCentreOffset() const
{
    return centreOffset;
}

const Vector3D& OBB::getAxis(unsigned int axis) const
{
    assert(axis <= 2);
    return axes[axis];
}

void OBB::sync(const Vector3D &position, const Quaternion &orientation)
{
    this->orientation = orientation * orientationOffset;
    this->orientation.normalise();

//rotate centreOffset by the orientation of RigidBody, as the centreOffset is defined in that coordinate system, as is orientationOffset
    centre = position + orientation.rotate(centreOffset);
    updateAxes(this->orientation); //must be careful not to confuse and use the argument orientation
}


bool OBB::testIntersection(const OBB &obb0, const OBB &obb1)
{
    double c[3][3];       //c[i][j] = obb0.axes[i] . obb1.axes[j]
    double absC[3][3];    //absc[i][j] = fabs(c[i][j]
    double d[3];          //(obb1.centre - obb0.centre) . obb0.axes[i]
    double r, r0, r1;     //interval radii and distance between centres
    bool existsParallelPair = false;

    Vector3D diff = obb1.centre - obb0.centre;

//testinging separation direction as surface normal of obb0
//axis go in loop is in turn axis obb0.centre + t * obb0.axes[i] for  0, 1, 2
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            c[i][j] = (obb0.axes[i]) * (obb1.axes[j]);
            absC[i][j] = fabs(c[i][j]);
            if (absC[i][j] > NEAR_ONE) existsParallelPair = true;
        }

        d[i] = diff * (obb0.axes[i]);
        r = fabs(d[i]);
        r0 = obb0.extents[i];
        r1 = obb1.extents[0] * absC[i][0] + obb1.extents[1] * absC[i][1] + obb1.extents[2] * absC[i][2];

        if (r > (r0 + r1)) return false;
    }

//testinging separation direction as surface normal of obb1
//axis go in loop is in turn axis obb1.centre + t * obb1.axes[i] for  0, 1, 2
    for (int i = 0; i < 3; i++)
    {
        r = fabs(diff * (obb1.axes[i]));
        r0 = obb0.extents[0] * absC[0][i] + obb0.extents[1] * absC[1][i] + obb0.extents[2] * absC[2][i];
        r1 = obb1.extents[i];

        if (r > (r0 + r1)) return false;
    }

//early rejection
    if (existsParallelPair) return true;

//axis obb0.centre + t * (obb0.axes[0] X obb1.axes[0])
    r = fabs(d[2] * c[1][0] - d[1] * c[2][0]);
    r0 = obb0.extents[1] * absC[2][0] + obb0.extents[2] * absC[1][0];
    r1 = obb1.extents[1] * absC[0][2] + obb1.extents[2] * absC[0][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[0] X obb1.axes[1])
    r = fabs(d[2] * c[1][1] - d[1] * c[2][1]);
    r0 = obb0.extents[1] * absC[2][1] + obb0.extents[2] * absC[1][1];
    r1 = obb1.extents[0] * absC[0][2] + obb1.extents[2] * absC[0][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[0] X obb1.axes[2])
    r = fabs(d[2] * c[1][2] - d[1] * c[2][2]);
    r0 = obb0.extents[1] * absC[2][2] + obb0.extents[2] * absC[1][2];
    r1 = obb1.extents[0] * absC[0][1] + obb1.extents[1] * absC[0][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[1] X obb1.axes[0])
    r = fabs(d[0] * c[2][0] - d[2] * c[0][0]);
    r0 = obb0.extents[0] * absC[2][0] + obb0.extents[2] * absC[0][0];
    r1 = obb1.extents[1] * absC[1][2] + obb1.extents[2] * absC[1][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[1] X obb1.axes[1])
    r = fabs(d[0] * c[2][1] - d[2] * c[0][1]);
    r0 = obb0.extents[0] * absC[2][1] + obb0.extents[2] * absC[0][1];
    r1 = obb1.extents[0] * absC[1][2] + obb1.extents[2] * absC[1][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[1] X obb1.axes[2])
    r = fabs(d[0] * c[2][2] - d[2] * c[0][2]);
    r0 = obb0.extents[0] * absC[2][2] + obb0.extents[2] * absC[0][2];
    r1 = obb1.extents[0] * absC[1][1] + obb1.extents[1] * absC[1][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[2] X obb1.axes[0])
    r = fabs(d[1] * c[0][0] - d[0] * c[1][0]);
    r0 = obb0.extents[0] * absC[1][0] + obb0.extents[1] * absC[0][0];
    r1 = obb1.extents[1] * absC[2][2] + obb1.extents[2] * absC[2][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[2] X obb1.axes[1])
    r = fabs(d[1] * c[0][1] - d[0] * c[1][1]);
    r0 = obb0.extents[0] * absC[1][1] + obb0.extents[1] * absC[0][1];
    r1 = obb1.extents[0] * absC[2][2] + obb1.extents[2] * absC[2][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.axes[2] X obb1.axes[2])
    r = fabs(d[1] * c[0][2] - d[0] * c[1][2]);
    r0 = obb0.extents[0] * absC[1][2] + obb0.extents[1] * absC[0][2];
    r1 = obb1.extents[0] * absC[2][1] + obb1.extents[1] * absC[2][0];
    if (r > (r0 + r1)) return false;

    return true;
}
