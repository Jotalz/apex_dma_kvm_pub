#include "Client/main.h"
#include "Game.h"
#include "apex_sky.h"
#include "vector.h"
#include "items.h"
#include <array>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <atomic>

#define UPDATEITEM 0
// this is a test, with seconds
Memory apex_mem;
uint64_t g_Base;
uint64_t ViewMatrix = 0;

// Just setting things up, dont edit.
aimbot_state_t aimbot;
int LocalTeamID = 0;
const int ToRead = 100;
std::atomic<bool> TriggerReady = false;
std::atomic<bool> FlickReady = false;
std::atomic<bool> QuickGlow = true;
std::atomic<bool> QuickAim = true;
std::atomic<bool> Isdone = false; // Prevent frequent writes during the superGrpple
std::atomic<int> local_held_id = 2147483647;

bool actions_t = false;
bool cactions_t = false;
bool terminal_t = false;
bool overlay_t = false;
bool esp_t = false;
bool aim_t = false;
bool item_t = false;

extern Vector aim_target; // for esp
extern float bulletspeed;
extern float bulletgrav;
bool next2 = false;
bool valid = false;
Vector esp_local_pos;

uint32_t local_weapon_id = 2147483647;
int itementcount = 10000;
int map = 0;
std::vector<TreasureClue> treasure_clues;
std::map<uint64_t, uint64_t> centity_to_index; // Map centity to entity index
float lastvis_esp[ToRead];
float lastvis_aim[ToRead];
std::vector<Entity> spectators, allied_spectators;
std::mutex spectatorsMtx;

int EntTeam;
int LocTeam;
//^^ Don't EDIT^^

uint32_t button_state[4];
bool isPressed(uint32_t button_code)
{
    return (button_state[static_cast<uint32_t>(button_code) >> 5] &
            (1 << (static_cast<uint32_t>(button_code) & 0x1f))) != 0;
}

void memory_io_panic(const char *info)
{
    quit_tui_menu();
    std::cout << "Error " << info << std::endl;
    exit(0);
}

// Define rainbow color function
void rainbowColor(int frame_number, std::array<float, 3> &colors)
{
    const float frequency = 0.1; // Adjust the speed of color change
    const float amplitude = 0.5; // Adjust the amplitude of color change

    // Use the sine function to generate rainbow color variation
    float r = sin(frequency * frame_number + 0) * amplitude + 0.5;
    float g = sin(frequency * frame_number + 2) * amplitude + 0.5;
    float b = sin(frequency * frame_number + 4) * amplitude + 0.5;

    // Clamp the colors to the range [0, 1]
    colors[0] = fmax(0, fmin(1, r));
    colors[1] = fmax(0, fmin(1, g));
    colors[2] = fmax(0, fmin(1, b));
}

bool IsInTriggerZone(WeaponXEntity &weapon, Vector localCameraPos, Entity &target,
                     bool isInXHair, float screen_width, float screen_height)
{
    bool light = false;
    bool is_triggerzone = false;
    int boneIndex;
    int delay = 100;
    float boxWidth, boxDepth, boxHeight;
    Vector targetBonePositionPre;
    Vector screenTargetBonePositionPre;
    float projectile_speed = weapon.get_projectile_speed();
    float projectile_scale = weapon.get_projectile_gravity();
    Matrix view_matrix_data = {};
    apex_mem.Read<Matrix>(ViewMatrix, view_matrix_data);

    switch (local_weapon_id)
    {
    case idweapon_eva8:
        delay = 300;
        boneIndex = 2;
        break;
    case idweapon_p2020:
    case idweapon_mozambique:
        delay = 200;
        boneIndex = 2;
        break;
    case idweapon_mastiff:
    case idweapon_peacekeeper:
        delay = 400;
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
        delay = 500;
        boneIndex = 0;
        break;
    default:
        delay = 50;
        boneIndex = 2;
    }
    static std::chrono::time_point<std::chrono::steady_clock> last_trigger_time;
    auto now_ms = std::chrono::steady_clock::now();
    if (now_ms <= last_trigger_time + std::chrono::milliseconds(delay))
    {
        return false;
    }
    last_trigger_time = now_ms;
    Vector targetBonePosition = target.getBonePositionByHitbox(boneIndex);
    if (projectile_speed > 1.f)
    {
        float distanceToTarget = (targetBonePosition - localCameraPos).Length();
        float timeToTarget = distanceToTarget / projectile_speed;
        Vector targetPosAhead = targetBonePosition + (target.getAbsVelocity() * timeToTarget);
        float drop = 0.5f * projectile_scale * timeToTarget * timeToTarget;
        targetPosAhead.z += drop;
        targetBonePositionPre = targetPosAhead;
    }
    else
    {
        targetBonePositionPre = targetBonePosition;
        light = true;
    }
    if (light)
    {
        return isInXHair;
    }
    if (boneIndex == 0)
    {
        boxWidth = boxDepth = boxHeight = 5.0;
    }
    else if (boneIndex == 2)
    {
        boxWidth = 5.0;
        boxDepth = 5.0;
        boxHeight = 8.0;
    }
    std::vector<Vector> corners = {
        {targetBonePositionPre.x + boxWidth, targetBonePositionPre.y + boxDepth, targetBonePositionPre.z + boxHeight},
        {targetBonePositionPre.x - boxWidth, targetBonePositionPre.y - boxDepth, targetBonePositionPre.z - boxHeight},
        {targetBonePositionPre.x + boxWidth, targetBonePositionPre.y - boxDepth, targetBonePositionPre.z + boxHeight},
        {targetBonePositionPre.x - boxWidth, targetBonePositionPre.y + boxDepth, targetBonePositionPre.z - boxHeight},
    };
    float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
    for (const auto &corner : corners)
    {
        Vector screenPos;
        if (WorldToScreen(corner, view_matrix_data.matrix, screen_width, screen_height, screenPos))
        {
            minX = (std::min)(minX, screenPos.x);
            maxX = (std::max)(maxX, screenPos.x);
            minY = (std::min)(minY, screenPos.y);
            maxY = (std::max)(maxY, screenPos.y);
        }
    }
    Vector2D Center(screen_width / 2, screen_height / 2);
    if ((Center.x >= (minX - 1)) && (Center.x <= (maxX + 1)) &&
        (Center.y >= (minY - 1.5)) && (Center.y <= (maxY + 1.5)))
    {
        is_triggerzone = true;
    }
    return is_triggerzone;
}

bool IsInCrossHair(Entity &target)
{
    static uint64_t last_t = 0;
    static float last_crosshair_target_time = -1.f;
    float now_crosshair_target_time = target.lastCrossHairTime();
    bool is_trigger = false;
    if (last_t == target.ptr)
    {
        if (last_crosshair_target_time != -1.f)
        {
            if (now_crosshair_target_time > last_crosshair_target_time)
            {
                is_trigger = true;
                last_crosshair_target_time = -1.f;
            }
            else
            {
                is_trigger = false;
                last_crosshair_target_time = now_crosshair_target_time;
            }
        }
        else
        {
            is_trigger = false;
            last_crosshair_target_time = now_crosshair_target_time;
        }
    }
    else
    {
        last_t = target.ptr;
        last_crosshair_target_time = -1.f;
    }
    return is_trigger;
}

void MapRadarTesting(uint64_t localptr)
{
    int localPlayerTeamID;
    apex_mem.Read<int>(localptr + OFFSET_TEAM, localPlayerTeamID);
    if (localPlayerTeamID != 1)
    {
        using namespace std::chrono;
        auto start_time = steady_clock::now();
        auto end_time = start_time + milliseconds(200); // 200 milliseconds
        while (steady_clock::now() < end_time)
        {
            apex_mem.Write<int>(localptr + OFFSET_TEAM, 1);
        }
        start_time = steady_clock::now();
        end_time = start_time + milliseconds(200); // 200 milliseconds
        while (steady_clock::now() < end_time)
        {
            apex_mem.Write<int>(localptr + OFFSET_TEAM, localPlayerTeamID);
        }
    }
}

