#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#include <assimp/matrix4x4.h>

struct BoneTransformData
{
    aiMatrix4x4 boneOffset;
    aiMatrix4x4 finalTransform;
};

#endif // BONETRANSFORM_H
