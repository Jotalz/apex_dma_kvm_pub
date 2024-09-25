#include "Math.h"
#include "memory.hpp"
#include "offsets.h"
#include "vector.h"
#include <cstdint>
#include <shared_mutex>

#define NUM_ENT_ENTRIES (1 << 12)
#define ENT_ENTRY_MASK (NUM_ENT_ENTRIES - 1)

class Entity
{
public:
  uint64_t ptr;
  uint8_t buffer[0x3FF0];
  Vector getPosition();
  bool isDummy();
  bool isDummy2();
  bool isPlayer();
  bool isKnocked();
  bool isAlive();
  float lastVisTime();
  bool isSpec(uint64_t localptr);
  int getTeamId();
  int getHealth();
  int getShield();
  int getArmortype();
  int getMaxshield();
  bool isZooming();
  Vector getViewOffset();
  Vector getAbsVelocity();
  QAngle GetSwayAngles();
  QAngle GetViewAngles();
  Vector GetCamPos();
  QAngle GetRecoil();
  float GetYaw();
  void enableGlow(int setting_index, uint8_t insidetype, uint8_t outline_size,
                  std::array<float, 3> highlight_parameter, float glow_dist);
  float lastCrossHairTime();
  void SetViewAngles(QAngle &angles);
  Vector getBonePositionByHitbox(int id);
  void get_name(uint64_t g_Base, uint64_t index, char *name);
  void glow_weapon_model(uint64_t g_Base, bool enable_glow,
                         std::array<float, 3> highlight_colors);
  bool check_love_player(uint64_t entity_index);
};

class Item
{
public:
  uint64_t ptr;
  uint8_t buffer[0x3FF0];
  Vector getPosition();
  static std::array<unsigned char, 4> ItemRarityIds;
  bool isItem();
  bool isBox();
  bool isTrap();
  void enableGlow(int setting_index, uint8_t outline_size, std::array<float, 3> highlight_parameter);
  static void setItemGlow();
};

class WeaponXEntity
{
public:
  void update(uint64_t LocalPlayer);
  float get_projectile_speed();
  float get_projectile_gravity();
  float get_zoom_fov();
  int get_ammo();
  const char *get_name_str();
  int get_mod_bitfield();
  uint32_t get_weap_id();

private:
  float projectile_scale;
  float projectile_speed;
  float zoom_fov;
  int ammo;
  char name_str[200];
  int mod_bitfield;
  uint32_t weap_id;
};

struct ClientClass
{
  uint64_t pCreateFn;
  uint64_t pCreateEventFn;
  uint64_t pNetworkName;
  uint64_t pRecvTable;
  uint64_t pNext;
  uint32_t ClassID;
  uint32_t ClassSize;
};

// DONE WITH THE EDITING
// Player Definitions, dont edit unless you know what you are doing.
typedef struct player
{
  float dist = 0;
  int entity_team = 0;
  float boxMiddle = 0;
  float h_y = 0;
  float width = 0;
  float height = 0;
  float b_x = 0;
  float b_y = 0;
  bool knocked = false;
  bool visible = false;
  int health = 0;
  int shield = 0;
  // seer
  int maxshield = 0;
  int armortype = 0;
  Vector EntityPosition;
  Vector LocalPlayerPosition;
  QAngle localviewangle;
  float targetyaw = 0;
  bool is_alive = true;
  bool is_love = false;
  bool is_spectator = false;
  char name[33] = {0};
} player;

struct Matrix
{
  float matrix[16];
};

Entity getEntity(uintptr_t ptr);
Item getItem(uintptr_t ptr);

bool WorldToScreen(Vector from, float *m_vMatrix, int targetWidth,
                   int targetHeight, Vector &to);
float CalculateFov(Entity &from, Entity &target);
QAngle CalculateBestBoneAim(Entity &from, Entity &target, WeaponXEntity &weapon, float max_fov, float smooth);
void DoFlick(Entity &from, Entity &target, float *m_vMatrix);
void get_class_name(uint64_t entity_ptr, char *out_str);

enum weapon_id : int32_t
{
  idweapon_r301 = 0,
  idweapon_sentinel = 1,
  idweapon_bow = 2,
  idsheila_stationary = 14,
  idsheila = 58,
  idweapon_rampage = 6,
  idmelee = 121,
  idsnipers_mark = 78,
  idweapon_alternator = 84,
  idweapon_re45 = 85,
  idweapon_charge_rifle = 87,
  idweapon_devotion = 88,
  idweapon_longbow = 90,
  idweapon_havoc = 91,
  idweapon_eva8 = 92,
  idweapon_flatline = 94,
  idweapon_g7_scout = 95,
  idweapon_hemlock = 96,
  idweapon_kraber = 98,
  idweapon_lstar = 99,
  idweapon_mastiff = 101,
  idweapon_mozambique = 102,
  idweapon_prowler = 107,
  idweapon_peacekeeper = 109,
  idweapon_r99 = 111,
  idweapon_p2020 = 112,
  idweapon_spitfire = 113,
  idweapon_triple_take = 114,
  idweapon_wingman = 115,
  idweapon_volt = 117,
  idweapon_3030_repeater = 118,
  idweapon_car_smg = 119,
  idweapon_nemesis = 120,
  idthrowing_knife = 178,
  idgrenade_thermite = 179,
  idgrenade_frag = 180,
  idgrenade_arc_star = 181,
  idmax
};

typedef struct
{
  uint64_t item_id;
  Vector position;
  float distance;
} TreasureClue;

struct GlobalVar
{
  uint64_t g_Base;
  uint64_t ViewMatrix = 0;
  int local_held_id = 2147483647;
  uint32_t local_weapon_id = 2147483647;

  mutable std::shared_mutex GlobalVarMutex;

  enum class Field
  {
    g_Base,
    ViewMatrix,
    local_held_id,
    local_weapon_id
  };

  template <typename T>
  void SetGlobalVar(Field field, T value)
  {
    std::unique_lock<std::shared_mutex> lock(GlobalVarMutex);

    switch (field)
    {
    case Field::g_Base:
      g_Base = static_cast<uint64_t>(value);
      break;
    case Field::ViewMatrix:
      ViewMatrix = static_cast<uint64_t>(value);
      break;
    case Field::local_held_id:
      local_held_id = static_cast<int>(value);
      break;
    case Field::local_weapon_id:
      local_weapon_id = static_cast<uint32_t>(value);
      break;
    default:
      throw std::invalid_argument("Invalid field specified.");
    }
  }

  template <typename T>
  T GetGlobalVar(Field field) const
  {
    std::shared_lock<std::shared_mutex> lock(GlobalVarMutex);

    switch (field)
    {
    case Field::g_Base:
      return static_cast<T>(g_Base);
    case Field::ViewMatrix:
      return static_cast<T>(ViewMatrix);
    case Field::local_held_id:
      return static_cast<T>(local_held_id);
    case Field::local_weapon_id:
      return static_cast<T>(local_weapon_id);
    default:
      throw std::invalid_argument("Invalid field specified.");
    }
  }
};