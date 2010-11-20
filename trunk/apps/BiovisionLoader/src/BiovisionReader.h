#ifndef BIOVISION_READER_H
#define BIOVISION_READER_H

#include "KinematicModel.h"
#include "BiovisionJoint.h"
#include "BiovisionAnimation.h"

#include <string>
#include <vector>


typedef KinematicModel<BiovisionJoint> BiovisionModel;

class BiovisionBone
{
    public:
        std::string name;

        Vector3D offset;

        std::vector<int> orders;

        int parent;
        std::vector<int> children;


        BiovisionBone();
        BiovisionBone(const std::string &bonename);
};

class BiovisionReader
{
    public:
        BiovisionReader();
        BiovisionReader(const std::string &filename);

        const BiovisionModel& model() const;
        const BiovisionAnimation& animation() const;

    private:
        std::vector<BiovisionBone> bones;
        int total_nchannels;

        BiovisionModel m_model;
        BiovisionAnimation m_animation;

        void parse(const std::vector<std::string> &lines);
        int parseHierarchy(const std::vector<std::string> &lines, int start);
        int parseMotion(const std::vector<std::string> &lines, int start);   

        void buildModel(); 
};

void setPose(BiovisionModel &model, const std::vector<double> &pose);

#endif
