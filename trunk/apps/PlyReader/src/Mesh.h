#ifndef MESH_H
#define MESH_H

#include <vector>

#include "PlyReader.h"
#include "Vector3D.h"
#include "shaobo_array1d.h"
#include "shaobo_array2d.h"


using TNT::Array1D;
using TNT::Array2D;


class Mesh
{
    public:
        Mesh();
        Mesh(const PlyFile &ply);
        virtual ~Mesh();

        const Array2D<double>& getVertices() const;
        const Array2D<unsigned int>& getFaces() const;
        const Array2D<double>& getColours() const;
        const Array2D<double>& getVertexNormals() const;

        void print(std::ostream &out) const;

    private:
        Array2D<double> vertices;
        Array1D<double> confidences;
        Array2D<double> colours;
        Array2D<unsigned int> faces;

        Array2D<double> vertexNormals;
        Array2D<double> faceNormals;

        void computeVertexNormals();
        void computeFaceNormals();
};

#endif
