#ifndef POLYTOPE_H
#define POLYTOPE_H

#include "Vector3D.h"

#include <vector>
#include <GL/gl.h>

class Triangle
{
    public:
        Triangle();
        Triangle(unsigned int a, unsigned int b, unsigned int c);

        int& operator[](unsigned int i);
        const int& operator[](unsigned int i) const;

    private:
        int inds[3];
};

class Vertex
{
    public:
        int index;
        Vector3D position;
        // flag variable
        bool marked;

        Vertex();
        Vertex(const Vector3D &position, int index);
        virtual ~Vertex();
};

class Facet; //earlier declaration of Facet class for Edge class

class Edge  //possibly add static list of dynamically allocated pointer to edges
{
    public:
        // derived variable
        Vector3D direction;
        double length;

        // external pointers
        Vertex *start, *end;
        Facet *facet;
        Edge *twin, *next, *prev;
        // flag variable
        bool marked;

        Edge();
        Edge(Vertex *start, Vertex *end, Facet *facets);
        Edge(const Edge &other);
        virtual ~Edge();

        Edge& operator=(const Edge &other);

        void calculateDerivedStates();
        bool connect(Edge *edge);
        bool onHorizon() const;
        bool findHorizon(std::vector<Edge *> &horizonEdges);  //true if the return horizon edges is a complete loop and nothing went wrong
        bool match(const Vertex *const start, const Vertex *const end) const;
        bool isTwin(const Edge *const other) const;

        Vector3D pointOnLine(const double t) const;
        double projectToLine(const Vector3D &point) const;
        Vector3D nearestPointOnLine(const Vector3D &point) const;
        double distanceToLine(const Vector3D &point) const;
        //double distanceToEdge(const Vector3D &point) const;

    private:
        void deepCopy(const Edge &other);
};

class Facet
{
    public:
        int index;
        // derived variable
        Vector3D centre;
        Vector3D normal;
        double distance;
        double area;
        // external pointers
        Vertex *vertices[3];
        // internal pointers
        Edge *edges[3];
        // flag variables
        bool marked;
        bool wellFormed;
        // outside set
        std::vector<Vertex*> outsideSet;

        Facet();
        Facet(Vertex *a, Vertex *b, Vertex *c, int index);
        Facet(Vertex *a, Vertex *b, Vertex *c, const Vector3D &behind, int index);
        Facet(const Facet &other);
        virtual ~Facet();

        Facet& operator=(const Facet &other);

        void calculateDerivedStates();
        bool connect(Facet *adjacent, const Vertex *const a, Vertex *const b);
        bool connect(Edge *const edge);
        bool findVisibleFacets(const Vector3D &point, std::vector<Facet *> &visibleFacets, double tolerance);
        Edge* getHorizonEdge() const;
        Edge* getMatchingEdge(const Vertex *const start, const Vertex *const end) const;

        double distanceToPlane(const Vector3D &point) const;

        double volume(const Vector3D &point) const;
        bool above(const Vector3D &point, double tolerance) const;
        bool below(const Vector3D &point, double tolerance) const;
        void orient(const Vector3D &reference);
        void createEdges();

        // functions that helps implements quickhull
        // update the outside set of each facet
        static void updateOutsideSets(std::vector<Facet*> &facets, std::vector<Vertex*> &points, const double tolerance);
        // get the farthest point in a facet's outside set
        bool getFarthestOutsidePoint(Vertex *&farthestPoint);

    private:
        void deallocateMemory();
        void deepCopy(const Facet &other);
};

//typically the subclass of polytope should define the methods in SupportMappable
class Polytope
{
    public:
        Polytope();
        Polytope(const Polytope &other);
        virtual ~Polytope();

        Polytope& operator=(const Polytope &other);

        unsigned int numVertices() const;
        const Vertex& vertex(unsigned int i) const;

        virtual void draw() const;

    protected:
        std::vector<Vertex *> vertices;
        std::vector<Facet *> facets;

        bool checkConnectivity() const;

    private:
        void deallocateMemory();
        void deepCopy(const Polytope &other);
};

#endif
