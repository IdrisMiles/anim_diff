#ifndef ANIMNODEDIFF_H
#define ANIMNODEDIFF_H

#include <glm/ext.hpp>
#include <vector>

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

    std::vector<animDelta> animationChanges;

};

#endif // ANIMNODEDIFF_H
