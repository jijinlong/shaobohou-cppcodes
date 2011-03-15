#include "CrossPlatform.h"

#include "ConvexHull.h"

#include "special.h"

#include <cstdlib>
#include <iostream>

using std::vector;
using std::swap;
using std::cout;
using std::endl;

ConvexHull3D::ConvexHull3D()
{
    surface = 0.0;
}

ConvexHull3D::ConvexHull3D(const vector<Vector3D> &points)
{
    assert(points.size() >= 4);
    addPointsToHull(points, 1e-5, false);
}

ConvexHull3D::ConvexHull3D(const vector<Vector3D> &points, double perturb_scale, bool verbose)
{
    assert(points.size() >= 4);
    addPointsToHull(points, perturb_scale, verbose);
}

ConvexHull3D::~ConvexHull3D()
{
}

bool ConvexHull3D::addPointsToHull(const vector<Vector3D> &points, double perturb_scale, bool verbose)
{
    // cast to float
    vector<Vector3D> tempPoints;
    for(unsigned int i = 0; i < points.size(); i++)
        tempPoints.push_back(Vector3D::getFloatVersion(points[i]));

    if((vertices.size() == 0) && (facets.size() == 0))
    {
        bool setup_success = setup(tempPoints);

        // perturb points and try again
        if(!setup_success && (perturb_scale > 0.0))
        {
            if(verbose) cout << "Initial setup failed, perturb points with scale " << perturb_scale << std::endl;

            for(unsigned int i = 0; i < tempPoints.size(); i++)
                tempPoints[i] += Vector3D(uniform_rand(), uniform_rand(), uniform_rand())*perturb_scale;
            setup_success = setup(tempPoints);
        }

        if(!setup_success)
        {for(unsigned int i = 0; i < tempPoints.size(); i++)
            std::cout << tempPoints[i] << std::endl;
            //if setup failed, cleanup rountine follows
            for(unsigned int i = 0; i < vertices.size(); i++)
                vertices[i]->marked = true;
            for(unsigned int i = 0; i < vertices.size(); i++)
                facets[i]->marked = true;

            compactFacets();
            compactVertices();

            cout << "Error setup initial tetrahedron!" << endl;
            exit(1);
        }
    }

    for(unsigned int i = 0; i < facets.size(); i++)
    {
        facets[i]->outsideSet.clear();
        facets[i]->updateOutsideSet(tempPoints, small_tol);
    }

    vector<Facet *> visibleFacets;
    vector<Edge *> horizonEdges;
    for(unsigned int i = 0; i < facets.size(); i++)
    {
        if((facets[i]->index >= 0) && (facets[i]->outsideSet.size() > 0))
        {
            Vector3D furthestPoint;

            if(facets[i]->getFurthestOutsidePoint(furthestPoint))
            {
                if(!getVisibleFacets(*(facets[i]), furthestPoint, visibleFacets))//get visible facets error
                {
                    cout << "Get Visible Facets error!" << endl;

                    for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->marked = false;
                    visibleFacets.clear();

                    return false;
                }
                else if(visibleFacets.size() == 0)
                {
                    cout << "Bah, there should be at least one visible facet, the one that contained the point in its outside set" << endl;
                    exit(1);
                }
            }
            else
            {
                cout << "Doh, there is should be a furthest point if the outside set is non-empty" << endl;
                exit(1);
            }

            if(!getHorizonEdges(visibleFacets, horizonEdges))
            {
                cout << "Get Horizon Edges error!" << endl;

                for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->marked = false;
                for(unsigned int j = 0; j < horizonEdges.size(); j++) horizonEdges[j]->marked = false;

                visibleFacets.clear();
                horizonEdges.clear();
                return false;
            }

            if(!remakeHull(furthestPoint, horizonEdges, visibleFacets))
            {
                cout << "Remake Hull error!" << endl;

                for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->marked = false;
                for(unsigned int j = 0; j < horizonEdges.size(); j++) horizonEdges[j]->marked = false;

                visibleFacets.clear();
                horizonEdges.clear();
                return false;
            }

            for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->marked = false;
            for(unsigned int j = 0; j < visibleFacets.size(); j++) visibleFacets[j]->index = -1;
            for(unsigned int j = 0; j < horizonEdges.size(); j++) horizonEdges[j]->marked = false;

            visibleFacets.clear();
            horizonEdges.clear();
        }
    }

    //clean up by removing non-hull vertices and facets
    compactFacets();
    compactVertices();

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
        cout << "Total surface area = " << surface << "!" << endl;
        cout << "Convex Hull's centre = " << centre.toString() << endl;
        cout << "Convex Hull's covariance matrix = " << endl << covariance.toString() << endl;
    }

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

    for(unsigned int i = 0; (i < facets.size()) && (facets[i]->index >= 0); i++)
    {
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

        if(facets[i]->isInFront(centre, smaller_tol))
        {
            wellFormed = false;
            cout << "Facet " << facets[i]->index + 1 << " out of " << facets.size() << " at " << facets[i] << " is facing the wrong way" << endl;
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

        if(facets[i]->outsideSet.size() > 0)
        {
            wellFormed = false;
            cout << "Facet " << i << " at " << facets[i] << " still has non empty outside set" << endl;
        }
    }

    return wellFormed;
}

