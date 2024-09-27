#include "apex_sky.h"
#include "prediction.h"
#include "vector.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>
#include <thread>

extern Memory apex_mem;
extern uint64_t g_Base;
extern GlobalVar globals;
float bulletspeed = 0.08;
float bulletgrav = 0.05;

std::array<unsigned char, 4> Item::ItemRarityIds = {15, 42, 47, 54};

// setting up vars, dont edit
extern float veltest;
extern Vector aim_target;

Entity getEntity(uint64_t ptr)
{
    Entity entity = Entity();
    entity.ptr = ptr;
    apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
    return entity;
}

// https://github.com/CasualX/apexbot/blob/master/src/state.cpp#L104
void get_class_name(uint64_t entity_ptr, char *out_str)
{
    uint64_t client_networkable_vtable;
    apex_mem.Read<uint64_t>(entity_ptr + 8 * 3, client_networkable_vtable);

    uint64_t get_client_class;
    apex_mem.Read<uint64_t>(client_networkable_vtable + 8 * 3, get_client_class);

    uint32_t disp;
    apex_mem.Read<uint32_t>(get_client_class + 3, disp);
    const uint64_t client_class_ptr = get_client_class + disp + 7;

    ClientClass client_class;
    apex_mem.Read<ClientClass>(client_class_ptr, client_class);

    apex_mem.ReadArray<char>(client_class.pNetworkName, out_str, 32);
}

int Entity::getTeamId() { return *(int *)(buffer + OFFSET_TEAM); }

int Entity::getHealth() { return *(int *)(buffer + OFFSET_HEALTH); }

int Entity::getShield() { return *(int *)(buffer + OFFSET_SHIELD); }

int Entity::getMaxshield() { return *(int *)(buffer + OFFSET_MAXSHIELD); }

int Entity::getArmortype()
{
    int armortype;
    apex_mem.Read<int>(ptr + OFFSET_ARMORTYPE, armortype);
    return armortype;
}

bool Entity::isZooming() { return *(int *)(buffer + OFFSET_ZOOMING) == 1; }

Vector Entity::getAbsVelocity()
{
    return *(Vector *)(buffer + OFFSET_ABS_VELOCITY);
}

Vector Entity::getPosition() { return *(Vector *)(buffer + OFFSET_ORIGIN); }

Vector Entity::getViewOffset()
{
    return *(Vector *)(buffer + OFFSET_VIEW_OFFSET);
}

void Entity::SetViewAngles(QAngle &angles)
{
    apex_mem.Write<QAngle>(ptr + OFFSET_VIEWANGLES, angles);
}

Vector Entity::GetCamPos() { return *(Vector *)(buffer + OFFSET_CAMERAPOS); }

QAngle Entity::GetRecoil() { return *(QAngle *)(buffer + OFFSET_AIMPUNCH); }

QAngle Entity::GetSwayAngles() { return *(QAngle *)(buffer + OFFSET_BREATH_ANGLES); }

QAngle Entity::GetViewAngles() { return *(QAngle *)(buffer + OFFSET_VIEWANGLES); }

float Entity::GetYaw()
{
    float yaw = 0;
    apex_mem.Read<float>(ptr + OFFSET_YAW, yaw);
    if (yaw < 0)
        yaw += 360;
    yaw += 90;
    if (yaw > 360)
        yaw -= 360;
    return yaw;
}

bool Entity::isVisable(std::unordered_map<uint64_t, float> &vistime,std::unordered_map<uint64_t, float> &aimtime){
        if (vistime[ptr] < lastVisTime()) 
            return true;
        if (aimtime[ptr] < lastCrossHairTime())
            return true;
        return false;
}

void Entity::get_name(uint64_t g_Base, uint64_t index, char *name)
{
    index *= 0x18;
    uint64_t name_ptr = 0;
    apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + index, name_ptr);
    apex_mem.ReadArray<char>(name_ptr, name, 32);
}

