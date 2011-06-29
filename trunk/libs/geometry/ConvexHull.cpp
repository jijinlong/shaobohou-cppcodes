#include "CrossPlatform.h"

#include "ConvexHull.h"

#include "special.h"
#include "Constants.h"

using MathConstants::DOUBLE_EPSILON;

#include <cstdlib>
#include <numeric>
#include <iostream>

using std::vector;
using std::swap;
using std::cout;
using std::endl;


#define HULL_DEBUG


Vector3D mean(const std::vector<Vector3D> &vecs)
{
    assert(vecs.size() > 0);

    Vector3D cen(0, 0, 0);
    for(unsigned int i = 0; i < vecs.size(); i++)
        cen += vecs[i];
    cen /= vecs.size();

    return cen;
}

Matrix3x3 covar(const std::vector<Vector3D> &vecs)
{
    assert(vecs.size() > 0);

    Matrix3x3 cov;
    for(unsigned int i = 0; i < vecs.size(); i++)
        cov += Matrix3x3(vecs[i], vecs[i]);
    cov /= vecs.size();

    return cov;
}


ConvexHull3D::ConvexHull3D()
    : eps(0.0)
{
    m_surface = 0.0;
}

ConvexHull3D::ConvexHull3D(const vector<Vector3D> &points, const double epsilon, bool verbose)
    : eps(epsilon)
{
    assert(points.size() >= 4);

    if(verbose)
    {
        cout << "Creating convex hull from " << points.size() << " points" << endl;
    }

    addPointsToHull(points, verbose);
}

ConvexHull3D::~ConvexHull3D()
{
}

bool ConvexHull3D::addPointsToHull(const vector<Vector3D> &points, bool verbose)
{
    // compute temporary centroid
    m_centre = mean(points);

    // temp vertices
    vector<Vertex*> tempPoints;
    for(unsigned int i = 0; i < points.size(); i++)
        tempPoints.push_back(new Vertex(points[i], i));


    // setup conve xhull
    if((vertices.size() == 0) && (facets.size() == 0))
    {
        bool setup_success = setup(tempPoints);

        if(!setup_success)
        {
            for(unsigned int i = 0; i < tempPoints.size(); i++)
                std::cout << tempPoints[i] << std::endl;
            for(unsigned int i = 0; i < vertices.size(); i++)
                vertices[i]->marked = true;
            for(unsigned int i = 0; i < facets.size(); i++)
                facets[i]->marked = true;

            compactFacets();
            compactVertices();

#ifdef HULL_DEBUG
            cout << "Error setup initial tetrahedron!" << endl;
#endif

            m_surface = eps;
            m_centre = mean(points);
            m_covariance = covar(points);
            vertices.push_back(new Vertex(mean(points), 0));

            return true;
        }
    }


    // update each facet, maximum two passes
    bool nonconvex = false;
    for(unsigned int iter = 0; iter < 3; iter++)
    {
        if(tempPoints.size() == 0) break;

        Facet::updateOutsideSets(facets, tempPoints, eps*10);
        for(unsigned int f = 0; f < facets.size(); f++)
            updateFacet(facets[f], tempPoints);

        nonconvex = nonconvex || !isConvex();
    }
    if(nonconvex)
    {
        const int bah = 0;
    }


    //clean up by removing non-hull vertices and facets
    assert(checkConnectivity());
    compactFacets();
    compactVertices();
    assert(checkConnectivity());

    //check for well formedness
    if(!isWellFormed())
        cout << "Convex Hull is not well formed" << endl;

    //calculate surface area, covariance matrix and centre (mean)
    computeDerivedStates();

    //printout info
    if(verbose)
    {
        cout << "Total " << points.size() << " processed vertices!" << endl;
        cout << "Total " << vertices.size() << " hull vertices!" << endl;
        cout << "Total " << facets.size() << " hull facets!" << endl;
        cout << "Total surface area = " << m_surface << "!" << endl;
        cout << "Convex Hull's centre = " << m_centre.toString() << endl;
        cout << "Convex Hull's covariance matrix = " << endl << m_covariance.toString() << endl;
    }

#ifdef HULL_DEBUG
    bool somePointsOutsideOfHull = false;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        const Vector3D &tempPoint = points[i];

        if(!insideHull(tempPoint, eps*10))
        {
            somePointsOutsideOfHull = true;
            const double dist = distance2hull(tempPoint);
            cout << "Point " << i << "    " << tempPoint << "   is " << dist << " away from hull." << endl;
        }
    }

    double maxDist = -std::numeric_limits<double>::max();
    if(tempPoints.size() > 0)
    {
        maxDist = distance2hull(tempPoints[0]->position);
        for(unsigned int i = 0; i < tempPoints.size(); i++)
        {
            maxDist = std::max(maxDist, distance2hull(tempPoints[i]->position));
        }
        cout << "Maximum distance of " << tempPoints.size() << " remaining point to the hull is " << maxDist << endl;
    }

    unsigned int outsideCount = 0;
    for(unsigned int i = 0; i < facets.size(); i++)
        outsideCount += facets[i]->outsideSet.size();
    if(outsideCount > 0)
        cout << outsideCount << " outside points remain" << endl;

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i]->index < 0) continue;

        const Vector3D &tempPoint = vertices[i]->position;

        if(!insideHull(tempPoint, eps*10))
        {
            somePointsOutsideOfHull = true;
            const double dist = distance2hull(tempPoint);
            cout << "Vertex " << i << "    " << tempPoint << "   is " << dist << " away from hull." << endl;

            for(unsigned int f = 0; f < facets.size(); f++)
            {
                if(facets[f]->index < 0) continue;

                if(facets[f]->edges[0]->start->index == i || facets[f]->edges[0]->end->index == i ||
                   facets[f]->edges[1]->start->index == i || facets[f]->edges[1]->end->index == i ||
                   facets[f]->edges[2]->start->index == i || facets[f]->edges[2]->end->index == i)
                {
                    cout << "Vertex " << i << " is part of Facet " << f << "  at distance " << facets[f]->distanceToPlane(tempPoint) << endl;
                }

                if(facets[f]->distanceToPlane(tempPoint) > eps)
                {
                    cout << "Vertex " << i << " is outside Facet " << f << "  at distance " << facets[f]->distanceToPlane(tempPoint) << endl;
                }
            }
        }
    }
