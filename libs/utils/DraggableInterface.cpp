#include "CrossPlatform.h"

#include "DraggableInterface.h"

//DraggableHandle implementation
DraggableHandle::DraggableHandle()
{
    isActive = false;
}

DraggableHandle::DraggableHandle(const Sphere &sphere, const Vector3D &colour)
{
    this->sphere = sphere;
    this->colour = colour;
    isActive = false;
}

DraggableHandle::~DraggableHandle()
{}

void DraggableHandle::setPosition(const Vector3D &newPos)
{
    sphere.position = newPos;
}

void DraggableHandle::updatePosition(const Vector3D &diffPos)
{
    sphere.position += diffPos;
}

const Vector3D& DraggableHandle::getPosition() const
{
    return sphere.position;
}

const Vector3D& DraggableHandle::getColour() const
{
    return colour;
}

bool DraggableHandle::intersect(double &t, const Ray &ray) const
{
    if(isActive)
        return intersectRaySphere(ray, sphere, t);
    else
        return false;
}

void DraggableHandle::setActive(bool active)
{
    isActive = active;
}

void DraggableHandle::draw(double alpha)
{
    if(isActive)
    {
        sphereQuadric = gluNewQuadric();
        gluQuadricDrawStyle(sphereQuadric, GLU_FILL);
        glColor4d(colour[0], colour[1], colour[2], alpha);
        gluSphere(sphereQuadric, sphere.radius, 10, 10);
        gluDeleteQuadric(sphereQuadric);
    }
}

// DraggableInterface implementations

DraggableInterface::DraggableInterface()
{
    currentHandle = -1;
}

DraggableInterface::~DraggableInterface()
{}

int DraggableInterface::getCurrentHandle() const
{
    return currentHandle;
}

void DraggableInterface::unselect()
{
    currentHandle = -1;
}

double DraggableInterface::selectObject(const Ray &ray)
{
    currentHandle = -1;
    double bestT = -1.0;
    for(unsigned int i = 0; i < size(); i++)
    {
        double t;
        if(at(i).intersect(t, ray))
            if((bestT < 1.0) || (t < bestT))
            {
                currentHandle = i;
                bestT = t;
            }
    }

    return bestT;
}

void DraggableInterface::draw()
{
    for (unsigned int i = 0; i < size(); i++)
    {
        const Vector3D &pos  = at(i).getPosition();
        glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2]);
            at(i).draw(1.0);
        glPopMatrix();
    }

    if (currentHandle >= 0)
    {
        //const Vector3D &colour = at(currentHandle).getColour();

        glPushMatrix();
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(hitPosition[0], hitPosition[1], hitPosition[2]);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(newPosition[0], newPosition[1], newPosition[2]);
        glEnd();

        Vector3D diffPosition = newPosition - hitPosition;
        DraggableHandle tempHandle = at(currentHandle);
        tempHandle.updatePosition(diffPosition);

        const Vector3D &tempPosition = tempHandle.getPosition();
        glPushMatrix();
        glTranslatef(tempPosition[0], tempPosition[1], tempPosition[2]);
        tempHandle.draw(0.5);
        glPopMatrix();
    }
}