bool Entity::isPlayer()
{
    return *(uint64_t *)(buffer + OFFSET_NAMECLASS) == 125780153691248;
}
// firing range dummys
bool Entity::isDummy()
{
    char class_name[33] = {};
    get_class_name(ptr, class_name);
    return strncmp(class_name, "CAI_BaseNPC", 11) == 0;
}

bool Entity::isDummy2() { return getTeamId() == 97; }

bool Entity::isKnocked() { return *(int *)(buffer + OFFSET_BLEED_OUT_STATE) > 0; }

bool Entity::isAlive() { return *(int *)(buffer + OFFSET_LIFE_STATE) == 0; } //>0 dead

float Entity::lastVisTime() { return *(float *)(buffer + OFFSET_VISIBLE_TIME); }

float Entity::lastCrossHairTime() { return *(float *)(buffer + OFFSET_CROSSHAIR_LAST); }

Vector Entity::getBonePositionByHitbox(int id)
{
    Vector origin = getPosition();

    // BoneByHitBox
    uint64_t Model = *(uint64_t *)(buffer + OFFSET_STUDIOHDR);

    // get studio hdr
    uint64_t StudioHdr;
    apex_mem.Read<uint64_t>(Model + 0x8, StudioHdr);

    // get hitbox array
    uint16_t HitboxCache;
    apex_mem.Read<uint16_t>(StudioHdr + 0x34, HitboxCache);
    uint64_t HitboxArray =
        StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));

    uint16_t IndexCache;
    apex_mem.Read<uint16_t>(HitboxArray + 0x4, IndexCache);
    int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));

    uint16_t Bone;
    apex_mem.Read<uint16_t>(HitboxIndex + HitboxArray + (id * 0x20), Bone);

    if (Bone < 0 || Bone > 255)
        return Vector();

    // hitpos
    uint64_t Bones = *(uint64_t *)(buffer + OFFSET_BONES);

    matrix3x4_t Matrix = {};
    apex_mem.Read<matrix3x4_t>(Bones + Bone * sizeof(matrix3x4_t), Matrix);

    return Vector(Matrix.m_flMatVal[0][3] + origin.x,
                  Matrix.m_flMatVal[1][3] + origin.y,
                  Matrix.m_flMatVal[2][3] + origin.z);
}

void Entity::enableGlow(int setting_index, uint8_t insidetype, uint8_t outline_size, std::array<float, 3> highlight_parameter, float glow_dist)
{
    const unsigned char outsidetype = 125;
    std::array<unsigned char, 4> highlightFunctionBits = {
        insidetype,   // InsideFunction
        outsidetype,  // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE
        outline_size, // OutlineRadius: size * 255 / 8
        64            // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7)
    };
    apex_mem.Write<uint8_t>(ptr + OFFSET_GLOW_CONTEXT_ID, setting_index);
    apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 2);

    long highlight_settings_ptr;
    apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlight_settings_ptr);
    apex_mem.Write<typeof(highlightFunctionBits)>(
        highlight_settings_ptr + HIGHLIGHT_TYPE_SIZE * setting_index + 0x0, highlightFunctionBits);
    apex_mem.Write<typeof(highlight_parameter)>(
        highlight_settings_ptr + HIGHLIGHT_TYPE_SIZE * setting_index + 0x4, highlight_parameter);
    apex_mem.Write<float>(ptr + 0x264, glow_dist);
    apex_mem.Write(ptr + OFFSET_GLOW_FIX, 0);
}

bool Entity::isSpec(uint64_t localptr)
{
    uint64_t ObserverList;
    apex_mem.Read<uint64_t>(g_Base + OFF_OBSERVER_LIST, ObserverList);
    uint64_t nameIndex = 0;
    apex_mem.Read<uint64_t>(ptr + 0X38, nameIndex);
    int Index;
    apex_mem.Read<int>(ObserverList + nameIndex * 8 + 0x974, Index);
    if (Index != -1)
    {
        uint64_t SpectatorAddr;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_ENTITYLIST + ((Index & 0xFFFF) << 5), SpectatorAddr);
        if ((SpectatorAddr == localptr))
        {
            return true;
        }
    }
    return false;
}