#endif

    return true;
}

bool ConvexHull3D::isWellFormed() const
{
    bool wellFormed = true;

    Vector3D centre;
    for(unsigned int i = 0; i < vertices.size(); i++)
        centre += vertices[i]->position;

    centre /= vertices.size();

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        if(vertices[i]->marked)
        {
            wellFormed = false;
            cout << "Vertex " << vertices[i]->index + 1 << " is still marked!" << endl;
        }
    }

    for(unsigned int i = 0; i < facets.size(); i++)
    {
        if(facets[i]->index < 0) continue;

        for(int k = 0; k < 3; k++)
        {
            if(facets[i]->edges[k]->twin == 0)
            {
                wellFormed = false;
                cout << "Facet " << facets[i]->index + 1 << " out of " << facets.size() << "'s edge " << k << " at " << facets[i]->edges[k] << " has no twin!" << endl;
            }
            else if(facets[i]->edges[k]->twin->twin != facets[i]->edges[k])
            {
                wellFormed = false;
                cout << "Facet " << facets[i]->index + 1 << " out of " << facets.size() << "'s edge " << k << " at " << facets[i]->edges[k] << " 's twins twin is connected to " << k << endl;
            }

            if(facets[i]->edges[k]->prev == 0)
            {
                wellFormed = false;
                cout << "Facet " << facets[i]->index + 1 << " out of " << facets.size() << "'s edge " << k << " at " << facets[i]->edges[k] << " has no prev!" << endl;
            }

            if(facets[i]->edges[k]->next == 0)
            {
                wellFormed = false;
                cout << "Facet " << i << "'s edge " << k << " at " << facets[i]->edges[k] << " has no next!" << endl;
            }

            if(facets[i]->edges[k]->marked)
            {
                wellFormed = false;
                cout << "Facet " << i << "'s edge " << k << " at " << facets[i]->edges[k] << " is still marked!" << endl;
            }

            if(facets[i]->index == facets[i]->edges[k]->twin->facet->index)
            {
                wellFormed = false;
                cout << "Facet " << i << "'s edge " << k << " at " << facets[i]->edges[k] << " loops back to itself" << endl;
            }
        }

        if(facets[i]->above(centre, eps))
        {
            wellFormed = false;
            cout << "Facet " << facets[i]->index + 1 << " out of " << facets.size() << " at " << facets[i] << " is facing the wrong way with distance " << facets[i]->distanceToPlane(centre) << endl;
        }

        if(facets[i]->marked)
        {
            wellFormed = false;
            cout << "Facet " << i << " at " << facets[i] << " is still marked!" << endl;
        }

        if(facets[i]->index < 0)
        {
            wellFormed = false;
            cout << "Facet " << i << " at " << facets[i] << " has index less than 0" << endl;
        }

        //if(facets[i]->outsideSet.size() > 0)
        //{
        //    wellFormed = false;
        //    cout << "Facet " << i << " at " << facets[i] << " still has " << facets[i]->outsideSet.size() << " points in outside set" << endl;
        //    double d = -1.0;
        //    for(unsigned int t = 0; t < facets[i]->outsideSet.size(); t++)
        //        d = std::max(d, facets[i]->distanceToPlane(facets[i]->outsideSet[t]));
        //    cout << "Facet " << i << " at " << facets[i] << " has farthest at " << d << endl;
        //}
    }

    return wellFormed;
}

