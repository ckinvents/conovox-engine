#include <conovox/camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace Conovox {
    // Constructors
    Camera::Camera(glm::vec3 pos, glm::vec3 target, float fov, float dist, float ratio)
    {
        this->fov = fov;
        this->dist = dist;
        this->ratio = ratio;
        this->isTracking = true;
        this->target = target;
        getTargetAngles();
        this->pos = pos;
    }

    Camera::Camera(float x, float y, float z, float targetX, float targetY, float targetZ, float fov, float dist, float ratio)
        : Camera(glm::vec3(x, y, z), glm::vec3(targetX, targetY, targetZ), fov, dist, ratio)
    {}

    Camera::Camera(glm::vec3 pos, float pitch, float yaw, float fov, float dist, float ratio)
    {
        this->fov = fov;
        this->dist = dist;
        this->ratio = ratio;
        this->isTracking = false;
        this->pitch = pitch;
        this->yaw = yaw;
        setTargetVect();
        this->pos = pos;
    }

    Camera::Camera(float x, float y, float z, float pitch, float yaw, float fov, float dist, float ratio)
        : Camera(glm::vec3(x, y, z), pitch, yaw, fov, dist, ratio)
    {}

    // Private functions
    void Camera::getTargetAngles()
    {
        dir = glm::normalize(target - pos);
        pitch = asinf32(dir.y);
        yaw = asinf32(dir.x / cosf32(pitch));
    }

    void Camera::setTargetVect()
    {
        dir.x = sin(yaw) * cos(pitch);
        dir.y = sin(pitch);
        dir.z = -cos(yaw) * cos(pitch);
        target = dir + pos;
    }

    // Public functions
    // Getters
    float Camera::getFOV()
    {
        return fov;
    }

    float Camera::getViewDist()
    {
        return dist;
    }

    glm::mat4 Camera::getViewMat()
    {
        return glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 Camera::getProjMat()
    {
        return glm::perspective(fov, ratio, 0.1f, dist);
    }

    glm::vec3 Camera::getPos()
    {
        return pos;
    }

    glm::vec3 Camera::getDir()
    {
        return dir;    
    }

    float Camera::getPitch()
    {
        return pitch;
    }

    float Camera::getYaw()
    {
        return yaw;
    }

    // Setters
    void Camera::setFOV(float fov)
    {
        this->fov = fov;
    }

    void Camera::setViewDist(float dist)
    {
        this->dist = dist;
    }

    void Camera::setRatio(float ratio)
    {
        this->ratio = ratio;
    }

    void Camera::setPos(glm::vec3 pos)
    {
        this->pos = pos;
        if (isTracking)
        {
            getTargetAngles();
        }
        else
        {
            target = dir + pos;
        }
    }

    void Camera::setPos(float x, float y, float z)
    {
        setPos(glm::vec3(x, y, z));
    }

    void Camera::setTarget(glm::vec3 target)
    {
        isTracking = true;
        this->target = target;
        getTargetAngles();
    }

    void Camera::setTarget(float x, float y, float z)
    {
        isTracking = true;
        this->target = glm::vec3(x, y, z);
        getTargetAngles();
    }

    void Camera::setAngles(float pitch, float yaw)
    {
        isTracking = false;
        this->pitch = pitch;
        this->yaw = yaw;
        setTargetVect();
    }
};
