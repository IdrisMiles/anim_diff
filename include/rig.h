#ifndef RIG_H
#define RIG_H

#include <vector>
#include <memory>

class Bone;

class Rig
{
public:
    Rig();
    virtual ~Rig() {};

//protected:
    std::shared_ptr<Bone> m_rootBone;

};

#endif // RIG_H