void ConvexHull3D::computeDerivedStates()
{
    //calculate surface area
    surface = 0.0;
    for(unsigned int i = 0; i < facets.size(); i++)
        surface += facets[i]->area;

    //calculate centre
    centre = Vector3D();
    for(unsigned int i = 0; i < facets.size(); i++)
        centre += (facets[i]->area * facets[i]->centre);
    centre /= surface;

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

            e[i][j] /= (surface * 12.0);
            e[i][j] -= (centre[i] * centre[j]);    //multiply by surface are here rather than divide by surface area before
        }

    covariance = Matrix3x3(e[0][0], e[0][1], e[0][2], e[1][0], e[1][1], e[1][2], e[2][0], e[2][1], e[2][2]);
}

bool ConvexHull3D::setup(vector<Vector3D> &points)
{
    assert(points.size() > 3);

    bool success = false;
    for(unsigned int i = 0; i < points.size()-3; i++)
    {
        for(unsigned int j = i+1; j < points.size()-2; j++)
        {
            for(unsigned int k = j+1; k < points.size()-1; k++)
            {
                for(unsigned int l = k+1; l < points.size()-0; l++)
                {
                    //begin constructing the two triangles
                    Vertex *a = new Vertex(points[i], 0);
                    Vertex *b = new Vertex(points[j], 1);
                    Vertex *c = new Vertex(points[k], 2);
                    Vertex *d = new Vertex(points[l], 2);


                    Facet *f1 = new Facet(a, b, c, d->position, 0);
                    Facet *f2 = new Facet(a, c, d, b->position, 1);
                    Facet *f3 = new Facet(a, b, d, c->position, 2);
                    Facet *f4 = new Facet(b, c, d, a->position, 3);

                    bool coplanar_test = (fabs(f1->distanceToPlane(d->position)) > small_tol) &&
                                         (fabs(f2->distanceToPlane(b->position)) > small_tol) &&
                                         (fabs(f3->distanceToPlane(c->position)) > small_tol) &&
                                         (fabs(f4->distanceToPlane(a->position)) > small_tol);
                    bool triangle_test = f1->wellFormed && f2->wellFormed && f3->wellFormed && f4->wellFormed;
                    bool connect_test = f1->connect(f2, a, c) && f1->connect(f3, a, b) && f1->connect(f4, b, c) &&
                                        f2->connect(f3, a, d) && f2->connect(f4, c, d) && f3->connect(f4, b, d);
                    bool connect_test2 = checkConnectivity();

                    if(coplanar_test && triangle_test && connect_test && connect_test2)
                    {
                        //std::cout << i << "  " << j << "  " << k << "  " << l << std::endl;
                        success = true;

                        vertices.push_back(a);
                        vertices.push_back(b);
                        vertices.push_back(c);
                        vertices.push_back(d);

                        facets.push_back(f1);
                        facets.push_back(f2);
                        facets.push_back(f3);
                        facets.push_back(f4);

                        points.erase(points.begin() + l);
                        points.erase(points.begin() + k);
                        points.erase(points.begin() + j);
                        points.erase(points.begin() + i);
                    }
                    else
                    {
                        delete f4;
                        delete f3;
                        delete f2;
                        delete f1;
                        delete d;
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

        if(success) break;
    }

    return success;
}

bool ConvexHull3D::getVisibleFacets(Facet &startFacet, const Vector3D &point, vector<Facet *> &visibleFacets)
{
    if(startFacet.findVisibleFacets(point, visibleFacets, smaller_tol))
    {
        bool is_valid = true;
        for(unsigned int i = 0; i < facets.size(); i++)
            if((facets[i]->index >= 0) && facets[i]->isInFront(point, small_tol) && !(facets[i]->marked))   //point in front of facet and not marked by previous algo, error
            {
                is_valid = false;;
                std::cout << "The point is " << facets[i]->distanceToPlane(point) << " in front of Facet " << i << ", with tolerance " << small_tol << std::endl;
            }

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

bool ConvexHull3D::remakeHull(const Vector3D &point, vector<Edge *> &horizonEdges, const vector<Facet *> &visibleFacets)
{
    assert(checkConnectivity());

    vector<Facet *> createdFacets;
    Vertex *newVertex = new Vertex(point, vertices.size());

    Facet *last = 0, *first = 0;
    for(unsigned int i = 0; i < horizonEdges.size(); i++)
    {
        Edge *e = horizonEdges[i];
        Facet *f = new Facet(newVertex, e->end, e->start, facets.size()+i);
        createdFacets.push_back(f);

        if(!(f->wellFormed))
        {
            for(unsigned int j = 0; j < createdFacets.size(); j++)
                delete createdFacets[j];

            delete newVertex;

            return false;
        }
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
    for(unsigned int i = 0; i < createdFacets.size(); i++)
        for(unsigned int j = 0; j < visibleFacets.size(); j++)
            if(visibleFacets[j]->outsideSet.size() > 0)
                createdFacets[i]->updateOutsideSet(visibleFacets[j]->outsideSet, small_tol);

    createdFacets.clear();

    for(unsigned int i = 0; i < visibleFacets.size(); i++)
        visibleFacets[i]->index = -1;

    assert(checkConnectivity());
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

const Matrix3x3& ConvexHull3D::getCovariance() const
{
    return covariance;
}

void ConvexHull3D::draw() const
{
    //any custom code for convex hull

    Polytope::draw();
}

void ConvexHull3D::sync(const Vector3D &position, const Quaternion &orientation)
{
    this->position = position;
    this->orientation = orientation;
}

const Vector3D& ConvexHull3D::getPosition() const
{
    return position;
}

const Quaternion& ConvexHull3D::getOrientation() const
{
    return orientation;
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
    Matrix3x3 rot(orientation);
    Matrix3x3 tRot = rot.transpose();

    unsigned int index = getSupportPoint(supportPoint, tRot * direction);
    supportPoint = rot * supportPoint + position;

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
        if(fabs(max - distance) < small_tol)			//same value for support function, add to feature
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
    Vector3D newDirection = (~orientation).rotate(direction);
    getSupportFeature(supportFeature, newDirection);
    for(unsigned int i = 0; i < supportFeature.size(); i++)
        supportFeature[i] = orientation.rotate(supportFeature[i]) + position;

    return supportFeature.size();
}

//ConvexHull2D implementation
ConvexHull2D:: ConvexHull2D()
{
}

ConvexHull2D::ConvexHull2D(const vector<Vector3D> &points)
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

        if(fabs(temp[1] - lowestLeft[1]) < tol)
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
        if(fabs(temp[1] - lowestLeft[1]) < tol)
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
    vector<Vertex *>::iterator it = vertices.begin();
    vector<double>::iterator itPolar = polarAngles.begin();

    it++;
    while (it != vertices.end())
    {
        Vector3D prev = (*(it - 1))->position;
        Vector3D current = (*it)->position;
        Vector3D next;
        if((it + 1) != vertices.end())
            next = (*(it + 1))->position;
        else
            next = (*(vertices.begin()))->position;

        //true if the current point is left of the line from prev to next
        //based on cross product (non communtative, direction of resultant dependant on order)
        //z value is calculated here
        if(pointLineRelationXY(prev, next, current) == left_of_line)
        {
            delete *it;
            vertices.erase(it);
            polarAngles.erase(itPolar);
        }
        else
        {
            it++;
            itPolar++;
        }
    }
}

const vector<Vertex *> &ConvexHull2D::getVertices() const
{
    return vertices;
}
