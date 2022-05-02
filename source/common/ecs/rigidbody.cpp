#include "rigidbody.hpp"
#include "entity.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

namespace our {
    void RigidBody::syncWithTransform(glm::mat4 worldRotation, glm::vec3 worldTranslation)  {
        btTransform t;
        glm::mat4 wRot = worldRotation;
        glm::mat4 wPos = glm::translate(glm::mat4(1), worldTranslation);
        t.setFromOpenGLMatrix(glm::value_ptr(wPos * wRot));

        int colFlags  = body->getCollisionFlags();
        int kinematic = btCollisionObject::CF_KINEMATIC_OBJECT;
        if(colFlags & kinematic) {
            body->getMotionState()->setWorldTransform(t);
            body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
            body->clearForces();
        } else
            body->setWorldTransform(t);
    }
} // namespace our