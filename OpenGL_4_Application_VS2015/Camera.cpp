//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		this->cameraUp =  glm::cross(cameraDirection, cameraRightDirection);
    }
	
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , glm::vec3(0.0f, 1.0f, 0.0f));
    }

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}
    
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraDirection * speed;
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraDirection * speed;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;
        }
    }
    
    void Camera::rotate(float pitch, float yaw)
    {
		cameraDirection.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraDirection.y = sin(glm::radians(pitch));
		cameraDirection.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    }
	
	glm::mat4 Camera::spinOnCircle(double angle)
	{	
		float radius = length(cameraPosition + cameraDirection);

		float camX = sin(angle) * radius;
		float camZ = cos(angle) * radius;
		
//		cameraPosition = glm::vec3(camX, cameraPosition.y, camZ);
//		cameraTarget = glm::vec3(0.0, cameraPosition.y, -10.0);
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

		return glm::lookAt( glm::vec3(camX, cameraPosition.y, camZ), glm::vec3(0.0, cameraPosition.y, -10.0), up);
	}
    
}



