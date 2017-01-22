
#include "viewerUtilities.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <float.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for ASSIMP animation structures
//-----------------------------------------------------------------------------------------------------------------------------------------


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


const aiBone* ViewerUtilities::GetBone(const aiScene *_aiScene, std::string _name)
{
    for(unsigned int i=0; i<_aiScene->mNumMeshes; i++)
    {
        for(unsigned int j=0; j<_aiScene->mMeshes[i]->mNumBones; j++)
        {
            if(std::string(_aiScene->mMeshes[i]->mBones[j]->mName.data) == _name)
            {
                return _aiScene->mMeshes[i]->mBones[j];
            }
        }
    }

    return NULL;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for Conversion
//-----------------------------------------------------------------------------------------------------------------------------------------
glm::mat4 ViewerUtilities::ConvertToGlmMat(const aiMatrix4x4 &m)
{
    glm::mat4 a(  m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
    return a;
}


BoneAnim ViewerUtilities::TransferAnim(const aiNodeAnim *_pNodeAnim)
{
    BoneAnim newBoneAnim;

    if(_pNodeAnim != NULL)
    {
        // Rotation animation
        for(unsigned int i=0; i<_pNodeAnim->mNumRotationKeys; i++)
        {
            float time = _pNodeAnim->mRotationKeys[i].mTime;
            glm::quat rot;
            rot.x = _pNodeAnim->mRotationKeys[i].mValue.x;
            rot.y = _pNodeAnim->mRotationKeys[i].mValue.y;
            rot.z = _pNodeAnim->mRotationKeys[i].mValue.z;
            rot.w = _pNodeAnim->mRotationKeys[i].mValue.w;
            RotAnim rotAnim = {time, rot};
            newBoneAnim.m_rotAnim.push_back(rotAnim);
        }

        // Position animation
        for(unsigned int i=0; i<_pNodeAnim->mNumPositionKeys; i++)
        {
            float time = _pNodeAnim->mPositionKeys[i].mTime;
            glm::vec3 pos;
            pos.x = _pNodeAnim->mPositionKeys[i].mValue.x;
            pos.y = _pNodeAnim->mPositionKeys[i].mValue.y;
            pos.z = _pNodeAnim->mPositionKeys[i].mValue.z;
            PosAnim posAnim = {time, pos};
            newBoneAnim.m_posAnim.push_back(posAnim);
        }

        // Scaling animation
        for(unsigned int i=0; i<_pNodeAnim->mNumScalingKeys; i++)
        {
            float time = _pNodeAnim->mScalingKeys[i].mTime;
            glm::vec3 scale;
            scale.x = _pNodeAnim->mScalingKeys[i].mValue.x;
            scale.y = _pNodeAnim->mScalingKeys[i].mValue.y;
            scale.z = _pNodeAnim->mScalingKeys[i].mValue.z;
            ScaleAnim scaleAnim = {time, scale};
            newBoneAnim.m_scaleAnim.push_back(scaleAnim);
        }
    }
    else
    {
        std::cout<<"Invalid aiNodeAnim\n";
    }

    return newBoneAnim;
}

void ViewerUtilities::CopyRigStructure(const std::map<std::string, unsigned int> &_boneMapping, const aiScene *_aiScene, aiNode *_aiNode, std::shared_ptr<ModelRig> _rig, std::shared_ptr<Bone> _parentBone, const glm::mat4 &_parentTransform)
{
    std::shared_ptr<Bone> newBone = std::shared_ptr<Bone>(new Bone());
    glm::mat4 newParentTransform = _parentTransform;

    // Check if this is a bone
    if(_boneMapping.find(std::string(_aiNode->mName.data)) != _boneMapping.end())
    {
        newBone->m_name = std::string(_aiNode->mName.data);
        newBone->m_transform = ViewerUtilities::ConvertToGlmMat(_aiNode->mTransformation) * _parentTransform;

        // Get bone offset matrix
        const aiBone* paiBone = ViewerUtilities::GetBone(_aiScene, newBone->m_name);
        if(paiBone)
        {
            newBone->m_boneOffset = ViewerUtilities::ConvertToGlmMat(paiBone->mOffsetMatrix);
        }
        else
        {
            std::cout<<"Well sheet, didn't find a bone in aiScene with name matching: "<<newBone->m_name<<". Thus not boneOffsetMatrix\n";
            newBone->m_boneOffset = glm::mat4(1.0f);
        }

        // Get animation data
        const aiNodeAnim *pNodeAnim = ViewerUtilities::FindNodeAnim(_aiScene->mAnimations[_aiScene->mNumAnimations-1], newBone->m_name);
        if(pNodeAnim)
        {
            _rig->m_boneAnims[newBone->m_name] = ViewerUtilities::TransferAnim(pNodeAnim);
            newBone->m_boneAnim = &_rig->m_boneAnims[newBone->m_name];
        }
        else
        {
            std::cout<<"Daaannnng, didn't find aiNodeAnim in aiAnimation["<<_aiScene->mNumAnimations<<"] with matching name: "<<newBone->m_name<<". Thus No animationz so creating blank animation.\n";
            BoneAnim blankAnim;
            blankAnim.m_name = newBone->m_name;
            blankAnim.m_posAnim.push_back(PosAnim(0.0f, glm::vec3(0, 0, 0)));
            blankAnim.m_scaleAnim.push_back(ScaleAnim(0.0f, glm::vec3(1, 1, 1)));
            //blankAnim.m_rotAnim.push_back(RotAnim(0.0f, glm::quat(0,0,0,1)));

            _rig->m_boneAnims[newBone->m_name] = blankAnim;
            newBone->m_boneAnim = &_rig->m_boneAnims[newBone->m_name];
        }

        // Set parent and set child
        newBone->m_parent = _parentBone;
        _parentBone->m_children.push_back(newBone);

        newParentTransform = glm::mat4(1.0f);
    }
    else
    {
        // forward on this nodes transform to affect the next bone
        std::cout<<"Model::CopyRigStructure | "<<std::string(_aiNode->mName.data)<<" Is not a Bone, probably an arbitrary transform.\n";
        newBone = _parentBone;
        newParentTransform = ViewerUtilities::ConvertToGlmMat(_aiNode->mTransformation) * _parentTransform;
    }


    unsigned int numChildren = _aiNode->mNumChildren;
    for (unsigned int i=0; i<numChildren; i++)
    {
        CopyRigStructure(_boneMapping, _aiScene, _aiNode->mChildren[i], _rig, newBone, newParentTransform);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for Animating Our Rig Structure
//-----------------------------------------------------------------------------------------------------------------------------------------

void ViewerUtilities::ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, std::shared_ptr<ModelRig> _pRig, std::shared_ptr<Bone> _pBone, const glm::mat4& _parentTransform)
{
    if(_pBone == nullptr)
    {
        return;
    }

    int BoneIndex = -1;
    if (_boneMapping.find(_pBone->m_name) != _boneMapping.end())
    {
        BoneIndex = _boneMapping.at(_pBone->m_name);
    }

    // Set defualt to bind pose
    glm::mat4 boneTransform(_pBone->m_transform);

    const BoneAnim* pBoneAnim = &_pRig->m_boneAnims[_pBone->m_name];


    // Interpolate scaling and generate scaling transformation matrix
    glm::vec3 scalingVec;
    CalcInterpolatedScaling(scalingVec, _animationTime, pBoneAnim);
    glm::mat4 scalingMat;
    scalingMat = glm::scale(scalingMat, scalingVec);

    // Interpolate rotation and generate rotation transformation matrix
    glm::quat rotationQ;
    CalcInterpolatedRotation(rotationQ, _animationTime, pBoneAnim);
    glm::mat4 rotationMat(1.0f);
    rotationMat = glm::mat4_cast(rotationQ);

    // Interpolate translation and generate translation transformation matrix
    glm::vec3 translationVec;
    CalcInterpolatedPosition(translationVec, _animationTime, pBoneAnim);
    glm::mat4 translationMat;
    translationMat = glm::translate(translationMat, translationVec);

    // Combine the above transformations
    boneTransform =  translationMat * rotationMat * scalingMat;


    glm::mat4 globalTransformation = glm::transpose(boneTransform)*_parentTransform;


    if (BoneIndex != -1)
    {
        _pBone->m_currentTransform = _boneInfo[BoneIndex] = _pBone->m_boneOffset * globalTransformation * _globalInverseTransform;
    }

    for (uint i = 0 ; i < _pBone->m_children.size() ; i++)
    {
        ReadNodeHierarchy(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _pRig, std::shared_ptr<Bone>(_pBone->m_children[i]), globalTransformation);
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

void ViewerUtilities::CalcInterpolatedRotation(glm::quat& _out, const float _animationTime, const BoneAnim* _pBoneAnin)
{
    if (_pBoneAnin->m_rotAnim.size() < 1) {
        glm::mat4 a(1.0f);
        _out = glm::quat_cast(a);
        return;
    }

    // we need at least two values to interpolate...
    if (_pBoneAnin->m_rotAnim.size() == 1) {
        _out = _pBoneAnin->m_rotAnim[0].rot;
        return;
    }

    uint RotationIndex = FindRotationKeyFrame(_animationTime, _pBoneAnin);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < _pBoneAnin->m_rotAnim.size());
    float DeltaTime = _pBoneAnin->m_rotAnim[NextRotationIndex].time - _pBoneAnin->m_rotAnim[RotationIndex].time;
    float Factor = (_animationTime - _pBoneAnin->m_rotAnim[RotationIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::quat StartRotationQ = _pBoneAnin->m_rotAnim[RotationIndex].rot;
    glm::quat EndRotationQ = _pBoneAnin->m_rotAnim[NextRotationIndex].rot;
    _out = glm::slerp(StartRotationQ, EndRotationQ, Factor);
    glm::normalize(_out);
}

void ViewerUtilities::CalcInterpolatedPosition(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim)
{
    if (_boneAnim->m_posAnim.size() < 1) {
        _out = glm::vec3(0.0f, 0.0f, 0.0f);
        return;
    }

    // we need at least two values to interpolate...
    if (_boneAnim->m_posAnim.size() == 1) {
        _out = _boneAnim->m_posAnim[0].pos;
        return;
    }

    uint PositionIndex = FindPositionKeyFrame(_animationTime, _boneAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < _boneAnim->m_posAnim.size());
    float DeltaTime = _boneAnim->m_posAnim[NextPositionIndex].time - _boneAnim->m_posAnim[PositionIndex].time;
    float Factor = (_animationTime - _boneAnim->m_posAnim[PositionIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startPositionV = _boneAnim->m_posAnim[PositionIndex].pos;
    glm::vec3 endPositionV = _boneAnim->m_posAnim[NextPositionIndex].pos;
    _out = startPositionV + (Factor*(endPositionV-startPositionV));

}

void ViewerUtilities::CalcInterpolatedScaling(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim)
{
    if (_boneAnim->m_scaleAnim.size() < 1) {
        _out = glm::vec3(1.0f, 1.0f, 1.0f);
        return;
    }

    // we need at least two values to interpolate...
    if (_boneAnim->m_scaleAnim.size() == 1) {
        _out = _boneAnim->m_scaleAnim[0].scale;
        return;
    }

    uint scalingIndex = FindScalingKeyFrame(_animationTime, _boneAnim);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _boneAnim->m_scaleAnim.size());
    float DeltaTime = _boneAnim->m_scaleAnim[nextScalingIndex].time - _boneAnim->m_scaleAnim[scalingIndex].time;
    float Factor = (_animationTime - (float)_boneAnim->m_scaleAnim[scalingIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startScalingV = _boneAnim->m_scaleAnim[scalingIndex].scale;
    glm::vec3 endScalingV = _boneAnim->m_scaleAnim[nextScalingIndex].scale;
    _out = startScalingV + (Factor*(endScalingV-startScalingV));

}

uint ViewerUtilities::FindRotationKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim)
{
    assert(_pBoneAnim->m_rotAnim.size() > 0);

    for (uint i = 0 ; i < _pBoneAnim->m_rotAnim.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnim->m_rotAnim[i+1].time)
        {
            return i;
        }
    }

    assert(0);
}

uint ViewerUtilities::FindPositionKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim)
{
    assert(_pBoneAnim->m_posAnim.size() > 0);

    for (uint i = 0 ; i < _pBoneAnim->m_posAnim.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnim->m_posAnim[i + 1].time)
        {
            return i;
        }
    }

    assert(0);
}

uint ViewerUtilities::FindScalingKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim)
{
    assert(_pBoneAnim->m_scaleAnim.size() > 0);

    for (uint i = 0 ; i < _pBoneAnim->m_scaleAnim.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnim->m_scaleAnim[i + 1].time)
        {
            return i;
        }
    }

    assert(0);
}



//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for Animating and Visualising Our Rig Difference Structure
//-----------------------------------------------------------------------------------------------------------------------------------------

void ViewerUtilities::ColourBoneDifferences(const std::map<std::string, unsigned int> &_boneMapping, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, DiffRig _pDiffRig, std::vector<glm::vec3> &_rigJointColour)
{
    if(_pMasterBone == nullptr)
    {
        return;
    }

    int BoneIndex = -1;
    if (_boneMapping.find(_pMasterBone->m_name) != _boneMapping.end())
    {
        BoneIndex = _boneMapping.at(_pMasterBone->m_name);
    }

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimDiff* pBoneAnimDiff = &_pDiffRig.m_boneAnimDiffs[_pMasterBone->m_name];

    float delta = _boneDeltas.at(_pMasterBone->m_name);


    // Interpolate scaling and generate scaling transformation matrix
    glm::vec3 masterScalingVec;
    glm::vec3 deltaScalingVec;
    CalcInterpolatedScaling(masterScalingVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedScaling(deltaScalingVec, _animationTime, pBoneAnimDiff);
    glm::vec3 scalingVec = /*masterScalingVec + */(delta * deltaScalingVec);

    // Interpolate rotation and generate rotation transformation matrix
    glm::quat masterRotationQ;
    glm::quat deltaRotationQ;
    CalcInterpolatedRotation(masterRotationQ, _animationTime, pMasterBoneAnim);
    CalcInterpolatedRotation(deltaRotationQ, _animationTime, pBoneAnimDiff);
    glm::quat rotationQ = /*masterRotationQ **/ glm::slerp(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), deltaRotationQ, delta);

    // Interpolate translation and generate translation transformation matrix
    glm::vec3 masterTranslationVec;
    glm::vec3 deltaTranslationVec;
    CalcInterpolatedPosition(masterTranslationVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedPosition(deltaTranslationVec, _animationTime, pBoneAnimDiff);
    glm::vec3 translationVec = /*masterTranslationVec + */(delta * deltaTranslationVec);

    // Check if this keyframe has been flagged as different
    glm::vec3 jointColour = glm::vec3(0.6f,0.6f,0.6f);

    if( (glm::length2(scalingVec) < FLT_EPSILON)        &&
        (glm::length2(translationVec) < FLT_EPSILON)    &&
        (glm::length2(glm::eulerAngles(rotationQ)) < FLT_EPSILON)   )
    {
        // Joint has NO difference - grey
        jointColour = glm::vec3(0.6f,0.6f,0.6f);
    }
    else
    {
        // joint has a difference - blue
        jointColour = glm::vec3(0.0f, 0.0f, 1.0f);
    }


    if (BoneIndex != -1)
    {
        _rigJointColour[BoneIndex] = jointColour;
    }

    for (uint i = 0 ; i < _pMasterBone->m_children.size() ; i++)
    {
        ColourBoneDifferences(_boneMapping, _animationTime, _boneDeltas, _pMasterRig, std::shared_ptr<Bone>(_pMasterBone->m_children[i]), _pDiffRig, _rigJointColour);
    }
}

void ViewerUtilities::ReadNodeHierarchyDiff(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, DiffRig _pDiffRig, const glm::mat4& _parentTransform)
{
    if(_pMasterBone == nullptr)
    {
        return;
    }

    int BoneIndex = -1;
    if (_boneMapping.find(_pMasterBone->m_name) != _boneMapping.end())
    {
        BoneIndex = _boneMapping.at(_pMasterBone->m_name);
    }

    // Set defualt to bind pose
    glm::mat4 nodeTransform(_pMasterBone->m_transform);

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimDiff* pBoneAnimDiff = &_pDiffRig.m_boneAnimDiffs[_pMasterBone->m_name];

    float delta = _boneDeltas.at(_pMasterBone->m_name);


    glm::mat4 scalingMat(1.0f);
    glm::mat4 rotationMat(1.0f);
    glm::mat4 translationMat;

    // Interpolate scaling and generate scaling transformation matrix
    GenerateScalingMatrix(scalingMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);

    // Interpolate rotation and generate rotation transformation matrix
    GenerateRotationMatrix(rotationMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);

    // Interpolate translation and generate translation transformation matrix
    GenerateTranslationMatrix(translationMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);


    // Combine the above transformations
    nodeTransform =  translationMat * rotationMat * scalingMat;


    glm::mat4 globalTransformation = glm::transpose(nodeTransform)*_parentTransform;


    if (BoneIndex != -1)
    {
        _pMasterBone->m_currentTransform = _boneInfo[BoneIndex] = _pMasterBone->m_boneOffset * globalTransformation * _globalInverseTransform;
    }

    for (uint i = 0 ; i < _pMasterBone->m_children.size() ; i++)
    {
        ReadNodeHierarchyDiff(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _boneDeltas, _pMasterRig, std::shared_ptr<Bone>(_pMasterBone->m_children[i]), _pDiffRig, globalTransformation);
    }
}

void ViewerUtilities::CalcInterpolatedRotation(glm::quat& _out, const float _animationTime, const BoneAnimDiff* _pBoneAninDiff)
{
    if (_pBoneAninDiff->m_rotAnimDeltas.size() < 1) {
        glm::mat4 a(1.0f);
        _out = glm::quat_cast(a);
        return;
    }

    // we need at least two values to interpolate...
    if (_pBoneAninDiff->m_rotAnimDeltas.size() == 1) {
        _out = _pBoneAninDiff->m_rotAnimDeltas[0].rot;
        return;
    }

    uint RotationIndex = FindRotationKeyFrame(_animationTime, _pBoneAninDiff);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < _pBoneAninDiff->m_rotAnimDeltas.size());
    float DeltaTime = _pBoneAninDiff->m_rotAnimDeltas[NextRotationIndex].time - _pBoneAninDiff->m_rotAnimDeltas[RotationIndex].time;
    float Factor = (_animationTime - _pBoneAninDiff->m_rotAnimDeltas[RotationIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::quat StartRotationQ = _pBoneAninDiff->m_rotAnimDeltas[RotationIndex].rot;
    glm::quat EndRotationQ = _pBoneAninDiff->m_rotAnimDeltas[NextRotationIndex].rot;
    _out = glm::slerp(StartRotationQ, EndRotationQ, Factor);
    glm::normalize(_out);
}

void ViewerUtilities::CalcInterpolatedPosition(glm::vec3& _out, const float _animationTime, const BoneAnimDiff* _boneAnimDiff)
{
    if (_boneAnimDiff->m_posAnimDeltas.size() < 1) {
        _out = glm::vec3(0.0f, 0.0f, 0.0f);
        return;
    }

    // we need at least two values to interpolate...
    if (_boneAnimDiff->m_posAnimDeltas.size() == 1) {
        _out = _boneAnimDiff->m_posAnimDeltas[0].pos;
        return;
    }

    uint PositionIndex = FindPositionKeyFrame(_animationTime, _boneAnimDiff);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < _boneAnimDiff->m_posAnimDeltas.size());
    float DeltaTime = _boneAnimDiff->m_posAnimDeltas[NextPositionIndex].time - _boneAnimDiff->m_posAnimDeltas[PositionIndex].time;
    float Factor = (_animationTime - _boneAnimDiff->m_posAnimDeltas[PositionIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startPositionV = _boneAnimDiff->m_posAnimDeltas[PositionIndex].pos;
    glm::vec3 endPositionV = _boneAnimDiff->m_posAnimDeltas[NextPositionIndex].pos;
    _out = startPositionV + (Factor*(endPositionV-startPositionV));

}

void ViewerUtilities::CalcInterpolatedScaling(glm::vec3& _out, const float _animationTime, const BoneAnimDiff* _boneAnimDiff)
{
    if (_boneAnimDiff->m_scaleAnimDeltas.size() < 1) {
        _out = glm::vec3(1.0f, 1.0f, 1.0f);
        return;
    }

    // we need at least two values to interpolate...
    if (_boneAnimDiff->m_scaleAnimDeltas.size() == 1) {
        _out = _boneAnimDiff->m_scaleAnimDeltas[0].scale;
        return;
    }

    uint scalingIndex = FindScalingKeyFrame(_animationTime, _boneAnimDiff);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < _boneAnimDiff->m_scaleAnimDeltas.size());
    float DeltaTime = _boneAnimDiff->m_scaleAnimDeltas[nextScalingIndex].time - _boneAnimDiff->m_scaleAnimDeltas[scalingIndex].time;
    float Factor = (_animationTime - (float)_boneAnimDiff->m_scaleAnimDeltas[scalingIndex].time) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    glm::vec3 startScalingV = _boneAnimDiff->m_scaleAnimDeltas[scalingIndex].scale;
    glm::vec3 endScalingV = _boneAnimDiff->m_scaleAnimDeltas[nextScalingIndex].scale;
    _out = startScalingV + (Factor*(endScalingV-startScalingV));

}


uint ViewerUtilities::FindRotationKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff)
{
    assert(_pBoneAnimDiff->m_rotAnimDeltas.size() > 0);

    for (uint i = 0 ; i < _pBoneAnimDiff->m_rotAnimDeltas.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnimDiff->m_rotAnimDeltas[i+1].time)
        {
            return i;
        }
    }

    assert(0);
}

uint ViewerUtilities::FindPositionKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff)
{
    assert(_pBoneAnimDiff->m_posAnimDeltas.size() > 0);

    for (uint i = 0 ; i < _pBoneAnimDiff->m_posAnimDeltas.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnimDiff->m_posAnimDeltas[i + 1].time)
        {
            return i;
        }
    }

    assert(0);
}

uint ViewerUtilities::FindScalingKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff)
{
    assert(_pBoneAnimDiff->m_scaleAnimDeltas.size() > 0);

    for (uint i = 0 ; i < _pBoneAnimDiff->m_scaleAnimDeltas.size() - 1 ; i++) {
        if (_animationTime < (float)_pBoneAnimDiff->m_scaleAnimDeltas[i + 1].time)
        {
            return i;
        }
    }

    assert(0);
}



