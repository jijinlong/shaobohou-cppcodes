#include "CrossPlatform.h"

#include "Polytope.h"

#include "Constants.h"

using MathConstants::EPSILON;

#include <map>
#include <iostream>
#include <cmath>
#include <cassert>

using std::vector;
using std::map;
using std::swap;
using std::cout;
using std::endl;


//Triangle class
Triangle::Triangle()
{
    inds[0] = -1; inds[1] = -1; inds[2] = -2;
}

Triangle::Triangle(unsigned int a, unsigned int b, unsigned int c)
{
    inds[0] = a; inds[1] = b; inds[2] = c;
}

int& Triangle::operator[](unsigned int i)
{
    assert(i < 3);
    return inds[i];
}

const int& Triangle::operator[](unsigned int i) const
{
    assert(i < 3);
    return inds[i];
}


//Vertex class
Vertex::Vertex()
{
    this->index = -1;
    this->marked = false;
}

Vertex::Vertex(const Vector3D &position, int index)
{
    this->position = position;
    this->index = index;
    this->marked = false;
}

Vertex::~Vertex()
{
}


//Edge class implementation
Edge::Edge()
{
    this->length = 0;

    this->start = 0;
    this->end = 0;
    this->facet = 0;
    this->twin = 0;
    this->next = 0;
    this->prev = 0;
    // flag variable
    this->marked = false;
}

Edge::Edge(Vertex *start, Vertex *end, Facet *facet)
{
    this->start = start;
    this->end = end;
    this->facet = facet;
    this->twin = 0;
    this->next = 0;
    this->prev = 0;
    // flag variable
    this->marked = false;

    calculateDerivedStates();
}

Edge::Edge(const Edge &other)
{
    if(this != &other)
        deepCopy(other);
}

Edge::~Edge()
{
    if(facet)
        for(int i = 0; i < 3; i++)
            if(facet->edges[i] == this) facet->edges[i] = 0;

    if(twin && twin->twin == this) twin->twin = 0;
    if(next && next->prev == this) next->prev = 0;
    if(prev && prev->next == this) prev->next = 0;
}

Edge& Edge::operator=(const Edge &other)
{
    if(this != &other)
        deepCopy(other);
    return *this;
}

void Edge::deepCopy(const Edge &other)
{
    if(this == &other)
        return;

    this->direction = other.direction;
    // external pointers
    this->start = other.start;
    this->end = other.end;
    this->facet = other.facet;
    this->twin = other.twin;
    this->next = other.next;
    this->prev = other.prev;
    // flag variable
    this->marked = false;
}

void Edge::calculateDerivedStates()
{
    this->direction = end->position - start->position;
    this->length = this->direction.magnitude();
    direction.normalise();
}

Vector3D Edge::pointOnLine(const double t) const
{
    return this->direction * t + start->position;
}

double Edge::projectToLine(const Vector3D &point) const
{
    return this->direction * (point - start->position);
}

Vector3D Edge::nearestPointOnLine(const Vector3D &point) const
{
    return pointOnLine(projectToLine(point));
}

double Edge::distanceToLine(const Vector3D &point) const
{
    return Vector3D::distance(point, nearestPointOnLine(point));
}

double Edge::distanceToEdge(const Vector3D &point) const
{
    double t = projectToLine(point);
    return Vector3D::distance(point, pointOnLine(std::max(0.0, std::min(t, 1.0))));
}

bool Edge::connect(Edge *edge)
{
    if(match(edge->start, edge->end))
    {
        if(twin && twin->twin) twin->twin = 0;    //the twin to this edge is no longer connected to this edge
        if(edge->twin && edge->twin->twin) edge->twin->twin = 0; //similar for edge

        twin = edge;
        edge->twin = this;
        return true;
    }
    else
        return false;
}