void Entity::glow_weapon_model(uint64_t g_Base, bool enable_glow, std::array<float, 3> highlight_colors)
{
    uint64_t view_model_handle;
    apex_mem.Read<uint64_t>(ptr + OFFSET_VIEW_MODELS, view_model_handle);
    view_model_handle &= 0xFFFF;
    uint64_t view_model_ptr = 0;
    apex_mem.Read<uint64_t>(g_Base + OFFSET_ENTITYLIST + (view_model_handle << 5), view_model_ptr);

    std::array<unsigned char, 4> highlightFunctionBits = {0, 125, 64, 64};
    if (!enable_glow)
    {
        apex_mem.Write<uint8_t>(view_model_ptr + OFFSET_GLOW_CONTEXT_ID, 90);
        return;
    }

    long highlightSettingsPtr;
    apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
    uint8_t context_id = 71;
    apex_mem.Write<uint8_t>(view_model_ptr + OFFSET_GLOW_CONTEXT_ID, context_id);
    apex_mem.Write<typeof(highlightFunctionBits)>(
        highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * context_id + 0x0, highlightFunctionBits);
    apex_mem.Write<typeof(highlight_colors)>(
        highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * context_id + 0x4, highlight_colors);
}

bool Entity::check_love_player(uint64_t entity_index)
{
    if (global_settings().yuan_p)
    {
        if (this->isDummy())
            return true;
    }
    else
    {
        if (!this->isPlayer())
            return false;
    }
    uint64_t data_fid[4];
    data_fid[0] = *((uint64_t *)(buffer + OFFSET_PLATFORM_UID + 0));
    data_fid[1] = *((uint64_t *)(buffer + OFFSET_PLATFORM_UID + 4));
    data_fid[2] = *((uint64_t *)(buffer + OFFSET_PLATFORM_UID + 16));
    data_fid[3] = *((uint64_t *)(buffer + OFFSET_PLATFORM_UID + 20));
    uint64_t platform_lid = data_fid[0] | data_fid[1] << 32;
    uint64_t eadp_lid = data_fid[1] | data_fid[2] << 32;
    char name[33] = {0};
    this->get_name(g_Base, entity_index - 1, &name[0]);
    return ::check_love_player(platform_lid, eadp_lid, name);
}

// Items
Item getItem(uint64_t ptr)
{
    Item entity = Item();
    entity.ptr = ptr;
    apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
    return entity;
}

Vector Item::getPosition() { return *(Vector *)(buffer + OFFSET_ORIGIN); }

bool Item::isItem()
{
    char class_name[33] = {};
    get_class_name(ptr, class_name);
    return strncmp(class_name, "CPropSurvival", 13) == 0;
}

// Deathboxes
bool Item::isBox()
{
    char class_name[33] = {};
    get_class_name(ptr, class_name);
    return strncmp(class_name, "CDeathBoxProp", 13) == 0;
}

// Traps
bool Item::isTrap()
{
    char class_name[33] = {};
    get_class_name(ptr, class_name);
    return strncmp(class_name, "caustic_trap", 13) == 0;
}

void Item::enableGlow(int setting_index, uint8_t outline_size, std::array<float, 3> highlight_parameter)
{
    std::array<unsigned char, 4> highlightFunctionBits = {
        global_settings().loot_filled, // InsideFunction  HIGHLIGHT_FILL_LOOT_SCANNED
        125,                           // OutlineFunction OutlineFunction
        outline_size,                  // HIGHLIGHT_OUTLINE_LOOT_SCANNED
        64};
    uint8_t contextId = setting_index;
    apex_mem.Write<uint8_t>(ptr + OFFSET_GLOW_CONTEXT_ID, contextId);
    long highlightSettingsPtr;
    apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
    apex_mem.Write<typeof(highlightFunctionBits)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x0, highlightFunctionBits);
    apex_mem.Write<typeof(highlight_parameter)>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * contextId + 0x4, highlight_parameter);
}

