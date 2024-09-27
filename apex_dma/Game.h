#pragma once
#include "Math.h"
#include "memory.hpp"
#include "offsets.h"
#include "vector.h"
#include "FNVHash.h"
#include <cstdint>
#include <shared_mutex>
#include <variant>
#include <unordered_map>
#include <string>
#include <iostream>

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
  float lastCrossHairTime();
  bool isSpec(uint64_t localptr);
  int getTeamId();
  int getHealth();
  int getShield();
  int getArmortype();
  int getMaxshield();
  bool isZooming();
  bool isVisable(std::unordered_map<uint64_t, float> &vistime,std::unordered_map<uint64_t, float> &aimtime);
  Vector getViewOffset();
  Vector getAbsVelocity();
  QAngle GetSwayAngles();
  QAngle GetViewAngles();
  Vector GetCamPos();
  QAngle GetRecoil();
  float GetYaw();
  void enableGlow(int setting_index, uint8_t insidetype, uint8_t outline_size,
                  std::array<float, 3> highlight_parameter, float glow_dist);
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

Entity getEntity(uint64_t ptr);
Item getItem(uint64_t ptr);

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


struct GlobalVar {
    using VarType = std::variant<int8_t, uint32_t, uint64_t, int, float, bool, Matrix>;
    std::unordered_map<std::string, bool> mixtape = { {"control", true}, {"freedm", true}, {"arenas", true},{"survival", false} };
    std::unordered_map<std::string, VarType, FnvHash> variables;
    mutable std::shared_mutex GlobalVarMutex;

    void Set(const std::string &key, VarType value) {
        std::unique_lock<std::shared_mutex> lock(GlobalVarMutex);
        variables[key] = value;
    }

    VarType Get(const std::string &key) const {
        std::shared_lock<std::shared_mutex> lock(GlobalVarMutex);
        return variables.at(key);
    }

    template<typename T>
    T GetOrDefault(const std::string& key, T default_value) const {
        std::shared_lock<std::shared_mutex> lock(GlobalVarMutex);
        auto it = variables.find(key);
        if (it != variables.end()) {
            if (const T* value = std::get_if<T>(&it->second)) {
                return *value;
            }
        }
        return default_value;
    }

    void Clear(){
      std::unique_lock<std::shared_mutex> lock(GlobalVarMutex);
      variables.clear();
    }
};

struct AimAssist {
    private:
        bool aiming = false;
        bool gun_safety = true;
        bool locked = false;
        float max_fov = 10;
        float smooth = 120;
        float target_score_max = 10000000.0;
        uint64_t aim_entity = 0;
        uint64_t tmp_aim_entity = 0;
        uint64_t locked_aim_entity = 0;
        mutable std::shared_mutex AimAssistMutex;
    public:
        void SetAimingState(bool aim_state) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            aiming = aim_state;
        }

        bool GetAimingState() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return aiming;
        }

        void SetGunSafety(bool safety) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            gun_safety = safety;
        }

        bool GetGunSafety() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return gun_safety;
        }

        void SetLock(bool islock) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            locked = islock;
        }

        bool GetLock() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return locked;
        }

        void SetMaxFov(float max_fov) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            this->max_fov = max_fov;
        }

        float GetMaxFov() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return max_fov;
        }

        void SetSmooth(float smooth) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            this->smooth = smooth;
        }

        float GetSmooth() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return smooth;
        }

        void SetTargetScoreMax(float target_score_max) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            this->target_score_max = target_score_max;
        }

        float GetTargetScoreMax() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return target_score_max;
        }

        void SetAimentity(uint64_t aimentity) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            aim_entity = aimentity;
        }

        uint64_t GetAimentity() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return aim_entity;
        }

        void SetTmpAimentity(uint64_t tmp_aimentity) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            tmp_aim_entity = tmp_aimentity;
        }

        uint64_t GetTmpAimentity() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return tmp_aim_entity;
        }

        void SetLockedAimentity(uint64_t locked_aimentity) {
            std::unique_lock<std::shared_mutex> lock(AimAssistMutex);
            locked_aim_entity = locked_aimentity;
        }

        uint64_t GetLockedAimentity() const {
            std::shared_lock<std::shared_mutex> lock(AimAssistMutex);
            return locked_aim_entity;
        }
};