void ViewerUtilities::GenerateScalingMatrix(glm::mat4 &scalingMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff *pBoneAnimDiff)
{
    glm::vec3 masterScalingVec;
    glm::vec3 deltaScalingVec;
    CalcInterpolatedScaling(masterScalingVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedScaling(deltaScalingVec, _animationTime, pBoneAnimDiff);
    glm::vec3 scalingVec = masterScalingVec + (_delta * deltaScalingVec);
    scalingMat = glm::scale(scalingMat, scalingVec);
}

void ViewerUtilities::GenerateRotationMatrix(glm::mat4 &rotationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff *pBoneAnimDiff)
{
    glm::quat masterRotationQ;
    glm::quat deltaRotationQ;
    CalcInterpolatedRotation(masterRotationQ, _animationTime, pMasterBoneAnim);
    CalcInterpolatedRotation(deltaRotationQ, _animationTime, pBoneAnimDiff);
    glm::quat rotationQ = masterRotationQ * glm::slerp(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)), deltaRotationQ, _delta);
    rotationMat = glm::mat4_cast(rotationQ);
}

void ViewerUtilities::GenerateTranslationMatrix(glm::mat4 &translationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff *pBoneAnimDiff)
{
    glm::vec3 masterTranslationVec;
    glm::vec3 deltaTranslationVec;
    CalcInterpolatedPosition(masterTranslationVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedPosition(deltaTranslationVec, _animationTime, pBoneAnimDiff);
    glm::vec3 translationVec = masterTranslationVec + (_delta * deltaTranslationVec);
    translationMat = glm::translate(translationMat, translationVec);
}




//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for Animating and Visualising Our Rig Merge Structure
//-----------------------------------------------------------------------------------------------------------------------------------------

void ViewerUtilities::ColourBoneMerges(const std::map<std::string, unsigned int> &_boneMapping, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, MergeRig _pMergeRig, std::vector<glm::vec3> &_rigJointColour)
{
    if(_pMasterBone == nullptr)
    {
        return;
    }

    int BoneIndex = -1;
    if (_boneMapping.find(_pMasterBone->m_name) != _boneMapping.end())
    {
        BoneIndex = _boneMapping.at(_pMasterBone->m_name);
    }

    glm::vec3 jointColour = glm::vec3(0.6f,0.6f,0.6f);
    float delta = _boneDeltas.at(_pMasterBone->m_name);
    bool master = false;
    bool branch = false;

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimMerge boneAnimMerge = _pMergeRig.m_boneAnimMerges[_pMasterBone->m_name];

    glm::mat4 scalingMat(1.0f);
    glm::mat4 rotationMat(1.0f);
    glm::mat4 translationMat;

    // Interpolate scaling and generate scaling transformation matrix
    GenerateScalingMatrix(scalingMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);

    // Interpolate rotation and generate rotation transformation matrix
    GenerateRotationMatrix(rotationMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);

    // Interpolate translation and generate translation transformation matrix
    GenerateTranslationMatrix(translationMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);

    if(master && !branch)
    {
        // Branch A - greenish
        jointColour = glm::vec3(0.4f,0.8f,0.4f);
    }
    else if(!master && branch)
    {
        // Branch B - blueish
        jointColour = glm::vec3(0.4f,0.4f,0.8f);
    }
    else if(!master && !branch)
    {
        // parent - grey
        jointColour = glm::vec3(0.6f,0.6f,0.6f);
    }
    else if(master && branch)
    {
        // merge conflict - red
        jointColour = glm::vec3(1.0f,0.0f,0.0f);

        if(delta < 0.4f)
        {
            // fade with A
            jointColour = glm::mix(glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.4f,0.8f,0.4f), fabs(2.0f*(delta-0.5f)));
        }
        else if(delta > 0.6f)
        {
            // fade with B
            jointColour = glm::mix(glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.4f,0.4f,0.8f), fabs(2.0f*(delta-0.5f)));
        }
    }


    if (BoneIndex != -1)
    {
        _rigJointColour[BoneIndex] = jointColour;
    }

    for (uint i = 0 ; i < _pMasterBone->m_children.size() ; i++)
    {
        ColourBoneMerges(_boneMapping, _animationTime, _boneDeltas, _pMasterRig, std::shared_ptr<Bone>(_pMasterBone->m_children[i]), _pMergeRig, _rigJointColour);
    }
}


