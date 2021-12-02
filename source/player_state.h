#pragma once

#include <rosemath.h>

//@Imposter
/*
struct vector3 {
    float x,y,z;
};

bool operator==(vector3 lhs, vector3 rhs);
bool operator!=(vector3 lhs, vector3 rhs);
void serialize(vector3 &o, ISerializer &s);
void deserialize(vector3 &o, IDeserializer &s);
*/

struct Collectable {
    vector3 position;
    int id;
    bool collected;
};

struct PlayerState {
    vector3 position;
    vector3 look;
    std::vector<int> collectable_ids;
};

struct WorldState {
    PlayerState hero;
    PlayerState other;
    std::vector<Collectable> collectables;
};