void ClientActions()
{
    cactions_t = true;
    while (cactions_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        while (g_Base != 0)
        {
            const auto g_settings = global_settings();

            // read player ptr
            uint64_t local_player_ptr = 0;
            uint64_t viewRender_ptr = 0;
            apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, local_player_ptr);
            if (local_player_ptr == 0)
                continue;
            apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRender_ptr);
            apex_mem.Read<uint64_t>(viewRender_ptr + OFFSET_MATRIX, ViewMatrix);
            // read game states
            apex_mem.Read<typeof(button_state)>(g_Base + OFFSET_INPUT_SYSTEM + 0xb0, button_state);
            int attack_state = 0, zoom_state = 0, jump_state = 0, backWardState = 0, curFrameNumber = 0, skyDriveState = 0,
                duck_state = 0, force_foreward = 0, foreward_state = 0, flags = 0;
            float wallrunStart = 0, wallrunClear = 0;
            bool longclimb = false;
            apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK, attack_state);              // 108开火
            apex_mem.Read<int>(g_Base + OFFSET_IN_ZOOM, zoom_state);                  // 109瞄准
            apex_mem.Read<int>(g_Base + OFFSET_IN_JUMP, jump_state);                  // 跳跃状态
            apex_mem.Read<int>(g_Base + OFFSET_IN_BACKWARD, backWardState);           // 后退状态
            apex_mem.Read<int>(g_Base + OFFSET_GLOBAL_VARS + 0x0008, curFrameNumber); // GlobalVars + 0x0008
            apex_mem.Read<int>(local_player_ptr + OFFSET_FLAGS, flags);               // 玩家空间状态？
            apex_mem.Read<float>(local_player_ptr + OFFSET_WALLRUNSTART, wallrunStart);
            apex_mem.Read<float>(local_player_ptr + OFFSET_WALLRUNCLEAR, wallrunClear);
            apex_mem.Read<int>(local_player_ptr + OFFSET_SKYDRIVESTATE, skyDriveState); // 跳伞状态
            apex_mem.Read<int>(local_player_ptr + OFFSET_IN_DUCKSTATE, duck_state);     // 玩家下蹲状态
            apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD, foreward_state);             // 前进状态
            apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD + 0x8, force_foreward);       // 前进按键

            // apex_mem.Read<int>(g_Base + OFFSET_IN_TOGGLE_DUCK, tduck_state); // 切换下蹲
            // apex_mem.Read<int>(g_Base + OFFSET_IN_TOGGLE_DUCK + 0x8, force_toggle_duck); //切换蹲起按键
            // apex_mem.Read<int>(g_Base + OFFSET_IN_JUMP + 0x8, force_jump);
            // apex_mem.Read<int>(g_Base + OFFSET_IN_DUCK + 0x8, force_duck); //下蹲按键
            float world_time, traversal_start_time, traversal_progress;
            if (!apex_mem.Read<float>(local_player_ptr + OFFSET_TIME_BASE, world_time))
            {
                // memory_io_panic("read time_base");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                break;
            }
            if (!apex_mem.Read<float>(local_player_ptr + OFFSET_TRAVERSAL_STARTTIME,
                                      traversal_start_time))
            {
                // memory_io_panic("read traversal_starttime");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                break;
            }
            if (!apex_mem.Read<float>(local_player_ptr + OFFSET_TRAVERSAL_PROGRESS,
                                      traversal_progress))
            {
                memory_io_panic("read traversal_progress");
            }

            if (g_settings.auto_tapstrafe)
            {
                bool ts_start = true;
                // autoTapstrafe
                if (wallrunStart > wallrunClear)
                {
                    float climbTime = world_time - wallrunStart;
                    if (climbTime > 0.8) // 长时间爬墙不是ts
                    {
                        longclimb = true;
                        ts_start = false;
                    }
                    else
                    {
                        ts_start = true;
                    }
                }
                if (ts_start)
                {
                    // printf("longclimb:%d\n", longclimb);
                    // printf("duck_state:%d"\n, duck_state); 向下蹲1 完全蹲下2 起身过程3 其他0
                    // printf("jump_state:%d"\n, jump_state); 按着跳跃65 其他0
                    // printf("foreward_state:%d"\n, foreward_state); 按w时33，其他0 滚轮前进不触发
                    // printf("flags:%d"\n, flags);  空中状态64 蹲下67 站立65
                    // printf("force_foreward :%d\n", force_foreward);按下w是1 其他0
                    // printf("force_jump :%d\n", force_jump);按着跳跃5 其他4
                    //  when player is in air  and  not skydrive    and  not longclimb and not backward
                    if (((flags & 0x1) == 0) && !(skyDriveState > 0) && !longclimb && !(backWardState > 0))
                    {
                        if (((duck_state > 0) && (foreward_state == 33)))
                        { // previously 33
                            if (force_foreward == 0)
                            {
                                apex_mem.Write<int>(g_Base + OFFSET_IN_FORWARD + 0x8, 1);
                            }
                            else
                            {
                                apex_mem.Write<int>(g_Base + OFFSET_IN_FORWARD + 0x8, 0);
                            }
                        }
                    }
                    else if ((flags & 0x1) != 0)
                    {
                        if (foreward_state == 0)
                        {
                            apex_mem.Write<int>(g_Base + OFFSET_IN_FORWARD + 0x8, 0);
                        }
                        else if (foreward_state == 33)
                        {
                            apex_mem.Write<int>(g_Base + OFFSET_IN_FORWARD + 0x8, 1);
                        }
                    }
                }
            }
            ////// bunny hop
            /*
            if (jump_state == 65 && ((flags & 0x1) != 0)) {
                if (force_jump == 5 && !bunnyhop && (world_time > (bhopTick + 0.1))) {
                    apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
                    bunnyhop = true;
                }
                else if (bunnyhop) {
                    apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
                    bunnyhop = false;
                    bhopTick = world_time;
                }
            }*/

            if (g_settings.super_glide)
            {
                /** SuperGlide
                 * https://www.unknowncheats.me/forum/apex-legends/578160-external-auto-superglide-3.html
                 */
                float hang_on_wall = world_time - traversal_start_time;

                static float start_jump_time = 0;
                static bool start_sg = false;
                static std::chrono::time_point<std::chrono::steady_clock> last_sg_finish;

                float hang_start, hang_cancel, trav_start, hang_max, action_interval, total_interval;
                int release_wait;
                {
                    // for 75 fps
                    hang_start = 0.1;
                    hang_cancel = 0.12;
                    trav_start = 0.87;
                    hang_max = 1.5;
                    action_interval = 0.011;
                    release_wait = 50;
                    total_interval = 800;
                    if (abs(g_settings.game_fps - 144.0) < abs(g_settings.game_fps - 75.0))
                    {
                        // for 144 fps
                        hang_start = 0.05;
                        hang_cancel = 0.07;
                        trav_start = 0.90;
                        hang_max = 0.75;
                        action_interval = 0.007;
                        release_wait = 35;
                        total_interval = 560;
                        if (abs(g_settings.game_fps - 240.0) < abs(g_settings.game_fps - 144.0))
                        {
                            // for 240 fps
                            hang_start = 0.033;
                            hang_cancel = 0.04;
                            trav_start = 0.95;
                            hang_max = 0.2;
                            action_interval = 0.004;
                            release_wait = 20;
                            total_interval = 320;
                        }
                    }
                }

                if (hang_on_wall > hang_start)
                {
                    if (hang_on_wall < hang_cancel)
                    {
                        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
                    }
                    if (traversal_progress > trav_start && hang_on_wall < hang_max &&
                        !start_sg)
                    {
                        auto now_ms = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(now_ms - last_sg_finish).count() > total_interval && jump_state > 0)
                        {
                            // start SG
                            start_jump_time = world_time;
                            start_sg = true;
                        }
                    }
                }
                if (start_sg)
                {
                    // press button
                    // g_logger += "sg Press jump\n";
                    apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);

                    float current_time;
                    while (true)
                    {
                        if (apex_mem.Read<float>(local_player_ptr + OFFSET_TIME_BASE, current_time))
                        {
                            if (current_time - start_jump_time < action_interval)
                            {
                                // keep looping
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    apex_mem.Write<int>(g_Base + OFFSET_IN_DUCK + 0x8, 6);
                    std::this_thread::sleep_for(std::chrono::milliseconds(release_wait));
                    apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
                    // Write<int>(g_Base + OFFSET_IN_DUCK + 0x8, 4);
                    last_sg_finish = std::chrono::steady_clock::now();
                    // g_logger += "sg\n";
                    start_sg = false;
                }
            }

            if (g_settings.super_grpple)
            {
                int isGrppleActived, isGrppleAttached;
                apex_mem.Read<int>(local_player_ptr + OFFSET_GRAPPLE_ACTIVE, isGrppleActived);
                if (isGrppleActived)
                {
                    apex_mem.Read<int>(local_player_ptr + OFFSET_GRAPPLE + OFFSET_GRAPPLE_ATTACHED, isGrppleAttached);
                    if (isGrppleAttached == 1 && !Isdone)
                    {
                        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x08, 5);
                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x08, 4);
                    }
                    Isdone = isGrppleAttached;
                }
            }

            { /* calc game fps */
                static int last_checkpoint_frame = 0;
                static std::chrono::time_point<std::chrono::steady_clock> checkpoint_time;
                if (g_settings.calc_game_fps && curFrameNumber % 100 == 0)
                {
                    auto ms = std::chrono::steady_clock::now();
                    int delta_frame = curFrameNumber - last_checkpoint_frame;
                    if (delta_frame > 90 && delta_frame < 120)
                    {
                        auto duration = ms - checkpoint_time;
                        auto settings_state = g_settings;
                        settings_state.game_fps = delta_frame * 1000.0f / std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                        update_settings(settings_state);
                    }
                    last_checkpoint_frame = curFrameNumber;
                    checkpoint_time = ms;
                }
            }

            if (local_held_id == -251)
            { // no_nade_aim 为true时，按下瞄准为使用高抛
                if ((g_settings.no_nade_aim && zoom_state == 0) ||
                    (!g_settings.no_nade_aim && zoom_state > 0))
                {
                    aimbot.gun_safety = true;
                }
                else
                {
                    aimbot.gun_safety = false;
                }
            }
            if (g_settings.keyboard)
            {
                if (isPressed(g_settings.aimbot_hot_key_2))
                {
                    aimbot.aiming = true;
                    aimbot.smooth = g_settings.smooth_sub;
                }
                else if (isPressed(g_settings.aimbot_hot_key_1))
                {
                    aimbot.aiming = true;
                    aimbot.smooth = g_settings.smooth;
                }
                else
                {
                    aimbot.aiming = false;
                }
            }
            if (g_settings.gamepad)
            {
                // attackState == 120 || zoomState == 119
                if (attack_state > 0 || zoom_state > 0)
                {
                    aimbot.aiming = true;
                }
                else
                {
                    aimbot.aiming = false;
                }
            }
            bool triggerbot_clickgun;
            switch (local_weapon_id)
            {
            case idweapon_eva8:
            case idweapon_mastiff:
            case idweapon_mozambique:
            case idweapon_peacekeeper:
            case idweapon_sentinel:
            case idweapon_wingman:
            case idweapon_longbow:
            case idweapon_g7_scout:
            case idweapon_kraber:
            case idweapon_p2020:
            case idweapon_triple_take:
            case idweapon_3030_repeater:
                triggerbot_clickgun = true;
                break;
            default:
                triggerbot_clickgun = false;
            }
            if (triggerbot_clickgun)
            {
                if (isPressed(g_settings.flick_bot_hot_key))
                {
                    FlickReady = true;
                }
                else if (g_settings.trigger_bot_shot && isPressed(g_settings.trigger_bot_hot_key))
                {
                    TriggerReady = true;
                }
                else
                {
                    TriggerReady = false;
                    FlickReady = false;
                }
            }
            else
            {
                TriggerReady = false;
                FlickReady = false;
            }
            if (zoom_state > 0)
            { // 根据是否开镜选择不同的自瞄范围
                aimbot.max_fov = g_settings.ads_fov;
            }
            else
            {
                aimbot.max_fov = g_settings.non_ads_fov;
            }
            if (isPressed(g_settings.quickglow_hot_key))
            {
                static std::chrono::time_point<std::chrono::steady_clock> lastPressTime;
                auto now_ms = std::chrono::steady_clock::now();
                if (now_ms >= lastPressTime + std::chrono::milliseconds(200))
                {
                    QuickGlow = !QuickGlow;
                    lastPressTime = now_ms;
                }
            }
            if (isPressed(g_settings.quickaim_hot_key))
            { // PRESS F3 Temporarily disable aim
                static std::chrono::time_point<std::chrono::steady_clock> lastPressTime_aim;
                auto now_ms = std::chrono::steady_clock::now();
                if (now_ms >= lastPressTime_aim + std::chrono::milliseconds(200))
                {
                    QuickAim = !QuickAim;
                    lastPressTime_aim = now_ms;
                }
            }
            if (isPressed(g_settings.map_radar_hotkey))
            { // KEY_F8
                MapRadarTesting(local_player_ptr);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    cactions_t = false;
}