bool Edge::findHorizon(vector<Edge *> &horizonEdges)
{
    if(onHorizon())
    {
        if(horizonEdges.size() > 0 && this == horizonEdges[0])
            return true;
        else
        {
            horizonEdges.push_back(this);
            return next->findHorizon(horizonEdges);  //look along the edges of the facet
        }
    }
    else
    {
        if(twin != 0)
        {
            if(horizonEdges.size() > 0)
            {
                // find common vertex with previous search
                // take advantage of the facet that it must initialised on an
                // horizon edge or its twin, jump from facets to facet looking for
                // possible horizon edge, easier to explain with a diagram
                return twin->next->findHorizon(horizonEdges);
            }
            else
            {
                std::cout << "No enough horizon edge error" << std::endl;
                return false;
            }
        }
        else
        {
            std::cout << "No twin edge error" << std::endl;
            return false;
        }
    }
}

bool Edge::onHorizon() const
{
    if(twin == 0) //not connected to another facet, anomalous case
        return false;
    else           //if own facet is not marked and the adjacent is
        return (!(facet->marked) && twin->facet->marked);
}

bool Edge::match(const Vertex *const start, const Vertex *const end) const
{
    return ((this->start == start && this->end == end) || (this->start == end && this->end == start));
}

bool Edge::isTwin(const Edge *const other) const
{
    return (this->start == other->end) && (this->end == other->start);
}


//Facet implementation
Facet::Facet()
{
    this->index = -1;
    this->distance = 0.0;
    this->area = 0.0;

    this->vertices[0] = 0;
    this->vertices[1] = 0;
    this->vertices[2] = 0;

    this->edges[0] = 0;
    this->edges[1] = 0;
    this->edges[2] = 0;

    this->marked = false;
    this->wellFormed = false;
}

Facet::Facet(Vertex *a, Vertex *b, Vertex *c, int index)
{
    this->index = index;
    this->distance = 0.0;
    this->area = 0.0;

    this->vertices[0] = a;
    this->vertices[1] = b;
    this->vertices[2] = c;

    this->edges[0] = 0;
    this->edges[1] = 0;
    this->edges[2] = 0;

    this->marked = false;

    calculateDerivedStates();
    createEdges();
}

Facet::Facet(Vertex *a, Vertex *b, Vertex *c, const Vector3D &behind, int index)
{
    this->index = index;
    this->distance = 0.0;
    this->area = 0.0;

    this->vertices[0] = a;
    this->vertices[1] = b;
    this->vertices[2] = c;

    this->edges[0] = 0;
    this->edges[1] = 0;
    this->edges[2] = 0;

    this->marked = false;
    this->wellFormed = false;

    calculateDerivedStates();
    orient(behind);
    createEdges();
}

Facet::Facet(const Facet &other)
{
    this->vertices[0] = 0;
    this->vertices[1] = 0;
    this->vertices[2] = 0;

    this->edges[0] = 0;
    this->edges[1] = 0;
    this->edges[2] = 0;

    if(this != &other)
        deepCopy(other);
}

Facet::~Facet()
{
    deallocateMemory();
}

Facet& Facet::operator=(const Facet &other)
{
    if(this != &other)
        deepCopy(other);
    return *this;
}

void Facet::deallocateMemory()
{
    for(int i = 0; i < 3; i++)
        if(edges[i])
        {
            delete edges[i];
            edges[i] = 0;
        }
}

void Facet::deepCopy(const Facet &other)
{
    if(this == &other)
        return;
    else
        deallocateMemory();

    this->index = other.index;
    // derived variables
    this->centre = other.centre;
    this->normal = other.normal;
    this->distance = other.distance;
    this->area = other.area;
    // external pointers
    this->vertices[0] = other.vertices[0];
    this->vertices[1] = other.vertices[1];
    this->vertices[2] = other.vertices[2];
    // internal pointers
    this->edges[0] = new Edge(*(other.edges[0]));
    this->edges[1] = new Edge(*(other.edges[1]));
    this->edges[2] = new Edge(*(other.edges[2]));
    // flag variables
    this->marked = other.marked;
    this->wellFormed = other.wellFormed;
    // outside set
    this->outsideSet = other.outsideSet;
}

