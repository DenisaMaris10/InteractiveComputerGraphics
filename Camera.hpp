#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT };

    class Camera {
        friend class CarCamera;
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        // return camera position
        glm::vec3 getCameraPosition();  
        glm::vec3 getCameraFrontDirection();  
        glm::vec3 getCameraTarget();  
        void setCameraPosition(glm::vec3 cameraPosition);  
        void setCameraTarget(glm::vec3 cameraTarget);  
        void setCameraFrontDirection(glm::vec3 cameraFrontDirection);  
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed, bool canFly);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        float angle = 0;
        glm::vec3 up = glm::vec3(0, 1, 0);
    };

    class CarCamera : public Camera {
        public:
            CarCamera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
            void move(MOVE_DIRECTION direction, float speed, bool canFly);
            float carYAngle;
            glm::mat3 carYRotation;
    };
}

#endif /* Camera_hpp */
