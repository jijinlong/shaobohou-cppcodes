#ifndef SUPPORT_MAPPABLE_H
#define SUPPORT_MAPPABLE_H

#include <vector>

#include "Vector3D.h"

class SupportMappable
{
    public:
        SupportMappable();
        virtual ~SupportMappable();

        //vanilla support mapping of the object in object space, results in returned in support point, the return value is the index of the vertex that matches a support point
        virtual unsigned int getSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const = 0;

        //support mapping of the object under affine transform, assume T(x) = Bx + c, but subclass make final decision, if not support should just return the same result as above
        virtual unsigned int  getTransformedSupportPoint(Vector3D &supportPoint, const Vector3D &direction) const = 0;

        //support mapping for the minkowski sum of A and -B, this is always done with transformation
        //Sa-b(v) = Sa(v) - Sb(-v)
        //return squared distance from origin
        static double getMinkowskiSupportPoint(Vector3D &supportPoint, int &aIndex, int &bIndex, const SupportMappable &A, const SupportMappable&B, const Vector3D &direction)
        {
            Vector3D aPoint, bPoint;

            aIndex = A.getTransformedSupportPoint(aPoint, direction);   //Sa(v)
            bIndex = B.getTransformedSupportPoint(bPoint, -direction);  //Sb(-v)

            supportPoint = aPoint - bPoint;

            return supportPoint * supportPoint;
        }

        //get the support feature in a particular direction, assume polygon (supportFeature) is in CCW winding
        virtual unsigned int getSupportFeature(std::vector<Vector3D> &supportFeature, const Vector3D &direction) const = 0;
        virtual unsigned int getTransformedSupportFeature(std::vector<Vector3D> &supportFeature, const Vector3D &direction) const = 0;
};

class Simplex
{
    public:
        Simplex();

        bool addVertex(const Vector3D &vertex, int aIndex, int bIndex);
        const Vector3D& subsimplex();

        void getClosestPointsInfo(std::vector<double> &lambdas, std::vector<int> &aIndex, std::vector<int> &bIndex) const;
        int getNumberOfPoints() const;

        void print() const;

    private:
        Vector3D Y[4];
        double lambdas[4];
        int aCoordsIndex[4], bCoordsIndex[4];
        Vector3D v;
        int theSimplex;      //bits that have corresponding point
        double dots[4][4];
        double deltas[16][4];

        //used to stop constant looping
        Vector3D lastVertexAdded;   //w(k-1)
        std::vector<Vector3D> previousY; //W(k-1)

        //used by optimisation
        int index;

        //subsimplex is one of the any possible subset X of current simplex Y, yI is index of an element in Y
        double delta(int subsimplex, int yI) const;
        bool hasSubset(int set) const;
};

//functions for accessing table, based on Cameron's implementation'usage of tables, code that uses it are much different though
//simplex, means simplex by bits
//order, means the first etc, e.g. the first element of 6 is the element at index two
//index, refers to actual index into Y
int card(int simplex);
int elementIndex(int simplex, int order);
int nonElementIndex(int simplex, int order);
int subset(int simplex, int order);
int superset(int simplex, int order);
bool presence(int simplex, int index);
int value(int index);

static const int cardinality[16] = {
                                       0,  1,  1,  2,  1,  2,  2,  3,  1,  2,  2,  3,  2,  3,  3,  4
                                   };
static const int max_element[16] = {
                                       -1,  0,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3
                                   };
static const int elements[16][4] = {
                                       {  0,  0,  0,  0},
                                       {  0,  0,  0,  0},
                                       {  1,  0,  0,  0},
                                       {  0,  1,  0,  0},
                                       {  2,  0,  0,  0},
                                       {  0,  2,  0,  0},
                                       {  1,  2,  0,  0},
                                       {  0,  1,  2,  0},
                                       {  3,  0,  0,  0},
                                       {  0,  3,  0,  0},
                                       {  1,  3,  0,  0},
                                       {  0,  1,  3,  0},
                                       {  2,  3,  0,  0},
                                       {  0,  2,  3,  0},
                                       {  1,  2,  3,  0},
                                       {  0,  1,  2,  3}
                                   };
static const int non_elements[16][4] = {
                                           {  0,  1,  2,  3},
                                           {  1,  2,  3,  0},
                                           {  0,  2,  3,  0},
                                           {  2,  3,  0,  0},
                                           {  0,  1,  3,  0},
                                           {  1,  3,  0,  0},
                                           {  0,  3,  0,  0},
                                           {  3,  0,  0,  0},
                                           {  0,  1,  2,  0},
                                           {  1,  2,  0,  0},
                                           {  0,  2,  0,  0},
                                           {  2,  0,  0,  0},
                                           {  0,  1,  0,  0},
                                           {  1,  0,  0,  0},
                                           {  0,  0,  0,  0},
                                           {  0,  0,  0,  0}
                                       };
static const int predecessor[16][4] = {
                                          {  0,  0,  0,  0},
                                          {  0,  0,  0,  0},
                                          {  0,  0,  0,  0},
                                          {  2,  1,  0,  0},
                                          {  0,  0,  0,  0},
                                          {  4,  1,  0,  0},
                                          {  4,  2,  0,  0},
                                          {  6,  5,  3,  0},
                                          {  0,  0,  0,  0},
                                          {  8,  1,  0,  0},
                                          {  8,  2,  0,  0},
                                          { 10,  9,  3,  0},
                                          {  8,  4,  0,  0},
                                          { 12,  9,  5,  0},
                                          { 12, 10,  6,  0},
                                          { 14, 13, 11,  7}
                                      };
static const int successor[16][4] = {
                                        {  1,  2,  4,  8},
                                        {  3,  5,  9,  0},
                                        {  3,  6, 10,  0},
                                        {  7, 11,  0,  0},
                                        {  5,  6, 12,  0},
                                        {  7, 13,  0,  0},
                                        {  7, 14,  0,  0},
                                        { 15,  0,  0,  0},
                                        {  9, 10, 12,  0},
                                        { 11, 13,  0,  0},
                                        { 11, 14,  0,  0},
                                        { 15,  0,  0,  0},
                                        { 13, 14,  0,  0},
                                        { 15,  0,  0,  0},
                                        { 15,  0,  0,  0},
                                        {  0,  0,  0,  0}
                                    };

static const int present1[16][4] = {
                                       {  0,  0,  0,  0},
                                       {  1,  0,  0,  0},
                                       {  0,  1,  0,  0},
                                       {  1,  1,  0,  0},
                                       {  0,  0,  1,  0},
                                       {  1,  0,  1,  0},
                                       {  0,  1,  1,  0},
                                       {  1,  1,  1,  0},
                                       {  0,  0,  0,  1},
                                       {  1,  0,  0,  1},
                                       {  0,  1,  0,  1},
                                       {  1,  1,  0,  1},
                                       {  0,  0,  1,  1},
                                       {  1,  0,  1,  1},
                                       {  0,  1,  1,  1},
                                       {  1,  1,  1,  1}
                                   };

static const int indexValue[4] = {
                                     1, 2, 4, 8
                                 };

#endif
