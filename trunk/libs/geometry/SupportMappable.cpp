#include "SupportMappable.h"

#include <cstdlib>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

SupportMappable::SupportMappable()
{
}

SupportMappable::~SupportMappable()
{
}

Simplex::Simplex()
{
    theSimplex = 0;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dots[i][j] = 0.0;

    for (int s = 0; s < 16; s++)
        for (int j = 0; j < 4; j++)
            deltas[s][j] = 0.0;

    for (int i = 0; i < 4; i++)
    {
        lambdas[i] = 0.0;
        aCoordsIndex[i] = -1;
        bCoordsIndex[i] = -1;
    }

    index = -1;
}

bool Simplex::hasSubset(int set) const
    {
        if ((theSimplex / 8) < (set / 8))
            return false;

        int tempSlots = theSimplex % 8, tempSet = set % 8;
        if ((tempSlots / 4) < (tempSet / 4))
            return false;

        tempSlots = tempSlots % 4; tempSet = tempSet % 4;
        if ((tempSlots / 2) < (tempSet / 2))
            return false;

        return true;
    }

bool Simplex::addVertex(const Vector3D &vertex, int aIndex, int bIndex)
{
//test if the new vertex is valid
    Vector3D diff = lastVertexAdded - vertex;
    lastVertexAdded = vertex;      //udpate lastVertexAdded with the new vertex arg
    if (getNumberOfPoints() == 4)  //no free slots
        return false;    //no space left
    if (getNumberOfPoints() > 0)
    {
        if ((diff * diff) < 0.0000000001) //check with w(k-1)
            return false;
    }
    for (unsigned int i = 0; i < previousY.size(); i++) //check with W(k-1)
    {
        diff = previousY[i] - vertex;
        if ((diff * diff) < 0.0000000001)
            return false;
    }
    previousY.clear();  //clear previous, as new vertex is valid so update needed
    for (int i = 0; i < card(theSimplex); i++) //add in the current points for use by next iteration.
    {
        int yI = elementIndex(theSimplex, i);
        previousY.push_back(Y[yI]);
    }

//find the first empty slot, first in the list of empty in this simplex
    index = nonElementIndex(theSimplex, 0);

//updated slots and vertices, new filled is the successor of current filled with its first empty slot occupied
    Y[index] = vertex;
    aCoordsIndex[index] = aIndex;
    bCoordsIndex[index] = bIndex;
    theSimplex = superset(theSimplex, 0);

//udpated dot products, get the elements of
    for (int i = 0; i < card(theSimplex); i++)
    { //for each non empty element of this simplex, calculate new dot product with the new point
        int yI = elementIndex(theSimplex, i);   //index into ys
        dots[yI][index] = dots[index][yI] = Y[index] * Y[yI];
    }

//update delta terms
    for (int s = 1; s < 16; s++) //test each possible subsimplex
        if (presence(s, index) && hasSubset(s)) //must refer to the index of new vertex and can be formed from curren simplex if simplex less than 4 points.
        {
            for (int j = 0; j < card(s); j++) //for each member of subsimplex, update their delta value for this subsimplex
            {
                int yI = elementIndex(s, j);
                deltas[s][yI] = delta(s, yI);
            }
        }

    return true;
}

const Vector3D& Simplex::subsimplex()
{
    int validSubsimplex = 0;
    for (int s = 1; s < 16; s++)
        if (presence(s, index) && (hasSubset(s)))    //for each sensible subsimplex
        {
            bool status = true;

            for (int j = 0; j < card(s); j++)  //make sure delta of in Ix is > 0.0
            {
                if (deltas[s][elementIndex(s, j)] <= 0.0)
                {
                    status = false;
                    break;
                }
            }
            if (status)
                for (int j = 0; j < 4 - card(s); j++)    //make sure delta of element not in Ix and Wk are <= 0.0
                {
                    int supersubsimplex = superset(s, j);
                    if (hasSubset(supersubsimplex))
                    {
                        int nonEI = nonElementIndex(s, j);

                        if (deltas[supersubsimplex][nonEI] > 0.0)
                        {
                            status = false;
                            break;
                        }
                    }
                }

            //update slots the current simplex to the subsimplex
            if (status)
            {
                validSubsimplex = s;
                break;
            }
        }

    if (validSubsimplex != 0)
        theSimplex = validSubsimplex;
    else
    {
        cout << "bah no subsimplex is valid, not even the original simplex." << endl;
        exit(1);
    }

//caclulate the lambda terms and closest point to origin v
    double deltaSum = 0.0;
    for (int i = 0; i < card(theSimplex); i++)
    {
        int yI = elementIndex(theSimplex, i);
        deltaSum += deltas[theSimplex][yI];
    }
    v = Vector3D();
    for (int i = 0; i < card(theSimplex); i++)
    {
        int yI = elementIndex(theSimplex, i);
        lambdas[yI] = deltas[theSimplex][yI] / deltaSum;
        v += Y[yI] * lambdas[yI];
    }

    return v;
}

