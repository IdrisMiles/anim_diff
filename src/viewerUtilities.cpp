#include "include/viewerUtilities.h"


glm::mat4 ViewerUtilities::ConvertToGlmMat(const aiMatrix4x4 &m)
{
    glm::mat4 a(  m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);

    return a;
}

void ViewerUtilities::ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<BoneTransformData> &_boneInfo, const aiMatrix4x4 _globalInverseTransform, const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform)
{
    if(!_pAnimation || !_pNode)
    {
        return;
    }

    std::string nodeName(_pNode->mName.data);

    // Set defualt to bind pose
    aiMatrix4x4 nodeTransform(_pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(_pAnimation, nodeName);


    // Check if valid aiNodeAnim, as not all aiNode have corresponding aiNodeAnim.
    if (pNodeAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D scalingVec;
        CalcInterpolatedScaling(scalingVec, _animationTime, pNodeAnim);
        aiMatrix4x4 scalingMat = aiMatrix4x4();
        aiMatrix4x4::Scaling(scalingVec, scalingMat);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion rotationQ = aiQuaternion();
        CalcInterpolatedRotation(rotationQ, _animationTime, pNodeAnim);
        aiMatrix4x4 rotationMat = aiMatrix4x4();
        rotationMat = aiMatrix4x4(rotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D translationVec;
        CalcInterpolatedPosition(translationVec, _animationTime, pNodeAnim);
        aiMatrix4x4 translationMat = aiMatrix4x4();
        aiMatrix4x4::Translation(translationVec, translationMat);

        // Combine the above transformations
        nodeTransform = translationMat * rotationMat * scalingMat;
    }

    aiMatrix4x4 globalTransformation = _parentTransform * nodeTransform;

    if (_boneMapping.find(nodeName) != _boneMapping.end()) {
        uint BoneIndex = _boneMapping.at(nodeName);
        _boneInfo[BoneIndex].finalTransform = _globalInverseTransform * globalTransformation * _boneInfo[BoneIndex].boneOffset;
    }

    for (uint i = 0 ; i < _pNode->mNumChildren ; i++) {
        ReadNodeHierarchy(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _pAnimation, _pNode->mChildren[i], globalTransformation);
    }

}


const aiNodeAnim* ViewerUtilities::FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName)
{
    for (uint i = 0 ; i < _pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = _pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == _nodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}


void ViewerUtilities::CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumRotationKeys == 1) {
        _out = _pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotationKeyFrame(_animationTime, _pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < _pNodeAnim->mNumRotationKeys);
    float DeltaTime = _pNodeAnim->mRotationKeys[NextRotationIndex].mTime - _pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = _pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = _pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(_out, StartRotationQ, EndRotationQ, Factor);
    _out = _out.Normalize();
}


void ViewerUtilities::CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumPositionKeys == 1) {
        _out = _pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPositionKeyFrame(_animationTime, _pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < _pNodeAnim->mNumPositionKeys);
    float DeltaTime = _pNodeAnim->mPositionKeys[NextPositionIndex].mTime - _pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& startPositionV = _pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& endPositionV = _pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    _out = startPositionV + (Factor*(endPositionV-startPositionV));

}


void ViewerUtilities::CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    // we need at least two values to interpolate...
    if (_pNodeAnim->mNumScalingKeys == 1) {
        _out = _pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint scalingIndex = FindScalingKeyFrame(_animationTime, _pNodeAnim);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _pNodeAnim->mNumScalingKeys);
    float DeltaTime = _pNodeAnim->mScalingKeys[nextScalingIndex].mTime - _pNodeAnim->mScalingKeys[scalingIndex].mTime;
    float Factor = (_animationTime - (float)_pNodeAnim->mScalingKeys[scalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& startScalingV = _pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& endScalingV = _pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
    _out = startScalingV + (Factor*(endScalingV-startScalingV));

}


uint ViewerUtilities::FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}


uint ViewerUtilities::FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumPositionKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}


uint ViewerUtilities::FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
{
    assert(_pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < _pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (_animationTime < (float)_pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}

const aiNode* ViewerUtilities::getParentBone(const std::map<std::__cxx11::string, unsigned int> _boneMapping, const aiNode* _pNode)
{
    if(_pNode->mParent != NULL)
    {
        if(_boneMapping.find(std::string(_pNode->mName.data)) != _boneMapping.end())
        {
            return _pNode->mParent;
        }
        else
        {
            return getParentBone(_boneMapping, _pNode->mParent);
        }
    }
    else
    {
        return NULL;
    }
}