// 位于ProcessPlayer
void SetPlayerGlow(Entity &LPlayer, Entity &Target, int index, int frame_number)
{
    const auto g_settings = global_settings();
    int setting_index = 0;
    std::array<float, 3> highlight_parameter = {0, 0, 0};
    // set glow color
    if (!(g_settings.firing_range) && (Target.isKnocked() || !Target.isAlive()))
    { // 不在训练场并且倒地或者没活着
        setting_index = 70;
        highlight_parameter = {g_settings.glow_r_knocked,
                               g_settings.glow_g_knocked,
                               g_settings.glow_b_knocked};
    }
    else if (Target.lastVisTime() > lastvis_aim[index] ||
             (Target.lastVisTime() < 0.f && lastvis_aim[index] > 0.f))
    {
        setting_index = 69;
        highlight_parameter = {g_settings.glow_r_viz, g_settings.glow_g_viz,
                               g_settings.glow_b_viz};
    }
    else
    {
        if (g_settings.player_glow_armor_color)
        {
            int shield = Target.getShield();
            int health = Target.getHealth();
            if (shield + health <= 100)
            { // Orange
                setting_index = 66;
                highlight_parameter = {255 / 255.0, 165 / 255.0, 0 / 255.0};
            }
            else if (shield + health <= 150)
            { // white
                setting_index = 67;
                highlight_parameter = {247 / 255.0, 247 / 255.0, 247 / 255.0};
            }
            else if (shield + health <= 175)
            { // blue
                setting_index = 64;
                highlight_parameter = {39 / 255.0, 178 / 255.0, 255 / 255.0};
            }
            else if (shield + health <= 200)
            { // purple
                setting_index = 63;
                highlight_parameter = {206 / 255.0, 59 / 255.0, 255 / 255.0};
            }
            else if (shield + health <= 225)
            { // red
                setting_index = 61;
                highlight_parameter = {219 / 255.0, 2 / 255.0, 2 / 255.0};
            }
            else
            {
                setting_index = 60;
                highlight_parameter = {2 / 255.0, 2 / 255.0, 2 / 255.0};
            }
        }
        else
        {
            setting_index = 68;
            highlight_parameter = {g_settings.glow_r_not, g_settings.glow_g_not,
                                   g_settings.glow_b_not};
        }
    }
    // love player glow
    if (g_settings.player_glow_love_user)
    {
        auto it = centity_to_index.find(Target.ptr);
        if (it != centity_to_index.end() &&
            Target.check_love_player(it->second))
        {
            int frame_frag = frame_number / ((int)g_settings.game_fps);
            if (setting_index == 69 ||
                frame_frag % 2 == 0)
            { // vis: always, else: 1s time slice
                setting_index = 76;
                rainbowColor(frame_number, highlight_parameter); // 返回一个rgb色彩到highlight_parameter
            }
        }
    }

    // enable glow
    if (g_settings.player_glow && QuickGlow)
    { // 如果设置里开了发光，就执行发光
        Target.enableGlow(setting_index, g_settings.player_glow_inside_value,
                          g_settings.player_glow_outline_size, highlight_parameter, g_settings.glow_dist);
    }
    if (!g_settings.player_glow || !QuickGlow)
    { // 如果设置里关闭了发光，并且玩家仍在发光，就将发光效果取消掉
        Target.enableGlow(setting_index, 0, 0, highlight_parameter, g_settings.glow_dist);
    }
}

