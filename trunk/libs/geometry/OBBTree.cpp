#include "CrossPlatform.h"

#include "OBBTree.h"

#include <algorithm>
#include <iostream>

using std::vector;
using std::max;
using std::pair;
using std::cout;
using std::endl;

OBBNode::OBBNode()
{
    parent = 0;
    left = 0;
    right = 0;
    numPoints = 0;
    numTriangles = 0;
}

OBBNode::OBBNode(const std::vector<Vector3D> &vertices, const std::vector<Triangle> &triangles, int maxDepth, double scaling, bool verbose)
{
    parent = 0;
    left = 0;
    right = 0;

    // find all used vertices
    vector<unsigned int> counts(vertices.size(), 0);
    for(unsigned int i = 0; i < triangles.size(); i++)
    {
        counts[triangles[i][0]]++;
        counts[triangles[i][1]]++;
        counts[triangles[i][2]]++;
    }

    vector<Vector3D> points;
    for(unsigned int i = 0; i < vertices.size(); i++)
        if(counts[i] > 0)
            points.push_back(vertices[i]);

    numPoints = points.size();
    numTriangles = triangles.size();
    assert(points.size() >= 3);
    if((points.size() == 3) && (triangles.size() == 1))
    {
        for(unsigned int i = 0; i < 3; i++)
            triangle.push_back(vertices[triangles[0][i]]);
        return;
    }

    hull = ConvexHull3D(points, 1e-5, verbose);
    assert(hull.isWellFormed());
    assert(points.size() > 3);
    obb = OBB(hull, scaling);


    // determine order of axes
    Vector3D extents = obb.getExtents();
    bool checked[3] = {false, false, false};
    vector<unsigned int> orders;

    unsigned longest = 0;
    for(unsigned int i = 0; i < 3; i++)
        if(!checked[i])
            if(extents[i] > extents[longest])
                longest = i;
    orders.push_back(longest);
    checked[longest] = true;

    unsigned longer = 0;
    if(checked[longer])
        longer = 1;
    for(unsigned int i = 0; i < 3; i++)
        if(!checked[i])
            if(extents[i] > extents[longer])
                longer = i;
    orders.push_back(longer);
    checked[longer] = true;

    for(unsigned int i = 0; i < 3; i++)
        if(!checked[i])
        {
            orders.push_back(i);
            break;
        }

    if(verbose)
    {
        cout << "order =";
        for(unsigned int i = 0; i < 3; i++)
            cout << " " << orders[i];
        cout << endl;
    }


    // splits triangles into two groups
    typedef pair<double, unsigned int> Distpair;
    vector<Distpair> distances;
    for(unsigned int i = 0; i < triangles.size(); i++)
    {
        Vector3D centre = (vertices[triangles[i][0]]+vertices[triangles[i][1]]+vertices[triangles[i][2]]) / 3.0;
        distances.push_back(Distpair(centre*obb.getAxis(orders[0]), i));
    }
    sort(distances.begin(), distances.end());

    unsigned int half_triangle_count = distances.size()/2;
    vector<Triangle> leftTriangles, rightTriangles;
    for(unsigned int i = 0; i < triangles.size(); i++)
        if(i < half_triangle_count)
            leftTriangles.push_back(triangles[i]);
        else
            rightTriangles.push_back(triangles[i]);
    if(verbose)
    {
        cout << "left = " << leftTriangles.size() << endl;
        cout << "right = " << rightTriangles.size() << endl;
        cout << endl;
    }

    if(maxDepth != 0)
    {
        left = new OBBNode(vertices, leftTriangles, maxDepth-1, scaling, verbose);
        right = new OBBNode(vertices, rightTriangles, maxDepth-1, scaling, verbose);
        left->parent = this;
        right->parent = this;
    }
}

OBBNode::OBBNode(const OBBNode &other)
{
    parent = 0;
    left = 0;
    right = 0;

    if(this != &other)
        deepCopy(other);
}

OBBNode::~OBBNode()
{
    deallocateMemory();
}

OBBNode& OBBNode::operator=(const OBBNode &other)
{
    if(this != &other)
        deepCopy(other);
    return *this;
}

const OBB& OBBNode::getOBB() const
{
    return obb;
}

const ConvexHull3D& OBBNode::getConvexHull() const
{
    return hull;
}

vector<OBB> OBBNode::getOBBs(unsigned int depth) const
{
    vector<OBB> allOBBs, leftOBBs, rightOBBs;
    if(depth > 0)
    {
        if(left) leftOBBs = left->getOBBs(depth-1);
        if(right) rightOBBs = right->getOBBs(depth-1);
        allOBBs.insert(allOBBs.end(), leftOBBs.begin(), leftOBBs.end());
        allOBBs.insert(allOBBs.end(), rightOBBs.begin(), rightOBBs.end());
    }
    else
        allOBBs.push_back(obb);

    return allOBBs;
}

unsigned int OBBNode::getCurrentDepth() const
{
    unsigned int depth = 0;
    OBBNode *temp = parent;
    while(temp != 0)
    {
        depth++;
        temp = parent->parent;
    }

    return depth;
}

unsigned int OBBNode::getMaxDepth() const
{
    unsigned leftDepth = 0, rightDepth = 0;
    if(left) leftDepth = left->getMaxDepth();
    if(right) rightDepth = right->getMaxDepth();

    if((left != 0) || (right != 0))
        return 1 + max(leftDepth, rightDepth);
    else
        return max(leftDepth, rightDepth);
}

void OBBNode::sync(const Vector3D &position, const Quaternion &orientation)
{
    obb.sync(position, orientation);
    hull.sync(position, orientation);
    if(left) left->sync(position, orientation);
    if(right) right->sync(position, orientation);
}

void OBBNode::print(unsigned int tabLevel) const
{
    if(triangle.size() == 0)
    {
        for(unsigned int i = 0; i < tabLevel; i++) cout << "\t";
        cout << "Processed " << numPoints << " Points." << endl;
        for(unsigned int i = 0; i < tabLevel; i++) cout << "\t";
        cout << "Processed " << numTriangles << " Triangles." << endl;
    }
    else
    {
        for(unsigned int i = 0; i < tabLevel; i++) cout << "\t";
        cout << "A Triangle" << endl;
    }

    if(left) left->print(tabLevel+1);
    if(right) right->print(tabLevel+1);
}

void OBBNode::deallocateMemory()
{
    if(parent)
    {
        if(parent->left == this) parent->left = 0;
        if(parent->right == this) parent->right = 0;
        parent = 0;
    }
    if(left)
    {
        delete left;
        left = 0;
    }
    if(right)
    {
        delete right;
        right = 0;
    }
}

void OBBNode::deepCopy(const OBBNode &other)
{
    if(this == &other)
        return;
    else
        deallocateMemory();

    if(other.left)
    {
        this->left = new OBBNode(*(other.left));
        this->left->parent = this;
    }
    if(other.right)
    {
        this->right = new OBBNode(*(other.right));
        this->right->parent = this;
    }
    this->parent = other.parent;
    this->obb = other.obb;
    this->hull = other.hull;
    this->triangle = other.triangle;
    this->numPoints = other.numPoints;
    this->numTriangles = other.numTriangles;
}
