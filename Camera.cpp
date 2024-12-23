#include "Camera.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->up = cameraUp;
        glm::vec3 front = cameraTarget - cameraPosition;
        this->cameraFrontDirection = glm::normalize(front); //cameraFrontDirection = -(cameraTarget-cameraPosition)/lungime
        glm::vec3 right = glm::cross((-this->cameraFrontDirection), this->up);
        this->cameraRightDirection = -glm::normalize(right);
        this->cameraUpDirection = glm::cross((-this->cameraFrontDirection), this->cameraRightDirection);

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO

        return glm::lookAt(this->cameraPosition, this->cameraTarget, this->up);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO

        //doar pozitia camerei se modifica, axele raman aceleasi
        if (direction == MOVE_FORWARD)
        {
            this->cameraPosition = this->cameraPosition + speed * (this->cameraFrontDirection);
        }
        else if (direction == MOVE_BACKWARD)
        {
            this->cameraPosition = this->cameraPosition + speed * (-this->cameraFrontDirection);
        }
        else if (direction == MOVE_RIGHT)
        {
            this->cameraPosition = this->cameraPosition + speed * (this->cameraRightDirection);
        }
        else if (direction == MOVE_LEFT)
        {
            this->cameraPosition = this->cameraPosition + speed * (-this->cameraRightDirection);
        }
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;

    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::mat4 eulerRotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
        glm::vec4 cameraFront = eulerRotation * glm::vec4(0.0f, 0.0, -1.0f, 0.0f);//glm::vec4(this->cameraFrontDirection, 0);
        this->cameraFrontDirection = glm::vec3(glm::normalize(cameraFront));
        glm::vec3 right = glm::cross((-this->cameraFrontDirection), this->up);
        this->cameraRightDirection = -glm::normalize(right);
        this->cameraUpDirection = cross((-this->cameraFrontDirection), this->cameraRightDirection);
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;
    }
}