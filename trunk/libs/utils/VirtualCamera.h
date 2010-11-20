#ifndef VIRTUAL_CAMERA_H
#define VIRTUAL_CAMERA_H

#include "Vector2D.h"
#include "Vector3D.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "LinearAlgebra.h"

#include <vector>

class VirtualCamera
{
    public:
        VirtualCamera();
        VirtualCamera(const std::string &filename);

        void setCamera2WorldTransformation(const Vector3D &position, const Quaternion &orientation);
        void setWorld2CameraTransformation(const Vector3D &position, const Quaternion &orientation);

        Vector3D camera2world_position() const;
        Quaternion camera2world_orientation() const;

        int width() const;
        int height() const;

        Vector3D world2camera(const Vector3D &world_point) const;
        Vector3D camera2world(const Vector3D &camera_point) const;

        std::vector<Vector3D> world2camera(const std::vector<Vector3D> &world_points) const;

        Vector3D project(const Vector3D &pt3D) const;
        Vector2D projectWithoutDistortion(const Vector3D &pt3D) const;
        Vector2D projectWithoutDistortionAndInvertX(const Vector3D &pt3d) const;
        Vector2D projectWithDistortion(const Vector3D &pt3D) const;

        std::vector<Vector2D> projectWithoutDistortion(const std::vector<Vector3D> &points) const;

        const Array2D<double>& getInverseTransformation() const;

        bool load(const std::string &filename);

    private:
	    unsigned int m_width, m_height;

        // essential variables
	    Vector3D world2camera_position;
        Quaternion world2camera_orientation;
	    Matrix3x3 world2camera_matrix;

	    Vector2D focal_length;
	    Vector2D principal_point;
	    double k1, k2, k3, k4, k5;

        // auxilliary variables
	    Matrix3x3 K_KR;
	    Vector3D K_KT;

        Array2D<double> K;
        Array2D<double> T, invT;   // T here means transformation, not just translation


        void updateK();
        void updateT();
};

#endif
