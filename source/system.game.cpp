#include <components/camera.h>
#include <player_state.h>
#include <raylib.h>
#include <rlgl.h>
#include <rose.h>
#include <roseio.h>
#include <rosemath.h>


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