#include "Object.h"

#include "special.h"
#include "GLUtils.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

void Object::init()
{
    mInfo.indice = NULL;
    mInfo.colour = NULL;
    mInfo.vertice = NULL;
    mInfo.normal = NULL;
    mInfo.vertexCount = 0;
    tInfo.textureID = -1;
    tInfo.textureCoord = NULL;

    move.bind("rotXPos", 'k');
    move.bind("rotXNeg", 'i');
    move.bind("rotYPos", 'j');
    move.bind("rotYNeg", 'l');
    move.bind("rotZPos", 'u');
    move.bind("rotZNeg", 'o');
    move.bind("upward", '\0');
    move.bind("downward", '\0');
    move.bind("forward", 't');
    move.bind("backward", 'g');
    move.bind("leftward", '\0');
    move.bind("rightward", '\0');
}

Object::Object()
{
    init();
}

Object::Object(const Vector3D &position, const Quaternion &orientation)
{
    pInfo.setPosition(position);
    pInfo.setOrientation(orientation);
    init();
}

Object::Object(const Vector3D &position, const Quaternion &orientation, double mass, double restitution, double friction, bool isDynamic, bool movable)
{
    pInfo = GenericShip(1.0, Matrix3x3(), -0.5, -0.5);
    pInfo.setPosition(position);
    pInfo.setOrientation(orientation);
    pInfo.setMass(mass);
    pInfo.turnPower = 100.0;
    pInfo.forwardPower = 100.0;
    init();
}

Object::~Object()
{
    if (mInfo.vertice) delete [] mInfo.vertice;
    if (mInfo.indice) delete [] mInfo.indice;
    if (mInfo.colour) delete [] mInfo.colour;
    if (mInfo.normal) delete [] mInfo.normal;
    if (tInfo.textureCoord) delete [] tInfo.textureCoord;
}