void ConvexHull3D::computeDerivedStates()
{
    //calculate surface area
    m_surface = 0.0;
    for(unsigned int i = 0; i < facets.size(); i++)
        m_surface += facets[i]->area;

    //calculate centre
    m_centre = Vector3D();
    for(unsigned int i = 0; i < facets.size(); i++)
        m_centre += (facets[i]->area * facets[i]->centre);
    m_centre /= m_surface;

    //calculate covariance matrix
    double e[3][3];
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
        {
            e[i][j] = 0.0;
            for(unsigned int k = 0; k < facets.size(); k++)
            {
                Vector3D m = facets[k]->centre;
                Vector3D p = facets[k]->vertices[0]->position;
                Vector3D r = facets[k]->vertices[1]->position;
                Vector3D q = facets[k]->vertices[2]->position;

                e[i][j] += (facets[k]->area  * (9.0 * m[i] * m[j] + p[i] * p[j] + r[i] * r[j] + q[i] * q[j]));
            }

            e[i][j] /= (m_surface * 12.0);
            e[i][j] -= (m_centre[i] * m_centre[j]);    //multiply by surface are here rather than divide by surface area before
        }

    m_covariance = Matrix3x3(e[0][0], e[0][1], e[0][2], e[1][0], e[1][1], e[1][2], e[2][0], e[2][1], e[2][2]);
}

bool ConvexHull3D::setup(vector<Vertex*> &points)
{
    assert(points.size() > 3);

    bool success = false;
    for(unsigned int i = 0; i < points.size()-3; i++)
    {
        for(unsigned int j = i+1; j < points.size()-2; j++)
        {
            for(unsigned int k = j+1; k < points.size()-1; k++)
            {
                //begin constructing the two triangles
                Vertex *a = new Vertex(points[i]->position, 0);
                Vertex *b = new Vertex(points[j]->position, 1);
                Vertex *c = new Vertex(points[k]->position, 2);

                Facet *f1 = new Facet(a, b, c, 0);
                Facet *f2 = new Facet(c, b, a, 1);

                bool coplanar_test = true;
                bool triangle_test = f1->wellFormed && f2->wellFormed;
                bool connect_test = f1->connect(f2, a, b) && f1->connect(f2, a, c) && f1->connect(f2, b, c);
                bool connect_test2 = checkConnectivity();

                if(coplanar_test && triangle_test && connect_test && connect_test2)
                {
                    //std::cout << i << "  " << j << "  " << k << "  " << l << std::endl;
                    success = true;

                    vertices.push_back(a);
                    vertices.push_back(b);
                    vertices.push_back(c);

                    facets.push_back(f1);
                    facets.push_back(f2);

                    points.erase(points.begin() + k);
                    points.erase(points.begin() + j);
                    points.erase(points.begin() + i);
                }
                else
                {
                    delete f2;
                    delete f1;
                    delete c;
                    delete b;
                    delete a;
                }

                if(success) break;
            }

            if(success) break;
        }

        if(success) break;
    }

    return success;
}