void Item::setItemGlow()
{
    std::array<unsigned char, 4> highlightFunctionBits = {0, 125, 64, 64};
    long highlightSettingsPtr;
    apex_mem.Read<long>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
    for (int highlightId : ItemRarityIds)
    {
        apex_mem.Write<typeof(highlightFunctionBits)>(highlightSettingsPtr + (HIGHLIGHT_TYPE_SIZE * highlightId) + 0, highlightFunctionBits);
    }
}

float CalculateFov(Entity &from, Entity &target)
{
    QAngle ViewAngles = from.GetSwayAngles();
    Vector LocalCamera = from.GetCamPos();
    Vector EntityPosition = target.getPosition();
    QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
    return Math::GetFov(ViewAngles, Angle);
}

auto fun_calc_angles = [](Vector LocalCameraPosition, Vector TargetBonePosition,
                          Vector targetVel, float BulletSpeed, float BulletGrav,
                          float deltaTime, int weapid)
{
    QAngle CalculatedAngles = QAngle(0, 0, 0);
    if (BulletSpeed > 1.f)
    {
        if (weapid == 2)
        {
            bulletspeed = 10.08;
            bulletgrav = 10.05;
        }
        else
        {
            bulletspeed = 0.08;
            bulletgrav = 0.05;
        }
        PredictCtx Ctx;
        Ctx.StartPos = LocalCameraPosition;
        Ctx.TargetPos = TargetBonePosition;
        Ctx.BulletSpeed = BulletSpeed - (BulletSpeed * bulletspeed);
        Ctx.BulletGravity = BulletGrav + (BulletGrav * bulletgrav);

        // Add the target's velocity to the prediction context, with an offset
        // in the y direction
        float distanceToTarget = (TargetBonePosition - LocalCameraPosition).Length();
        float timeToTarget = distanceToTarget / BulletSpeed;
        Vector targetPosAhead = TargetBonePosition + (targetVel * timeToTarget);
        Ctx.TargetVel = Vector(targetVel.x, targetVel.y + (targetVel.Length() * deltaTime), targetVel.z);
        Ctx.TargetPos = targetPosAhead;

        aim_target = Ctx.TargetPos;

        if (BulletPredict(Ctx))
            CalculatedAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
    }

    if (CalculatedAngles == QAngle(0, 0, 0))
        CalculatedAngles = Math::CalcAngle(LocalCameraPosition, TargetBonePosition);
    return CalculatedAngles;
};

