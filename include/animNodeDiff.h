#ifndef ANIMNODEDIFF_H
#define ANIMNODEDIFF_H

#include <glm/ext.hpp>

struct animDelta {
    glm::vec3 deltaPos;
    glm::quat deltaRot;
    glm::vec3 deltaScale;
};

class AnimNodeDiff
{
public:
    AnimNodeDiff();
    ~AnimNodeDiff();

};

#endif // ANIMNODEDIFF_H