void ConvexHull3D::updateFacet(Facet *facet, std::vector<Vertex*> &nearPoints)
{
    while(!updateFacetOnce(facet, nearPoints))
        return;

    // retain undecided points
    nearPoints.insert(nearPoints.begin(), facet->outsideSet.begin(), facet->outsideSet.end());
    facet->outsideSet.clear();
}

bool ConvexHull3D::updateFacetOnce(Facet *facet, std::vector<Vertex*> &nearPoints)
{
    if(!facet) return true;
    if( facet->index < 0) return true;
    if( facet->outsideSet.size() == 0) return true;


    // get farthest point in the facet's outside set
    Vertex *farthestPoint = NULL;
    bool success = facet->getFarthestOutsidePoint(farthestPoint);


    // get all facets visible from the farthest point
    vector<Facet *> visibleFacets;
    if(success)
    {
        if(!getVisibleFacets(farthestPoint, facet, visibleFacets))//get visible facets error
        {
#ifdef HULL_DEBUG
            cout << "Get Visible Facets error!  " << facet->distanceToPlane(farthestPoint->position) << "  " << facet << endl;
#endif
            success = false;
        }
    }


    // get the horizons from the visible facets
    vector<Edge *> horizonEdges;
    if(success)
    {
        if(!getHorizonEdges(visibleFacets, horizonEdges))
        {
#ifdef HULL_DEBUG
            cout << "Get Horizon Edges error!" << endl;
#endif
            success = false;
        }
    }


    // replace visible facets with new facets connectint a vertex at the farthest point
    if(success)
    {
        if(!remakeHull(farthestPoint, horizonEdges, visibleFacets, nearPoints))
        {
#ifdef HULL_DEBUG
            cout << "Remake Hull error!" << endl;
#endif
            success = false;
        }
    }


    // mark visible facets for deletion
    if(success)
    {
        if(facet->outsideSet.size() > 0)
        {
            const int bah = 0;
        }
        facet->outsideSet.clear();
        for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->index = -1;
    }


    // clean up
    for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->marked = false;
    for(unsigned int j = 0; j < horizonEdges.size(); j++) horizonEdges[j]->marked = false;


    assert(isWellFormed());


    return success;
}

