#include "rigidbody.hpp"
#include "components/mesh-renderer.hpp"
#include "entity.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <stdexcept>
#include <string>

namespace our { // namespace our

    void RigidBody::fromMeshRenderer(MeshRendererComponent* meshRenderer, std::string type, float mass) {
        auto [aabbCenter, aabbHalfExtents] = getAABBWorldScale(meshRenderer->mesh);
        glm::vec3 wRot                     = glm::eulerAngles(glm::quat(getOwner()->getWorldRotation()));
        this->fromExtentsTranslationRotation(aabbHalfExtents, getOwner()->getWorldTranslation(), wRot, type, mass);
    }

    void RigidBody::syncWithTransform(glm::mat4 worldRotation, glm::vec3 worldTranslation) {
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

    void RigidBody::deserialize(const nlohmann::json& data) {
        {
            if(!data.is_object()) return;

            float mass              = 1;
            glm::vec3 halfExtents   = glm::vec3(1, 1, 1);
            bool useMeshBoundingBox = true;
            std::string type        = "dynamic";

            mass               = data.value("mass", mass);
            useMeshBoundingBox = data.value("useMeshBoundingBox", useMeshBoundingBox);
            type               = data.value("rigidBodyType", type);

            if(!useMeshBoundingBox) {
                halfExtents         = data.value("halfExtents", halfExtents);
                glm::vec3 wRotEuler = glm::eulerAngles(glm::quat(getOwner()->getWorldRotation()));
                this->fromExtentsTranslationRotation(halfExtents, getOwner()->getWorldTranslation(), wRotEuler, type, mass);
            } else {
                if(auto meshRenderer = getOwner()->getComponent<MeshRendererComponent>())
                    this->fromMeshRenderer(meshRenderer, type, mass);
                else
                    throw std::runtime_error("Rigidbody with no given AABB is not attached to an entity with a mesh renderer\n");
            }
        }
    }

    std::pair<glm::vec3, glm::vec3> RigidBody::getAABBWorldScale(Mesh* meshComp) {

        glm::vec3 wPos   = getOwner()->getWorldTranslation();
        glm::vec3 wRot   = glm::eulerAngles(glm::quat(getOwner()->getWorldRotation()));
        glm::vec3 wScale = getOwner()->getWorldScale();

        auto [wAABBPos, aabbHalfExtents] = meshComp->getAABB(wScale);
        wAABBPos                         = glm::translate(glm::mat4(1), wPos) * glm::vec4(wAABBPos, 1);
        return {wAABBPos, aabbHalfExtents};
    }
} // namespace our