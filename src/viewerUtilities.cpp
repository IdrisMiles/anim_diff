
#include "viewerUtilities.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <float.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for ASSIMP animation structures
//-----------------------------------------------------------------------------------------------------------------------------------------

glm::mat4 ViewerUtilities::ConvertToGlmMat(const aiMatrix4x4 &m)
{
    glm::mat4 a(  m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
    return a;
}

//void ViewerUtilities::ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<BoneTransformData> &_boneInfo, const aiMatrix4x4 _globalInverseTransform, const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform)
//{
//    if(!_pAnimation || !_pNode)
//    {
//        return;
//    }

//    std::string nodeName(_pNode->mName.data);

//    // Set defualt to bind pose
//    aiMatrix4x4 nodeTransform(_pNode->mTransformation);

//    const aiNodeAnim* pNodeAnim = FindNodeAnim(_pAnimation, nodeName);


//    // Check if valid aiNodeAnim, as not all aiNode have corresponding aiNodeAnim.
//    if (pNodeAnim)
//    {
//        // Interpolate scaling and generate scaling transformation matrix
//        aiVector3D scalingVec;
//        CalcInterpolatedScaling(scalingVec, _animationTime, pNodeAnim);
//        aiMatrix4x4 scalingMat = aiMatrix4x4();
//        aiMatrix4x4::Scaling(scalingVec, scalingMat);

//        // Interpolate rotation and generate rotation transformation matrix
//        aiQuaternion rotationQ = aiQuaternion();
//        CalcInterpolatedRotation(rotationQ, _animationTime, pNodeAnim);
//        aiMatrix4x4 rotationMat = aiMatrix4x4();
//        rotationMat = aiMatrix4x4(rotationQ.GetMatrix());

//        // Interpolate translation and generate translation transformation matrix
//        aiVector3D translationVec;
//        CalcInterpolatedPosition(translationVec, _animationTime, pNodeAnim);
//        aiMatrix4x4 translationMat = aiMatrix4x4();
//        aiMatrix4x4::Translation(translationVec, translationMat);

//        // Combine the above transformations
//        nodeTransform = translationMat * rotationMat * scalingMat;
//    }

//    aiMatrix4x4 globalTransformation = _parentTransform * nodeTransform;

//    if (_boneMapping.find(nodeName) != _boneMapping.end()) {
//        uint BoneIndex = _boneMapping.at(nodeName);
//        //_boneInfo[BoneIndex].finalTransform = _globalInverseTransform * globalTransformation * _boneInfo[BoneIndex].boneOffset;
//    }

//    for (uint i = 0 ; i < _pNode->mNumChildren ; i++)
//    {
//        ReadNodeHierarchy(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _pAnimation, _pNode->mChildren[i], globalTransformation);
//    }

//}

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

//void ViewerUtilities::CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    // we need at least two values to interpolate...
//    if (_pNodeAnim->mNumRotationKeys == 1) {
//        _out = _pNodeAnim->mRotationKeys[0].mValue;
//        return;
//    }

//    uint RotationIndex = FindRotationKeyFrame(_animationTime, _pNodeAnim);
//    uint NextRotationIndex = (RotationIndex + 1);
//    assert(NextRotationIndex < _pNodeAnim->mNumRotationKeys);
//    float DeltaTime = _pNodeAnim->mRotationKeys[NextRotationIndex].mTime - _pNodeAnim->mRotationKeys[RotationIndex].mTime;
//    float Factor = (_animationTime - (float)_pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//    //assert(Factor >= 0.0f && Factor <= 1.0f);
//    const aiQuaternion& StartRotationQ = _pNodeAnim->mRotationKeys[RotationIndex].mValue;
//    const aiQuaternion& EndRotationQ = _pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
//    aiQuaternion::Interpolate(_out, StartRotationQ, EndRotationQ, Factor);
//    _out = _out.Normalize();
//}

//void ViewerUtilities::CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    // we need at least two values to interpolate...
//    if (_pNodeAnim->mNumPositionKeys == 1) {
//        _out = _pNodeAnim->mPositionKeys[0].mValue;
//        return;
//    }

//    uint PositionIndex = FindPositionKeyFrame(_animationTime, _pNodeAnim);
//    uint NextPositionIndex = (PositionIndex + 1);
//    assert(NextPositionIndex < _pNodeAnim->mNumPositionKeys);
//    float DeltaTime = _pNodeAnim->mPositionKeys[NextPositionIndex].mTime - _pNodeAnim->mPositionKeys[PositionIndex].mTime;
//    float Factor = (_animationTime - (float)_pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//    //assert(Factor >= 0.0f && Factor <= 1.0f);
//    const aiVector3D& startPositionV = _pNodeAnim->mPositionKeys[PositionIndex].mValue;
//    const aiVector3D& endPositionV = _pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
//    _out = startPositionV + (Factor*(endPositionV-startPositionV));

//}

//void ViewerUtilities::CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    // we need at least two values to interpolate...
//    if (_pNodeAnim->mNumScalingKeys == 1) {
//        _out = _pNodeAnim->mScalingKeys[0].mValue;
//        return;
//    }

//    uint scalingIndex = FindScalingKeyFrame(_animationTime, _pNodeAnim);
//    uint nextScalingIndex = (scalingIndex + 1);
//    assert(nextScalingIndex < _pNodeAnim->mNumScalingKeys);
//    float DeltaTime = _pNodeAnim->mScalingKeys[nextScalingIndex].mTime - _pNodeAnim->mScalingKeys[scalingIndex].mTime;
//    float Factor = (_animationTime - (float)_pNodeAnim->mScalingKeys[scalingIndex].mTime) / DeltaTime;
//    //assert(Factor >= 0.0f && Factor <= 1.0f);
//    const aiVector3D& startScalingV = _pNodeAnim->mScalingKeys[scalingIndex].mValue;
//    const aiVector3D& endScalingV = _pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
//    _out = startScalingV + (Factor*(endScalingV-startScalingV));

//}

//uint ViewerUtilities::FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    assert(_pNodeAnim->mNumRotationKeys > 0);

//    for (uint i = 0 ; i < _pNodeAnim->mNumRotationKeys - 1 ; i++) {
//        if (_animationTime < (float)_pNodeAnim->mRotationKeys[i + 1].mTime) {
//            return i;
//        }
//    }

//    assert(0);
//}

//uint ViewerUtilities::FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    assert(_pNodeAnim->mNumPositionKeys > 0);

//    for (uint i = 0 ; i < _pNodeAnim->mNumPositionKeys - 1 ; i++) {
//        if (_animationTime < (float)_pNodeAnim->mPositionKeys[i + 1].mTime)
//        {
//            return i;
//        }
//    }

//    assert(0);
//}

//uint ViewerUtilities::FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
//{
//    assert(_pNodeAnim->mNumScalingKeys > 0);

//    for (uint i = 0 ; i < _pNodeAnim->mNumScalingKeys - 1 ; i++) {
//        if (_animationTime < (float)_pNodeAnim->mScalingKeys[i + 1].mTime) {
//            return i;
//        }
//    }

//    assert(0);
//}

//const aiNode* ViewerUtilities::getParentBone(const std::map<std::string, unsigned int> _boneMapping, const aiNode* _pNode)
//{
//    if(_pNode->mParent != NULL)
//    {
//        if(_boneMapping.find(std::string(_pNode->mName.data)) != _boneMapping.end())
//        {
//            return _pNode->mParent;
//        }
//        else
//        {
//            return getParentBone(_boneMapping, _pNode->mParent);
//        }
//    }
//    else
//    {
//        return NULL;
//    }
//}


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


//const aiNode* ViewerUtilities::GetNode(const aiScene *_aiScene, std::string _name)
//{
//    return GetNode(_aiScene->mRootNode, _name);
//}

//const aiNode* ViewerUtilities::GetNode(const aiNode *_aiNode, std::string _name)
//{
//    if(std::string(_aiNode->mName.data) == _name)
//    {
//        return _aiNode;
//    }
//    else
//    {
//        for(unsigned int i=0; i<_aiNode->mNumChildren; i++)
//        {
//            const aiNode* childNode = GetNode(_aiNode->mChildren[i], _name);
//            if(childNode)
//            {
//                return childNode;
//            }
//        }
//    }
//    return NULL;
//}


//-----------------------------------------------------------------------------------------------------------------------------------------
// ViewerUtilities for new rig structure
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
    glm::mat4 nodeTransform(_pBone->m_transform);

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
    nodeTransform =  translationMat * rotationMat * scalingMat;


    glm::mat4 globalTransformation = glm::transpose(nodeTransform)*_parentTransform;


    if (BoneIndex != -1)
    {
        _pBone->m_currentTransform = _boneInfo[BoneIndex] = _pBone->m_boneOffset * globalTransformation * _globalInverseTransform;
    }

    for (uint i = 0 ; i < _pBone->m_children.size() ; i++)
    {
        ReadNodeHierarchy(_boneMapping, _boneInfo, _globalInverseTransform, _animationTime, _pRig, std::shared_ptr<Bone>(_pBone->m_children[i]), globalTransformation);
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

const std::shared_ptr<Bone> ViewerUtilities::getParentBone(const std::shared_ptr<Bone> _pNode)
{
    if(_pNode->m_parent != NULL)
    {
        if(!_pNode->m_parent->m_name.empty())
        {
            return _pNode->m_parent;
        }
        else
        {
            return getParentBone(_pNode->m_parent);
        }
    }
    else
    {
        return NULL;
    }
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
        /*
        newBone->m_name = std::string(_aiNode->mName.data);
        newBone->m_transform = ViewerUtilities::ConvertToGlmMat(_aiNode->mTransformation) * _parentTransform;

        // Set bone offset matrix
        newBone->m_boneOffset = glm::mat4(1.0f);

        // Set animation data
        _rig->m_boneAnims[newBone->m_name] = BoneAnim();
        newBone->m_boneAnim = &_rig->m_boneAnims[newBone->m_name];

        // Set parent and set child
        newBone->m_parent = _parentBone;
        _parentBone->m_children.push_back(newBone);
        newParentTransform = glm::mat4(1.0f);
        */

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
    glm::vec3 jointColour;

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

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimMerge pBoneAnimMerge = _pMergeRig.m_boneAnimMerges[_pMasterBone->m_name];
    const BoneAnimDiff* pBoneAnimDiff = &pBoneAnimMerge.m_mainDiff;

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
    glm::vec3 jointColour;

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

    // Set defualt to bind pose
    glm::mat4 boneTransform(_pMasterBone->m_transform);
    glm::mat4 scalingMat(1.0f);
    glm::mat4 rotationMat(1.0f);
    glm::mat4 translationMat;

    const BoneAnim* pMasterBoneAnim = &_pMasterRig->m_boneAnims[_pMasterBone->m_name];
    const BoneAnimMerge boneAnimMerge = _pMergeRig.m_boneAnimMerges[_pMasterBone->m_name];
    const BoneAnimDiff* pBoneAnimDiff;

    if(boneAnimMerge.m_type != MERGE::CONFLICT)
    {
        if(boneAnimMerge.m_type == MERGE::MAIN)
        {
            pBoneAnimDiff = &boneAnimMerge.m_mainDiff;
        }
        else if(boneAnimMerge.m_type == MERGE::BRANCH)
        {
            pBoneAnimDiff = &boneAnimMerge.m_branchDiff;
        }
        else
        {
            pBoneAnimDiff = &boneAnimMerge.m_mainDiff;
        }

        // Interpolate scaling and generate scaling transformation matrix
        GenerateScalingMatrix(scalingMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);

        // Interpolate rotation and generate rotation transformation matrix
        GenerateRotationMatrix(rotationMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);

        // Interpolate translation and generate translation transformation matrix
        GenerateTranslationMatrix(translationMat, _animationTime, delta, pMasterBoneAnim, pBoneAnimDiff);
    }
    else if(boneAnimMerge.m_type == MERGE::CONFLICT)
    {

    }


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

void ViewerUtilities::GenerateScalingMatrix(glm::mat4 &scalingMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge)
{
    if(boneAnimMerge.m_type == MERGE::CONFLICT)
    {
        glm::vec3 masterScalingVec;
        glm::vec3 deltaScalingVec;
        CalcInterpolatedScaling(masterScalingVec, _animationTime, pMasterBoneAnim);
        CalcInterpolatedScaling(deltaScalingVec, _animationTime, pBoneAnimDiff);
        glm::vec3 scalingVec = masterScalingVec + (_delta * deltaScalingVec);
        scalingMat = glm::scale(scalingMat, scalingVec);
    }
}

void ViewerUtilities::GenerateRotationMatrix(glm::mat4 &rotationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge)
{

}

void ViewerUtilities::GenerateTranslationMatrix(glm::mat4 &translationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge)
{

}