bool ConvexHull3D::getVisibleFacets(Vertex *point, Facet *startFacet, vector<Facet *> &visibleFacets)
{
    //if(startFacet->findVisibleFacets(point->position, visibleFacets, -eps))    // negative tolerance
    if(startFacet->findVisibleFacets(point->position, visibleFacets, -DOUBLE_EPSILON))    // negative tolerance
    {
        bool added = true;
        while(added)
        {
            added = false;
            for(unsigned int f = 0; f < visibleFacets.size(); f++)
            {
                const Facet *facet = visibleFacets[f];
                for(unsigned int e = 0; e < 3; e++)
                {
                    const Edge *edge = facet->edges[e];
                    if(facet->marked && !edge->twin->facet->marked)
                    {
                        Facet f(edge->start, edge->end, point, facets.size());
                        const double aboveDist = f.distanceToPlane(edge->twin->facet->centre);
                        //const Vector3D norm = Vector3D::normal(edge->start->position, edge->end->position, point->position);
                        //const double dist = -(norm * point->position);
                        //const double aboveDist = norm * edge->twin->facet->centre + dist;

                        const double area = Vector3D::area(edge->start->position, edge->end->position, point->position);
                        const Vector3D normAreaVec = f.edges[1]->direction^f.edges[2]->direction;
                        const double normArea = normAreaVec*normAreaVec*0.5;

                        //if(norm*norm < eps || area < eps)
                        //if(norm*norm < eps || (facets.size() > 2 && aboveDist > -eps))
                        //if(f.normal*f.normal < eps || (facets.size() > 2 && aboveDist > -eps))
                        //if(f.normal*f.normal < eps || (facets.size() > 2 && aboveDist > -eps) || normArea < eps)
                        //if(f.normal*f.normal < eps || normArea < eps)
                        if(f.normal*f.normal < eps)
                        {
                            std::cout << "correcting in findVisibleFacet for Point " << point->index << " : " << point->position << endl;
                            std::cout << "area = " << area << "   normArea = " << normArea << endl;
                            std::cout << edge->facet->distanceToPlane(point->position) << endl;
                            std::cout << edge->twin->facet->distanceToPlane(point->position) << endl;
                            std::cout << point->position << std::endl;
                            std::cout << edge->end->position << std::endl;
                            std::cout << edge->start->position << std::endl;
                            std::cout << "-----------" << endl;
                            std::cout << f.edges[0]->direction << endl;
                            std::cout << f.edges[1]->direction << endl;
                            std::cout << f.edges[2]->direction << endl;
                            std::cout << std::endl;
                            const int bah = 0;

                            added = true;
                            edge->twin->facet->marked = true;
                            visibleFacets.push_back(edge->twin->facet);
                            edge->twin->facet->edges[0]->twin->facet->findVisibleFacets(point->position, visibleFacets, -eps);
                            edge->twin->facet->edges[1]->twin->facet->findVisibleFacets(point->position, visibleFacets, -eps);
                            edge->twin->facet->edges[2]->twin->facet->findVisibleFacets(point->position, visibleFacets, -eps);
                            const int pah = 0;
                        }
                    }
                }
            }
        }


        bool is_valid = true;

        // compute distance to farthest visible facet, make sure at least one facet is actually visible
        for(unsigned int i = 0; i < visibleFacets.size(); i++)
        {
            is_valid = is_valid || visibleFacets[i]->above(point->position, eps*10);
        }

#ifdef HULL_DEBUG
        for(unsigned int i = 0; i < facets.size(); i++)
        {
            const Facet *const testFacet = facets[i];
            if((testFacet->index >= 0) && testFacet->above(point->position, 0) && !(testFacet->marked))   //point in front of facet and not marked by previous algo, error
            {
                //is_valid = false;
                std::cout << "The point " << point->index << "  at  " << point->position << " is " << testFacet->distanceToPlane(point->position) << " in front of Facet " << i << ", with tolerance " << -eps << std::endl;
                std::cout << testFacet->edges[0]->twin->facet->distanceToPlane(point->position) << std::endl;
                std::cout << testFacet->edges[1]->twin->facet->distanceToPlane(point->position) << std::endl;
                std::cout << testFacet->edges[2]->twin->facet->distanceToPlane(point->position) << std::endl;
                std::cout << "-volume = " << -testFacet->volume(point->position) << endl;
                std::cout << -testFacet->edges[0]->twin->facet->volume(point->position) << std::endl;
                std::cout << -testFacet->edges[1]->twin->facet->volume(point->position) << std::endl;
                std::cout << -testFacet->edges[2]->twin->facet->volume(point->position) << std::endl;
                const int bah = 0;
            }
        }
#endif

        return is_valid;
    }
    else     //returned false from Facet::findVisibleFacets()
        return false;

    return true;
}

bool ConvexHull3D::getHorizonEdges(vector<Facet *> &visibleFacets, vector<Edge *> &horizonEdges)
{
    if(visibleFacets.size() < 1) return true;     //true if no visible facets

    Edge *e = 0;
    for(unsigned int i = 0; i < visibleFacets.size(); i++)
    {
        e = visibleFacets[i]->getHorizonEdge();

        if(e != 0) break;
    }

    if(e == 0)
    {
        cout << "Visible facets are present but can't find horizon edges!" << endl;
        return false;                      //this should not happen if there are visible facets
    }

    if(e->findHorizon(horizonEdges))
    {
        if(horizonEdges.size() < 3)
        {
            cout << "Less than 3 horizon edges error!" << endl;
            return false;
        }
        else
        {
            for(unsigned int i = 0; i < horizonEdges.size(); i++)
                horizonEdges[i]->marked = true;

            return true;
        }
    }
    else
    {
        cout << "Find horizon traversal error!" << endl;
        return false;
    }

    return true;
}

