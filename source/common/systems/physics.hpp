#pragma once
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "ImGuizmo.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "application.hpp"
#include "btBulletDynamicsCommon.h"
#include "components/camera.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/IImGuiDrawable.h"
#include "ecs/entity.hpp"
#include "ecs/transform.hpp"
#include "ecs/world.hpp"
#include "glm/common.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include "mesh/mesh.hpp"
#include "../components/rigidbody.hpp"
#include "systems/physics-debug.hpp"
#include <cmath>
#include <stdio.h>
#include <string>

#include "GLFW/glfw3.h"
namespace our {
    struct Config {
        int mWidth;
        int mHeight;
        bool mFullscreen;
    };

    class PhysicsSystem : public IImGuiDrawable {
    public:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        // MeshRendererComponent box;
        bool simulate        = true;
        int framesToSimulate = 10000;

        PhysDebugDraw* debugDrawer;

        World* world;

        bool hit = false;

        bool drawDebug = false;

        void initialize(World* w);

        void update(float dt);

        void destroy();

        void EditTransform(const CameraComponent* camera, glm::mat4* matrix, glm::vec2 windowSize, Keyboard* kb);

        // Perhaps it'd be wise to split the real physics simulation from the picking system
        // By having 2 dynamic worlds. One for picking and the other for simulating physics.
        // We'd also have 2 components: RigidBody and Pickable.
        // The picking world would not need to simulate physics, it would only keep track of objects and their
        // AABBs, it would get updated after each frame with new object positions.

        // Might be good to look into btGhostObject
        void editorPickObject();

        // Reads data from bullet physics and updates object transform for graphics drawing
        void bulletToOur();

        void debugDraw() const;

        // Reads our data and sets bullet rigidbodies with it
        void ourToBullet();

        // Checks collisions and calls proper callbacks for each collision
        void collisionCallbacks();

        virtual void onImmediateGui() override;
    };
} // namespace our