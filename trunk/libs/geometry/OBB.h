#ifndef OBB_H
#define OBB_H

#include "Vector3D.h"
#include "Quaternion.h"

#include "ConvexHull.h"

class OBB
{
    public:
        OBB();
        OBB(const Quaternion &orientationOffset, const Vector3D &centreOffset, const Vector3D &extents);
        OBB(const Vector3D &position, const Quaternion &orientation, const Quaternion &orientationOffset, const Vector3D &centreOffset, const Vector3D &extents);
        OBB(const ConvexHull3D &hull, double scaling);
        virtual ~OBB();

        void setExtents(const Vector3D &extents);

        const Quaternion& getOrientation() const;
        const Vector3D& getCentre() const;
        const Vector3D& getExtents() const;
        const Vector3D& getCentreOffset() const;
        const Vector3D& getAxis(unsigned int axis) const;  //0 = X, 1 = Y, 2 = Z

        void sync(const Vector3D &position, const Quaternion &orientation);

        static bool testIntersection(const OBB &obb1, const OBB &obb2);

    private:
        Quaternion  orientation,
        orientationOffset;
        Vector3D    centre,
        centreOffset;
        Vector3D    extents;
        Vector3D    axes[3];       //axes, left, up and forward, calculate from orientation

        void updateAxes(const Quaternion &orientation);
};

#endif
