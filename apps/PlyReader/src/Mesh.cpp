#include "Mesh.h"

#include "MiscUtils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;
using std::ostream;


Mesh::Mesh()
{
}

Mesh::Mesh(const PlyFile &ply)
{
    for(unsigned int i = 0; i < ply.elements.size(); i++)
    {
        const PlyElement &element = ply.elements[i];

        if(element.name == "vertex")
        {
            vertices = Array2D<double>(3, element.numElements, 0.0);
            confidences = Array1D<double>(element.numElements, 1.0);
            colours = Array2D<double>(3, element.numElements, 1.0);

            for(unsigned int j = 0; j < element.properties.size(); j++)
            {
                PlyProperty *property = element.properties[j];
                if(property->type == "list") break;

                if(property->name == "x")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        vertices(0, k) = stream_cast<double>(property->data[k]);
                }
                else if(property->name == "y")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        vertices(1, k) = stream_cast<double>(property->data[k]);
                }
                else if(property->name == "z")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        vertices(2, k) = stream_cast<double>(property->data[k]);
                }
                else if(property->name == "confidence")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        confidences[k] = stream_cast<double>(property->data[k]);
                }
                else if(property->name == "diffuse_red")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        colours(0, k) = stream_cast<double>(property->data[k]) / 255.0;
                }
                else if(property->name == "diffuse_green")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        colours(1, k) = stream_cast<double>(property->data[k]) / 255.0;
                }
                else if(property->name == "diffuse_blue")
                {
                    for(unsigned int k = 0; k < property->data.size(); k++)
                        colours(2, k) = stream_cast<double>(property->data[k]) / 255.0;
                }
                else
                    cout << "Property <" << property->name << "> not recognised." << endl;
            }
        }
        else if(element.name == "face")
        {
            for(unsigned int j = 0; j < element.properties.size(); j++)
            {
                if(element.properties[j]->type != "list") break;
                PlyListProperty *listProperty = static_cast<PlyListProperty *>(element.properties[j]);

                assert(listProperty->data.size() > 0);
                faces = Array2D<unsigned int>(listProperty->data[0].size(), element.numElements);

                if((listProperty->name == "vertex_indices") || (listProperty->name == "vertex_index"))
                {
                    for(unsigned int k = 0; k < listProperty->data.size(); k++)
                    {
                        assert(static_cast<int>(listProperty->data[k].size()) == faces.dim1());
                        for(unsigned int l = 0; l < listProperty->data[k].size(); l++)
                            faces(l, k) = stream_cast<unsigned int>(listProperty->data[k][l]);
                    }
                }
                else
                    assert(false);
            }
        }
        else
            cout << "ply element " << element.name << " not recognised." << endl;
    }

    computeFaceNormals();
    computeVertexNormals();
}

Mesh::~Mesh()
{
}

const Array2D<double>& Mesh::getVertices() const
{
    return vertices;
}

const Array2D<unsigned int>& Mesh::getFaces() const
{
    return faces;
}

const Array2D<double>& Mesh::getColours() const
{
    return colours;
}

const Array2D<double>& Mesh::getVertexNormals() const
{
    return vertexNormals;
}

void Mesh::computeVertexNormals()
{
std::cout << faces.dim2() << "  " << faceNormals.dim2() << std::endl;
    assert(faces.dim2() == faceNormals.dim2());

    vector<Vector3D> tempNormals(vertices.dim2());
    for(int i = 0; i < faces.dim1(); i++)
        for(int j = 0; j < faces.dim2(); j++)
            tempNormals[faces(i, j)] += Vector3D(faceNormals(0, i), faceNormals(1, i), faceNormals(2, i));

    vertexNormals = Array2D<double>(3, vertices.dim2());
    for(unsigned int i = 0; i < tempNormals.size(); i++)
    {
        tempNormals[i].normalise();
        vertexNormals(0, i) = tempNormals[i][0];
        vertexNormals(1, i) = tempNormals[i][1];
        vertexNormals(2, i) = tempNormals[i][2];
    }

    cout << "Computed vertex normals." << endl;
}

void Mesh::computeFaceNormals()
{
    faceNormals = Array2D<double>(3, faces.dim2());

    for(int i = 0; i < faces.dim2(); i++)
    {
        Vector3D v1(vertices(0, faces(0, i)), vertices(1, faces(0, i)), vertices(2, faces(0, i)));
        Vector3D v2(vertices(0, faces(1, i)), vertices(1, faces(1, i)), vertices(2, faces(1, i)));
        Vector3D v3(vertices(0, faces(2, i)), vertices(1, faces(2, i)), vertices(2, faces(2, i)));

        Vector3D n = Vector3D::normal(v1, v2, v3);
        if((n*n) < EPSILON)
            cout << "Face normal " << i << " not properly computed." << endl;

        n.normalise();
        faceNormals(0, i) = n[0];
        faceNormals(1, i) = n[1];
        faceNormals(2, i) = n[2];
    }

    cout << "Computed face normals." << endl;
}

void Mesh::print(ostream &out) const
{
    out << "Vertex" << endl;
    for(int i = 0; i < vertices.dim2(); i++)
    {
        out << vertices(0, i) << " " << vertices(1, i) << " " << vertices(2, i) << " ";
        out << confidences[i] << " ";
        out << colours(0, i) << " " << colours(1, i) << " " << colours(2, i) << endl;;
    }
    cout << endl;

    out << "Face" << endl;
    for(int i = 0; i < faces.dim2(); i++)
    {
        for(int j = 0; j < faces.dim1(); j++)
            out << faces(j, i) << " ";
        out << endl;
    }
    cout << endl;
}
