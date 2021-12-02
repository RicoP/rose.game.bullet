#include <components/camera.h>
#include <player_state.h>
#include <raylib.h>
#include <rlgl.h>
#include <rose.h>
#include <roseio.h>
#include <rosemath.h>




//////
// https://raw.githubusercontent.com/bulletphysics/bullet3/master/examples/BasicDemo/BasicExample.cpp
/////

#if 1

/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2015 Google Inc. http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

//#include "BasicExample.h"

#include "btBulletDynamicsCommon.h"
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"

#include "../CommonInterfaces/CommonRigidBodyBase.h"

struct BasicExample : public CommonRigidBodyBase
{
  BasicExample(struct GUIHelperInterface* helper)
    : CommonRigidBodyBase(helper)
  {
  }
  virtual ~BasicExample() {}
  virtual void initPhysics();
  virtual void renderScene();
  void resetCamera()
  {
    float dist = 4;
    float pitch = -35;
    float yaw = 52;
    float targetPos[3] = {0, 0, 0};
    m_guiHelper->resetCamera(dist, yaw, pitch, targetPos[0], targetPos[1], targetPos[2]);
  }
};

void BasicExample::initPhysics()
{
  m_guiHelper->setUpAxis(1);

  createEmptyDynamicsWorld();
  //m_dynamicsWorld->setGravity(btVector3(0,0,0));
  m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);

  if (m_dynamicsWorld->getDebugDrawer())
    m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints);

  ///create a few basic rigid bodies
  btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

  //groundShape->initializePolyhedralFeatures();
  //btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);

  m_collisionShapes.push_back(groundShape);

  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(0, -50, 0));

  {
    btScalar mass(0.);
    createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
  }

  {
    //create a few dynamic rigidbodies
    // Re-using the same collision is better for memory usage and performance

    btBoxShape* colShape = createBoxShape(btVector3(.1, .1, .1));

    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    m_collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar mass(1.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
      colShape->calculateLocalInertia(mass, localInertia);

    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
      for (int i = 0; i < ARRAY_SIZE_X; i++)
      {
        for (int j = 0; j < ARRAY_SIZE_Z; j++)
        {
          startTransform.setOrigin(btVector3(
            btScalar(0.2 * i),
            btScalar(2 + .2 * k),
            btScalar(0.2 * j)));

          createRigidBody(mass, startTransform, colShape);
        }
      }
    }
  }

  m_guiHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

void BasicExample::renderScene()
{
  CommonRigidBodyBase::renderScene();
}

CommonExampleInterface* BasicExampleCreateFunc(CommonExampleOptions& options)
{
  return new BasicExample(options.m_guiHelper);
}

B3_STANDALONE_EXAMPLE(BasicExampleCreateFunc)


#endif


/////////END




// TODO: had to edit by hand because vector3 is already defined in rosemath.h
#define IMPL_SERIALIZER
#include "player_state_ser.h"

float fix_death_zone(float f, float death_zone = .15f) {
  float x = f - (f > 0 ? death_zone : -death_zone);
  bool sign_changed_or_zero = x * f <= 0;
  if (sign_changed_or_zero)
    return 0;
  else {
    // return (x) * (1.0f / (1.0f - death_zone));
    return x / (1.0f - death_zone);
  }
}

Vector3 make_Vector3(float x, float y, float z) {
  Vector3 v;
  v.x = x;
  v.y = y;
  v.z = z;
  return v;
}

Vector3 make_Vector3(vector3 p) { return make_Vector3(p.x, p.y, p.z); }

rose::ecs::Camera camera;

vector3 position = vector(0, 15, 15);
vector3 target = vector(0, 0, 0);
vector3 up = vector(0, 1, 0);

WorldState worldState;
size_t record_index = 0;

struct StateFile {
  // TODO: make this more compact
  char path[260];
};

std::vector<StateFile> record;
bool replay = false;

void draw_player(const PlayerState &player, Color color) {
  vector3 p = player.position;
  vector3 look = player.look;

  float eps = 0.01f;
  // Vector3 v = make_Vector3(p.x,p.y,p.z);
  Vector3 Z = make_Vector3(0, 0, 0);
  rlPushMatrix();
  {
    rlTranslatef(p.x, p.y, p.z);

    DrawCylinder(Z, 0.0f, 1.5f, 3.0f, 8, color);
    color.r = ~color.r;
    color.g = ~color.g;
    color.b = ~color.b;
    DrawCylinderWires(Z, 0.0f, 1.5f + eps, 3.0f + eps, 8, color);

    color = BLACK;
    DrawCylinderEx(Z, make_Vector3(look * 2.5f), .3f, .3f, 8, color);
    color = WHITE;
    DrawCylinderWiresEx(Z, make_Vector3(look * 2.5f), .3f + eps, .3f + eps, 8,
                        color);
  }
  rlPopMatrix();
}

float clamp(float x, float lowerlimit, float upperlimit) {
  if (x < lowerlimit)
    x = lowerlimit;
  if (x > upperlimit)
    x = upperlimit;
  return x;
}

