#ifndef CONOVOX_LIGHT_H
#define CONOVOX_LIGHT_H

#include <conovox/core.h>

namespace Conovox {
    typedef struct {
        glm::vec3 dir;
        glm::vec3 color;
        float ambFact;
        float diffFact;
        float specFact;
    } SunSource;

    typedef struct {
        glm::vec3 pos;
        glm::vec3 dir;
        glm::vec3 color;
        float inArc;
        float outArc;
        float ambFact;
        float diffFact;
        float specFact;
        float kConst;
        float kLinear;
        float kQuad;
    } SpotSource;

    typedef struct {
        glm::vec3 pos;
        glm::vec3 color;
        float ambFact;
        float diffFact;
        float specFact;
        float kConst;
        float kLinear;
        float kQuad;
    } PointSource;
};

#endif //CONOVOX_LIGHT_H