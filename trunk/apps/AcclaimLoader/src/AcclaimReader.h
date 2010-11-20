#ifndef ACCLAIM_READER_H
#define ACCLAIM_READER_H

#include "AcclaimModel.h"

#include <vector>
#include <string>
#include <map>
#include <fstream>


class AcclaimBone
{
    public:
        std::string name;
        int id;

        Vector3D global_direction;
        double length;
        Vector3D axis;

        std::vector<int> orders;

        int parent;
        std::vector<int> children;


        // auxilliary variables
        Quaternion global_orientation;
        Quaternion local_orientation_offset;
        Vector3D local_direction;


        AcclaimBone();
        AcclaimBone(const std::string &bonename);
};

class AcclaimReader
{
    public:
        AcclaimReader();
        AcclaimReader(const std::string &filename);

        void parseSkeleton(const std::vector<std::string> &lines);

        void computeLocalParameters();

        void buildModel();

        const AcclaimModel& model() const;

    private:
        std::string version;
        std::string name;

        double mass;
        double length;
        std::string angle;

        Vector3D root_position;
        Quaternion root_orientation;
        Vector3D root_angles;

        std::vector<AcclaimBone> bones;

        std::map<std::string, int> dof2index;
        std::map<std::string, int> bone2index;

        AcclaimModel m_model;


        int parseUnits(const std::vector<std::string> &lines, int start);
        int parseRoot(const std::vector<std::string> &lines, int start);
        int parseBoneData(const std::vector<std::string> &lines, int start);
        int parseHierarchy(const std::vector<std::string> &lines, int start);

        void computeLocalOffset(int ind);
};

#endif