void Facet::calculateDerivedStates()
{
    centre = (vertices[0]->position + vertices[1]->position + vertices[2]->position) / 3.0;
    normal = Vector3D::normal(vertices[0]->position, vertices[1]->position, vertices[2]->position);
    distance = -(normal * vertices[0]->position);
    area = 0.5 * ((vertices[0]->position - vertices[1]->position) ^ (vertices[2]->position - vertices[1]->position)).magnitude();

    if(fabs(normal * normal) < EPSILON)
        wellFormed = false;
    else
        wellFormed = true;
}

double Facet::distanceToPlane(const Vector3D &point) const
{
    return (normal * point) + distance;
}

bool Facet::isBefore(const Vector3D &point, double tolerance) const
{
    return distanceToPlane(point) > tolerance;
}

bool Facet::isBehind(const Vector3D &point, double tolerance) const
{
    return distanceToPlane(point) < -tolerance;
}

bool Facet::connect(Edge *const edge)
{
    Edge *inner = getMatchingEdge(edge->start, edge->end);

    if(inner)
    {
        inner->connect(edge);
        return true;
    }
    else
        return false;
}

bool Facet::connect(Facet *adjacent, const Vertex *const a, Vertex *const b)
{
    Edge *inner = getMatchingEdge(a, b);
    Edge *outter = adjacent->getMatchingEdge(a, b);

    if(inner && outter)
    {
        inner->connect(outter);
        return true;
    }
    else
        return false;
}

bool Facet::findVisibleFacets(const Vector3D &point, vector<Facet *> &visibleFacets, double tolerance)
{
    if(marked)
        return true;
    else if(isBefore(point, tolerance))
    {
        this->marked = true;
        visibleFacets.push_back(this);
        if((edges[0]->twin != 0) && (edges[1]->twin != 0) && (edges[2]->twin != 0))
        {
            return (edges[0]->twin->facet->findVisibleFacets(point, visibleFacets, tolerance) &&
                    edges[1]->twin->facet->findVisibleFacets(point, visibleFacets, tolerance) &&
                    edges[2]->twin->facet->findVisibleFacets(point, visibleFacets, tolerance));
        }
        else
        {
            cout << "Some of the edges of the facet does not have twin!" << endl;
            return false;
        }
    }
    else
        return true;
}

Edge* Facet::getMatchingEdge(const Vertex *const start, const Vertex *const end) const
{
    for(int i = 0; i < 3; i++)
        if(edges[i]->match(start, end) != 0)
            return edges[i];

    return 0;
}

Edge* Facet::getHorizonEdge() const
{
    Edge *opposite;
    for(int i = 0; i < 3; i++)
    {
        opposite = edges[i]->twin;
        assert(opposite != 0);
        if(opposite->onHorizon())
            return opposite;
    }

    return 0;
}

void Facet::orient(const Vector3D &reference)
{
    if(!isBehind(reference, EPSILON))
    {
        swap(vertices[1], vertices[2]);
        normal = -normal;
        distance = -distance;
    }
}

void Facet::createEdges()
{
    edges[0] = new Edge(vertices[0], vertices[1], this);
    edges[1] = new Edge(vertices[1], vertices[2], this);
    edges[2] = new Edge(vertices[2], vertices[0], this);

    edges[0]->next = edges[1];
    edges[0]->prev = edges[2];
    edges[1]->next = edges[2];
    edges[1]->prev = edges[0];
    edges[2]->next = edges[0];
    edges[2]->prev = edges[1];
}

