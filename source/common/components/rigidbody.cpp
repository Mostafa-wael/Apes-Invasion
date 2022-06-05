#include "rigidbody.hpp"
#include "../ecs/entity.hpp"
#include "../systems/physics.hpp"
#include "CollisionShapes/btCollisionShape.h"
#include "CollisionShapes/btConvexHullShape.h"
#include "CollisionShapes/btSphereShape.h"
#include "components/mesh-renderer.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "systems/physics.hpp"
#include <cstdio>
#include <stdexcept>
#include <string>

namespace our { // namespace our

    void RigidBody::init(PhysicsSystem* phys) {
        physicsSystem = phys;
    }

    void RigidBody::fromMeshRenderer(MeshRendererComponent* meshRenderer, std::string type, float mass) {
        auto [aabbCenter, aabbHalfExtents] = getAABBWorldScale(meshRenderer->mesh);
        glm::vec3 wRot                     = glm::eulerAngles(glm::quat(getOwner()->getWorldRotation()));
        this->fromExtentsTranslationRotation(aabbHalfExtents, getOwner()->getWorldTranslation(), wRot, type, mass);
    }

    void RigidBody::syncWithTransform(glm::mat4 worldRotation, glm::vec3 worldTranslation, bool resetMovement) {
        btTransform t;
        glm::mat4 wRot = worldRotation;
        glm::mat4 wPos = glm::translate(glm::mat4(1), worldTranslation);
        t.setFromOpenGLMatrix(glm::value_ptr(wPos * wRot));

        int colFlags  = bulletRB->getCollisionFlags();
        int kinematic = btCollisionObject::CF_KINEMATIC_OBJECT;

        if(resetMovement) {
            bulletRB->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
            bulletRB->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
            bulletRB->clearForces();
        }

        if(colFlags & kinematic) {
            bulletRB->getMotionState()->setWorldTransform(t);
        } else
            bulletRB->setWorldTransform(t);
    }

    void RigidBody::deserialize(const nlohmann::json& data) {
        if(!data.is_object()) return;

        float mass              = 1;
        glm::vec3 halfExtents   = glm::vec3(1, 1, 1);
        bool useMeshBoundingBox = true;
        std::string type        = "dynamic";

        mass                = data.value("mass", mass);
        type                = data.value("rigidBodyType", type);
        auto colliderShape  = data.value("colliderShape", "fromMeshRenderer");
        glm::vec3 wRotEuler = glm::eulerAngles(glm::quat(getOwner()->getWorldRotation()));

        if(colliderShape == "box") {
            halfExtents = data.value("halfExtents", halfExtents);
            this->fromExtentsTranslationRotation(halfExtents, getOwner()->getWorldTranslation(), wRotEuler, type, mass);
        } else if(colliderShape == "fromMeshRenderer") {
            if(auto meshRenderer = getOwner()->getComponent<MeshRendererComponent>())
                this->fromMeshRenderer(meshRenderer, type, mass);
            else
                throw std::runtime_error("Rigidbody with no given AABB is not attached to an entity with a mesh renderer\n");

        } else if(colliderShape == "sphere") {
            btCollisionShape* sphereColl = new btSphereShape(data.value("radius", 1));
            this->fromCollisionShapeTranslationRotation(sphereColl, getOwner()->getWorldTranslation(), wRotEuler, type, mass);
        } else if(colliderShape == "convexHull") {

            if(auto meshRenderer = getOwner()->getComponent<MeshRendererComponent>()) {
                btCollisionShape* hull = new btConvexHullShape((float*)meshRenderer->mesh->verts.data(), meshRenderer->mesh->verts.size(), sizeof(glm::vec3));
                this->fromCollisionShapeTranslationRotation(hull, getOwner()->getWorldTranslation(), wRotEuler, type, mass);
            } else
                throw std::runtime_error("Rigidbody with no given AABB is not attached to an entity with a mesh renderer\n");

        } else {
            throw std::runtime_error("Rigid body collider type ( " + type + " ). Type must be one of \"box\", \"fromMeshRenderer\", \"sphere\", \"convexHul\"");
        }
    }

    void RigidBody::fromCollisionShapeTranslationRotation(btCollisionShape* btColl, glm::vec3 translation, glm::vec3 rotation, std::string type, float m) {
        btTransform transform;

        transform.setIdentity();
        transform.setOrigin({translation.x, translation.y, translation.z});

        btQuaternion quat = btQuaternion();
        quat.setEuler(rotation.y, rotation.x, rotation.z);
        transform.setRotation(quat);

        btScalar mass(m);

        //rigidbody is dynamic if and only if mass is non zero, otherwise static
        bool isDynamic = (m != 0.f);

        btVector3 localInertia(0, 0, 0);
        if(isDynamic)
            btColl->calculateLocalInertia(mass, localInertia);

        //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
        btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(m, myMotionState, btColl, localInertia);

        bulletRB = new btRigidBody(rbInfo);

        // Simulates physics, gets affected by other dynamic objects
        if(type == "dynamic")
            bulletRB->setCollisionFlags(bulletRB->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);

        // Simulates physics, NOT affected by other dynamic objects but affects them.
        else if(type == "kinematic")
            bulletRB->setCollisionFlags(bulletRB->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

        // No physics simulation, only collision?
        else if(type == "static")
            bulletRB->setCollisionFlags(bulletRB->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

        // Always simulate
        // Bullet disables simulation after an object stays still or moves very slowly for some time.
        // Just for debugging
        bulletRB->setUserPointer(getOwner());
        bulletRB->setActivationState(DISABLE_DEACTIVATION);

        if(getOwner()->getParent()) {
            // printf("Entity %s has a rigidbody component, setting parent to null due to technical limitations\n", getOwner()->name.c_str());
            getOwner()->setParent(nullptr);
        }
    }

    std::pair<glm::vec3, glm::vec3> RigidBody::getAABBWorldScale(Mesh* meshComp) {

        glm::vec3 wPos   = getOwner()->getWorldTranslation();
        glm::vec3 wScale = getOwner()->getWorldScale();

        auto [wAABBPos, aabbHalfExtents] = meshComp->getAABB(wScale);
        wAABBPos                         = glm::translate(glm::mat4(1), wPos) * glm::vec4(wAABBPos, 1);
        return {wAABBPos, aabbHalfExtents};
    }

    RigidBody::~RigidBody() {
        physicsSystem->dynamicsWorld->removeRigidBody(bulletRB);
    }
} // namespace our