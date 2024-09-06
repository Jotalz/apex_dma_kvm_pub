#include "Client/main.h"
#include "Game.h"
#include "apex_sky.h"
#include "vector.h"
#include <array>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // For the system() function
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
#include <unordered_map> // Include the unordered_map header
#include <vector>
#include <fstream>
// this is a test, with seconds
Memory apex_mem;

// Just setting things up, dont edit.
bool active = true;
aimbot_state_t aimbot;
int LocalTeamID = 0;
const int toRead = 100;
bool trigger_ready = false;
bool quick_glow = true;
extern Vector aim_target; // for esp
int map_testing_local_team = 0;

// float triggerdist = 50.0f;
bool actions_t = false;
bool cactions_t = false;
bool terminal_t = false;
bool overlay_t = false;
bool esp_t = false;
bool aim_t = false;
bool item_t = false;
bool control_t = false;
bool isdone = false; // Prevent frequent writes during the superGrpple
uint64_t g_Base;
bool next2 = false;
bool valid = false;
extern float bulletspeed;
extern float bulletgrav;
Vector esp_local_pos;
int local_held_id = 2147483647;
uint32_t local_weapon_id = 2147483647;
int itementcount = 10000;
int map = 0;
std::vector<TreasureClue> treasure_clues;
std::map<uint64_t, uint64_t> centity_to_index; // Map centity to entity index
float lastvis_esp[toRead];
float lastvis_aim[toRead];
std::vector<Entity> spectators, allied_spectators;
std::mutex spectatorsMtx;

uint64_t PlayerLocal;
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

void TriggerBotRun()
{
  // 设置随机数生成器
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 20); // 正常或稍快的反应时间
  // 生成随机时间间隔，防止行为检测
  int randomInterval = dis(gen);
  std::this_thread::sleep_for(std::chrono::milliseconds(randomInterval));
  apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
  // printf("TriggerBotRun\n");
}

bool IsInCrossHair(Entity &target)
{
  static uintptr_t last_t = 0;
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
        // printf("Trigger\n");
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

void MapRadarTesting()
{
  uintptr_t pLocal;
  apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, pLocal);
  int dt;
  apex_mem.Read<int>(pLocal + OFFSET_TEAM, dt);
  map_testing_local_team = dt;

  for (uintptr_t i = 0; i <= 80000; i++)
  {
    apex_mem.Write<int>(pLocal + OFFSET_TEAM, 1);
  }

  for (uintptr_t i = 0; i <= 80000; i++)
  {
    apex_mem.Write<int>(pLocal + OFFSET_TEAM, dt);
  }
  map_testing_local_team = 0;
}

