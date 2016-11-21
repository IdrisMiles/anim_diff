#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>

struct BoneTransformData
{
    aiMatrix4x4 boneOffset;
    aiMatrix4x4 finalTransform;

    glm::mat4 boneOffset2;
    glm::mat4 finalTransform2;
};

#endif // BONETRANSFORM_H
