#include "PlyReader.h"

#include <cassert>

using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::ios;
using std::string;
using std::stringstream;
using std::streampos;
using std::vector;
using std::pair;

PlyProperty::~PlyProperty()
{
}

PlyProperty* PlyProperty::makeCopy() const
{
    return new PlyProperty(*this);
}

PlyListProperty::~PlyListProperty()
{
}

PlyListProperty* PlyListProperty::makeCopy() const
{
    return new PlyListProperty(*this);
}

PlyElement::PlyElement()
{
    numElements = 0;
}

PlyElement::PlyElement(const PlyElement &other)
{
    deepCopy(other);
}

PlyElement::~PlyElement()
{
    for(unsigned int i = 0; i < properties.size(); i++)
        delete properties[i];
}

PlyElement& PlyElement::operator=(const PlyElement &other)
{
    deepCopy(other);
    return *this;
}

void PlyElement::deepCopy(const PlyElement &other)
{
    this->name = other.name;
    this->numElements = other.numElements;
    for(unsigned int i = 0; i < other.properties.size(); i++)
        this->properties.push_back(other.properties[i]->makeCopy());
}

PlyFile::PlyFile()
{
}

PlyFile::PlyFile(const string &filename)
{
    readFile(filename);
}

void PlyFile::readFile(const string &filename)
{
	ifstream in(filename.c_str(), ios::in);
	if(in.fail())
    {
        cout << "Failed to load open file " << filename << endl;
        return;
    }

    unsigned int lineCount = 0;
    string line, keyword;
    if(!in.eof())
    {
        getline(in, line);
        lineCount++;
        stringstream ss(line);
        ss >> keyword;

        if(keyword != "ply")
        {
            cout << "Not ply file." << endl;
            in.close();
            return;
        }
    }
    if(!in.eof())
    {
        getline(in, line);
        lineCount++;
        stringstream ss(line);
        ss >> keyword;
        ss >> format;

        if((keyword != "format") || (format != "ascii"))
        {
            cout << "Not ascii ply file." << endl;
            in.close();
            return;
        }
    }

    readHeader(in, lineCount);
    print(cout);

    for(unsigned int i = 0; i < elements.size(); i++)
        readData(in, elements[i]);

    in.close();
}

void PlyFile::readHeader(ifstream &in, unsigned int &lineCount)
{
    string line, keyword;
    vector<string> propertyLines;
    while(!in.eof())
    {
        getline(in, line);
        lineCount++;
        stringstream ss(line);
        ss >> keyword;

        if(keyword == "element")
        {   // create the last element with all its property lines
            if(propertyLines.size() > 0)
            {
                readElement(propertyLines);
                propertyLines.clear();
            }

            elements.push_back(PlyElement());
            ss >> elements.back().name;
            ss >> elements.back().numElements;

            //readElement(in, lineCount);
        }
        else if(keyword == "property")
            propertyLines.push_back(line);
        else if(keyword == "comment")
            comments.push_back(pair<unsigned int, string>(lineCount-1, line));
        else if(keyword == "end_header")
        {   // create the last element with all its property lines
            if(propertyLines.size() > 0)
                readElement(propertyLines);

            return;
        }
        else
            cout << "Unrecognised line: " << line << endl;
    }
}

void PlyFile::readElement(const vector<string> &lines)
{
    assert(elements.size() > 0);

    for(unsigned int i = 0; i < lines.size(); i++)
    {
        stringstream ss(lines[i]);

        string keyword;
        ss >> keyword;
        assert(keyword == "property");

        string propertyType;
        ss >> propertyType;
        if(propertyType == "list")
        {
            PlyListProperty *newList = new PlyListProperty();
            newList->type = propertyType;
            ss >> newList->numberType;
            ss >> newList->elementType;
            ss >> newList->name;
            elements.back().properties.push_back(newList);
        }
        else
        {
            PlyProperty *newScalar = new PlyProperty();
            newScalar->type = propertyType;
            ss >> newScalar->name;
            elements.back().properties.push_back(newScalar);
        }
    }
}

void PlyFile::readData(ifstream &in, const PlyElement &element)
{
    unsigned int count = 0;
    string line, keyword;

    while(!in.eof())
    {
        getline(in, line);
        stringstream ss(line);

        if(line.find(element.name) == 0)
            continue;

        for(unsigned int i = 0; i < element.properties.size(); i++)
        {
            if(element.properties[i]->type == "list")
            {
                PlyListProperty *tempList = static_cast<PlyListProperty *>(element.properties[i]);
                if(tempList->data.size() == 0)
                    tempList->data = vector<vector<string> >(element.numElements);

                unsigned int listSize;
                ss >> listSize;
                tempList->data[count] = vector<string>(listSize);
                for(unsigned int j = 0; j < listSize; j++)
                    ss >> tempList->data[count][j];
            }
            else
            {
                PlyProperty *tempScalar = element.properties[i];
                if(tempScalar->data.size() == 0)
                    tempScalar->data = vector<string>(element.numElements);

                ss >> tempScalar->data[count];
            }
        }

        count++;
        if(count == element.numElements)
            break;
    }

    assert(count == element.numElements);
    for(unsigned int i = 0; i < element.properties.size(); i++)
    {
        if(element.properties[i]->type == "list")
        {
            PlyListProperty *tempList = static_cast<PlyListProperty *>(element.properties[i]);
            assert(element.numElements == tempList->data.size());
        }
        else
            assert(element.numElements == element.properties[i]->data.size());
    }
}


// print functions
void PlyProperty::print(ostream &out) const
{
    out << "property " << type << " " << name << endl;
}

void PlyListProperty::print(ostream &out) const
{
    out << "property " << type << " " << numberType << " " << elementType << " " << name << endl;
}

void PlyElement::print(ostream &out) const
{
    out << "element " << name << " " << numElements << endl;

    for(unsigned int i = 0; i < properties.size(); i++)
    {
        out << "\t";
        properties[i]->print(out);
    }
}

void PlyFile::print(ostream &out) const
{
    out << "format " << format << endl;
    for(unsigned int i = 0; i < elements.size(); i++)
        elements[i].print(out);

    for(unsigned int i = 0; i < comments.size(); i++)
        out << comments[i].first << " " << comments[i].second << endl;
}

void PlyProperty::printData(ostream &out, unsigned int index) const
{
    assert(index < data.size());
    out << data[index] << " ";
}

void PlyListProperty::printData(ostream &out, unsigned int index) const
{
    assert(index < data.size());
    out << data[index].size() << " ";
    for(unsigned int i = 0; i < data[index].size(); i++)
        out << data[index][i] << " ";
}

void PlyElement::printData(ostream &out) const
{
    for(unsigned int i = 0; i < numElements; i++)
    {
        for(unsigned int j = 0; j < properties.size(); j++)
            properties[j]->printData(out, i);

        out << endl;
    }
}

void PlyFile::printData(ostream &out) const
{
    for(unsigned int i = 0; i < elements.size(); i++)
        elements[i].printData(out);
}