void MapRadarTesting2(uint64_t localptr)
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
      apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, local_player_ptr);

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

        float hang_start, hang_cancel, trav_start, hang_max, action_interval,total_interval;
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
          if (isGrppleAttached == 1 && !isdone)
          {
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x08, 5);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x08, 4);
          }
          isdone = isGrppleAttached;
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
      {
        if ((g_settings.no_nade_aim && zoom_state == 0) || // 手雷右键瞄准
            (!g_settings.no_nade_aim && zoom_state > 0))
        { // 右键取消手雷自瞄
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
      if (g_settings.shotgun_auto_shot && triggerbot_clickgun && zoom_state)
      {
        trigger_ready = true;
      }
      else
      {
        trigger_ready = false;
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
          quick_glow = !quick_glow;
          lastPressTime = now_ms;
        }
      }
      // Trigger ring check on F8 key press for over 0.5 seconds
      //static std::chrono::steady_clock::time_point tduckStartTime;
      //static bool mapRadarTestingEnabled = false;
      if (isPressed(99) && g_settings.map_radar_testing)
      { // KEY_F8
        MapRadarTesting2(local_player_ptr);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  cactions_t = false;
}

void ControlLoop()
{ // 根据观战人数闪烁键盘背光
  control_t = true;
  while (control_t)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    spectatorsMtx.lock();
    int spec_count = spectators.size();
    spectatorsMtx.unlock();
    if (spec_count > 0)
    {
      kbd_backlight_blink(spec_count);
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000 - 100));
    }
  }
  control_t = false;
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
    setting_index = 68;
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
      setting_index = 67;
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
      if (setting_index == 75 ||
          frame_frag % 2 == 0)
      { // vis: always, else: 1s time slice
        setting_index = 76;
        rainbowColor(frame_number, highlight_parameter); // 返回一个rgb色彩到highlight_parameter
      }
    }
  }

  // enable glow
  if (g_settings.player_glow && quick_glow)
  { // 如果设置里开了发光，就执行发光
    Target.enableGlow(setting_index, g_settings.player_glow_inside_value,
                      g_settings.player_glow_outline_size, highlight_parameter, g_settings.glow_dist);
  }
  if (!g_settings.player_glow || !quick_glow)
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

  if (!target.isAlive() || !LPlayer.isAlive())
  {
    if (target.ptr != LPlayer.ptr && target.isSpec(LPlayer.ptr))
    {
      tmp_specs.insert(target.ptr);
    }
    return;
  }

  if (g_settings.tdm_toggle)
  {
    if (entity_team % 2)
      EntTeam = 1;
    else
      EntTeam = 2;
    if (local_team % 2)
      LocTeam = 1;
    else
      LocTeam = 2;

    // printf("Target Team: %i\nLocal Team: %i\n", EntTeam, LocTeam);
    if (EntTeam == LocTeam)
      return;
  }

  // Firing range stuff
  if (!g_settings.firing_range)
  {
    if (entity_team < 0 || entity_team > 50 ||
        (entity_team == LocalTeamID && !g_settings.onevone))
    {
      return;
    }
    if (local_team != 0 && entity_team == local_team)
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
    return;

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
    }

    // TriggerBot
    if (aimbot.aimentity != 0)
    {
      // uint64_t LocalPlayer = 0;
      // apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
      Entity Target = getEntity(aimbot.aimentity);
      // Entity LPlayer = getEntity(LocalPlayer);
      if (trigger_ready && IsInCrossHair(Target))
      {
        TriggerBotRun();
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
    {                                                             // 读到游戏基址后开始循环
      std::this_thread::sleep_for(std::chrono::milliseconds(30)); // don't change xD

      uint64_t LocalPlayer = 0;
      apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer); // 读取本地玩家的所在地址,即是当前视角的玩家，自己或者你死亡后观战的人
      if (LocalPlayer == 0)
        continue;
      const auto g_settings = global_settings();
      char level_name[128] = {0};
      uint64_t LevelName_ptr;
      apex_mem.Read<uint64_t>(g_Base + OFFSET_LEVELNAME, LevelName_ptr); // 根据偏移读取当前地图名
      apex_mem.ReadArray<char>(LevelName_ptr, level_name, 128);
      // printf("%s\n", level_name);
      if (strcmp(level_name, "mp_lobby") == 0)
      {
        map = 0;
      }
      else if (strcmp(level_name, "mp_rr_canyonlands_hu") == 0)
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
      else if (strcmp(level_name, "mp_rr_divided_moon_mu1") == 0)
      {
        map = 5;
      }
      else
      {
        map = -1;
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

      LocalTeamID = LPlayer.getTeamId(); // 获取自己所在队伍的id
      if (LocalTeamID < 0 || LocalTeamID > 50)  // 不在游戏中不继续执行
      {
        continue;
      }
      uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;

      uint64_t baseEntity = 0;
      apex_mem.Read<uint64_t>(entityListPtr, baseEntity); // Check base entity is not Null
      if (baseEntity == 0) // CWORLD 实体，应该是每一局开局创建的
      {
        continue;
      }
      int frame_number = 0;
      apex_mem.Read<int>(g_Base + OFFSET_GLOBAL_VARS + 0x0008, frame_number); // 读取游戏的实际帧数
      std::set<uintptr_t> tmp_specs = {};
      aimbot.target_score_max = (50 * 50) * 100 + (g_settings.aim_dist * 0.025) * 10; // 初始化分数
      aimbot.tmp_aimentity = 0;
      centity_to_index.clear();
      // tmp_specs.clear();
      if (g_settings.firing_range)
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
          if (Target.isDummy() || (Target.isPlayer() && g_settings.onevone)){
            ProcessPlayer(LPlayer, Target, c, frame_number, tmp_specs);
            c++;
          }
          /*
          if (!Target.isDummy() && !g_settings.onevone)
          {
            continue;
          }
          ProcessPlayer(LPlayer, Target, c, frame_number, tmp_specs);
          c++;
          */
        }
      }
      else
      {
        for (int i = 0; i < toRead; i++)
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
      if (g_settings.aim == 1 && local_held_id != -251)
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

std::vector<player> players(toRead);
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

        uint64_t viewRenderer = 0;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer); // displays ESp, heath dist names etc
        uint64_t viewMatrix = 0;
        apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);

        apex_mem.Read<Matrix>(viewMatrix, view_matrix_data);

        uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;

        players.clear();

        {
          Vector LocalPlayerPosition = LPlayer.getPosition();
          QAngle localviewangle = LPlayer.GetViewAngles();

          // Ammount of ents to loop, dont edit.
          for (int i = 0; i < toRead; i++)
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
      const auto g_settings = global_settings();

      // Read LocalPlayer
      uint64_t LocalPlayer = 0;
      apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

      // Read HeldID
      int HeldID;
      apex_mem.Read<int>(LocalPlayer + OFFSET_OFF_WEAPON, HeldID); // 0x1a1c
      local_held_id = HeldID;                                      // 读取本地玩家手持物品id赋值给local_held_id

      // Read WeaponID
      uint64_t ehWeaponHandle;
      apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_ACTIVE_WEAPON, ehWeaponHandle);
      ehWeaponHandle &= 0xFFFF; // eHandle
      uint64_t pWeapon;
      uint64_t entityListPtr = g_Base + OFFSET_ENTITYLIST;
      apex_mem.Read<uint64_t>(entityListPtr + (ehWeaponHandle * 0x20), pWeapon);
      uint32_t weaponID;
      apex_mem.Read<uint32_t>(pWeapon + OFFSET_WEAPON_NAME, weaponID);
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

        if (!aimbot.aiming)
        { // aimbot的元素值由DoAction和ClientAction函数修改
          cancel_targeting();
          continue;
        }

        lock_target(aimbot.aimentity);
        if (aimbot.gun_safety)
        { // gun_safety用于可见性检查
          continue;
        }

        Entity LPlayer = getEntity(LocalPlayer);
        if (LocalPlayer == 0)
        {
          continue;
        }
        if (LPlayer.isKnocked())
        {
          cancel_targeting();
          continue;
        }

        /* Fine-tuning for each weapon */
        // bow
        if (weaponID == 2)
        {
          // Ctx.BulletSpeed = BulletSpeed - (BulletSpeed*0.08);
          // Ctx.BulletGravity = BulletGrav + (BulletGrav*0.05);
          bulletspeed = 10.08;
          bulletgrav = 10.05;
        }

        if (HeldID == -251)
        { // auto throw
          QAngle Angles_g = CalculateBestBoneAim(LPlayer, target, 999.9f, aimbot.smooth);
          if (Angles_g.x == 0 && Angles_g.y == 0)
          {
            cancel_targeting();
            continue;
          }
          LPlayer.SetViewAngles(Angles_g);
        }
        else
        {
          QAngle Angles = CalculateBestBoneAim(LPlayer, target, aimbot.max_fov, aimbot.smooth);
          if (Angles.x == 0 && Angles.y == 0)
          {
            cancel_targeting();
            continue;
          }
          LPlayer.SetViewAngles(Angles);
        }
      }
    }
  }
  aim_t = false;
}
// Item Glow Stuff