void Facet::updateOutsideSets(std::vector<Facet*> &facets, std::vector<Vector3D> &points, const double tolerance)
{
    int count = 0;
    for(vector<Vector3D>::const_iterator it = points.begin(); it != points.end(); it++)
    {
        // assign as outside point
        bool success = false;
        for(unsigned int f = 0; f < facets.size(); f++)
        {
            if(!facets[f]) continue;
            if( facets[f]->index < 0) continue;

            if(facets[f]->isBefore(*it, tolerance))
            {
                success = true;
                facets[f]->outsideSet.push_back(*it);
                break;
            }
        }
        
        // store as inside point
        if(!success)
        {
            points[count] = *it;
            count++;
        }
    }
    
    // erase all outside points
    points.erase(points.begin()+count, points.end());
}

bool Facet::getFarthestOutsidePoint(Vector3D &farthestPoint) const
{
    if(outsideSet.size() > 0)
    {
        farthestPoint = *outsideSet.begin();
        double farthestDistance = distanceToPlane(farthestPoint);

        for(vector<Vector3D>::const_iterator it = outsideSet.begin(); it != outsideSet.end(); it++)
        {
            double tempDistance = distanceToPlane(*it);
            if(tempDistance > farthestDistance)
            {
                farthestPoint = *it;
                farthestDistance = tempDistance;
            }
        }

        return true;
    }


    return true;
}


//Polyhedron implementation
Polytope::Polytope()
{
}

Polytope::Polytope(const Polytope &other)
{
    if(this != &other)
        deepCopy(other);
}

Polytope::~Polytope()
{
    deallocateMemory();
}

Polytope& Polytope::operator=(const Polytope &other)
{
    if(this != &other)
        deepCopy(other);
    return *this;
}

void Polytope::deallocateMemory()
{
    for(unsigned int i = 0; i < vertices.size(); i++)
        delete vertices[i];
    vertices.clear();

    for(unsigned int i = 0; i < facets.size(); i++)
    {
        delete facets[i]->edges[0];
        delete facets[i]->edges[1];
        delete facets[i]->edges[2];
        delete facets[i];
    }
    facets.clear();
}

void Polytope::deepCopy(const Polytope &other)
{
    if(this == &other)
        return;
    else
        deallocateMemory();

    // allocated new variables
    vertices.clear();
    for(unsigned int i = 0; i < other.vertices.size(); i++)
        vertices.push_back(new Vertex(*(other.vertices[i])));

    facets.clear();
    for(unsigned int i = 0; i < other.facets.size(); i++)
        facets.push_back(new Facet(*(other.facets[i])));

    // compute edge to edge mapping
    map<Edge *, Edge *> edge2edge;
    for(unsigned int i = 0; i < facets.size(); i++)
        for(unsigned int j = 0; j < 3; j++)
            edge2edge[other.facets[i]->edges[j]] = facets[i]->edges[j];

    // setup all external pointers
    for(unsigned int i = 0; i < facets.size(); i++)
        for(unsigned int j = 0; j < 3; j++)
        {
            facets[i]->vertices[j] = vertices[other.facets[i]->vertices[j]->index];
            facets[i]->edges[j] = edge2edge[other.facets[i]->edges[j]];

            // setup external pointers in Edge
            facets[i]->edges[j]->start  = vertices[other.facets[i]->edges[j]->start->index];
            facets[i]->edges[j]->end    = vertices[other.facets[i]->edges[j]->end->index];
            facets[i]->edges[j]->facet  = facets[other.facets[i]->edges[j]->facet->index];
            facets[i]->edges[j]->twin   = edge2edge[other.facets[i]->edges[j]->twin];
            facets[i]->edges[j]->next   = edge2edge[other.facets[i]->edges[j]->next];
            facets[i]->edges[j]->prev   = edge2edge[other.facets[i]->edges[j]->prev];
        }

    bool all_vertices_good = true;
    for(unsigned int i = 0; i < vertices.size(); i++)
        all_vertices_good = all_vertices_good && (vertices[i]->index >= 0);
    bool all_facets_good = true;
    for(unsigned int i = 0; i < facets.size(); i++)
        all_facets_good = all_facets_good && (facets[i]->index >= 0);
    assert(all_vertices_good);
    assert(all_facets_good);
}

unsigned int Polytope::numVertices() const
{
    return vertices.size();
}

