#version 330
in vec3 vertex;
in vec3 normal;
in ivec4 BoneIDs;
in vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 projMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 Bones[MAX_BONES];

out vec3 vert;
out vec3 vertNormal;


void main()
{
    mat4 BoneTransform = Bones[BoneIDs[0]] * Weights[0];
    BoneTransform += Bones[BoneIDs[1]] * Weights[1];
    BoneTransform += Bones[BoneIDs[2]] * Weights[2];
    BoneTransform += Bones[BoneIDs[3]] * Weights[3];

    /*
    vert = vertex.xyz;
    vertNormal = normalMatrix * normal;
    gl_Position = projMatrix * mvMatrix * vec4(vertex,1.0);
    */

   vert = vertex.xyz;
   //vertNormal = normalMatrix * vec3(BoneTransform * vec4(normal, 1.0));
   vertNormal = normalMatrix * normal;
   //gl_Position = projMatrix * mvMatrix * vec4((BoneTransform * vec4(vertex, 1.0)).xyz, 1.0);
   gl_Position = projMatrix * mvMatrix * BoneTransform * vec4(vertex, 1.0);

}
