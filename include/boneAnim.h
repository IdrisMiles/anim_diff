#ifndef BONEANIM_H
#define BONEANIM_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class BoneAnim
{
public:
    BoneAnim();

    std::string m_name;
    std::vector<glm::vec3> m_pos;
    std::vector<glm::vec3> m_scale;
    std::vector<glm::quat> m_rot;
    std::vector<float> m_time;

};

#endif // BONEANIM_H