// 位于DoAction
void ProcessPlayer(Entity &LPlayer, Entity &target, int index, int frame_number, std::set<uintptr_t> &tmp_specs)
{

    const auto g_settings = global_settings();

    int entity_team = target.getTeamId();
    int local_team = LPlayer.getTeamId();

    if (!target.isAlive())
    {
        if (target.isSpec(LPlayer.ptr))
        {
            tmp_specs.insert(target.ptr);
        }
        return;
    }
    if (!LPlayer.isAlive())
    {
        SetPlayerGlow(LPlayer, target, index, frame_number);
        lastvis_aim[index] = target.lastVisTime();
        return;
    }

    if (g_settings.tdm_toggle)
    {
        if ((entity_team % 2) == (local_team % 2))
        {
            return;
        }
    }

    // Firing range stuff
    if (!g_settings.firing_range)
    {
        if (entity_team < 0 || entity_team > 50 ||
            (entity_team == LocalTeamID && !g_settings.onevone))
        {
            return;
        }
    }

    Vector EntityPosition = target.getPosition();
    Vector LocalPlayerPosition = LPlayer.getPosition();
    float dist = LocalPlayerPosition.DistTo(EntityPosition);

    // aim distance check
    if ((local_held_id == -251 && dist > g_settings.skynade_dist) ||
        dist > g_settings.aim_dist)
    {
        SetPlayerGlow(LPlayer, target, index, frame_number);
        lastvis_aim[index] = target.lastVisTime();
        return; // need setglow though it's out of aimdist
    }

    // Targeting
    const float vis_weights = 12.5f;
    float fov = CalculateFov(LPlayer, target);
    bool vis = target.lastVisTime() > lastvis_aim[index];
    float score =
        (fov * fov) * 100 + (dist * 0.025) * 10 + (vis ? 0 : vis_weights);
    /*
     fov:dist:score
      1  10m  100
      2  40m  400
      3  90m  900
      4  160m 1600
    */
    if (score < aimbot.target_score_max)
    {
        aimbot.target_score_max = score;
        aimbot.tmp_aimentity = target.ptr;
    }

    if (g_settings.aim == 2)
    {
        // vis check for shooting current aim entity
        if (local_held_id != -251 && aimbot.aimentity == target.ptr)
        {
            if (!vis)
            {
                // turn on safety
                aimbot.gun_safety = true;
            }
            else
            {
                aimbot.gun_safety = false;
            }
            if (FlickReady && !aimbot.gun_safety)
            {
                Matrix view_matrix_data = {};
                apex_mem.Read<Matrix>(ViewMatrix, view_matrix_data);
                float *m_vMatrix = view_matrix_data.matrix;
                DoFlick(LPlayer, target, m_vMatrix);
            }
        }
    }
    SetPlayerGlow(LPlayer, target, index, frame_number);
    lastvis_aim[index] = target.lastVisTime();
}

// Main stuff, dont edit.
void DoActions()
{
    actions_t = true;
    while (actions_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        while (g_Base != 0)
        {                                                               // 读到游戏基址后开始循环
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); // don't change xD

            uint64_t LocalPlayer = 0;
            apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
            if (LocalPlayer == 0)
                continue;
            const auto g_settings = global_settings();
            char level_name[128] = {0};
            uint64_t LevelName_ptr = g_Base + OFFSET_LEVELNAME;
            apex_mem.ReadArray<char>(LevelName_ptr, level_name, 128);
            // printf("%s\n", level_name);
            if (strcmp(level_name, "mp_lobby") == 0)
            {
                map = 0;
            }
            else if (strcmp(level_name, "mp_rr_canyonlands_staging_mu1") == 0)
            {
                map = 1;
            }
            else if (strcmp(level_name, "mp_rr_tropic_island_mu2") == 0)
            {
                map = 2;
            }
            else if (strcmp(level_name, "mp_rr_desertlands_hu") == 0)
            {
                map = 3;
            }
            else if (strcmp(level_name, "mp_rr_olympus_mu2") == 0)
            {
                map = 4;
            }
            else if (strcmp(level_name, "mp_rr_district") == 0)
            {
                map = 5;
            }
            else if (strcmp(level_name, "mp_rr_divided_moon_mu1") == 0)
            {
                map = 6;
            }
            else if (strcmp(level_name, "mp_rr_canyonlands_hu") == 0)
            {
                map = 7;
            }
            else if (strcmp(level_name, "mp_rr_tropic_island_mu1") == 0)
            {
                map = 8;
            }
            else
            {
                map = -1;
            }
            if (map < 1)
                continue;
            static std::chrono::time_point<std::chrono::steady_clock> checkpoint_time;
            auto now_ms = std::chrono::steady_clock::now();
            if (now_ms >= checkpoint_time + std::chrono::milliseconds(10000))
            {
                auto settings_state = g_settings;
                if (map == 1)
                {
                    settings_state.firing_range = true;
                }
                else
                {
                    settings_state.firing_range = false;
                }
                update_settings(settings_state);
                checkpoint_time = now_ms;
            }
            if (g_settings.deathbox)
            { // 如果开了死亡之箱高亮则需要更多物品循环
                itementcount = 15000;
            }
            else
            {
                itementcount = 10000;
            }

            Entity LPlayer = getEntity(LocalPlayer); // 根据地址生成玩家实体对象entity

            LocalTeamID = LPlayer.getTeamId();       // 获取自己所在队伍的id
            if (LocalTeamID < 0 || LocalTeamID > 50) // 不在游戏中不继续执行
            {
                continue;
            }
            uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;

            uint64_t baseEntity = 0;
            apex_mem.Read<uint64_t>(entityListPtr, baseEntity); // Check base entity is not Null
            if (baseEntity == 0)                                // CWORLD 实体，应该是每一局开局创建的
            {
                continue;
            }
            int frame_number = 0;
            apex_mem.Read<int>(g_Base + OFFSET_GLOBAL_VARS + 0x0008, frame_number); // 读取游戏的实际帧数
            std::set<uintptr_t> tmp_specs = {};
            aimbot.target_score_max = (50 * 50) * 100 + (g_settings.aim_dist * 0.025) * 10; // 初始化分数
            aimbot.tmp_aimentity = 0;
            centity_to_index.clear();
            if (map == 1)
            {
                int c = 0;
                for (int i = 0; i < 15000; i++)
                {
                    uint64_t entityAddr = 0;
                    apex_mem.Read<uint64_t>(entityListPtr + ((uint64_t)i << 5), entityAddr);
                    if (entityAddr == 0)
                        continue;
                    centity_to_index.insert_or_assign(entityAddr, i);
                    if (LocalPlayer == entityAddr)
                    {
                        continue;
                    }
                    Entity Target = getEntity(entityAddr);
                    if (Target.isDummy() || (Target.isPlayer() && g_settings.onevone))
                    {
                        ProcessPlayer(LPlayer, Target, c, frame_number, tmp_specs);
                        c++;
                    }
                }
            }
            else
            {
                for (int i = 0; i < ToRead; i++)
                {
                    uint64_t entityAddr = 0;
                    apex_mem.Read<uint64_t>(entityListPtr + ((uint64_t)i << 5), entityAddr);
                    if (entityAddr == 0)
                        continue;
                    centity_to_index.insert_or_assign(entityAddr, i);

                    if (LocalPlayer == entityAddr)
                        continue;
                    Entity Target = getEntity(entityAddr);
                    if (!Target.isPlayer())
                    {
                        continue;
                    }
                    ProcessPlayer(LPlayer, Target, i, frame_number, tmp_specs);
                }
            }

            { // refresh spectators count
                std::lock_guard<std::mutex> lock(spectatorsMtx);
                std::vector<Entity> tmp_spec, tmp_ally_spec;
                for (auto it = tmp_specs.begin(); it != tmp_specs.end(); it++)
                {
                    Entity target = getEntity(*it);
                    if (target.getTeamId() == LocalTeamID)
                    {
                        tmp_ally_spec.push_back(target);
                    }
                    else
                    {
                        tmp_spec.push_back(target);
                    }
                }
                spectators.clear();
                allied_spectators.clear();
                spectators = tmp_spec;
                allied_spectators = tmp_ally_spec;
            }
            // set current aim entity
            if (aimbot.lock)
            { // locked target
                aimbot.aimentity = aimbot.locked_aimentity;
            }
            else
            { // or new target
                aimbot.aimentity = aimbot.tmp_aimentity;
            }
            // disable aimbot safety if vis check is turned off
            if (g_settings.aim == 1)
            {
                aimbot.gun_safety = false;
            }

            // weapon model glow
            // printf("%d\n", LPlayer.getHealth());
            if (g_settings.weapon_model_glow && LPlayer.getHealth() > 0)
            {
                std::array<float, 3> highlight_color;
                bool weapon_glow = false;
                int spectators_num = spectators.size();
                if (spectators_num > 6)
                {
                    rainbowColor(frame_number, highlight_color); // 大于6人观战彩色
                    weapon_glow = true;
                }
                else if (spectators_num > 4)
                {
                    highlight_color = {1, 0, 0}; // 大于4人红色
                    weapon_glow = true;
                }
                else if (spectators_num > 2)
                {
                    highlight_color = {1, 0.6, 0}; // 3-4人橙色
                    weapon_glow = true;
                }
                else if (spectators_num > 0)
                {
                    highlight_color = {0, 0.4, 1}; // 1-2人蓝色
                    weapon_glow = true;
                }
                else if (allied_spectators.size() > 0)
                { // 没有敌人但是队友观战绿色
                    highlight_color = {0, 1, 0};
                    weapon_glow = true;
                }
                else
                {
                    weapon_glow = false; // 没人观战不发光
                }
                LPlayer.glow_weapon_model(g_Base, weapon_glow, highlight_color);
            }
            else
            {
                LPlayer.glow_weapon_model(g_Base, false, {0, 0, 0});
            }
        }
    }
    actions_t = false;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<player> players(ToRead);
