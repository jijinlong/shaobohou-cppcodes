#ifndef DRAGGABLE_INTERFACE_H
#define DRAGGABLE_INTERFACE_H

#include "Vector3D.h"
#include "GraphicsMaths.h"

#include <vector>
#include <iostream>
#include <GL/glu.h>
#include <cassert>

class DraggableHandle
{
    public:
        DraggableHandle();
        DraggableHandle(const Sphere &sphere, const Vector3D &colour);
        ~DraggableHandle();

        void setPosition(const Vector3D &newPos);
        void updatePosition(const Vector3D &diffPos);
        const Vector3D& getPosition() const;
        const Vector3D& getColour() const;
        bool intersect(double &t, const Ray &ray) const;
        void setActive(bool active);

        void draw(double alpha);

    protected:
        Sphere sphere;
        Vector3D colour;
        bool isActive;

        GLUquadric *sphereQuadric;
};

class DraggableInterface : public std::vector<DraggableHandle>
{
    public:
        Vector3D hitPosition, newPosition;

        DraggableInterface();
        ~DraggableInterface();

        int getCurrentHandle() const;
        void unselect();
        double selectObject(const Ray &ray);

        void draw();

    protected:
        int currentHandle;
};

#endif
