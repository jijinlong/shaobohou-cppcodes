#ifndef OBJECT_H
#define OBJECT_H

#include <fstream>
#include <cstdio>
#include <vector>
#include <GL/gl.h>
#include "Vector3D.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "OBBTree.h"
#include "GraphicsMaths.h"
#include "GenericShip.h"
#include "MiscUtils.h"
#include "Movement.h"

#define ON true
#define OFF false

struct ModelInfo
{
    double *vertice;        //model in object coordinate
    double *normal;         //vertex normal
    double *colour;	        //vertex colour
    int    *indice;	        //triangle data which index into the vertice data
    int    *triangleID;	    //what object does the triangle belong to

    int    vertexCount;
    int    triangleCount;
};

struct TextureInfo
{
    int textureID;
    double *textureCoord;
};

class Object
{
    private:
        void init();

    public:
        GenericShip pInfo;
        ModelInfo   mInfo;
        TextureInfo tInfo;
        Vector3D    obbColour;
        OBBNode     tree;
        Movement    move;

        Object();
        Object(const Vector3D &position, const Quaternion &orientation);
        Object(const Vector3D &position, const Quaternion &orientation, double mass, double restitution, double friction, bool isDynamic, bool movable);
        virtual ~Object();

        void setTextureID(int textureID);	//set the texture to be use for the object

        //load AC3D files, not complete loader as ignores kids information and assume the model is triangulated
        void loadAC3D(const char *filename, double alpha);

        virtual void update(unsigned char key, bool flag);	//set movement flags
        virtual Vector3D updateView(double deltaTime);		//update physical attributes by certain time and return displacement from current position

        virtual void draw(bool drawHull, unsigned int obbLevel) const;
        void drawOBB(const OBB &obb) const;
};

#endif