//load AC3D files
void Object::loadAC3D(const char *filename, double alpha)
{
    char line[200];
    vector<Vector3D> colours;
    float dx, dy, dz;

    ifstream in(filename);
    if (!in.good())
    {
        cout << "unable to open file: " << filename << endl;
        exit(1);
    }

    char token[50];
    while (!in.eof())
    {
        in.getline(line, 200);
        sscanf(line, "%s", token);

        if (strcmp(token, "MATERIAL") == 0)	//colours of the model
        {
            float r = 0.0, g = 0.0, b = 0.0;
            sscanf(line, "%s %s %s %f %f %f", token, token, token, &r, &g, &b);
            colours.push_back(Vector3D(r, g, b));
        }
        else if (strcmp(token, "loc") == 0)  	//location offset of the model
        {
            sscanf(line, "%s %f %f %f", token, &dx, &dy, &dz);
        }
        else if (strcmp(token, "numvert") == 0)	//number of vertices in the model
        {
            int vertexCount  = 0;
            sscanf(line, "%s %d", token, &vertexCount);

            mInfo.vertexCount = vertexCount;
            mInfo.vertice = new double[vertexCount * 3];
            mInfo.normal = new double[vertexCount * 3];
            mInfo.colour = new double[vertexCount * 4];
            tInfo.textureCoord = new double[vertexCount * 2];

            double *vertice = mInfo.vertice;

            for (int i = 0; i < vertexCount; i++)
            {
                float x, y, z;
                in.getline(line, 200);
                sscanf(line, "%f %f %f", &x, &y, &z);

                vertice[i * 3 + 0] = x + dx;
                vertice[i * 3 + 1] = y + dy;
                vertice[i * 3 + 2] = z + dz;
            }
        }
        else if (strcmp(token, "numsurf") == 0)	//number of surfaces in the models, assume all triangles
        {
            int numberOfSurface  = 0;
            sscanf(line, "%s %d", token, &numberOfSurface);

            mInfo.triangleCount = numberOfSurface;
            mInfo.indice = new int[mInfo.triangleCount * 3];

            for (int i = 0; i < mInfo.triangleCount; i++)
            {
                int index, colourIndex;
                float u, v;
                double *colour = mInfo.colour;
                int	*indice = mInfo.indice;

                in.getline(line, 200);      //ignore the line read here

                in.getline(line, 200);
                sscanf(line, "%s %d", token, &colourIndex);

                in.getline(line, 200);      //ignore the line read here

                for (int j = 0; j < 3; j++)	//for each vertex in each triangle, read in the colour, indice into the vertex array and texture coordinate
                {
                    in.getline(line, 200);
                    sscanf(line, "%d %f %f", &index, &u, &v);

                    indice[i * 3 + j] = index;
                    colour[index * 4 + 0] = colours[colourIndex][0];
                    colour[index * 4 + 1] = colours[colourIndex][1];
                    colour[index * 4 + 2] = colours[colourIndex][2];
                    colour[index * 4 + 3] = alpha;
                    tInfo.textureCoord[index * 2 + 0] = u;
                    tInfo.textureCoord[index * 2 + 1] = v;
                }
            }
        }
    }

    Vector3D *normalT = new Vector3D[mInfo.vertexCount];
    double *vertice = mInfo.vertice;
    double *normal = mInfo.normal;
    int	*indice = mInfo.indice;
    //calculate vertex normals by averaging the normal of all surfaces the vertex is in
    for (int i = 0; i < mInfo.triangleCount; i++)
    {
        Vector3D v1(vertice[indice[i * 3 + 0] * 3 + 0], vertice[indice[i * 3 + 0] * 3 + 1], vertice[indice[i * 3 + 0] * 3 + 2]);
        Vector3D v2(vertice[indice[i * 3 + 1] * 3 + 0], vertice[indice[i * 3 + 1] * 3 + 1], vertice[indice[i * 3 + 1] * 3 + 2]);
        Vector3D v3(vertice[indice[i * 3 + 2] * 3 + 0], vertice[indice[i * 3 + 2] * 3 + 1], vertice[indice[i * 3 + 2] * 3 + 2]);
        Vector3D normalV = Vector3D::normal(v1, v2, v3);

        normalT[indice[i * 3 + 0]] += normalV;
        normalT[indice[i * 3 + 1]] += normalV;
        normalT[indice[i * 3 + 2]] += normalV;
    }
    for (int i = 0; i < mInfo.vertexCount; i++)
    {
        normalT[i].normalise();
        normal[i * 3 + 0] = normalT[i][0];
        normal[i * 3 + 1] = normalT[i][1];
        normal[i * 3 + 2] = normalT[i][2];
    }

    delete [] normalT;

    Quaternion q = Quaternion::makeFromEulerAngles(0.0, pi / 2.0, 0.0);

    vector<Vector3D> points;
    for(int i = 0; i < mInfo.vertexCount; i++)
        points.push_back(Vector3D(mInfo.vertice[i*3+0], mInfo.vertice[i*3+1], mInfo.vertice[i*3+2]));

    vector<Triangle> triangles;
    for(int i = 0; i < mInfo.triangleCount; i++)
        triangles.push_back(Triangle(mInfo.indice[i*3+0], mInfo.indice[i*3+1], mInfo.indice[i*3+2]));

    double time = getTime();
    tree = OBBNode(points, triangles, -1, 1.0, true);
    cout << "ConvexHull and OBB elapsed " << getTime() - time << " seconds" << endl;
    tree.print(0);

    cout << "succssfully loaded " << filename << endl << endl;
}

void Object::setTextureID(int textureID)
{
    tInfo.textureID = textureID;
}

void Object::update(unsigned char key, bool flag)
{
    move.set(key, flag);

    switch (key)
    {
    case 'r':
        pInfo.setPosition(Vector3D());;
        pInfo.setMomentum(Vector3D());
        pInfo.setAngularMomentum(Vector3D());
        pInfo.setForce(Vector3D());
        pInfo.setTorque(Vector3D());
    default:
        break;
    }
}

