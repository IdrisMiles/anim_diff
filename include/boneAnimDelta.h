#ifndef BONEANIMDELTA_H
#define BONEANIMDELTA_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class BoneAnimDelta
{
public:
    BoneAnimDelta();

private:
    std::vector<glm::vec3> m_posDelta;
    std::vector<glm::vec3> m_scaleDelta;
    std::vector<glm::quat> m_rotDelta;
};

#endif // BONEANIMDELTA_H
