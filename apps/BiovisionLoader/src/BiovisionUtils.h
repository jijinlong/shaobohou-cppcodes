#ifndef BIOVISION_UTILS_H
#define BIOVISION_UTILS_H

#include "BiovisionReader.h"
#include "BiovisionAnimation.h"

#include <iostream>

void printJointPositionsHeader(std::ostream &out, const BiovisionModel &model);
void printJointPositions(std::ostream &out, const BiovisionModel &model, const BiovisionAnimation &animation);

void printJointAnglesHeader(std::ostream &out, const BiovisionModel &model);
void printJointAngles(std::ostream &out, const BiovisionModel &model, const BiovisionAnimation &animation);


#endif
