#ifndef OBB_TREE_H
#define OBB_TREE_H

#include "OBB.h"

class OBBNode
{
    public:
        OBBNode();
        OBBNode(const std::vector<Vector3D> &points, const std::vector<Triangle> &triangles, int maxDepth, double scaling, bool verbose);
        OBBNode(const OBBNode &other);
        virtual ~OBBNode();
        
        OBBNode& operator=(const OBBNode &other);

        const OBB& getOBB() const;
        const ConvexHull3D& getConvexHull() const;

        std::vector<OBB> getOBBs(unsigned int depth) const;
        unsigned int getCurrentDepth() const;
        unsigned int getMaxDepth() const;

        void sync(const Vector3D &position, const Quaternion &orientation);
        
        void print(unsigned int tabLevel) const;

    private:
        OBBNode *parent;
        OBBNode *left;
        OBBNode *right;

        OBB obb;
        ConvexHull3D hull;
        std::vector<Vector3D> triangle;
        
        unsigned int numPoints;
        unsigned int numTriangles;

        void deallocateMemory();
        void deepCopy(const OBBNode &other);
};

#endif
