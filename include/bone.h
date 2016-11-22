#ifndef BONE_H
#define BONE_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "boneAnim.h"
#include "boneAnimDiff.h"

#include <memory>

class Bone
{
public:
    Bone();

    std::shared_ptr<Bone> m_parent;
    std::vector<std::shared_ptr<Bone>> m_children;

    std::string m_name;
    uint m_boneID;
    glm::mat4 m_transform;
    glm::mat4 m_boneOffset;
    glm::mat4 m_currentTransform;

    BoneAnim *m_boneAnim;
    BoneAnimDiff *m_boneAnimDiff;
};

#endif // BONE_H
