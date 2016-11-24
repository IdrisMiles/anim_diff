#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#include <glm/glm.hpp>

struct BoneTransformData
{
    glm::mat4 boneOffset2;
    glm::mat4 finalTransform2;
};

#endif // BONETRANSFORM_H