QAngle CalculateBestBoneAim(Entity &from, Entity &target, WeaponXEntity &weapon, float max_fov, float smooth)
{
    const auto g_settings = global_settings();
    if (!target.isAlive() || (!g_settings.firing_range && target.isKnocked()))
        return QAngle(0, 0, 0);
    uint32_t weap_id = weapon.get_weap_id();
    if (!g_settings.bow_charge_rifle_aim && (weap_id == weapon_id::idweapon_bow || weap_id == weapon_id::idweapon_charge_rifle))
    {
        return QAngle(0, 0, 0);
    }
    float BulletSpeed = weapon.get_projectile_speed();
    float BulletGrav = weapon.get_projectile_gravity();

    float zoom_fov = weapon.get_zoom_fov();
    if (zoom_fov != 0.0f && zoom_fov != 1.0f)
    {
        max_fov *= zoom_fov / 90.0f;
    }

    Vector LocalCamera = from.GetCamPos();
    QAngle ViewAngles = from.GetViewAngles();
    QAngle SwayAngles = from.GetSwayAngles();
    Vector targetVel = target.getAbsVelocity();

    Vector TargetBonePositionMin;
    Vector TargetBonePositionMax;
    // Calculate the time since the last frame (in seconds)
    float deltaTime = 1.0 / g_settings.game_fps;
    // Find best bone
    bool weap_headshot;
    switch (weap_id)
    {
    case idweapon_3030_repeater:
    case idweapon_bow:
    case idweapon_charge_rifle:
    case idweapon_g7_scout:
    case idweapon_kraber:
    case idweapon_longbow:
    case idweapon_sentinel:
    case idweapon_triple_take:
    case idweapon_wingman:
    case idsnipers_mark:
        weap_headshot = true;
        break;
    default:
        weap_headshot = false;
    }
    if (weap_headshot)
    {
        if (LocalCamera.DistTo(target.getPosition()) <= g_settings.headshot_dist)
        {
            TargetBonePositionMax = TargetBonePositionMin =
                target.getBonePositionByHitbox(0);
        }
        else
        {
            TargetBonePositionMax = TargetBonePositionMin =
                target.getBonePositionByHitbox(g_settings.bone);
        }
    }
    else if (g_settings.bone_nearest)
    {
        // find nearest bone
        float NearestBoneDistance = g_settings.max_dist;
        for (int i = 0; i < 4; i++)
        {
            Vector currentBonePosition = target.getBonePositionByHitbox(i);
            float DistanceFromCrosshair =
                (currentBonePosition - LocalCamera).Length();
            if (DistanceFromCrosshair < NearestBoneDistance)
            {
                TargetBonePositionMax = TargetBonePositionMin = currentBonePosition;
                NearestBoneDistance = DistanceFromCrosshair;
            }
        }
    }
    else if (g_settings.bone_auto)
    {
        TargetBonePositionMax = target.getBonePositionByHitbox(5);
        TargetBonePositionMin = target.getBonePositionByHitbox(0);
    }
    else
    {
        TargetBonePositionMax = TargetBonePositionMin =
            target.getBonePositionByHitbox(g_settings.bone);
    }
    int local_held_id = std::get<int>(globals.Get("HeldID"));
    if (local_held_id != -251)
    {
        QAngle CalculatedAnglesMin =
            fun_calc_angles(LocalCamera, TargetBonePositionMin, targetVel, BulletSpeed, BulletGrav, deltaTime, weap_id);
        QAngle CalculatedAnglesMax =
            fun_calc_angles(LocalCamera, TargetBonePositionMax, targetVel, BulletSpeed, BulletGrav, deltaTime, weap_id);

        double fov0 = Math::GetFov(SwayAngles, CalculatedAnglesMin);
        double fov1 = Math::GetFov(SwayAngles, CalculatedAnglesMax);
        if ((fov0 + fov1) * 0.5f > max_fov)
        {
            return QAngle(0, 0, 0);
        }
        Math::NormalizeAngles(CalculatedAnglesMin);
        Math::NormalizeAngles(CalculatedAnglesMax);
        QAngle DeltaMin = CalculatedAnglesMin - ViewAngles;
        QAngle DeltaMax = CalculatedAnglesMax - ViewAngles;
        Math::NormalizeAngles(DeltaMin);
        Math::NormalizeAngles(DeltaMax);

        QAngle Delta = QAngle(0, 0, 0);
        if (DeltaMin.x * DeltaMax.x > 0)
            Delta.x = (DeltaMin.x + DeltaMax.x) * 0.5f;
        if (DeltaMin.y * DeltaMax.y > 0)
            Delta.y = (DeltaMin.y + DeltaMax.y) * 0.5f;

        QAngle SmoothedAngles = ViewAngles + Delta / smooth;
        return SmoothedAngles;
    }
    else
    {
        int weapon_mod_bitfield = weapon.get_mod_bitfield();
        /*Vector local_origin = from.getPosition();
        Vector view_offset = from.getViewOffset();*/
        Vector view_origin = from.GetCamPos(); // local_origin + view_offset;
        Vector target_origin = target.getPosition() + targetVel * deltaTime;
        aim_target = target_origin;
        vector2d_t skynade_angles =
            skynade_angle(weap_id, weapon_mod_bitfield, BulletGrav / 750.0f,
                          BulletSpeed, view_origin.x, view_origin.y, view_origin.z,
                          target_origin.x, target_origin.y, target_origin.z);

        // printf("(%.1f, %.1f)\n", ViewAngles.x, ViewAngles.y);
        if (skynade_angles.x == 0 && skynade_angles.y == 0)
        {
            return ViewAngles;
        }

        const float PIS_IN_180 = 57.2957795130823208767981548141051703f;
        QAngle TargetAngles = QAngle(-skynade_angles.x * PIS_IN_180,
                                     skynade_angles.y * PIS_IN_180, 0);
        QAngle Delta = TargetAngles - ViewAngles;
        return ViewAngles + Delta / g_settings.skynade_smooth;
    }
}