const Vertex& Polytope::vertex(unsigned int i) const
{
    assert(i < vertices.size());
    return *(vertices[i]);
}

bool Polytope::checkConnectivity() const
{
    bool is_valid = true;
    for(unsigned int i = 0; i < facets.size(); i++)
    {
        if(facets[i]->index < 0) continue;

        for(int k = 0; k < 3; k++)
        {
            if(facets[i]->edges[k]->twin == 0)
            {
                std::cout << "Facet " << i << "   Edge " << k << " has no twin" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->facet == 0)
            {
                std::cout << "Facet " << i << "   Edge " << k << " has twin with no facet" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->facet->index < 0)
            {
                std::cout << "Facet " << i << "   Edge " << k << " has twin with invalid facet" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->twin == 0)
            {
                std::cout << "Facet " << i << "   Edge " << k << " has twin with no twin" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->twin != facets[i]->edges[k])
            {
                std::cout << "Facet " << i << "   Edge " << k << " has bad twin" << std::endl;
                is_valid = false;
            }
            else if(!facets[i]->edges[k]->isTwin(facets[i]->edges[k]->twin))
            {
                std::cout << "Facet " << i << "   Edge " << k << " has no true twin" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->twin->facet != facets[i])
            {
                std::cout << "Facet " << i << "   Edge " << k << " has bad twin connected to wrong facet" << std::endl;
                is_valid = false;
            }
            else if(facets[i]->edges[k]->twin->twin->facet->index < 0)
            {
                std::cout << "Facet " << i << "   Edge " << k << " has bad twin with invalid facet" << std::endl;
                is_valid = false;
            }
        }
    }
    return is_valid;
}

void Polytope::draw() const
{
    glDisable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
    for(unsigned int i = 0; i < facets.size(); i++)
    {
        if(facets[i]->marked)
            glColor4f(0.0, 0.0, 1.0, 0.5);
        else
            glColor4f(1.0, 0.0, 0.0, 0.5);
        glNormal3d(facets[i]->normal[0], facets[i]->normal[1], facets[i]->normal[2]);
        glVertex3d(facets[i]->vertices[0]->position[0], facets[i]->vertices[0]->position[1], facets[i]->vertices[0]->position[2]);
        glVertex3d(facets[i]->vertices[1]->position[0], facets[i]->vertices[1]->position[1], facets[i]->vertices[1]->position[2]);
        glVertex3d(facets[i]->vertices[2]->position[0], facets[i]->vertices[2]->position[1], facets[i]->vertices[2]->position[2]);
    }
    glEnd();

    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glEdgeFlag(true);
    for(unsigned int i = 0; i < facets.size(); i++)
    {
        Vector3D cn = facets[i]->centre + (facets[i]->normal) * 0.5;

        if(facets[i]->marked)
        {
            cn += (facets[i]->normal) * 0.5;
            glColor3f(0.0, 0.0, 1.0);
            glVertex3d(facets[i]->centre[0], facets[i]->centre[1], facets[i]->centre[2]);
            glColor3f(0.0, 1.0, 0.0);
            glVertex3d(cn[0], cn[1], cn[2]);
        }
        else
        {
            glColor3f(1.0, 0.0, 0.0);
            glVertex3d(facets[i]->centre[0], facets[i]->centre[1], facets[i]->centre[2]);
            glColor3f(0.0, 1.0, 0.0);
            glVertex3d(cn[0], cn[1], cn[2]);
        }

        for(int j = 0; j < 3; j++)
        {
            if(facets[i]->edges[j]->marked)
                glColor3f(1.0, 1.0, 1.0);
            else
                glColor3f(0.0, 0.0, 1.0);
            Vector3D start = facets[i]->edges[j]->start->position;
            Vector3D end = facets[i]->edges[j]->end->position;
            glVertex3d(start[0], start[1], start[2]);
            glVertex3d(end[0], end[1], end[2]);
        }
    }

    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
}