Matrix view_matrix_data = {};

// ESP loop.. this helps right?
static void EspLoop()
{
    esp_t = false; // i won't use it...
    while (esp_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        while (g_Base != 0 && overlay_t)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            const auto g_settings = global_settings();

            if (g_settings.esp)
            {
                valid = false;

                uint64_t LocalPlayer = 0;
                apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
                if (LocalPlayer == 0)
                {
                    next2 = true;
                    while (next2 && g_Base != 0 && overlay_t && g_settings.esp)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    continue;
                }
                Entity LPlayer = getEntity(LocalPlayer);
                int LocalTeamID = LPlayer.getTeamId();
                if (LocalTeamID < 0 || LocalTeamID > 50)
                {
                    next2 = true;
                    while (next2 && g_Base != 0 && overlay_t && g_settings.esp)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    continue;
                }
                Vector LocalPlayerPosition = LPlayer.getPosition();
                esp_local_pos = LocalPlayerPosition;
                apex_mem.Read<Matrix>(ViewMatrix, view_matrix_data);

                uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;

                players.clear();

                {
                    Vector LocalPlayerPosition = LPlayer.getPosition();
                    QAngle localviewangle = LPlayer.GetViewAngles();

                    // Ammount of ents to loop, dont edit.
                    for (int i = 0; i < ToRead; i++)
                    {
                        // Read entity pointer
                        uint64_t centity = 0;
                        apex_mem.Read<uint64_t>(entityListPtr + ((uint64_t)i << 5), centity);
                        if (centity == 0)
                        {
                            continue;
                        }

                        // Exclude self
                        if (LocalPlayer == centity)
                        {
                            continue;
                        }

                        // Get entity data
                        Entity Target = getEntity(centity);

                        // Exclude undesired entity
                        if (g_settings.firing_range)
                        {
                            if (!Target.isDummy() && !g_settings.onevone)
                            {
                                continue;
                            }
                        }
                        else
                        {
                            if (!Target.isPlayer())
                            {
                                continue;
                            }
                        }

                        int entity_team = Target.getTeamId();

                        // Exclude invalid team
                        if (entity_team < 0 || entity_team > 50)
                        {
                            continue;
                        }

                        // Exlude teammates if not 1v1
                        if (entity_team == LocalTeamID && !g_settings.onevone)
                        {
                            continue;
                        }

                        Vector EntityPosition = Target.getPosition();
                        float dist = LocalPlayerPosition.DistTo(EntityPosition);

                        // Excluding targets that are too far or too close
                        if (dist > g_settings.max_dist || dist < 20.0f)
                        {
                            continue;
                        }

                        Vector bs = Vector();
                        // Change res to your res here, default is 1080p but can copy paste
                        // 1440p here
                        WorldToScreen(EntityPosition, view_matrix_data.matrix,
                                      g_settings.screen_width, g_settings.screen_height,
                                      bs); // 2560, 1440
                        if (g_settings.esp)
                        {
                            Vector hs = Vector();
                            Vector HeadPosition = Target.getBonePositionByHitbox(0);
                            // Change res to your res here, default is 1080p but can copy
                            // paste 1440p here
                            WorldToScreen(HeadPosition, view_matrix_data.matrix,
                                          g_settings.screen_width, g_settings.screen_height,
                                          hs); // 2560, 1440
                            float height = abs(abs(hs.y) - abs(bs.y));
                            float width = height / 2.0f;
                            float boxMiddle = bs.x - (width / 2.0f);
                            int health = Target.getHealth();
                            int shield = Target.getShield();
                            int maxshield = Target.getMaxshield();
                            int armortype = Target.getArmortype();
                            Vector EntityPosition = Target.getPosition();
                            float targetyaw = Target.GetYaw();
                            uint64_t entity_index = i - 1;
                            player data_buf = {dist,
                                               entity_team,
                                               boxMiddle,
                                               hs.y,
                                               width,
                                               height,
                                               bs.x,
                                               bs.y,
                                               Target.isKnocked(),
                                               (Target.lastVisTime() > lastvis_esp[i]),
                                               health,
                                               shield,
                                               maxshield,
                                               armortype,
                                               EntityPosition,
                                               LocalPlayerPosition,
                                               localviewangle,
                                               targetyaw,
                                               Target.isAlive(),
                                               Target.check_love_player(entity_index),
                                               false};
                            Target.get_name(g_Base, entity_index, &data_buf.name[0]);
                            spectatorsMtx.lock();
                            for (auto &ent : spectators)
                            {
                                if (ent.ptr == centity)
                                {
                                    data_buf.is_spectator = true;
                                    break;
                                }
                            }
                            spectatorsMtx.unlock();
                            players.push_back(data_buf);
                            lastvis_esp[i] = Target.lastVisTime();
                            valid = true;
                        }
                    }
                }

                next2 = true;
                while (next2 && g_Base != 0 && overlay_t && g_settings.esp)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        }
    }
    esp_t = false;
}

