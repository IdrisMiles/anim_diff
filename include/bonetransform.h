#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

struct BoneTransformData
{
    aiMatrix4x4 boneOffset;
    aiMatrix4x4 finalTransform;
};

#endif // BONETRANSFORM_H