void DoFlick(Entity &from, Entity &target, float *m_vMatrix)
{
    const auto g_settings = global_settings();
    int delay = 0;
    int boneIndex = 0;
    float s2predictpos = 100;
    Vector aimBonePos;
    Vector bestAimBonePos;
    QAngle aimAngles = QAngle(0, 0, 0);
    if (!target.isAlive() || (!g_settings.firing_range && target.isKnocked()))
        return;
    WeaponXEntity weapon = WeaponXEntity();
    weapon.update(from.ptr);
    uint32_t weap_id = weapon.get_weap_id();
    switch (weap_id)
    {
    case idweapon_mastiff:
    case idweapon_peacekeeper:
        delay = 300;
        boneIndex = 2;
        break;
    case idweapon_sentinel:
        delay = 800;
        boneIndex = 0;
        break;
    case idweapon_longbow:
        delay = 600;
        boneIndex = 0;
        break;
    case idweapon_g7_scout:
        delay = 500;
        boneIndex = 0;
        break;
    case idweapon_kraber:
        delay = 1500;
        boneIndex = 0;
        break;
    case idweapon_triple_take:
    case idweapon_3030_repeater:
        delay = 1200;
        boneIndex = 0;
        break;
    case idweapon_wingman:
        delay = 400;
        boneIndex = 0;
        break;
    default:
        return;
    }
    int screenCenterW = g_settings.screen_width / 2;
    int screenCenterH = g_settings.screen_height / 2;
    if (g_settings.flick_nearest)
    {
        float bestPos = 4096.0;
        for (int bone = 0; bone < 6; ++bone)
        {
            aimBonePos = target.getBonePositionByHitbox(bone);
            Vector screenAimBonePos;
            if (WorldToScreen(aimBonePos, m_vMatrix, g_settings.screen_width, g_settings.screen_height, screenAimBonePos))
            {
                float distSquared = pow(screenCenterW - screenAimBonePos.x, 2) + pow(screenCenterH - screenAimBonePos.y, 2);
                if (distSquared < bestPos)
                {
                    s2predictpos = sqrtf(distSquared);
                    bestPos = distSquared;
                    bestAimBonePos = aimBonePos;
                }
            }
        }
    }
    else
    {
        bestAimBonePos = target.getBonePositionByHitbox(boneIndex);
        Vector screenAimBonePos;
        WorldToScreen(bestAimBonePos, m_vMatrix, g_settings.screen_width, g_settings.screen_height, screenAimBonePos);
        s2predictpos = (sqrtf(pow(screenCenterW - screenAimBonePos.x, 2) + pow(screenCenterH - screenAimBonePos.y, 2)));
    }
    // printf("s2predictpos:%d\n",s2predictpos);
    if (s2predictpos > g_settings.flick_fov)
        return;
    float bulletSpeed = weapon.get_projectile_speed();
    float bulletGrav = weapon.get_projectile_gravity();
    Vector LocalCamera = from.GetCamPos();
    // QAngle ViewAngles = from.GetViewAngles();
    // QAngle SwayAngles = from.GetSwayAngles();
    Vector targetVel = target.getAbsVelocity();
    float deltaTime = 1.0 / g_settings.game_fps;

    PredictCtx Ctx;
    Ctx.StartPos = LocalCamera;
    Ctx.TargetPos = bestAimBonePos;
    Ctx.BulletSpeed = bulletSpeed * 0.92;
    Ctx.BulletGravity = bulletGrav * 1.05;
    float distanceToTarget = (bestAimBonePos - LocalCamera).Length();
    float timeToTarget = distanceToTarget / bulletSpeed;
    Vector targetPosAhead = bestAimBonePos + (targetVel * timeToTarget);
    Ctx.TargetVel = Vector(targetVel.x, targetVel.y + (targetVel.Length() * deltaTime), targetVel.z);
    Ctx.TargetPos = targetPosAhead;

    if (BulletPredict(Ctx))
        aimAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
    if (aimAngles == QAngle(0, 0, 0))
        return;
    static std::chrono::time_point<std::chrono::steady_clock> last_flick_time;
    auto now_ms = std::chrono::steady_clock::now();
    if (now_ms <= last_flick_time + std::chrono::milliseconds(delay))
    {
        return;
    }
    Math::NormalizeAngles(aimAngles);
    from.SetViewAngles(aimAngles);
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
    last_flick_time = now_ms;
}