bool ConvexHull3D::remakeHull(Vertex *point, vector<Edge *> &horizonEdges, const vector<Facet *> &visibleFacets, std::vector<Vertex*> &nearPoints)
{
    Vector3D centre;
    for(unsigned int i = 0; i < vertices.size(); i++)
        centre += vertices[i]->position;

    centre /= vertices.size();


    vector<Facet *> createdFacets;
    Vertex *newVertex = new Vertex(point->position, vertices.size());

    Facet *last = 0, *first = 0;
    std::vector<int> valids(horizonEdges.size(), 0);
    for(unsigned int i = 0; i < horizonEdges.size(); i++)
    {
        Edge *e = horizonEdges[i];
        Facet *f = new Facet(newVertex, e->end, e->start, facets.size()+i);
        createdFacets.push_back(f);

        double dd = f->distanceToPlane(centre);

        if(!(f->wellFormed) || dd > 0)
        {
            double d = e->distanceToLine(point->position);
            double ed = e->projectToLine(point->position);
            double ed1 = e->next->projectToLine(point->position);
            double ed2 = e->prev->projectToLine(point->position);
            double fd1 = e->facet->distanceToPlane(point->position);
            double fd2 = e->twin->facet->distanceToPlane(point->position);
			std::cout << "problem with point " << point->index << ": " << point->position << endl;
            std::cout << newVertex->position << std::endl;
            std::cout << e->end->position << std::endl;
            std::cout << e->start->position << std::endl;
			std::cout << "volume = " << f->volume(centre) << endl;
			std::cout << "dists = " << fd1 << "  " << fd2 << endl;
            valids[i] = 1;
            Vector3D::normal(newVertex->position, e->end->position, e->start->position);
        }
    }


    if(std::accumulate(valids.begin(), valids.end(), 0) > 0)
    {
        for(unsigned int j = 0; j < createdFacets.size(); j++)
            delete createdFacets[j];
        delete newVertex;

        return false;
    }


    vertices.push_back(newVertex);   //all facets are well formed , add new vertex to list
    for(unsigned int i = 0; i < horizonEdges.size(); i++)
    {
        Edge *e = horizonEdges[i];
        Facet *f = createdFacets[i];

        facets.push_back(f);

        if(!(f->edges[1]->connect(e)))
        {
            cout << "D'oh, error trying to connect to horizon edges" << endl;
            exit(1);
        }

        if(last != 0)
        {
            // horizon edges are colllcted in clockwise order
            if(!(f->edges[2]->connect(last->edges[0])))
            {
                cout << "D'oh, error trying to connect to to last faces" << endl;
                exit(1);
            }
        }

        last = f;
        if(first == 0) first = f;
    }

    if(last != 0 && first != 0)
    {
        if(!(last->edges[0]->connect(first->edges[2])))
        {
            cout << "D'oh, error trying to connect two end of loops" << endl;
            exit(1);
        }
    }
    else
    {
        cout << "Fatal hull making algorithm error!" << endl;
        exit(1);
    }


    //this updates the outside set of new facets
    for(unsigned int f = 0; f < visibleFacets.size(); f++)
    {
        Facet::updateOutsideSets(createdFacets, visibleFacets[f]->outsideSet, eps*10);

        // retain undecided points
        nearPoints.insert(nearPoints.end(), visibleFacets[f]->outsideSet.begin(), visibleFacets[f]->outsideSet.end());
        visibleFacets[f]->outsideSet.clear();
    }
    

    return true;
}

void ConvexHull3D::compactFacets()
{
    vector<Facet *>::iterator it = facets.begin();

    while (it != facets.end())
    {
        Facet *f = *it;

        if(f->index < 0)
        {
            delete f;
            it = facets.erase(it);
        }
        else
            it++;
    }

    for(unsigned int i = 0; i < facets.size(); i++)
        facets[i]->index = i;
}

void ConvexHull3D::compactVertices()
{
    for(unsigned int i = 0; i < vertices.size(); i++)
        vertices[i]->marked = false;

    for(unsigned int i = 0; i < facets.size(); i++)
        for(int j = 0; j < 3; j++)
            facets[i]->vertices[j]->marked = true;   //true if vertex is still in hull, else free

    vector<Vertex *>::iterator it = vertices.begin();

    while (it != vertices.end())
    {
        Vertex *v = *it;

        if(v->marked)
            it++;
        else
        {
            delete v;
            it = vertices.erase(it);
        }
    }

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        vertices[i]->marked = false;
        vertices[i]->index = i;
    }
}

const Matrix3x3& ConvexHull3D::covariance() const
{
    return m_covariance;
}

