#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include "Polytope.h"
#include "GraphicsMaths.h"
#include "SupportMappable.h"

//ConvexHull algo in 3D using Quickhull algorithm
class ConvexHull3D: public Polytope, public SupportMappable
{
    public:
        ConvexHull3D();
        ConvexHull3D(const std::vector<Vector3D> &points);
        ConvexHull3D(const std::vector<Vector3D> &points, double perturb_scale, bool verbose);
        virtual ~ConvexHull3D();

        bool addPointsToHull(const std::vector<Vector3D> &points, double perturb_scale, bool verbose);

        void sync(const Vector3D &position, const Quaternion & orientation);
        const Vector3D& getPosition() const;
        const Quaternion& getOrientation() const;

        //supportmapping
        virtual unsigned int getSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const;
        virtual unsigned int getTransformedSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const;
        virtual unsigned int getSupportFeature(std::vector<Vector3D> &supportfeature, const Vector3D &direction) const;
        virtual unsigned int getTransformedSupportFeature(std::vector<Vector3D> &supportfeature, const Vector3D &direction) const;

        bool isWellFormed() const;
        const Matrix3x3& getCovariance() const;

        virtual void draw() const;

    protected:
        //derived states
        double surface;
        Vector3D centre;            //mean centroid of the convex hull
        Matrix3x3 covariance;       //covariance matrix
        //transformation
        Vector3D position;
        Quaternion orientation;

    private:
        bool setup(std::vector<Vector3D> &points);
        bool getVisibleFacets(Facet &startFacet, const Vector3D &point, std::vector<Facet *> &visibleFacets);
        bool getHorizonEdges(std::vector<Facet *> &visibleFacets, std::vector<Edge *> &horizonEdges); //true get horizon edges is successful
        bool remakeHull(const Vector3D &point, std::vector<Edge *> &horizonEdges, const std::vector<Facet *> &visibleFacets);  //true if remake was successful
        void compactFacets();
        void compactVertices();

        void computeDerivedStates();
};


//ConvexHull in 2D, using Graham's Scan
class ConvexHull2D
{
    public:
        ConvexHull2D();
        ConvexHull2D(const std::vector<Vector3D> &points);
        ConvexHull2D(const ConvexHull2D &other);
        virtual ~ConvexHull2D();

        ConvexHull2D& operator=(const ConvexHull2D &other);

        const std::vector<Vertex *> &getVertices() const;

    private:
        std::vector<Vertex *> vertices;
        std::vector<double> polarAngles;

        void computePolarAngles();
        void polarAngleQuickSort(int begin, int end);
        void grahamScan();

        void deallocateMemory();
        void deepCopy(const ConvexHull2D &other);
};

#endif