bool WorldToScreen(Vector from, float *m_vMatrix, int targetWidth,
                   int targetHeight, Vector &to)
{
    float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y +
              m_vMatrix[14] * from.z + m_vMatrix[15];

    if (w < 0.01f)
        return false;

    to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z +
           m_vMatrix[3];
    to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z +
           m_vMatrix[7];

    float invw = 1.0f / w;
    to.x *= invw;
    to.y *= invw;

    float x = targetWidth / 2.0;
    float y = targetHeight / 2.0;

    x += 0.5 * to.x * targetWidth + 0.5;
    y -= 0.5 * to.y * targetHeight + 0.5;

    to.x = x;
    to.y = y;
    to.z = 0;
    return true;
}

void WeaponXEntity::update(uint64_t LocalPlayer)
{
    uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
    uint64_t wephandle = 0;
    apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_ACTIVE_WEAPON, wephandle);

    wephandle &= 0xffff;

    uint64_t wep_entity = 0;
    apex_mem.Read<uint64_t>(entitylist + (wephandle << 5), wep_entity);

    projectile_speed = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SPEED, projectile_speed); //[Miscellaneous].CWeaponX!m_flProjectileSpeed in past
    projectile_scale = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SCALE, projectile_scale); //[Miscellaneous].CWeaponX!m_flProjectileScale
    zoom_fov = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_ZOOM_FOV, zoom_fov);
    ammo = 0;
    apex_mem.Read<int>(wep_entity + OFFSET_AMMO, ammo);
    memset(name_str, 0, sizeof(name_str));
    uint64_t name_ptr;
    apex_mem.Read<uint64_t>(wep_entity + OFFSET_MODELNAME, name_ptr);
    apex_mem.ReadArray<char>(name_ptr, name_str, 200);
    mod_bitfield = 0;
    apex_mem.Read<int>(wep_entity + OFFSET_WEAPON_BITFIELD, mod_bitfield);
    weap_id = 0;
    apex_mem.Read<uint32_t>(wep_entity + OFFSET_WEAPON_ID, weap_id);
}

float WeaponXEntity::get_projectile_speed() { return projectile_speed; }

float WeaponXEntity::get_projectile_gravity() { return 750.0f * projectile_scale; }

float WeaponXEntity::get_zoom_fov() { return zoom_fov; }

int WeaponXEntity::get_ammo() { return ammo; }

const char *WeaponXEntity::get_name_str() { return name_str; }

int WeaponXEntity::get_mod_bitfield() { return mod_bitfield; }

uint32_t WeaponXEntity::get_weap_id() { return weap_id; }
