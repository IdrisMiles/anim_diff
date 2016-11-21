#ifndef VIEWERUTILITIES_H
#define VIEWERUTILITIES_H

#include <vector>
#include <map>


#include "vertexBoneData.h"
#include "boneTransform.h"


// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "modelRig.h"
#include "bone.h"

namespace ViewerUtilities
{
    /// @brief Method to convert Assimps aiMatrix4x4 class to a glm::mat4 class. This is useful for sending assimps matrices to opengl.
    /// @param m : The matrix to convert.
    static glm::mat4 ConvertToGlmMat(const aiMatrix4x4 &m);

    /// @brief Method that recursively reads the animation node hierarchy and accumulates the relevant transforms for the current animation time.
    /// @param _animationTime : Animation time.
    /// @param _pAnimation : The animation we want to use.
    /// @param _pNode : The transform node we want to find a corresponding aiNodeAnim for to read its animation.
    /// @param _parentTransform : The accumulated parent transform, so as we traverse the hierarchy each bone has the correcct global transformation.
    static void ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<BoneTransformData> &_boneInfo, const aiMatrix4x4 _globalInverseTransform, const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform);

    /// @brief Method to find the animation node with the name matching NodeName.
    /// @param _pAnimation : The animation we want to use.
    /// @param _nodeName : The name of the animated node in the animation we are trying to find.
    /// @return const aiNodeAnim* : returns the aiNodeAnim with the same name as _nodeName in _pAnimation if found, else returns NULL.
    static const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName);

    /// @brief Method to interpolate between two frame of rotation animation.
    /// @param _out : Quaternion to hold the interpolated rotation animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the rotation from.
    static void CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of position animation.
    /// @param _out : Vector to hold the interpolated position animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the position from.
    static void CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of scaling animation.
    /// @param _out : Vector to hold the interpolated scaling animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the scaling from.
    static void CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the rotation animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for rotation animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    static uint FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the position animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for position animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    static uint FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the scaling animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for scaling animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    static uint FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to fine the parent bone to the current aiNode.
    /// @param _boneMapping : a map that holds bone names and their index.
    /// _pNode : The node we want to find the parent off.
    static const aiNode* getParentBone(const std::map<std::__cxx11::string, unsigned int> _boneMapping, const aiNode* _pNode);

    //-----------------------------------------------------------------------------------------------------------------------------
    void ReadNodeHierarchy2(const glm::mat4 _globalInverseTransform, const float _animationTime, const ModelRig *_pRig, Bone *_pBone, const glm::mat4& _parentTransform);

    BoneAnim FindBoneAnim(ModelRig _pRig, std::__cxx11::string _nodeName);

    void CalcInterpolatedRotation2(glm::quat& _out, const float _animationTime, const BoneAnim* _pBoneAnin);

    void CalcInterpolatedPosition2(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    void CalcInterpolatedScaling2(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    uint FindKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    const Bone* getParentBone2(const Bone* _pNode);



    //___________________________________________________________________________________________________________________________________________________
    //
    // IMPLEMENTATION
    //___________________________________________________________________________________________________________________________________________________

    glm::mat4 ConvertToGlmMat(const aiMatrix4x4 &m)
    {
        glm::mat4 a(  m.a1, m.a2, m.a3, m.a4,
                      m.b1, m.b2, m.b3, m.b4,
                      m.c1, m.c2, m.c3, m.c4,
                      m.d1, m.d2, m.d3, m.d4);

        return a;
    }

    void ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<BoneTransformData> &_boneInfo, const aiMatrix4x4 _globalInverseTransform, const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform)
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


    const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName)
    {
        for (uint i = 0 ; i < _pAnimation->mNumChannels ; i++) {
            const aiNodeAnim* pNodeAnim = _pAnimation->mChannels[i];

            if (std::string(pNodeAnim->mNodeName.data) == _nodeName) {
                return pNodeAnim;
            }
        }

        return NULL;
    }


    void CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
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


    void CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
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


    void CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim)
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


    uint FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
    {
        assert(_pNodeAnim->mNumRotationKeys > 0);

        for (uint i = 0 ; i < _pNodeAnim->mNumRotationKeys - 1 ; i++) {
            if (_animationTime < (float)_pNodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }

        assert(0);
    }


    uint FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
    {
        assert(_pNodeAnim->mNumPositionKeys > 0);

        for (uint i = 0 ; i < _pNodeAnim->mNumPositionKeys - 1 ; i++) {
            if (_animationTime < (float)_pNodeAnim->mPositionKeys[i + 1].mTime) {
                return i;
            }
        }

        assert(0);
    }


    uint FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim)
    {
        assert(_pNodeAnim->mNumScalingKeys > 0);

        for (uint i = 0 ; i < _pNodeAnim->mNumScalingKeys - 1 ; i++) {
            if (_animationTime < (float)_pNodeAnim->mScalingKeys[i + 1].mTime) {
                return i;
            }
        }

        assert(0);
    }

    const aiNode* getParentBone(const std::map<std::__cxx11::string, unsigned int> _boneMapping, const aiNode* _pNode)
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
}

#endif // VIEWERUTILITIES_H