bool ConvexHull3D::isConvex() const
{
    // check convexity
    bool nonconvex = false;
    for(unsigned int f = 0; f < facets.size(); f++)
    {
        const Facet *facet = facets[f];
        if(facet->index < 0) continue;

        for(unsigned int e = 0; e < 3; e++)
        {
            const Edge *edge = facet->edges[e];
            const Vertex *vertex = edge->twin->next->end;
            const double v = -facets[f]->volume(vertex->position);
            if(v > 0)
            {
                nonconvex = true;
                std::cout << "Vertex " << vertex->index << " of Facet " << edge->twin->facet->index << " is " << v << " outside of Facet " << facet->index << endl;
                const int bah = 0;
            }
        }
    }

    return !nonconvex;
}

void ConvexHull3D::draw() const
{
    //any custom code for convex hull

    Polytope::draw();
}

void ConvexHull3D::sync(const Vector3D &newPosition, const Quaternion &newOrientation)
{
    this->m_position = newPosition;
    this->m_orientation = newOrientation;
}

const Vector3D& ConvexHull3D::position() const
{
    return m_position;
}

const Quaternion& ConvexHull3D::orientation() const
{
    return m_orientation;
}

bool ConvexHull3D::insideHull(const Vector3D &point, const double tol) const
{
    bool inside =  true;
    for(unsigned int f = 0; f < facets.size(); f++)
        inside = inside && facets[f]->below(point, -tol);

    return inside;
}

double ConvexHull3D::distance2hull(const Vector3D &point) const
{
    assert(vertices.size() > 0);

    double dist = -std::numeric_limits<double>::max();

    // test facets
    for(unsigned int f = 0; f < facets.size(); f++)
        dist = std::max(dist, facets[f]->distanceToPlane(point));

    // test all edges (unimplemented)

    //// test all vertices
    //for(unsigned int v = 0; v < vertices.size(); v++)
    //    dist = std::min(dist, Vector3D::distance(point, vertices[v]->position));

    return dist;
}

//naive implemetation for testing purpose
unsigned int ConvexHull3D::getSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const
{
    unsigned int index = 0;
    double displacement = vertices[0]->position * direction;

    for(unsigned int i = 1; i < vertices.size(); i++)
    {
        double tempDisplacement = vertices[i]->position * direction;
        if(tempDisplacement > displacement)
        {
            index = i;
            displacement = tempDisplacement;
        }
    }

    supportPoint = vertices[index]->position;
    return index;
}

//assume that T(x) = Bx + c
//new support function = T(S(Bt * x))
unsigned int ConvexHull3D::getTransformedSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const
{
    Matrix3x3 rot(m_orientation);
    Matrix3x3 tRot = rot.transpose();

    unsigned int index = getSupportPoint(supportPoint, tRot * direction);
    supportPoint = rot * supportPoint + m_position;

    return index;
}

unsigned int ConvexHull3D::getSupportFeature(vector<Vector3D> &supportFeature, const Vector3D &direction) const
{
    double max = vertices[0]->position * direction;
    double distance;

    supportFeature.clear();
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        distance = vertices[i]->position * direction;
        if(fabs(max - distance) < eps)			//same value for support function, add to feature
            supportFeature.push_back(vertices[i]->position);
        else if(distance > max)				//bette support point found, rebuild support feature
        {
            max = distance;
            supportFeature.clear();
            supportFeature.push_back(vertices[i]->position);
        }
    }

    assert(supportFeature.size() != 0);

    return supportFeature.size();
}

unsigned int ConvexHull3D::getTransformedSupportFeature(vector<Vector3D> &supportFeature, const Vector3D &direction) const
{
    Vector3D newDirection = (~m_orientation).rotate(direction);
    getSupportFeature(supportFeature, newDirection);
    for(unsigned int i = 0; i < supportFeature.size(); i++)
        supportFeature[i] = m_orientation.rotate(supportFeature[i]) + m_position;

    return supportFeature.size();
}

//ConvexHull2D implementation
ConvexHull2D:: ConvexHull2D()
    : eps(0.0)
{
}

ConvexHull2D::ConvexHull2D(const vector<Vector3D> &points, const double epsilon, const bool verbose)
    : eps(epsilon)
{
    for(unsigned int i = 0; i < points.size(); i++)
        vertices.push_back(new Vertex(points[i], i));

    computePolarAngles();
    polarAngleQuickSort(0, polarAngles.size() - 1);
    grahamScan();
}