float smoothstep(float edge0, float edge1, float x) {
  // Scale, bias and saturate x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  // Evaluate polynomial
  return x * x * (3 - 2 * x);
}

float smoothstep(float x) { return smoothstep(0, 1, x); }

ModelAnimation *anims = nullptr;
Model model;
unsigned int animsCount = 0;
int animFrameCounter = 0;

ROSE_EXPORT void postload() {
  DummyGUIHelper noGfx;

  CommonExampleOptions options(&noGfx);
  CommonExampleInterface* example = BasicExampleCreateFunc(options);


  example->initPhysics();
  example->stepSimulation(1.f / 60.f);
  example->exitPhysics();

  SetWindowTitle("Shoot Yourself " __DATE__ " " __TIME__);

  model = LoadModel("star.glb");
  //anims = LoadModelAnimations("star.iqm", &animsCount);
  //assert(anims);

  construct_defaults(camera);
  rose::meta::request_camera_ownership();

  auto h = rose::hash_from_clock();
  worldState.hero.position.x =
      rose::next_range(h, 1, 10) * (rose::nexti(h) > 0 ? 1 : -1);
  worldState.hero.position.z =
      rose::next_range(h, 1, 10) * (rose::nexti(h) > 0 ? 1 : -1);
}

ROSE_EXPORT void draw() {
  SetWindowTitle("Shoot Yourself " __DATE__ " " __TIME__);
  // Vector3 position;
  // position.x = 0;
  // position.y = 0;
  // position.z = 0;

  // Star
  {
    animFrameCounter++;
    if(anims) {
      UpdateModelAnimation(model, anims[0], animFrameCounter);
      if (animFrameCounter >= anims[0].frameCount)
        animFrameCounter = 0;
    }
    //DrawModel(model, {0, 0, 0}, 1.0f, WHITE);
    //void DrawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint)
    static float angle = 0;
    angle += 2.5f;
    DrawModelEx(model, {0, 0, 0}, {0, 1, 0}, angle, {1, 1, 1}, WHITE);
  }

  constexpr float speed = .4f;

  float xl = fix_death_zone(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X));
  float yl = fix_death_zone(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y));
  float xr = fix_death_zone(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X));
  float yr = fix_death_zone(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y));

  if (xr != 0 && yr != 0) {
    worldState.hero.look = normalized(vector(xr, 0, yr));
  }

  worldState.hero.position.x += (speed * xl);
  worldState.hero.position.z += (speed * yl);
  draw_player(worldState.hero, GOLD);
  draw_player(worldState.other, BLUE);

  if (replay) {
    if (record_index < record.size()) {
      FILE *f = fopen(record[record_index].path, "rb");
      assert(f);

      fseek(f, 0, SEEK_END);
      long length = ftell(f) + 1;

      fseek(f, 0, SEEK_SET);
      char *buffer = (char *)std::malloc(length);
      assert(buffer);

      fread(buffer, 1, length, f);
      buffer[length - 1] = 0;
      fclose(f);

      JsonDeserializer jsond(buffer);
      rose::ecs::deserialize(worldState.other, jsond);

      std::free(buffer);
      record_index++;
    }
  }

  DrawGrid(10, 1.0f); // Draw a grid

  camera.lookat = smat4_look_at(target + position, target, up);

  auto &meta = rose::meta::system_data();
  meta.camera = camera;

  // add new other state
  auto &file = record.emplace_back();
  tmpnam(file.path);
  rose::io::json::write(worldState.hero, rose::io::Folder::Working, file.path);
}

void save() {
  rose::io::json::write(worldState, rose::io::Folder::Working, "player.json");
}

void clear_history() {
  for (auto &path : record) {
    remove(path.path);
  }
  record.clear();
  record_index = 0;
  replay = false;
}

ROSE_EXPORT void predestroy() { clear_history(); }

ROSE_EXPORT void ui() {
  if (rose::ui::button("Clear")) {
    clear_history();
  }

  char text[256];
  sprintf(text, "Replay Record: %d", record.size());
  if (rose::ui::button("Replay Record")) {
    // if (rose::ui::button(text)) {
    replay = true;
    record_index = 0;
  }

  if (rose::ui::button(anims ? "Anims OK" : "Anims NOT OK")) {
  }
}

// TODO: put this maybe in rose.h ???
// Rose binding implementation

#include <components/components.h>
#include <roselib/context.h>
#include <rosemath.h>

static rose::Context *rose_context = nullptr;
ROSE_EXPORT void set_context(rose::Context *new_context) {
  rose_context = new_context;
}

#define FUNC_SIG(ns, rt, name, sig, call)                                      \
  namespace rose {                                                             \
  namespace ns {                                                               \
  rt name sig { return rose_context->ns##_##name call; }                       \
  }                                                                            \
  }

#define PROC_SIG(ns, name, sig, call)                                          \
  namespace rose {                                                             \
  namespace ns {                                                               \
  void name sig { rose_context->ns##_##name call; }                            \
  }                                                                            \
  }

#include <roselib/internal/funcs.inc>

// Implementation cmath
#include <mathc/mathc.c>