void ViewerUtilities::ReadNodeHierarchyMerge(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, MergeRig _pMergeRig, const glm::mat4& _parentTransform)
{
    if(_pMasterBone == nullptr)
    {
        return;
    }

    int BoneIndex = -1;
    if (_boneMapping.find(_pMasterBone->m_name) != _boneMapping.end())
    {
        BoneIndex = _boneMapping.at(_pMasterBone->m_name);
    }


    float delta = _boneDeltas.at(_pMasterBone->m_name);
    bool master = false;
    bool branch = false;

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimMerge boneAnimMerge = _pMergeRig.m_boneAnimMerges[_pMasterBone->m_name];

    // Set defualt to bind pose
    glm::mat4 boneTransform(_pMasterBone->m_transform);
    glm::mat4 scalingMat(1.0f);
    glm::mat4 rotationMat(1.0f);
    glm::mat4 translationMat;


    // Interpolate scaling and generate scaling transformation matrix
    GenerateScalingMatrix(scalingMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);

    // Interpolate rotation and generate rotation transformation matrix
    GenerateRotationMatrix(rotationMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);

    // Interpolate translation and generate translation transformation matrix
    GenerateTranslationMatrix(translationMat, _animationTime, delta, pMasterBoneAnim, boneAnimMerge, master, branch);




    // Combine the above transformations
    boneTransform =  translationMat * rotationMat * scalingMat;


    glm::mat4 globalTransformation = glm::transpose(boneTransform)*_parentTransform;


    if (BoneIndex != -1)
    {
        _pMasterBone->m_currentTransform = _boneInfo[BoneIndex] = _pMasterBone->m_boneOffset * globalTransformation * _globalInverseTransform;
    }

    for (uint i = 0 ; i < _pMasterBone->m_children.size() ; i++)
    {
        ReadNodeHierarchyMerge(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _boneDeltas, _pMasterRig, std::shared_ptr<Bone>(_pMasterBone->m_children[i]), _pMergeRig, globalTransformation);
    }
}