ConvexHull2D::ConvexHull2D(const ConvexHull2D &other)
{
    if(this != &other)
        deepCopy(other);
}

ConvexHull2D& ConvexHull2D::operator=(const ConvexHull2D &other)
{
    if(this != &other)
        deepCopy(other);
    return *this;
}

ConvexHull2D::~ConvexHull2D()
{
    deallocateMemory();
}

void ConvexHull2D::deallocateMemory()
{
    for(unsigned int i = 0; i < vertices.size(); i++)
        if(vertices[i])
            delete vertices[i];
    vertices.clear();
}
void ConvexHull2D::deepCopy(const ConvexHull2D &other)
{
    if(this == &other)
        return;
    else
        deallocateMemory();

    vertices.clear();
    for(unsigned int i = 0; i < other.vertices.size(); i++)
        vertices.push_back(new Vertex(*(other.vertices[i])));
}

void ConvexHull2D::computePolarAngles()
{
    polarAngles.clear();

    Vector3D lowestLeft = vertices[0]->position, lowestRight = vertices[0]->position;
    vector<Vertex *>::iterator it = vertices.begin();
    vector<Vertex *>::iterator pivot = it;

    Vector3D temp;
    while (it != vertices.end())
    {
        temp = (*it)->position;

        if(fabs(temp[1] - lowestLeft[1]) < eps)
        {
            if(temp[0] < lowestLeft[0])
            {
                pivot = it;
                lowestLeft = temp;
            }
            else if(temp[0] > lowestRight[0])
            {
                lowestRight = temp;
            }
        }
        else if(temp[1] < lowestLeft[1])
        {
            pivot = it;
            lowestLeft = temp;
            lowestRight = temp;
        }

        it++;
    }

    it = vertices.begin();
    while (it != vertices.end())
    {
        temp = (*it)->position;
        if(fabs(temp[1] - lowestLeft[1]) < eps)
            if((temp[0] > lowestLeft[0]) && (temp[0] < lowestRight[0]))
            {
                vertices.erase(it);
                it--;
            }

        it++;
    }

    swap(*pivot, vertices[0]);
    for(unsigned int i = 1; i < vertices.size(); i++)
        polarAngles.push_back((vertices[i]->position[0] - lowestLeft[0]) / (vertices[i]->position[1] - lowestLeft[1]));
}

void ConvexHull2D::polarAngleQuickSort(int begin, int end)
{
    if(end > begin)
    {
        double pivot = polarAngles[begin];
        int l = begin;
        int r = end;

        while (l <= r)   //while indices are not crossed
        {   //once the indices cross, keep traversing until find something
            //not the same as pivot but should be on that side, in case of duplicates
            while ((l < end) && (polarAngles[l] > pivot)) l++;
            while ((r > begin) && (polarAngles[r] < pivot )) r--;

            if(l <= r)
            {
                swap(polarAngles[l], polarAngles[r]);
                swap(vertices[l + 1], vertices[r + 1]);
                l++;
                r--;
            }
        }

        if(begin < r) polarAngleQuickSort(begin, r);
        if(l < end) polarAngleQuickSort(l, end);
    }
}

void ConvexHull2D::grahamScan()
{
    //vector<Vertex *>::iterator it = vertices.begin();
    //vector<double>::iterator itPolar = polarAngles.begin();

    //it++;
    //while (it != vertices.end())
    //{
    //    Vector3D prev = (*(it - 1))->position;
    //    Vector3D current = (*it)->position;
    //    Vector3D next;
    //    if((it + 1) != vertices.end())
    //        next = (*(it + 1))->position;
    //    else
    //        next = (*(vertices.begin()))->position;

    //    //true if the current point is left of the line from prev to next
    //    //based on cross product (non communtative, direction of resultant dependant on order)
    //    //z value is calculated here
    //    if(pointLineRelationXY(prev, next, current) == LEFT_OF_LINE)
    //    {
    //        delete *it;
    //        vertices.erase(it);
    //        polarAngles.erase(itPolar);
    //    }
    //    else
    //    {
    //        it++;
    //        itPolar++;
    //    }
    //}
}

const vector<Vertex *> &ConvexHull2D::getVertices() const
{
    return vertices;
}