static void item_glow_t()
{
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
        if (g_settings.loot.lightbackpack && ItemID == 229)
        {                                                      // 白包
          std::array<float, 3> highlightParameter = {1, 1, 1}; // 高亮颜色，111是白色，因为lightbackpack是白包
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.medbackpack && ItemID == 230)
        { // 蓝包
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.heavybackpack && ItemID == 231)
        {                                                                // 紫包
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098}; // #4B0082
          int settingIndex = 73;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.goldbackpack && ItemID == 232)
        { // 金包
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.shieldupgrade1 &&
                 (ItemID == 220 || ItemID == 14073963583897798))
        { // 白甲
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgrade2 &&
                 (ItemID == 221 || ItemID == 21110945375846599))
        { // 蓝甲
          std::array<float, 3> highlightParameter = {0, 0, 1};

          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgrade3 &&
                 (ItemID == 222 || ItemID == 72776987629977800))
        { // 紫甲
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};

          int settingIndex = 73;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.shieldupgrade4 && (ItemID == 429496729796))
        { // 金甲
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgrade5 && ItemID == 223)
        { // 红甲
          std::array<float, 3> highlightParameter = {1, 0, 0};

          int settingIndex = 42;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgradehead1 && ItemID == 205)
        { // 白头
          std::array<float, 3> highlightParameter = {1, 1, 1};

          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgradehead2 && ItemID == 206)
        { // 蓝头

          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgradehead3 && ItemID == 207)
        { // 紫头
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};

          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldupgradehead4 && ItemID == 208)
        { // 金头
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.accelerant && ItemID == 199)
        { // 绝招加速剂
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.phoenix && ItemID == 200)
        { // 凤凰
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.skull &&
                 strstr(glowName, "mdl/Weapons/skull_grenade/skull_grenade_base_v.rmdl"))
        {
          std::array<float, 3> highlightParameter = {1, 0, 0};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.deathbox && item.isBox())
        { // 添加死亡之箱的开关判断
          std::array<float, 3> highlightParameter = {1, 0, 0};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }

        else if (item.isTrap())
        { // 判断名称是否是侵蚀陷阱？毒气罐？好像无效
          std::array<float, 3> highlightParameter = {0, 1, 0};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        // Gas Trap
        else if (strstr(glowName, "mdl/props/caustic_gas_tank/caustic_gas_tank.rmdl"))
        {
          std::array<float, 3> highlightParameter = {0, 1, 0};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.healthlarge && ItemID == 201)
        { // 大药包
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.healthsmall && ItemID == 202)
        { // 小药
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldbattsmall && ItemID == 204)
        { // 小电
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shieldbattlarge && ItemID == 203)
        { // 大电
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.sniperammo && ItemID == 154)
        {                                                                // 狙击弹药
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098}; // 紫色
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.heavyammo && ItemID == 153)
        {                                                      // 重型弹药
          std::array<float, 3> highlightParameter = {1, 1, 1}; // 改成墨绿色
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.optic1xhcog && ItemID == 237)
        { // 1倍镜
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lightammo && ItemID == 150)
        {                                                                   // 轻型子弹
          std::array<float, 3> highlightParameter = {0.6902, 0.60, 0.3098}; // 土黄
          int settingIndex = 77;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.energyammo && ItemID == 151)
        {                                                        // 能量子弹
          std::array<float, 3> highlightParameter = {0.2, 1, 0}; // 翠绿
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shotgunammo && ItemID == 152)
        {                                                             // 霰弹子弹
          std::array<float, 3> highlightParameter = {0.5, 0.0862, 0}; // 暗红
          int settingIndex = 79;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lasersight1 && ItemID == 251)
        { // 激光瞄准器，以下3个等级
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lasersight2 && ItemID == 252)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lasersight3 && ItemID == 253)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.sniperammomag1 && ItemID == 266)
        { // 狙击弹匣，以下四个等级
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.sniperammomag2 && ItemID == 267)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.sniperammomag3 && ItemID == 268)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.sniperammomag4 && ItemID == 269)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.energyammomag1 && ItemID == 262)
        { // 能量弹匣，四个等级
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.energyammomag2 && ItemID == 263)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.energyammomag3 && ItemID == 264)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.energyammomag4 && ItemID == 265)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.stocksniper1 && ItemID == 277)
        { // 狙击枪托
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.stocksniper2 && ItemID == 278)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.stocksniper3 && ItemID == 279)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.stockregular1 && ItemID == 274)
        { // 标准枪托
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.stockregular2 && ItemID == 275)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.stockregular3 && ItemID == 276)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shielddown1 && ItemID == 225)
        { // 击倒护盾
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shielddown2 && ItemID == 226)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shielddown3 && ItemID == 227)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shielddown4 && ItemID == 228)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.lightammomag1 && ItemID == 254)
        { // 轻型弹匣
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lightammomag2 && ItemID == 255)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lightammomag3 && ItemID == 256)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.lightammomag4 && ItemID == 257)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.heavyammomag1 && ItemID == 258)
        { // 重型弹匣
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.heavyammomag2 && ItemID == 259)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.heavyammomag3 && ItemID == 260)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.heavyammomag4 && ItemID == 261)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.optic2xhcog && ItemID == 238)
        { // 2倍镜
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.opticholo1x && ItemID == 239)
        { // 圆形1倍镜
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.opticholo1x2x && ItemID == 240)
        { // 1x2x切换
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.opticthreat && ItemID == 241)
        { // 金1倍
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.optic3xhcog && ItemID == 242)
        { // 3倍镜
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.optic2x4x && ItemID == 243)
        { // 2x4x
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.opticsniper6x && ItemID == 244)
        { // 6倍镜
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.opticsniper4x8x && ItemID == 245)
        { // 4x8x
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.opticsniperthreat && ItemID == 246)
        { // 金狙击镜
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.suppressor1 && ItemID == 247)
        { // 枪管？
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.suppressor2 && ItemID == 248)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.suppressor3 && ItemID == 249)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.turbo_charger && ItemID == 280)
        { // 涡轮增压器
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.skull_piecer && ItemID == 282)
        { // 穿颅器
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.hammer_point && ItemID == 284)
        { // 锤击点
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.disruptor_rounds && ItemID == 286)
        { // 干扰器
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.boosted_loader && ItemID == 295)
        { // 加速装填器
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        else if (g_settings.loot.shotgunbolt1 && ItemID == 270)
        { // 霰弹枪栓
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shotgunbolt2 && ItemID == 271)
        {
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shotgunbolt3 && ItemID == 272)
        {
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.shotgunbolt4 && ItemID == 273)
        {
          std::array<float, 3> highlightParameter = {1, 0.8431, 0};
          int settingIndex = 74;
          item.enableGlow(settingIndex, 48, highlightParameter);
        }
        // Nades
        else if (g_settings.loot.grenade_frag && ItemID == 235)
        { // 破片手雷
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }

        else if (g_settings.loot.grenade_thermite && ItemID == 234)
        { // 铝热剂
          std::array<float, 3> highlightParameter = {1, 1, 1};
          int settingIndex = 71;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.grenade_arc_star && ItemID == 236)
        { // 电弧星
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        // Weapons
        else if (g_settings.loot.weapon_kraber && ItemID == 1)
        { // 克莱伯
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_mastiff && ItemID == 2)
        { // 敖犬
          std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
          int settingIndex = 79;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_lstar && ItemID == 7)
        { // lstar
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_nemesis && ItemID == 145)
        { // 复仇女神
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_havoc && ItemID == 13)
        { // 哈沃克
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_devotion && ItemID == 19)
        { // 专注轻机枪
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_triple_take && ItemID == 24)
        { // 三重狙击枪
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_flatline && ItemID == 29)
        { // 平行
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_hemlock && ItemID == 34)
        { // 汉姆洛克
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_g7_scout && ItemID == 40)
        { // g7
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_alternator && ItemID == 73)
        { // 转换者
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_r99 && ItemID == 50)
        {
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_prowler && ItemID == 56)
        { // 猎兽
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_volt && ItemID == 62)
        { // 电能冲锋枪
          std::array<float, 3> highlightParameter = {0.2, 1, 0};
          int settingIndex = 78;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_longbow && ItemID == 67)
        { // 长弓
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_charge_rifle && ItemID == 72)
        { // 充能步枪
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_spitfire && ItemID == 77)
        { // 喷火
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_r301 && ItemID == 82)
        {
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_eva8 && ItemID == 87)
        {
          std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
          int settingIndex = 79;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_peacekeeper && ItemID == 93)
        {
          std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
          int settingIndex = 79;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_mozambique && ItemID == 98)
        {
          std::array<float, 3> highlightParameter = {0.5, 0.0862, 0};
          int settingIndex = 79;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_wingman && ItemID == 111)
        { // 小帮手
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_p2020 && ItemID == 117)
        {
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_re45 && ItemID == 126)
        {
          std::array<float, 3> highlightParameter = {1, 0.5490, 0};
          int settingIndex = 81;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_sentinel && ItemID == 132)
        { // 哨兵
          std::array<float, 3> highlightParameter = {0.2941, 0, 0.5098};
          int settingIndex = 73;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_bow && ItemID == 137)
        {
          std::array<float, 3> highlightParameter = {1, 0, 0};
          int settingIndex = 42;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_3030_repeater && ItemID == 139)
        {
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_rampage && ItemID == 147)
        { // 暴走
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (g_settings.loot.weapon_car_smg && ItemID == 742)
        {
          std::array<float, 3> highlightParameter = {0, 1, 1};
          int settingIndex = 80;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        else if (ItemID == 299)
        { // 保险库钥匙？
          std::array<float, 3> highlightParameter = {1, 0, 0};
          int settingIndex = 42;
          item.enableGlow(settingIndex, 64, highlightParameter);
        }
        else if (g_settings.loot.mobile_respawn && ItemID == 296)
        { // 重生信标
          std::array<float, 3> highlightParameter = {0, 0, 1};
          int settingIndex = 72;
          item.enableGlow(settingIndex, 32, highlightParameter);
        }
        // CREDITS to Rikkie
        // https://www.unknowncheats.me/forum/members/169606.html for all the
        // weapon ids and item ids code, you are a life saver!
      } // for(item) loop end
    } // while(item_glow) loop end
  } // while(item_t) loop end
  item_t = false;
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

  std::thread aimbot_thr;
  std::thread esp_thr;
  std::thread actions_thr;
  std::thread cactions_thr;
  // Used to change things on a timer
  // std::thread updateInsideValue_thr;
  std::thread TriggerBotRun_thr;
  std::thread terminal_thr;
  std::thread overlay_thr;
  std::thread itemglow_thr;
  std::thread control_thr;

  if (apex_mem.open_os() != 0)
  {
    exit(0);
  }

  while (active)
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
        control_t = false;
        g_Base = 0;
        quit_tui_menu();

        aimbot_thr.~thread();
        esp_thr.~thread();
        actions_thr.~thread();
        cactions_thr.~thread();
        // Used to change things on a timer
        // updateInsideValue_thr.~thread();
        TriggerBotRun_thr.~thread();
        terminal_thr.~thread();
        overlay_thr.~thread();
        itemglow_thr.~thread();
        control_thr.~thread();
      }

      std::this_thread::sleep_for(std::chrono::seconds(2));
      printf("Searching for apex process...\n");

      apex_mem.open_proc(ap_proc);

      if (apex_mem.get_proc_status() == process_status::FOUND_READY)
      {
        g_Base = apex_mem.get_proc_baseaddr();
        printf("\nApex process found\n");
        printf("Base: %lx\n", g_Base);

        aimbot_thr = std::thread(AimbotLoop);
        esp_thr = std::thread(EspLoop);
        actions_thr = std::thread(DoActions);
        cactions_thr = std::thread(ClientActions);
        // Used to change things on a timer
        // updateInsideValue_thr = std::thread(updateInsideValue);
        TriggerBotRun_thr = std::thread(TriggerBotRun);
        itemglow_thr = std::thread(item_glow_t);
        control_thr = std::thread(ControlLoop);
        aimbot_thr.detach();
        esp_thr.detach();
        actions_thr.detach();
        cactions_thr.detach();
        // Used to change things on a timer
        // updateInsideValue_thr.detach();
        TriggerBotRun_thr.detach();
        itemglow_thr.detach();
        control_thr.detach();
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