void ViewerUtilities::GenerateScalingMatrix(glm::mat4 &scalingMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch)
{
    const BoneAnimDiff* pBoneAnimDiffMaster = &boneAnimMerge.m_mainDiff;
    const BoneAnimDiff* pBoneAnimDiffBranch = &boneAnimMerge.m_branchDiff;
    glm::vec3 masterScalingVec;
    glm::vec3 deltaScalingVecMaster;
    glm::vec3 deltaScalingVecBranch;
    CalcInterpolatedScaling(masterScalingVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedScaling(deltaScalingVecMaster, _animationTime, pBoneAnimDiffMaster);
    CalcInterpolatedScaling(deltaScalingVecBranch, _animationTime, pBoneAnimDiffBranch);

    if(     (glm::length2(deltaScalingVecMaster) < FLT_EPSILON) &&
            !(glm::length2(deltaScalingVecBranch) < FLT_EPSILON))
    {
        branch = true;
        glm::vec3 scalingVec = masterScalingVec + ((1.0f) * deltaScalingVecBranch);
        scalingMat = glm::scale(scalingMat, scalingVec);
    }
    else if(!(glm::length2(deltaScalingVecMaster) < FLT_EPSILON) &&
            (glm::length2(deltaScalingVecBranch) < FLT_EPSILON))
    {
        master = true;
        glm::vec3 scalingVec = masterScalingVec + (1.0f * deltaScalingVecMaster) ;
        scalingMat = glm::scale(scalingMat, scalingVec);
    }
    else if((glm::length2(deltaScalingVecMaster) < FLT_EPSILON) &&
            (glm::length2(deltaScalingVecBranch) < FLT_EPSILON))
    {
        glm::vec3 scalingVec = masterScalingVec + (_delta * deltaScalingVecMaster) + ((1.0f-_delta) * deltaScalingVecBranch);
        scalingMat = glm::scale(scalingMat, scalingVec);
    }
    else if(!(glm::length2(deltaScalingVecMaster) < FLT_EPSILON) &&
            !(glm::length2(deltaScalingVecBranch) < FLT_EPSILON))
    {
        master = true;
        branch = true;
        glm::vec3 scalingVec = masterScalingVec + (_delta * deltaScalingVecMaster) + ((1.0f-_delta) * deltaScalingVecBranch);
        scalingMat = glm::scale(scalingMat, scalingVec);
    }

}

void ViewerUtilities::GenerateRotationMatrix(glm::mat4 &rotationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch)
{
    const BoneAnimDiff* pBoneAnimDiffMaster = &boneAnimMerge.m_mainDiff;
    const BoneAnimDiff* pBoneAnimDiffBranch = &boneAnimMerge.m_branchDiff;
    glm::quat masterRotationQ;
    glm::quat deltaRotationQMaster;
    glm::quat deltaRotationQBranch;
    CalcInterpolatedRotation(masterRotationQ, _animationTime, pMasterBoneAnim);
    CalcInterpolatedRotation(deltaRotationQMaster, _animationTime, pBoneAnimDiffMaster);
    CalcInterpolatedRotation(deltaRotationQBranch, _animationTime, pBoneAnimDiffBranch);

    if( (glm::length2(glm::eulerAngles(deltaRotationQMaster)) < FLT_EPSILON) &&
        !(glm::length2(glm::eulerAngles(deltaRotationQBranch)) < FLT_EPSILON))
    {
        branch = true;
        glm::quat rotationQ = masterRotationQ * deltaRotationQBranch;
        rotationMat = glm::mat4_cast(rotationQ);
    }
    else if(!(glm::length2(glm::eulerAngles(deltaRotationQMaster)) < FLT_EPSILON) &&
            (glm::length2(glm::eulerAngles(deltaRotationQBranch)) < FLT_EPSILON))
    {
        master = true;
        glm::quat rotationQ = masterRotationQ * deltaRotationQMaster;
        rotationMat = glm::mat4_cast(rotationQ);
    }
    else if((glm::length2(glm::eulerAngles(deltaRotationQMaster)) < FLT_EPSILON) &&
            (glm::length2(glm::eulerAngles(deltaRotationQBranch)) < FLT_EPSILON))
    {
        glm::quat rotationQ = masterRotationQ * glm::slerp(deltaRotationQMaster, deltaRotationQBranch, _delta);
        rotationMat = glm::mat4_cast(rotationQ);
    }
    else if(!(glm::length2(glm::eulerAngles(deltaRotationQMaster)) < FLT_EPSILON) &&
            !(glm::length2(glm::eulerAngles(deltaRotationQBranch)) < FLT_EPSILON))
    {
        master = true;
        branch = true;
        glm::quat rotationQ = masterRotationQ * glm::slerp(deltaRotationQMaster, deltaRotationQBranch, _delta);
        rotationMat = glm::mat4_cast(rotationQ);
    }
}

void ViewerUtilities::GenerateTranslationMatrix(glm::mat4 &translationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch)
{
    const BoneAnimDiff* pBoneAnimDiffMaster = &boneAnimMerge.m_mainDiff;
    const BoneAnimDiff* pBoneAnimDiffBranch = &boneAnimMerge.m_branchDiff;
    glm::vec3 masterTranslationVec;
    glm::vec3 deltaTranslationVecMaster;
    glm::vec3 deltaTranslationVecBranch;
    CalcInterpolatedPosition(masterTranslationVec, _animationTime, pMasterBoneAnim);
    CalcInterpolatedPosition(deltaTranslationVecMaster, _animationTime, pBoneAnimDiffMaster);
    CalcInterpolatedPosition(deltaTranslationVecBranch, _animationTime, pBoneAnimDiffBranch);

    if(     (glm::length2(deltaTranslationVecMaster) < FLT_EPSILON) &&
            !(glm::length2(deltaTranslationVecBranch) < FLT_EPSILON))
    {
        branch = true;
        glm::vec3 translationVec = masterTranslationVec + (1.0f * deltaTranslationVecBranch);
        translationMat = glm::translate(translationMat, translationVec);
    }
    else if(    !(glm::length2(deltaTranslationVecMaster) < FLT_EPSILON) &&
                (glm::length2(deltaTranslationVecBranch) < FLT_EPSILON))
    {
        master = true;
        glm::vec3 translationVec = masterTranslationVec + (1.0f * deltaTranslationVecMaster);
        translationMat = glm::translate(translationMat, translationVec);
    }
    else if(    (glm::length2(deltaTranslationVecMaster) < FLT_EPSILON) &&
                (glm::length2(deltaTranslationVecBranch) < FLT_EPSILON))
    {
        glm::vec3 translationVec = masterTranslationVec + (_delta * deltaTranslationVecMaster) + ((1.0f-_delta) * deltaTranslationVecBranch);
        translationMat = glm::translate(translationMat, translationVec);
    }
    else if(    !(glm::length2(deltaTranslationVecMaster) < FLT_EPSILON) &&
                !(glm::length2(deltaTranslationVecBranch) < FLT_EPSILON))
    {
        master = true;
        branch = true;
        glm::vec3 translationVec = masterTranslationVec + (_delta * deltaTranslationVecMaster) + ((1.0f-_delta) * deltaTranslationVecBranch);
        translationMat = glm::translate(translationMat, translationVec);
    }
}
