#ifndef CONOVOX_CAMERA_H
#define CONOVOX_CAMERA_H

#include <conovox/core.h>

namespace Conovox {
    class Camera
    {
        private:
            float fov;
            float dist;
            float ratio;
            glm::vec3 pos;
            glm::vec3 target;
            glm::vec3 dir;
            bool isTracking;
            float pitch;
            float yaw;
            // Target vector math functions
            void getTargetAngles();
            void setTargetVect();
        public:
            // Initialize new scene camera
            // 3rd person style, locked target
            Camera(glm::vec3 pos, glm::vec3 target, float fov, float dist, float ratio);
            Camera(float x, float y, float z, float targetX, float targetY, float targetZ, float fov, float dist, float ratio);
            // 1st person style, manual pitch/yaw control
            Camera(glm::vec3 pos, float pitch, float yaw, float fov, float dist, float ratio);
            Camera(float x, float y, float z, float pitch, float yaw, float fov, float dist, float ratio);
            // Getters & setters
            float getFOV();
            float getViewDist();
            glm::mat4 getViewMat();
            glm::mat4 getProjMat();
            glm::vec3 getPos();
            glm::vec3 getDir();
            float getPitch();
            float getYaw();

            void setFOV(float fov);
            void setViewDist(float dist);
            void setRatio(float ratio);
            void setPos(glm::vec3 pos);
            void setPos(float x, float y, float z);
            void setTarget(glm::vec3 target);
            void setTarget(float x, float y, float z);
            void setAngles(float pitch, float yaw);
    };
};

#endif //CONOVOX_CAMERA_H