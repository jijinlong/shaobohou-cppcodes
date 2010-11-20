#ifndef PLY_READER_H
#define PLY_READER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>

class PlyProperty
{
    public:
        std::string type;
        std::string name;
        std::vector<std::string> data;

        virtual ~PlyProperty();

        virtual PlyProperty* makeCopy() const;

        virtual void print(std::ostream &out) const;
        virtual void printData(std::ostream &out, unsigned int index) const;
};

class PlyListProperty: public PlyProperty
{
    public:
        std::string numberType, elementType;
        std::vector<std::vector<std::string> > data;

        virtual ~PlyListProperty();

        virtual PlyListProperty* makeCopy() const;

        virtual void print(std::ostream &out) const;
        virtual void printData(std::ostream &out, unsigned int index) const;
};

class PlyElement
{
    public:
        std::string name;
        unsigned int numElements;
        std::vector<PlyProperty *> properties;

        PlyElement();
        PlyElement(const PlyElement &other);
        ~PlyElement();

        PlyElement& operator=(const PlyElement &other);
        void deepCopy(const PlyElement &other);

        void print(std::ostream &out) const;
        void printData(std::ostream &out) const;
};

class PlyFile
{
    public:
        std::string format;
        std::vector<PlyElement> elements;
        std::vector<std::pair<unsigned int, std::string> > comments;

        PlyFile();
        explicit PlyFile(const std::string &filename);

        void print(std::ostream &out) const;
        void printData(std::ostream &out) const;

    private:
        void readFile(const std::string &filename);
        void readHeader(std::ifstream &in, unsigned int &lineCount);
        void readElement(const std::vector<std::string> &lines);
        void readData(std::ifstream &in, const PlyElement &element);
};

#endif
