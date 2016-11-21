#include "include/viewerUtilities.h"

void ViewerUtilities::ReadNodeHierarchy2(const glm::mat4 _globalInverseTransform, const float _animationTime, const ModelRig *_pRig, Bone* _pBone, const glm::mat4& _parentTransform)
{
    if(!_pRig || !_pBone)
    {
        return;
    }

    // Set defualt to bind pose
    glm::mat4 nodeTransform(_pBone->m_transform);

    const BoneAnim* pBoneAnim = _pBone->m_boneAnim;// FindNodeAnim(_pRig, _pBone->m_name);


    // Check if valid aiNodeAnim, as not all aiNode have corresponding aiNodeAnim.
    if (pBoneAnim)
    {
        // Interpolate scaling and generate scaling transformation matrix
        glm::vec3 scalingVec;
        CalcInterpolatedScaling2(scalingVec, _animationTime, pBoneAnim);
        glm::mat4 scalingMat;
        scalingMat = glm::scale(scalingMat, scalingVec);

        // Interpolate rotation and generate rotation transformation matrix
        glm::quat rotationQ;
        CalcInterpolatedRotation2(rotationQ, _animationTime, pBoneAnim);
        glm::mat4 rotationMat(1.0f);
        rotationMat = glm::mat4_cast(rotationQ);

        // Interpolate translation and generate translation transformation matrix
        glm::vec3 translationVec;
        CalcInterpolatedPosition2(translationVec, _animationTime, pBoneAnim);
        glm::mat4 translationMat;
        translationMat = glm::translate(translationMat, translationVec);

        // Combine the above transformations
        nodeTransform = translationMat * rotationMat * scalingMat;
    }

    glm::mat4 globalTransformation = _parentTransform * nodeTransform;

    if (!_pBone->m_name.empty())
    {
        _pBone->m_currentTransform = _globalInverseTransform * globalTransformation * _pBone->m_boneOffset;
    }

    for (uint i = 0 ; i < _pBone->m_children.size() ; i++) {
        ReadNodeHierarchy2(_globalInverseTransform, _animationTime, _pRig, _pBone->m_children[i], globalTransformation);
    }

}

BoneAnim ViewerUtilities::FindBoneAnim(ModelRig _pRig, std::string _nodeName)
{
    if(_pRig.m_boneAnims.find(_nodeName) != _pRig.m_boneAnims.end())
    {
        return _pRig.m_boneAnims[_nodeName];
    }

    return BoneAnim();
}

void ViewerUtilities::CalcInterpolatedRotation2(glm::quat& _out, const float _animationTime, const BoneAnim* _pBoneAnin)
{
    // we need at least two values to interpolate...
    if (_pBoneAnin->m_rot.size() == 1) {
        _out = _pBoneAnin->m_rot[0];
        return;
    }

    uint RotationIndex = FindKeyFrame(_animationTime, _pBoneAnin);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < _pBoneAnin->m_rot.size());
    float DeltaTime = _pBoneAnin->m_time[NextRotationIndex] - _pBoneAnin->m_time[RotationIndex];
    float Factor = (_animationTime - _pBoneAnin->m_time[RotationIndex]) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::quat StartRotationQ = _pBoneAnin->m_rot[RotationIndex];
    glm::quat EndRotationQ = _pBoneAnin->m_rot[NextRotationIndex];
    _out = glm::mix(StartRotationQ, EndRotationQ, Factor);
    glm::normalize(_out);
}


void ViewerUtilities::CalcInterpolatedPosition2(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim)
{
    // we need at least two values to interpolate...
    if (_boneAnim->m_pos.size() == 1) {
        _out = _boneAnim->m_pos[0];
        return;
    }

    uint PositionIndex = FindKeyFrame(_animationTime, _boneAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < _boneAnim->m_pos.size());
    float DeltaTime = _boneAnim->m_time[NextPositionIndex] - _boneAnim->m_time[PositionIndex];
    float Factor = (_animationTime - _boneAnim->m_time[PositionIndex]) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startPositionV = _boneAnim->m_pos[PositionIndex];
    glm::vec3 endPositionV = _boneAnim->m_pos[NextPositionIndex];
    _out = startPositionV + (Factor*(endPositionV-startPositionV));

}


void ViewerUtilities::CalcInterpolatedScaling2(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim)
{
    // we need at least two values to interpolate...
    if (_boneAnim->m_scale.size() == 1) {
        _out = _boneAnim->m_scale[0];
        return;
    }

    uint scalingIndex = FindKeyFrame(_animationTime, _boneAnim);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _boneAnim->m_scale.size());
    float DeltaTime = _boneAnim->m_time[nextScalingIndex] - _boneAnim->m_time[scalingIndex];
    float Factor = (_animationTime - (float)_boneAnim->m_time[scalingIndex]) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startScalingV = _boneAnim->m_scale[scalingIndex];
    glm::vec3 endScalingV = _boneAnim->m_scale[nextScalingIndex];
    _out = startScalingV + (Factor*(endScalingV-startScalingV));

}


uint ViewerUtilities::FindKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim)
{
    assert(_pBoneAnim->m_rot.size() > 0);

    for (uint i = 0 ; i < _pBoneAnim->m_rot.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnim->m_time[i + 1]) {
            return i;
        }
    }

    assert(0);
}



const Bone* ViewerUtilities::getParentBone2(const Bone* _pNode)
{
    if(_pNode->m_parent != NULL)
    {
        if(!_pNode->m_parent->m_name.empty())
        {
            return _pNode->m_parent;
        }
        else
        {
            return getParentBone2(_pNode->m_parent);
        }
    }
    else
    {
        return NULL;
    }
}
