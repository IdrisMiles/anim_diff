#ifndef VIEWERUTILITIES_H
#define VIEWERUTILITIES_H

#include <vector>
#include <map>


#include "include/vertexBoneData.h"
#include "include/boneTransform.h"


// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "include/modelRig.h"
#include "include/bone.h"

namespace ViewerUtilities
{
    /// @brief Method to convert Assimps aiMatrix4x4 class to a glm::mat4 class. This is useful for sending assimps matrices to opengl.
    /// @param m : The matrix to convert.
    glm::mat4 ConvertToGlmMat(const aiMatrix4x4 &m);

    /// @brief Method that recursively reads the animation node hierarchy and accumulates the relevant transforms for the current animation time.
    /// @param _animationTime : Animation time.
    /// @param _pAnimation : The animation we want to use.
    /// @param _pNode : The transform node we want to find a corresponding aiNodeAnim for to read its animation.
    /// @param _parentTransform : The accumulated parent transform, so as we traverse the hierarchy each bone has the correcct global transformation.
    void ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<BoneTransformData> &_boneInfo, const aiMatrix4x4 _globalInverseTransform, const float _animationTime, const aiAnimation *_pAnimation, const aiNode* _pNode, const aiMatrix4x4& _parentTransform);

    /// @brief Method to find the animation node with the name matching NodeName.
    /// @param _pAnimation : The animation we want to use.
    /// @param _nodeName : The name of the animated node in the animation we are trying to find.
    /// @return const aiNodeAnim* : returns the aiNodeAnim with the same name as _nodeName in _pAnimation if found, else returns NULL.
    const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName);

    /// @brief Method to interpolate between two frame of rotation animation.
    /// @param _out : Quaternion to hold the interpolated rotation animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the rotation from.
    void CalcInterpolatedRotation(aiQuaternion& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of position animation.
    /// @param _out : Vector to hold the interpolated position animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the position from.
    void CalcInterpolatedPosition(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to interpolate between two frame of scaling animation.
    /// @param _out : Vector to hold the interpolated scaling animation for the current animation time in the current animation.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the scaling from.
    void CalcInterpolatedScaling(aiVector3D& _out, const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the rotation animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for rotation animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindRotationKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the position animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for position animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindPositionKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to find the correct keyframe relating to the current animation time for the scaling animation of this aiNodeAnim.
    /// @param _animationTime : The current animation time.
    /// @param _pNodeAnim : The animated node we are getting the keyframe for scaling animation from.
    /// @return uint : The keyframe corresponding with _animationTime.
    uint FindScalingKeyFrame(const float _animationTime, const aiNodeAnim* _pNodeAnim);

    /// @brief Method to fine the parent bone to the current aiNode.
    /// @param _boneMapping : a map that holds bone names and their index.
    /// _pNode : The node we want to find the parent off.
    const aiNode* getParentBone(const std::map<std::string, unsigned int> _boneMapping, const aiNode* _pNode);

    const aiBone* GetBone(const aiScene *_aiScene, std::string _name);

    const aiNode* GetNode(const aiScene *_aiScene, std::string _name);

    const aiNode* GetNode(const aiNode *_aiNode, std::string _name);

    //-----------------------------------------------------------------------------------------------------------------------------
    void ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, ModelRig &_pRig, std::shared_ptr<Bone> _pBone, const glm::mat4& _parentTransform);

    BoneAnim FindBoneAnim(ModelRig _pRig, std::string _nodeName);

    void CalcInterpolatedRotation(glm::quat& _out, const float _animationTime, const BoneAnim* _pBoneAnin);

    void CalcInterpolatedPosition(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    void CalcInterpolatedScaling(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    uint FindRotationKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    uint FindPositionKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    uint FindScalingKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    const std::shared_ptr<Bone> getParentBone(const std::shared_ptr<Bone> _pNode);



}

#endif // VIEWERUTILITIES_H