Vector3D Object::updateView(double deltaTime)
{
    if (move.rotXPos) pInfo.rotation += Vector3D(1.0, 0.0, 0.0);
    if (move.rotXNeg) pInfo.rotation += Vector3D(-1.0, 0.0, 0.0);
    if (move.rotYPos) pInfo.rotation += Vector3D(0.0, 1.0, 0.0);
    if (move.rotYNeg) pInfo.rotation += Vector3D(0.0, -1.0, 0.0);
    if (move.rotZPos) pInfo.rotation += Vector3D(0.0, 0.0, 1.0);
    if (move.rotZNeg) pInfo.rotation += Vector3D(0.0, 0.0, -1.0);

    if (move.forward) pInfo.movement += Vector3D(0.0, 0.0, 1.0);
    if (move.backward) pInfo.movement += Vector3D(0.0, 0.0, -1.0);
    if (move.upward) pInfo.movement += Vector3D(0.0, 1.0, 0.0);
    if (move.downward) pInfo.movement += Vector3D(0.0, -1.0, .0);
    if (move.leftward) pInfo.movement += Vector3D(1.0, 0.0, 0.0);
    if (move.rightward) pInfo.movement += Vector3D(-1.0, 0.0, 0.0);

    pInfo.step(0.0, deltaTime);
    tree.sync(pInfo.getPosition(), pInfo.getOrientation());

    return Vector3D();
}

//object is drawn using vertex array for speed
void Object::draw(bool drawHull, unsigned int obbLevel) const
{
    if (mInfo.vertice && mInfo.colour && mInfo.normal)
    {
        glPushMatrix();
        glTranslate(pInfo.getPosition());
        glRotate(pInfo.getOrientation());

        glVertexPointer(3, GL_DOUBLE, 0, mInfo.vertice);
        glColorPointer(4, GL_DOUBLE, 0, mInfo.colour);
        glNormalPointer(GL_DOUBLE, 0, mInfo.normal);
        if ((tInfo.textureID >= 0) && (tInfo.textureCoord))
        {//if texture is present and the model has texture coord info
            glEnable(GL_TEXTURE_2D);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindTexture(GL_TEXTURE_2D, tInfo.textureID);
            glTexCoordPointer(2, GL_DOUBLE, 0, tInfo.textureCoord);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        glDrawElements(GL_TRIANGLES, mInfo.triangleCount * 3, GL_UNSIGNED_INT, mInfo.indice);

        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(1.0, 0.0, 0.0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 1.0, 0.0);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 1.0);
        glEnd();
        glEnable(GL_LIGHTING);

        if(drawHull) tree.getConvexHull().draw();

        glPopMatrix();

        unsigned int maxDepth =  tree.getMaxDepth();
        vector<OBB> obbs = tree.getOBBs(obbLevel%maxDepth);
        for(unsigned int i = 0; i < obbs.size(); i++)
            drawOBB(obbs[i]);

        glDisable(GL_LIGHTING);
        glPushMatrix();

        glTranslate(tree.getOBB().getCentre());

        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex(tree.getOBB().getAxis(0)*10.0);

        glColor3f(0.0, 1.0, 0.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex(tree.getOBB().getAxis(1)*10.0);

        glColor3f(0.0, 0.0, 1.0);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex(tree.getOBB().getAxis(2)*10.0);
        glEnd();

        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}

void Object::drawOBB(const OBB &obb) const
{
    Vector3D extents = obb.getExtents();

    glDisable(GL_LIGHTING);
    glPushMatrix();

    glTranslate(obb.getCentre());
    glRotate(obb.getOrientation());

    glColor(obbColour);
    glBegin(GL_LINE_LOOP);
    glVertex3d(-extents[0],  extents[1],  extents[2]);
    glVertex3d(-extents[0], -extents[1],  extents[2]);
    glVertex3d( extents[0], -extents[1],  extents[2]);
    glVertex3d( extents[0],  extents[1],  extents[2]);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3d(-extents[0],  extents[1], -extents[2]);
    glVertex3d(-extents[0], -extents[1], -extents[2]);
    glVertex3d( extents[0], -extents[1], -extents[2]);
    glVertex3d( extents[0],  extents[1], -extents[2]);
    glEnd();

    glBegin(GL_LINES);
    glVertex3d(-extents[0],  extents[1],  extents[2]);
    glVertex3d(-extents[0],  extents[1], -extents[2]);

    glVertex3d(-extents[0], -extents[1],  extents[2]);
    glVertex3d(-extents[0], -extents[1], -extents[2]);

    glVertex3d( extents[0], -extents[1],  extents[2]);
    glVertex3d( extents[0], -extents[1], -extents[2]);

    glVertex3d( extents[0],  extents[1],  extents[2]);
    glVertex3d( extents[0],  extents[1], -extents[2]);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}
