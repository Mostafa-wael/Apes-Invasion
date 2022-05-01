#pragma once

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "btBulletDynamicsCommon.h"

#include "ecs/component.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/vec3.hpp"
#include <cstdio>
#include <string>

namespace our {
    class RigidBody : public Component {
    public:
        btRigidBody* body;
        RigidBody(glm::vec3 boxHalfExtents = glm::vec3(50.0f, 50.0f, 50.0f),
                  glm::vec3 translation    = glm::vec3(0),
                  glm::vec3 rotation       = glm::vec3(0),
                  float m                  = 0) {
            btCollisionShape* boundingBox = new btBoxShape(btVector3(btScalar(boxHalfExtents.x), btScalar(boxHalfExtents.y), btScalar(boxHalfExtents.z)));

            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(translation.x, translation.y, translation.z));
            btQuaternion quat = btQuaternion();
            quat.setEuler(rotation.y, rotation.x, rotation.z);

            if(quat == btQuaternion(rotation.y, rotation.x, rotation.z))
                printf("%s", "KAK\n");
            transform.setRotation(quat);

            btScalar mass(m);

            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = (m != 0.f);

            btVector3 localInertia(0, 0, 0);
            if(isDynamic)
                boundingBox->calculateLocalInertia(mass, localInertia);

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(m, myMotionState, boundingBox, localInertia);
            body = new btRigidBody(rbInfo);
            body->setCustomDebugColor(btVector3(255, 0, 255));
        }

        RigidBody(btTransform t, glm::vec3 boxHalfExtents = glm::vec3(0.5f), float m = 0) {
            btCollisionShape* boundingBox = new btBoxShape(btVector3(btScalar(boxHalfExtents.x), btScalar(boxHalfExtents.y), btScalar(boxHalfExtents.z)));

            //rigidbody is dynamic if and only if mass is non zero, otherwise static
            bool isDynamic = (m != 0.f);

            btVector3 localInertia(0, 0, 0);
            if(isDynamic)
                boundingBox->calculateLocalInertia(m, localInertia);

            //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
            btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(m, myMotionState, boundingBox, localInertia);
            body = new btRigidBody(rbInfo);
        }

        virtual void deserialize(const nlohmann::json& data) {}

        static std::string getID() { return "rigidbody"; }
    };
} // namespace our