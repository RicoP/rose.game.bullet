#pragma once

#include <rose/hash.h>
#include <serializer/serializer.h>

///////////////////////////////////////////////////////////////////
//  AUTOGEN                                                      //
//  command:
//    rose.parser -I .\player_state.h -O .\player_state_ser.h
///////////////////////////////////////////////////////////////////

namespace rose {
  namespace ecs {
  }
  hash_value         hash(const vector3 &o);
  void construct_defaults(      vector3 &o); //TODO: implement me
}


struct                Collectable;
namespace rose {
  namespace ecs {
    void        serialize(Collectable &o, ISerializer &s);
    void      deserialize(Collectable &o, IDeserializer &s);
  }
  hash_value         hash(const Collectable &o);
  void construct_defaults(      Collectable &o); //TODO: implement me
}
bool operator==(const Collectable &lhs, const Collectable &rhs);
bool operator!=(const Collectable &lhs, const Collectable &rhs);


struct                PlayerState;
namespace rose {
  namespace ecs {
    void        serialize(PlayerState &o, ISerializer &s);
    void      deserialize(PlayerState &o, IDeserializer &s);
  }
  hash_value         hash(const PlayerState &o);
  void construct_defaults(      PlayerState &o); //TODO: implement me
}
bool operator==(const PlayerState &lhs, const PlayerState &rhs);
bool operator!=(const PlayerState &lhs, const PlayerState &rhs);


struct                WorldState;
namespace rose {
  namespace ecs {
    void        serialize(WorldState &o, ISerializer &s);
    void      deserialize(WorldState &o, IDeserializer &s);
  }
  hash_value         hash(const WorldState &o);
  void construct_defaults(      WorldState &o); //TODO: implement me
}
bool operator==(const WorldState &lhs, const WorldState &rhs);
bool operator!=(const WorldState &lhs, const WorldState &rhs);


#ifdef IMPL_SERIALIZER

    #include <cstring>

    //internal helper methods
    template<class T>
    bool rose_parser_equals(const T& lhs, const T& rhs) {
      return lhs == rhs;
    }

    template<class T, size_t N>
    bool rose_parser_equals(const T(&lhs)[N], const T(&rhs)[N]) {
      for (size_t i = 0; i != N; ++i) {
        if (lhs[i] != rhs[i]) return false;
      }
      return true;
    }

    template<size_t N>
    bool rose_parser_equals(const char(&lhs)[N], const char(&rhs)[N]) {
      for (size_t i = 0; i != N; ++i) {
        if (lhs[i] != rhs[i]) return false;
        if (lhs[i] == 0) return true;
      }
      return true;
    }

    template<class T>
    bool rose_parser_equals(const std::vector<T> &lhs, const std::vector<T> &rhs) {
      if (lhs.size() != rhs.size()) return false;
      for (size_t i = 0; i != lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) return false;
      }
      return true;
    }

    template<class TL, class TR>
    void assign(TL& lhs, TR&& rhs) {
      lhs = rhs;
    }

    template<class T>
    void construct_default(std::vector<T> & v) {
      c.clear();
    }
  
///////////////////////////////////////////////////////////////////
//  struct vector3
///////////////////////////////////////////////////////////////////
rose::hash_value rose::hash(const vector3 &o) {
  rose::hash_value h = rose::hash(o.x);
  h = rose::xor64(h);
  h ^= rose::hash(o.y);
  h = rose::xor64(h);
  h ^= rose::hash(o.z);
  return h;
}
///////////////////////////////////////////////////////////////////
//  struct Collectable
///////////////////////////////////////////////////////////////////
bool operator==(const Collectable &lhs, const Collectable &rhs) {
  return
    rose_parser_equals(lhs.position, rhs.position) &&
    rose_parser_equals(lhs.id, rhs.id) &&
    rose_parser_equals(lhs.collected, rhs.collected) ;
}

bool operator!=(const Collectable &lhs, const Collectable &rhs) {
  return
    !rose_parser_equals(lhs.position, rhs.position) ||
    !rose_parser_equals(lhs.id, rhs.id) ||
    !rose_parser_equals(lhs.collected, rhs.collected) ;
}

void rose::ecs::serialize(Collectable &o, ISerializer &s) {
  if(s.node_begin("Collectable", rose::hash("Collectable"), &o)) {
    s.key("position");
    serialize(o.position, s);
    s.key("id");
    serialize(o.id, s);
    s.key("collected");
    serialize(o.collected, s);
    s.node_end();
  }
  s.end();
}

