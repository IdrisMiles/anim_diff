#ifndef BONE_H
#define BONE_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "boneAnim.h"
#include "boneAnimDiff.h"


class Bone
{
public:
    Bone();

    Bone* m_parent;
    std::vector<Bone*> m_children;

    std::string m_name;
    glm::mat4 m_transform;
    glm::mat4 m_boneOffset;
    glm::mat4 m_currentTransform;

    BoneAnim *m_boneAnim;
    BoneAnimDiff *m_boneAnimDiff;
};

#endif // BONE_H
