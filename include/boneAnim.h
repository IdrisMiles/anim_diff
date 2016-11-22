#ifndef BONEANIM_H
#define BONEANIM_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct PosAnim
{
    float time;
    glm::vec3 pos;
};

struct ScaleAnim
{
    float time;
    glm::vec3 scale;
};

struct RotAnim
{
    float time;
    glm::quat rot;
};

class BoneAnim
{
public:
    BoneAnim();

    std::string m_name;
    std::vector<PosAnim> m_posAnim;
    std::vector<ScaleAnim> m_scaleAnim;
    std::vector<RotAnim> m_rotAnim;

};

#endif // BONEANIM_H