void rose::ecs::deserialize(Collectable &o, IDeserializer &s) {
  //TODO: implement me
  //construct_defaults(o);

  while (s.next_key()) {
    switch (s.hash_key()) {
      case rose::hash("position"):
        deserialize(o.position, s);
        break;
      case rose::hash("id"):
        deserialize(o.id, s);
        break;
      case rose::hash("collected"):
        deserialize(o.collected, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

rose::hash_value rose::hash(const Collectable &o) {
  rose::hash_value h = rose::hash(o.position);
  h = rose::xor64(h);
  h ^= rose::hash(o.id);
  h = rose::xor64(h);
  h ^= rose::hash(o.collected);
  return h;
}
///////////////////////////////////////////////////////////////////
//  struct PlayerState
///////////////////////////////////////////////////////////////////
bool operator==(const PlayerState &lhs, const PlayerState &rhs) {
  return
    rose_parser_equals(lhs.position, rhs.position) &&
    rose_parser_equals(lhs.look, rhs.look) &&
    rose_parser_equals(lhs.collectable_ids, rhs.collectable_ids) ;
}

bool operator!=(const PlayerState &lhs, const PlayerState &rhs) {
  return
    !rose_parser_equals(lhs.position, rhs.position) ||
    !rose_parser_equals(lhs.look, rhs.look) ||
    !rose_parser_equals(lhs.collectable_ids, rhs.collectable_ids) ;
}

void rose::ecs::serialize(PlayerState &o, ISerializer &s) {
  if(s.node_begin("PlayerState", rose::hash("PlayerState"), &o)) {
    s.key("position");
    serialize(o.position, s);
    s.key("look");
    serialize(o.look, s);
    s.key("collectable_ids");
    serialize(o.collectable_ids, s);
    s.node_end();
  }
  s.end();
}

void rose::ecs::deserialize(PlayerState &o, IDeserializer &s) {
  //TODO: implement me
  //construct_defaults(o);

  while (s.next_key()) {
    switch (s.hash_key()) {
      case rose::hash("position"):
        deserialize(o.position, s);
        break;
      case rose::hash("look"):
        deserialize(o.look, s);
        break;
      case rose::hash("collectable_ids"):
        deserialize(o.collectable_ids, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

rose::hash_value rose::hash(const PlayerState &o) {
  rose::hash_value h = rose::hash(o.position);
  h = rose::xor64(h);
  h ^= rose::hash(o.look);
  h = rose::xor64(h);
  h ^= rose::hash(o.collectable_ids);
  return h;
}
///////////////////////////////////////////////////////////////////
//  struct WorldState
///////////////////////////////////////////////////////////////////
bool operator==(const WorldState &lhs, const WorldState &rhs) {
  return
    rose_parser_equals(lhs.hero, rhs.hero) &&
    rose_parser_equals(lhs.other, rhs.other) &&
    rose_parser_equals(lhs.collectables, rhs.collectables) ;
}

bool operator!=(const WorldState &lhs, const WorldState &rhs) {
  return
    !rose_parser_equals(lhs.hero, rhs.hero) ||
    !rose_parser_equals(lhs.other, rhs.other) ||
    !rose_parser_equals(lhs.collectables, rhs.collectables) ;
}

void rose::ecs::serialize(WorldState &o, ISerializer &s) {
  if(s.node_begin("WorldState", rose::hash("WorldState"), &o)) {
    s.key("hero");
    serialize(o.hero, s);
    s.key("other");
    serialize(o.other, s);
    s.key("collectables");
    serialize(o.collectables, s);
    s.node_end();
  }
  s.end();
}

void rose::ecs::deserialize(WorldState &o, IDeserializer &s) {
  //TODO: implement me
  //construct_defaults(o);

  while (s.next_key()) {
    switch (s.hash_key()) {
      case rose::hash("hero"):
        deserialize(o.hero, s);
        break;
      case rose::hash("other"):
        deserialize(o.other, s);
        break;
      case rose::hash("collectables"):
        deserialize(o.collectables, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

rose::hash_value rose::hash(const WorldState &o) {
  rose::hash_value h = rose::hash(o.hero);
  h = rose::xor64(h);
  h ^= rose::hash(o.other);
  h = rose::xor64(h);
  h ^= rose::hash(o.collectables);
  return h;
}

#endif