void Simplex::getClosestPointsInfo(vector<double> &lambdas, vector<int> &aIndex, vector<int> &bIndex) const
{
    for (int i = 0; i < card(theSimplex); i++)
    {
        int yI = elementIndex(theSimplex, i);
        lambdas.push_back(this->lambdas[yI]);
        aIndex.push_back(this->aCoordsIndex[yI]);
        bIndex.push_back(this->bCoordsIndex[yI]);
    }
}


double Simplex::delta(int subsimplex, int yI) const
{
    double result = 0.0;

//possible early exit
    if ((card(subsimplex) <= 1) && (elementIndex(subsimplex, 0) == yI))
        return 1.0;

//general case
    if (!presence(subsimplex, yI)) //y element at index e is not in simplex
    {
        for (int i = 0; i < card(subsimplex); i++)
        {
            int inE = elementIndex(subsimplex, i);
            result += delta(subsimplex, inE) * (dots[inE][elementIndex(subsimplex, 0)] - dots[inE][yI]);
        }

        return result;
    }
    else   //find the subsubsimplex that that does not have yI and then recursively call delta()
    {
        int tI = -1;   //find where in the table is the element for this simplex
        for (int i = 0; i < 4; i++)
            if (elementIndex(subsimplex, i) == yI)
            {
                tI = i;
                break;
            }

        int subsubsimplex = subset(subsimplex, tI);
        return delta(subsubsimplex, yI);
    }

    return result;
}

int Simplex::getNumberOfPoints() const
{
    return card(theSimplex);
}

int card(int simplex)
{
    return cardinality[simplex];
}

int elementIndex(int simplex, int order)
{
    return elements[simplex][order];
}

int nonElementIndex(int simplex, int order)
{
    return non_elements[simplex][order];
}

int subset(int simplex, int order)
{
    return predecessor[simplex][order];
}

int superset(int simplex, int order)
{
    return successor[simplex][order];
}

bool presence(int simplex, int index)
{
    return (present1[simplex][index] == 1);
}

int value(int index)
{
    return indexValue[index];
}

void Simplex::print() const
{
    cout << "filled = " << theSimplex << endl;

    cout << endl;

    cout << "lastVertexAdded = " << lastVertexAdded.toString() << endl;

    cout << endl;

    for (unsigned int i = 0; i < previousY.size(); i++)
        cout <<"previousY " << i << " = " << previousY[i].toString() << endl;

    for (int i = 0; i < 4; i++)
        cout << dots[i][0] << " "  << dots[i][1] << " "  << dots[i][2] << " "  << dots[i][3] << endl;

    cout << endl;

    for (int i = 0; i < 16; i++)
        cout << i << "th " <<deltas[i][0] << "\t"  << deltas[i][1] << "\t"  << deltas[i][2] << "\t"  << deltas[i][3] << endl;

    cout << endl;

    cout << lambdas[0] << " "  << lambdas[1] << " "  << lambdas[2] << " "  << lambdas[3] << endl;
    cout << aCoordsIndex[0] << " "  << aCoordsIndex[1] << " "  << aCoordsIndex[2] << " "  << aCoordsIndex[3] << endl;
    cout << bCoordsIndex[0] << " "  << bCoordsIndex[1] << " "  << bCoordsIndex[2] << " "  << bCoordsIndex[3] << endl;


    cout << endl;
}
