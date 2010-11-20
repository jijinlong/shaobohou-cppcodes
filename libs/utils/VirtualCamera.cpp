#include "VirtualCamera.h"

#include <fstream>
#include <cassert>


using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;


VirtualCamera::VirtualCamera()
: m_width(0), m_height(0), k1(0.0), k2(0.0), k3(0.0), k4(0.0), k5(0.0)
{}

VirtualCamera::VirtualCamera(const std::string &filename)
{
    assert(load(filename));
}

void VirtualCamera::setCamera2WorldTransformation(const Vector3D &position, const Quaternion &orientation)
{
    setWorld2CameraTransformation((~orientation).rotate(Vector3D()-position), ~orientation);
}

// #include <iostream>
void VirtualCamera::setWorld2CameraTransformation(const Vector3D &position, const Quaternion &orientation)
{
    world2camera_position = position;
    world2camera_orientation = orientation;

    world2camera_matrix = orientation.makeRotationMatrix();

    updateK();
    updateT();
}

Vector3D VirtualCamera::camera2world_position() const
{
    return camera2world(Vector3D());
}

Quaternion VirtualCamera::camera2world_orientation() const
{
    return ~world2camera_orientation;
}

int VirtualCamera::width() const
{
    return m_width;
}

int VirtualCamera::height() const
{
    return m_height;
}

Vector3D VirtualCamera::world2camera(const Vector3D &world_point) const
{
    return world2camera_position + world2camera_orientation.rotate(world_point);
}

Vector3D VirtualCamera::camera2world(const Vector3D &camera_point) const
{
    return (~world2camera_orientation).rotate(camera_point - world2camera_position);
}

vector<Vector3D> VirtualCamera::world2camera(const std::vector<Vector3D> &world_points) const
{
    vector<Vector3D> camera_points(world_points.size());
    for(unsigned int i = 0; i < world_points.size(); i++)
        camera_points[i] = world_points[i];

    return camera_points;
}

Vector3D VirtualCamera::project(const Vector3D &pt3D) const
{
    return K_KR * pt3D + K_KT;
}

Vector2D VirtualCamera::projectWithoutDistortion(const Vector3D &pt3D) const
{
    Vector3D point = project(pt3D);
    return Vector2D(point[0]/(point[2]+realmin), point[1]/(point[2]+realmin));
}

Vector2D VirtualCamera::projectWithoutDistortionAndInvertX(const Vector3D &pt3D) const
{
    Vector3D point = project(pt3D);
    return Vector2D(width()-point[0]/(point[2]+realmin), point[1]/(point[2]+realmin));
}

// radial distortion first.
// the formula is: (x2,y2) = (x,y) * ( 1 + k1*r2 + k2*r2*r2 )
Vector2D VirtualCamera::projectWithDistortion(const Vector3D &pt3D) const
{
    Vector2D distortedPoint = projectWithoutDistortion(pt3D);

    const Vector2D &pp = principal_point;
    const Vector2D &fl = focal_length;

    double x = (distortedPoint[0] - pp[0]) / fl[0];
    double y = (distortedPoint[1] - pp[1]) / fl[1];

    double r2 = x*x + y*y;
    double r4 = r2 * r2;
    double cdist = 1.0 + (k1 * r2) + (k2 * r4);

    double x2 = x*cdist;
    double y2 = y*cdist;

    // tangential distortion now...
    double a1 = 2.0 * x * y;
    double a2 = r2 + 2.0 * x * x;
    double a3 = r2 + 2.0 * y * y;
    double deltaX = k3 * a1 + k4 * a2;
    double deltaY = k3 * a3 + k4 * a1;

    x2 += deltaX;
    y2 += deltaY;

    // now, we reproject in the pixel coordinates (u2, v2)
    Vector2D undistortedPoint;
    undistortedPoint[0] = (x2 * fl[0]) + pp[0];
    undistortedPoint[1] = (y2 * fl[1]) + pp[1];

    return undistortedPoint;
}

vector<Vector2D> VirtualCamera::projectWithoutDistortion(const vector<Vector3D> &points) const
{
    vector<Vector2D> points2d;
    for(unsigned int i = 0; i < points.size(); i++)
        points2d.push_back(projectWithoutDistortion(points[i]));

    return points2d;
}

const Array2D<double>& VirtualCamera::getInverseTransformation() const
{
    return invT;
}

bool VirtualCamera::load(const string &filename)
{
    ifstream infile(filename.c_str() , ifstream::in);
    if(infile.fail())
    {
        cout << "Failed to load open camera file " << filename << endl;
        return false;
    }

    string label;
    infile >> label >> m_width;
    infile >> label >> m_height;
    infile >> label >> world2camera_position[0] >> world2camera_position[1] >> world2camera_position[2];
    infile >> label >> world2camera_matrix(0, 0) >> world2camera_matrix(0, 1) >> world2camera_matrix(0, 2)
                    >> world2camera_matrix(1, 0) >> world2camera_matrix(1, 1) >> world2camera_matrix(1, 2)
                    >> world2camera_matrix(2, 0) >> world2camera_matrix(2, 1) >> world2camera_matrix(2, 2);
    infile >> label >> focal_length[0] >> focal_length[1];
    infile >> label >> principal_point[0] >> principal_point[1];
    infile >> label >> k1 >> k2 >> k3 >> k4 >> k5;

    infile.close();

    world2camera_orientation = Quaternion::makeFromRotationMatrix(world2camera_matrix);

    setWorld2CameraTransformation(world2camera_position, world2camera_orientation);
//     updateK();

    return true;
}

void VirtualCamera::updateK()
{
    // update K parameters
    K = Array2D<double>(3, 4, 0.0);
    K(0, 0) = focal_length[0];
    K(1, 1) = focal_length[1];
    K(0, 2) = principal_point[0];
    K(1, 2) = principal_point[1];
    K(2, 2) = 1.0;

    // upate K_KR and K_KT
    K_KR = Matrix3x3(focal_length[0], 0.0,            principal_point[0],
                     0.0,            focal_length[1], principal_point[1],
                     0.0,            0.0,            1.0                ) * world2camera_matrix;
    K_KT = Matrix3x3(focal_length[0], 0.0,            principal_point[0],
                     0.0,            focal_length[1], principal_point[1],
                     0.0,            0.0,            1.0                ) * world2camera_position;
}

void VirtualCamera::updateT()
{
    T = eye<double>(4);
    for(unsigned int i = 0; i < 3; i++)
        for(unsigned int j = 0; j < 3; j++)
            T(i, j) = world2camera_matrix(i, j);
    for(unsigned int i = 0; i < 3; i++)
        T(i, 3) = world2camera_position[i];

    // compute invese transformation
    Vector3D RT = world2camera_matrix*world2camera_position;
    invT = eye<double>(4);
    for(unsigned int i = 0; i < 3; i++)
        for(unsigned int j = 0; j < 3; j++)
            invT(i, j) = world2camera_matrix(j, i);
    for(unsigned int i = 0; i < 3; i++)
        invT(i, 3) = -RT[i];
}