// Aimbot Loop stuff
inline static void lock_target(uintptr_t target_ptr)
{ // 锁定目标函数，aimbot是包含自瞄信息的结构体
    aimbot.lock = true;
    aimbot.locked_aimentity = target_ptr;
}
inline static void cancel_targeting()
{ // 取消锁定
    aimbot.lock = false;
    aimbot.locked_aimentity = 0;
}
static void AimbotLoop()
{
    aim_t = true;
    while (aim_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        while (g_Base != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // Read LocalPlayer
            uint64_t LocalPlayer = 0;
            apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
            if (LocalPlayer == 0)
            {
                continue;
            }
            Entity LPlayer = getEntity(LocalPlayer);
            if (LPlayer.isKnocked() || !LPlayer.isAlive())
            {
                cancel_targeting();
                continue;
            }
            // Read HeldID
            const auto g_settings = global_settings();
            int HeldID;
            apex_mem.Read<int>(LocalPlayer + OFFSET_OFF_WEAPON, HeldID);
            local_held_id = HeldID; // 读取本地玩家手持物品id赋值给local_held_id
            if (g_settings.aim_no_recoil)
            {
                int in_attack;
                apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK, in_attack);
                if (in_attack > 0)
                {
                    QAngle viewAngles = LPlayer.GetViewAngles();
                    QAngle punchAngles = LPlayer.GetRecoil();
                    viewAngles.x -= punchAngles.x * (g_settings.recoil_pitch / 100.f);
                    viewAngles.y -= punchAngles.y * (g_settings.recoil_yaw / 100.f);
                    Math::NormalizeAngles(viewAngles);
                    if (viewAngles.x != viewAngles.x || viewAngles.y != viewAngles.y)
                        LPlayer.SetViewAngles(viewAngles);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            if (!QuickAim) continue;
            // Read WeaponID
            WeaponXEntity currentWeapon = WeaponXEntity();
            currentWeapon.update(LocalPlayer);
            uint32_t weaponID = currentWeapon.get_weap_id();
            local_weapon_id = weaponID;
            // printf("%d\n", weaponID);

            if (g_settings.aim > 0)
            { // 0为不自喵，1为不检查可见性，2为检查目标可见性
                if (aimbot.aimentity == 0)
                { // 如果无目标取消锁定
                    cancel_targeting();
                    continue;
                }

                Entity target = getEntity(aimbot.aimentity);
                // show target indicator before aiming
                aim_target = target.getPosition(); // 获取目标的位置

                if (!aimbot.aiming && HeldID != -251)
                { // aimbot的元素值由DoAction和ClientAction函数修改
                    cancel_targeting();
                    continue;
                }
                lock_target(aimbot.aimentity);
                if (aimbot.gun_safety)
                { // gun_safety用于可见性检查
                    continue;
                }
                if (HeldID == -251)
                { // auto throw
                    QAngle Angles_g = CalculateBestBoneAim(LPlayer, target, currentWeapon, 999.9f, aimbot.smooth);
                    if (Angles_g.x == 0 && Angles_g.y == 0)
                    {
                        cancel_targeting();
                        continue;
                    }
                    LPlayer.SetViewAngles(Angles_g);
                }
                else
                {
                    QAngle Angles = CalculateBestBoneAim(LPlayer, target, currentWeapon, aimbot.max_fov, aimbot.smooth);
                    if (Angles.x == 0 && Angles.y == 0)
                    {
                        cancel_targeting();
                        continue;
                    }
                    LPlayer.SetViewAngles(Angles);
                    if (TriggerReady)
                    {
                        bool isInhair = IsInCrossHair(target);
                        Vector loaclCamPos = LPlayer.GetCamPos();
                        float screenW = g_settings.screen_width;
                        float screenH = g_settings.screen_height;
                        if (IsInTriggerZone(currentWeapon, loaclCamPos, target, isInhair, screenW, screenH))
                        {
                            apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                            std::this_thread::sleep_for(std::chrono::milliseconds(20));
                            apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                        }
                    }
                }
            }
        }
    }
    aim_t = false;
}
// Item Glow Stuff

static void item_glow_t()
{
#if UPDATEITEM
    std::ofstream outFile("item_ids.txt", std::ios::out);
    if (!outFile.is_open())
    {
        std::cerr << "Unable to open file for writing!" << std::endl;
        return;
    }
    std::unordered_set<uint64_t> writtenItemIDs;
#endif
    item_t = true;
    while (item_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        while (g_Base != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            const auto g_settings = global_settings();
            if (!g_settings.item_glow)
            {
                break;
            }
            // Item::setItemGlow();
            uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;
            // item ENTs to loop, 10k-15k is normal. 10k might be better but will
            // not show all the death boxes i think.

            for (int i = 0; i < itementcount; i++)
            { // 开启10000个物品循环
                uint64_t entityAddr = 0;
                apex_mem.Read<uint64_t>(entityListPtr + ((uint64_t)i << 5), entityAddr); // 每个实体指针是32位,所以每次+32，读取结果保存到centity
                if (entityAddr == 0)
                    continue;
                Item item = getItem(entityAddr); // 读取到的是这个实体的数组地址，使用getitem获取到这个数组，此时item包含了ptr（数组指针）和buffer（包含实体所有数据）两个属性，其它还未读取

                // Item filter glow name setup and search.
                char glowName[200] = {0};
                uint64_t name_ptr;
                apex_mem.Read<uint64_t>(entityAddr + OFFSET_MODELNAME, name_ptr); // 这个实体的数组指针再加上模型名称偏移量，得到实体模型名称数组的地址
                apex_mem.ReadArray<char>(name_ptr, glowName, 200);                // 将实体模型名称存到glowName

                // item ids?
                uint64_t ItemID;
                apex_mem.Read<uint64_t>(entityAddr + OFFSET_ITEM_ID, ItemID);
// printf("Model Name: %s, Item ID: %lu\n", glowName, ItemID);

// Prints stuff you want to console
// if (strstr(glowName, "mdl/"))
//{
// printf("%ld\n", ItemID);
// }
// Search model name or id and if true sets glow, must be a better way to do
#if UPDATEITEM
                if (writtenItemIDs.find(ItemID) == writtenItemIDs.end())
                {
                    auto it = itemNameToEnum.find(glowName);
                    if (it != itemNameToEnum.end())
                    {
                        std::string enumName = getEnumName(it->second);
                        outFile << enumName << " = " << ItemID << "," << std::endl;
                        writtenItemIDs.insert(ItemID);
                    }
                }
#endif
                if (g_settings.loot.lightbackpack && ItemID == static_cast<uint64_t>(ItemList::lightbackpack))
                {                                                        // 白包
                    std::array<float, 3> highlightParameter = {1, 1, 1}; // 高亮颜色，111是白色，因为lightbackpack是白包
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.medbackpack && ItemID == static_cast<uint64_t>(ItemList::medbackpack))
                { // 蓝包
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.heavybackpack && ItemID == static_cast<uint64_t>(ItemList::heavybackpack))
                {                                                                  // 紫包
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098}; // #4B0082
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.goldbackpack && ItemID == static_cast<uint64_t>(ItemList::goldbackpack))
                { // 金包
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.shieldupgrade1 && ItemID == static_cast<uint64_t>(ItemList::shieldupgrade1))
                { // 白甲
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgrade2 && ItemID == static_cast<uint64_t>(ItemList::shieldupgrade2))
                { // 蓝甲
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgrade3 && ItemID == static_cast<uint64_t>(ItemList::shieldupgrade3))
                { // 紫甲
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.shieldupgrade4 && (ItemID == 429496729796))
                { // 金甲
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgrade5 && ItemID == 217)
                { // 红甲
                    std::array<float, 3> highlightParameter = {1, 0, 0};
                    int settingIndex = 34;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgradehead1 && ItemID == static_cast<uint64_t>(ItemList::shieldupgradehead1))
                { // 白头
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgradehead2 && ItemID == static_cast<uint64_t>(ItemList::shieldupgradehead2))
                { // 蓝头
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgradehead3 && ItemID == static_cast<uint64_t>(ItemList::shieldupgradehead3))
                { // 紫头
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldupgradehead4 && ItemID == static_cast<uint64_t>(ItemList::shieldupgradehead4))
                { // 金头
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.accelerant && ItemID == static_cast<uint64_t>(ItemList::accelerant))
                { // 绝招加速剂
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.phoenix && ItemID == static_cast<uint64_t>(ItemList::phoenix))
                { // 凤凰
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                /*else if (g_settings.loot.skull &&
                         strstr(glowName, "mdl/Weapons/skull_grenade/skull_grenade_base_v.rmdl"))
                {
                    std::array<float, 3> highlightParameter = {1, 0, 0};
                    int settingIndex = 34;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }*/
                else if (g_settings.deathbox && item.isBox())
                { // 添加死亡之箱的开关判断
                    std::array<float, 3> highlightParameter = {1, 0, 0};
                    int settingIndex = 34;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }

                else if (item.isTrap())
                { // 判断名称是否是侵蚀陷阱？毒气罐？好像无效
                    std::array<float, 3> highlightParameter = {0, 1, 0};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                // Gas Trap
                else if (strstr(glowName, "mdl/props/caustic_gas_tank/caustic_gas_tank.rmdl"))
                {
                    std::array<float, 3> highlightParameter = {0, 1, 0};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.healthlarge && ItemID == static_cast<uint64_t>(ItemList::healthlarge))
                { // 大药包
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.healthsmall && ItemID == static_cast<uint64_t>(ItemList::healthsmall))
                { // 小药
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldbattsmall && ItemID == static_cast<uint64_t>(ItemList::shieldbattsmall))
                { // 小电
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shieldbattlarge && ItemID == static_cast<uint64_t>(ItemList::shieldbattlarge))
                { // 大电
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.sniperammo && ItemID == static_cast<uint64_t>(ItemList::sniperammo))
                {                                                                  // 狙击弹药
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098}; // 紫色
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.heavyammo && ItemID == static_cast<uint64_t>(ItemList::heavyammo))
                {                                                        // 重型弹药
                    std::array<float, 3> highlightParameter = {1, 1, 1}; // 改成墨绿色
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.optic1xhcog && ItemID == static_cast<uint64_t>(ItemList::optic1xhcog))
                { // 1倍镜
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lightammo && ItemID == static_cast<uint64_t>(ItemList::lightammo))
                {                                                                     // 轻型子弹
                    std::array<float, 3> highlightParameter = {0.6902, 0.60, 0.3098}; // 土黄
                    int settingIndex = 35;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.energyammo && ItemID == static_cast<uint64_t>(ItemList::energyammo))
                {                                                          // 能量子弹
                    std::array<float, 3> highlightParameter = {0.2, 1, 0}; // 翠绿
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shotgunammo && ItemID == static_cast<uint64_t>(ItemList::shotgunammo))
                {                                                               // 霰弹子弹
                    std::array<float, 3> highlightParameter = {0.5, 0.0862, 0}; // 暗红
                    int settingIndex = 37;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lasersight1 && ItemID == static_cast<uint64_t>(ItemList::lasersight1))
                { // 激光瞄准器，以下3个等级
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lasersight2 && ItemID == static_cast<uint64_t>(ItemList::lasersight2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lasersight3 && ItemID == static_cast<uint64_t>(ItemList::lasersight3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.sniperammomag1 && ItemID == static_cast<uint64_t>(ItemList::sniperammomag1))
                { // 狙击弹匣，以下四个等级
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.sniperammomag2 && ItemID == static_cast<uint64_t>(ItemList::sniperammomag2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.sniperammomag3 && ItemID == static_cast<uint64_t>(ItemList::sniperammomag3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.sniperammomag4 && ItemID == static_cast<uint64_t>(ItemList::sniperammomag4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.energyammomag1 && ItemID == static_cast<uint64_t>(ItemList::energyammomag1))
                { // 能量弹匣，四个等级
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.energyammomag2 && ItemID == static_cast<uint64_t>(ItemList::energyammomag2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.energyammomag3 && ItemID == static_cast<uint64_t>(ItemList::energyammomag3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.energyammomag4 && ItemID == static_cast<uint64_t>(ItemList::energyammomag4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.stocksniper1 && ItemID == static_cast<uint64_t>(ItemList::stocksniper1))
                { // 狙击枪托
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.stocksniper2 && ItemID == static_cast<uint64_t>(ItemList::stocksniper2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.stocksniper3 && ItemID == static_cast<uint64_t>(ItemList::stocksniper3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.stockregular1 && ItemID == static_cast<uint64_t>(ItemList::stockregular1))
                { // 标准枪托
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.stockregular2 && ItemID == static_cast<uint64_t>(ItemList::stockregular2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.stockregular3 && ItemID == static_cast<uint64_t>(ItemList::stockregular3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shielddown1 && ItemID == static_cast<uint64_t>(ItemList::shielddown1))
                { // 击倒护盾
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shielddown2 && ItemID == static_cast<uint64_t>(ItemList::shielddown2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shielddown3 && ItemID == static_cast<uint64_t>(ItemList::shielddown3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shielddown4 && ItemID == static_cast<uint64_t>(ItemList::shielddown4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.lightammomag1 && ItemID == static_cast<uint64_t>(ItemList::lightammomag1))
                { // 轻型弹匣
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lightammomag2 && ItemID == static_cast<uint64_t>(ItemList::lightammomag2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lightammomag3 && ItemID == static_cast<uint64_t>(ItemList::lightammomag3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.lightammomag4 && ItemID == static_cast<uint64_t>(ItemList::lightammomag4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.heavyammomag1 && ItemID == static_cast<uint64_t>(ItemList::heavyammomag1))
                { // 重型弹匣
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.heavyammomag2 && ItemID == static_cast<uint64_t>(ItemList::heavyammomag2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.heavyammomag3 && ItemID == static_cast<uint64_t>(ItemList::heavyammomag3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.heavyammomag4 && ItemID == static_cast<uint64_t>(ItemList::heavyammomag4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.optic2xhcog && ItemID == static_cast<uint64_t>(ItemList::optic2xhcog))
                { // 2倍镜
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.opticholo1x && ItemID == static_cast<uint64_t>(ItemList::opticholo1x))
                { // 圆形1倍镜
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.opticholo1x2x && ItemID == static_cast<uint64_t>(ItemList::opticholo1x2x))
                { // 1x2x切换
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.opticthreat && ItemID == 235)
                { // 金1倍
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.optic3xhcog && ItemID == static_cast<uint64_t>(ItemList::optic3xhcog))
                { // 3倍镜
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.optic2x4x && ItemID == static_cast<uint64_t>(ItemList::optic2x4x))
                { // 2x4x
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.opticsniper6x && ItemID == static_cast<uint64_t>(ItemList::opticsniper6x))
                { // 6倍镜
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.opticsniper4x8x && ItemID == static_cast<uint64_t>(ItemList::opticsniper4x8x))
                { // 4x8x
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.opticsniperthreat && ItemID == static_cast<uint64_t>(ItemList::opticsniperthreat))
                { // 金狙击镜
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.suppressor1 && ItemID == static_cast<uint64_t>(ItemList::suppressor1))
                { // 枪管？
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.suppressor2 && ItemID == static_cast<uint64_t>(ItemList::suppressor2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.suppressor3 && ItemID == static_cast<uint64_t>(ItemList::suppressor3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.turbo_charger && ItemID == 274)
                { // 涡轮增压器
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.skull_piecer && ItemID == 276)
                { // 穿颅器
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.hammer_point && ItemID == static_cast<uint64_t>(ItemList::hammer_point))
                { // 锤击点
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.disruptor_rounds && ItemID == 281)
                { // 干扰器
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.boosted_loader && ItemID == static_cast<uint64_t>(ItemList::boosted_loader))
                { // 加速装填器
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                else if (g_settings.loot.shotgunbolt1 && ItemID == static_cast<uint64_t>(ItemList::shotgunbolt1))
                { // 霰弹枪栓
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shotgunbolt2 && ItemID == static_cast<uint64_t>(ItemList::shotgunbolt2))
                {
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shotgunbolt3 && ItemID == static_cast<uint64_t>(ItemList::shotgunbolt3))
                {
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.shotgunbolt4 && ItemID == static_cast<uint64_t>(ItemList::shotgunbolt4))
                {
                    std::array<float, 3> highlightParameter = {1, 0.8431, 0};
                    int settingIndex = 33;
                    item.enableGlow(settingIndex, 48, highlightParameter);
                }
                // Nades
                else if (g_settings.loot.grenade_frag && ItemID == static_cast<uint64_t>(ItemList::grenade_frag))
                { // 破片手雷
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }

                else if (g_settings.loot.grenade_thermite && ItemID == static_cast<uint64_t>(ItemList::grenade_thermite))
                { // 铝热剂
                    std::array<float, 3> highlightParameter = {1, 1, 1};
                    int settingIndex = 30;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.grenade_arc_star && ItemID == static_cast<uint64_t>(ItemList::grenade_arc_star))
                { // 电弧星
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                // Weapons
                else if (g_settings.loot.weapon_kraber && ItemID == static_cast<uint64_t>(ItemList::weapon_kraber))
                { // 克莱伯
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_mastiff && ItemID == static_cast<uint64_t>(ItemList::weapon_mastiff))
                { // 敖犬
                    std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
                    int settingIndex = 37;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_lstar && ItemID == static_cast<uint64_t>(ItemList::weapon_lstar))
                { // lstar
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_nemesis && ItemID == static_cast<uint64_t>(ItemList::weapon_nemesis))
                { // 复仇女神
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_havoc && ItemID == static_cast<uint64_t>(ItemList::weapon_havoc))
                { // 哈沃克
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_devotion && ItemID == static_cast<uint64_t>(ItemList::weapon_devotion))
                { // 专注轻机枪
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_triple_take && ItemID == static_cast<uint64_t>(ItemList::weapon_triple_take))
                { // 三重狙击枪
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_flatline && ItemID == static_cast<uint64_t>(ItemList::weapon_flatline))
                { // 平行
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_hemlock && ItemID == static_cast<uint64_t>(ItemList::weapon_hemlock))
                { // 汉姆洛克
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_g7_scout && ItemID == static_cast<uint64_t>(ItemList::weapon_g7_scout))
                { // g7
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_alternator && ItemID == static_cast<uint64_t>(ItemList::weapon_alternator))
                { // 转换者
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_r99 && ItemID == static_cast<uint64_t>(ItemList::weapon_r99))
                {
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_prowler && ItemID == static_cast<uint64_t>(ItemList::weapon_prowler))
                { // 猎兽
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_volt && ItemID == static_cast<uint64_t>(ItemList::weapon_volt))
                { // 电能冲锋枪
                    std::array<float, 3> highlightParameter = {0.2, 1, 0};
                    int settingIndex = 36;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_longbow && ItemID == static_cast<uint64_t>(ItemList::weapon_longbow))
                { // 长弓
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_charge_rifle && ItemID == static_cast<uint64_t>(ItemList::weapon_charge_rifle))
                { // 充能步枪
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_spitfire && ItemID == static_cast<uint64_t>(ItemList::weapon_spitfire))
                { // 喷火
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_r301 && ItemID == static_cast<uint64_t>(ItemList::weapon_r301))
                {
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_eva8 && ItemID == static_cast<uint64_t>(ItemList::weapon_eva8))
                {
                    std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
                    int settingIndex = 37;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_peacekeeper && ItemID == static_cast<uint64_t>(ItemList::weapon_peacekeeper))
                {
                    std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
                    int settingIndex = 37;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_mozambique && ItemID == static_cast<uint64_t>(ItemList::weapon_mozambique))
                {
                    std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
                    int settingIndex = 37;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_wingman && ItemID == static_cast<uint64_t>(ItemList::weapon_wingman))
                { // 小帮手
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_p2020 && ItemID == static_cast<uint64_t>(ItemList::weapon_p2020))
                {
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_re45 && ItemID == static_cast<uint64_t>(ItemList::weapon_re45))
                {
                    std::array<float, 3> highlightParameter = {1, 0.5490, 0};
                    int settingIndex = 39;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_sentinel && ItemID == static_cast<uint64_t>(ItemList::weapon_sentinel))
                { // 哨兵
                    std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
                    int settingIndex = 32;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_bow && ItemID == static_cast<uint64_t>(ItemList::weapon_bow))
                {
                    std::array<float, 3> highlightParameter = {1, 0, 0};
                    int settingIndex = 34;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_3030_repeater && ItemID == static_cast<uint64_t>(ItemList::weapon_3030_repeater))
                {
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_rampage && ItemID == static_cast<uint64_t>(ItemList::weapon_rampage))
                { // 暴走
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (g_settings.loot.weapon_car_smg && ItemID == static_cast<uint64_t>(ItemList::weapon_car_smg))
                {
                    std::array<float, 3> highlightParameter = {0, 1, 1};
                    int settingIndex = 38;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                else if (ItemID == static_cast<uint64_t>(ItemList::evac_tower))
                { // 跳伞塔
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 64, highlightParameter);
                }
                else if (g_settings.loot.mobile_respawn && ItemID == 299)
                { // 重生信标
                    std::array<float, 3> highlightParameter = {0, 0, 1};
                    int settingIndex = 31;
                    item.enableGlow(settingIndex, 32, highlightParameter);
                }
                // CREDITS to Rikkie
                // https://www.unknowncheats.me/forum/members/169606.html for all the
                // weapon ids and item ids code, you are a life saver!
            } // for(item) loop end
        } // while(item_glow) loop end
    } // while(item_t) loop end
    item_t = false;
#if UPDATEITEM
    outFile.close();
#endif
}

extern void start_overlay();

void terminal()
{
    terminal_t = true;
    run_tui_menu();
}

int main(int argc, char *argv[])
{
    load_settings();

    if (geteuid() != 0)
    {
        // run as root..
        print_run_as_root();

        // test menu
        run_tui_menu();
        return 0;
    }

    const char *ap_proc = "r5apex.exe";
    const char *ap_proc_dx12 = "r5apex_dx12.exe";
    std::thread aimbot_thr;
    std::thread esp_thr;
    std::thread actions_thr;
    std::thread cactions_thr;
    // std::thread TriggerBotRun_thr;
    std::thread terminal_thr;
    std::thread overlay_thr;
    std::thread itemglow_thr;

    if (apex_mem.open_os() != 0)
    {
        exit(0);
    }

    while (1)
    {
        if (apex_mem.get_proc_status() != process_status::FOUND_READY)
        {
            if (aim_t)
            {
                aim_t = false;
                esp_t = false;
                actions_t = false;
                cactions_t = false;
                terminal_t = false;
                overlay_t = false;
                item_t = false;
                g_Base = 0;
                quit_tui_menu();

                aimbot_thr.~thread();
                esp_thr.~thread();
                actions_thr.~thread();
                cactions_thr.~thread();
                // TriggerBotRun_thr.~thread();
                terminal_thr.~thread();
                overlay_thr.~thread();
                itemglow_thr.~thread();
            }

            std::this_thread::sleep_for(std::chrono::seconds(2));
            printf("Searching for apex process...\n");

            apex_mem.open_proc(ap_proc);
            if (apex_mem.get_proc_status() != process_status::FOUND_READY)
                apex_mem.open_proc(ap_proc_dx12);
            if (apex_mem.get_proc_status() == process_status::FOUND_READY)
            {
                g_Base = apex_mem.get_proc_baseaddr();
                printf("\nApex process found\n");
                printf("Base: %lx\n", g_Base);

                aimbot_thr = std::thread(AimbotLoop);
                esp_thr = std::thread(EspLoop);
                actions_thr = std::thread(DoActions);
                cactions_thr = std::thread(ClientActions);
                // TriggerBotRun_thr = std::thread(TriggerBotRun);
                itemglow_thr = std::thread(item_glow_t);

                aimbot_thr.detach();
                esp_thr.detach();
                actions_thr.detach();
                cactions_thr.detach();
                // TriggerBotRun_thr.detach();
                itemglow_thr.detach();
            }
        }
        else
        {
            apex_mem.check_proc();

            const auto g_settings = global_settings();
            if (g_settings.debug_mode)
            {
                if (terminal_t)
                {
                    quit_tui_menu();
                }
            }
            else
            {
                if (!terminal_t)
                {
                    terminal_thr = std::thread(terminal);
                    terminal_thr.detach();
                }
                // wish_list.clear();
            }
            if (g_settings.no_overlay)
            {
                if (overlay_t)
                {
                    overlay_t = false;
                }
            }
            else
            {
                if (!overlay_t)
                {
                    overlay_thr = std::thread(start_overlay);
                    overlay_thr.detach();
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
