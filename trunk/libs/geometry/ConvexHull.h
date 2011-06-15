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
        ConvexHull3D(const std::vector<Vector3D> &points, const double epsilon, const bool verbose=true);
        virtual ~ConvexHull3D();

        bool addPointsToHull(const std::vector<Vector3D> &points, const bool verbose);

        void sync(const Vector3D &newPosition, const Quaternion &newOrientation);
        const Vector3D& position() const;
        const Quaternion& orientation() const;

        bool insideHull(const Vector3D &point, const double tol) const;
        double distance2hull(const Vector3D &point) const;

        //supportmapping
        virtual unsigned int getSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const;
        virtual unsigned int getTransformedSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const;
        virtual unsigned int getSupportFeature(std::vector<Vector3D> &supportfeature, const Vector3D &direction) const;
        virtual unsigned int getTransformedSupportFeature(std::vector<Vector3D> &supportfeature, const Vector3D &direction) const;

        bool isWellFormed() const;
        const Matrix3x3& covariance() const;

        bool isConvex() const;

        virtual void draw() const;

    protected:
        //derived states
        double m_surface;
        Vector3D m_centre;            //mean centroid of the convex hull
        Matrix3x3 m_covariance;       //covariance matrix
        //transformation
        Vector3D m_position;
        Quaternion m_orientation;
        // fudging factor
        double eps;

    private:
        bool setup(std::vector<Vertex*> &points);
        void updateFacet(Facet *queryFacet, std::vector<Vertex*> &nearPoints);
        bool updateFacetOnce(Facet *queryFacet, std::vector<Vertex*> &nearPoints);

        bool getVisibleFacets(Vertex *point, Facet *startFacet, std::vector<Facet *> &visibleFacets);
        bool getHorizonEdges(std::vector<Facet *> &visibleFacets, std::vector<Edge *> &horizonEdges); //true get horizon edges is successful
        bool remakeHull(Vertex *point, std::vector<Edge *> &horizonEdges, const std::vector<Facet *> &visibleFacets, std::vector<Vertex*> &nearPoints);  //true if remake was successful
        
        void compactFacets();
        void compactVertices();

        void computeDerivedStates();
};


//ConvexHull in 2D, using Graham's Scan
class ConvexHull2D
{
    public:
        ConvexHull2D();
        ConvexHull2D(const std::vector<Vector3D> &points, const double epsilon, const bool verbose=true);
        ConvexHull2D(const ConvexHull2D &other);
        virtual ~ConvexHull2D();

        ConvexHull2D& operator=(const ConvexHull2D &other);

        const std::vector<Vertex *> &getVertices() const;

    private:
        std::vector<Vertex *> vertices;
        std::vector<double> polarAngles;
        // fudging factor
        double eps;

        void computePolarAngles();
        void polarAngleQuickSort(int begin, int end);
        void grahamScan();

        void deallocateMemory();
        void deepCopy(const ConvexHull2D &other);
};

#endif
