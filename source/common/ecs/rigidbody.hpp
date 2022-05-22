#pragma once

#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "btBulletDynamicsCommon.h"

#include "../deserialize-utils.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/component.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/vec3.hpp"
#include "imgui.h"
#include "mesh/mesh.hpp"
#include <cstdio>
#include <string>

namespace our {

    class RigidBody : public Component {
    public:
        btRigidBody* body;

        RigidBody() = default;

        void fromMeshRenderer(MeshRendererComponent* meshRenderer, std::string type, float mass);

        void fromExtentsTranslationRotation(glm::vec3 boxHalfExtents, glm::vec3 translation, glm::vec3 rotation, std::string type, float m) {
            btCollisionShape* boundingBox = new btBoxShape({boxHalfExtents.x, boxHalfExtents.y, boxHalfExtents.z});
            fromCollisionShapeTranslationRotation(boundingBox, translation, rotation, type, m);
        }

        void fromSphereTranslationRotation(float radius, glm::vec3 translation, glm::vec3 rotation, std::string type, float m) {
            btCollisionShape* boundingSphere = new btSphereShape(radius);
            fromCollisionShapeTranslationRotation(boundingSphere, translation, rotation, type, m);
        }

        void fromCollisionShapeTranslationRotation(btCollisionShape* btColl, glm::vec3 translation, glm::vec3 rotation, std::string type, float m);
        // Scales the AABB to match the world scale of the object
        // does not take into account rotation nor translation (bullet physics AABBs get their translation from their rigidbodies)
        std::pair<glm::vec3, glm::vec3> getAABBWorldScale(Mesh* meshComp);

        virtual void deserialize(const nlohmann::json& data) override;

        static std::string getID() { return "Rigid Body"; }

        virtual std::string getIDPolymorphic() override { return getID(); }

        // Synchronizes bullet's rigidbody with our object's data from the entity debugger.
        void syncWithTransform(glm::mat4 worldRotation, glm::vec3 worldTranslation, bool resetMovement = false);

        virtual void onImmediateGui() override {
            std::string id = std::to_string((long long)this);

            btTransform t;
            body->getMotionState()->getWorldTransform(t);
            float pos[3] = {t.getOrigin().x(), t.getOrigin().getY(), t.getOrigin().getZ()};

            ImGui::DragFloat3(("Rigid body position##" + id).c_str(), pos);

            std::string type = "dynamic";
            int collFlags    = body->getCollisionFlags();

            if(collFlags & btCollisionObject::CF_KINEMATIC_OBJECT)
                type = "kinematic";
            else if(collFlags & btCollisionObject::CF_STATIC_OBJECT)
                type = "static";

            ImGui::LabelText("", "Type: %s", type.c_str());
        }
    };
} // namespace our