//Date 2024/01/20
//GameVersion = v3.0.54.31

#define OFFSET_ENTITYLIST 0x1db2ed8 //[Miscellaneous].cl_entitylist 2024/01/19
#define OFFSET_LOCAL_ENT 0x21615d8 //[Miscellaneous].LocalPlayer 2024/01/19
#define OFFSET_NAME_LIST 0xc5ea050 //[Miscellaneous].NameList 2024/01/19
#define OFFSET_GLOBAL_VARS 0x16d65b0 //[Miscellaneous].GlobalVars 2024/01/19
#define OFFSET_MATRIX 0x11a350 //[Miscellaneous].ViewMatrix 2024/01/19
#define OFFSET_RENDER 0x73cc460 //[Miscellaneous].ViewRender 2024/01/19
#define OFFSET_CLIENTSTATE OFFSET_LEVELNAME - 0x1c0   //updated 1/11/2024
#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE + 0xAC //updated 1/10/2024 SignonState (ClientState + 0xAC)
#define OFFSET_INPUT_SYSTEM 0x1755c80 //[Miscellaneous].InputSystem 2024/01/19 
#define OFFSET_LEVELNAME 0x16d6ab0 //[Miscellaneous].LevelName 2024/01/19
#define OFFSET_VISIBLE_TIME 0x1950 //[Miscellaneous].CPlayer!lastVisibleTime 2024/01/19
#define OFFSET_STUDIOHDR 0xfb0 //[Miscellaneous].CBaseAnimating!m_pStudioHdr 2024/01/19
#define OFFSET_CAMERAPOS 0x1e90 //[Miscellaneous].CPlayer!camera_origin 2024/01/19
#define OFFSET_BULLET_SPEED 0x1e5c //[Miscellaneous].CWeaponX!m_flProjectileSpeed 2024/01/19
#define OFFSET_BULLET_SCALE 0x1e64 //[Miscellaneous].CWeaponX!m_flProjectileScale 2024/01/19
#define OFFSET_CROSSHAIR_LAST 0x1958 //[Miscellaneous].CWeaponX!lastCrosshairTargetTime 2024/01/19

#define OFFSET_ITEM_ID 0x1518 //updated 1/10/2024
#define OFFSET_HOST_MAP 0x015eee90 + 0x58 //[ConVars].host_map + 0x58 2024/01/19
#define OFFSET_WEAPON_NAME 0x1738 //[RecvTable.DT_WeaponX].m_weaponNameIndex 2024/01/19
#define OFFSET_WEAPON_BITFIELD 0x16c4 //[RecvTable.DT_WeaponX].m_modBitfieldFromPlayer 2024/01/19
#define OFFSET_ZOOM_FOV 0x1590 + 0x00b8 //[RecvTable.DT_WeaponX].m_playerData + m_curZoomFOV 2024/01/19
#define OFFSET_OFF_WEAPON 0x1904 //[DataMap.C_BaseCombatCharacter].m_latestNonOffhandWeapons 2024/01/19
#define OFFSET_PRIMARY_WEAPON 0x18f4 //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons 2024/01/19
#define OFFSET_GRAPPLE_ATTACHED 0x0048 //[RecvTable.DT_GrappleData].m_grappleAttached 2024/01/19
#define OFFSET_BONES 0x0d60 + 0x48 //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 2024/01/19
#define OFFSET_AMMO 0x1514 //[RecvTable.DT_PropSurvival].m_ammoInClip 2024/01/19
#define OFFSET_M_CUSTOMSCRIPTINT 0x1518 //[RecvTable.DT_PropSurvival].m_customScriptInt 2024/01/19

#define OFFSET_TEAM 0x030c //[RecvTable.DT_BaseEntity].m_iTeamNum 2024/01/19
#define OFFSET_SHIELD 0x01a0 //[RecvTable.DT_BaseEntity].m_shieldHealth 2024/01/19
#define OFFSET_MAXSHIELD 0x01a4 //[RecvTable.DT_BaseEntity].m_shieldHealthMax 2024/01/19
#define OFFSET_NAME 0x0449 //[RecvTable.DT_BaseEntity].m_iName 2024/01/19
#define OFFSET_SIGN_NAME 0x0440 //[RecvTable.DT_BaseEntity].m_iSignifierName 2024/01/19

#define OFFSET_ABS_VELOCITY 0x0170 //[DataMap.C_BaseEntity].m_vecAbsVelocity 2024/01/19
#define OFFSET_VIEW_OFFSET 0x00e8 //[DataMap.C_BaseEntity].m_currentFrame.viewOffset 2024/01/19
#define OFFSET_ORIGIN 0x017c //[DataMap.C_BaseEntity].m_vecAbsOrigin 2024/01/19
#define OFFSET_MODELNAME 0x0030 //[DataMap.C_BaseEntity].m_ModelName 2024/01/19

#define OFFSET_HEALTH 0x02fc //[RecvTable.DT_Player].m_iHealth 2024/01/19
#define OFFSET_ARMORTYPE 0x45cc //[RecvTable.DT_Player].m_armorType 2024/01/19
#define OFFSET_ZOOMING 0x1b91 //[RecvTable.DT_Player].m_bZooming 2024/01/19
#define OFFSET_PLATFORM_UID 0x2508 //[RecvTable.DT_Player].m_platformUserId 2024/01/19
#define OFFSET_SKYDRIVESTATE 0x462c //[RecvTable.DT_Player].m_skydiveState 2024/01/19
#define OFFSET_GRAPPLE 0x2c08 //[RecvTable.DT_Player].m_grapple 2024/01/19
#define OFFSET_GRAPPLE_ACTIVE 0x2c98 //[RecvTable.DT_Player].m_grappleActive 2024/01/19
#define OFFSET_LIFE_STATE 0x0658 //[RecvTable.DT_Player].m_lifeState 2024/01/19
#define OFFSET_BLEED_OUT_STATE 0x26a0 //[RecvTable.DT_Player].m_bleedoutState 2024/01/19
#define OFFSET_VIEWANGLES 0x24f4 - 0x14 //[RecvTable.DT_Player].m_ammoPoolCapacity - 0x14 2024/01/19
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10 //updated 1/11/2024
#define OFFSET_ACTIVE_WEAPON 0x1888 + 0x0058 //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons 2024/01/19
#define OFFSET_VIEW_MODELS 0x2ce0 //[RecvTable.DT_Player].m_hViewModels 2024/01/19

#define OFFSET_TRAVERSAL_PROGRESS 0x2abc //[DataMap.C_Player].m_traversalProgress 2024/01/19
#define OFFSET_TRAVERSAL_STARTTIME 0x2ac0 //[DataMap.C_Player].m_traversalStartTime 2024/01/19
#define OFFSET_FLAGS 0x00c8 //[DataMap.C_Player].m_fFlags 2024/01/19
#define OFFSET_WALLRUNSTART 0x3524 //[DataMap.C_Player].m_wallRunStartTime 2024/01/19
#define OFFSET_WALLRUNCLEAR 0x3528 //[DataMap.C_Player].m_wallRunClearTime float 2024/01/19
#define OFFSET_IN_DUCKSTATE 0x29ac //[DataMap.C_Player].m_duckState 2024/01/19
#define OFFSET_AIMPUNCH 0x23f8 //[DataMap.C_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle 2024/01/19
#define OFFSET_YAW 0x21fc - 0x8 //[DataMap.C_Player].m_currentFramePlayer.m_ammoPoolCount 2024/01/19 - 0x8
#define OFFSET_TIME_BASE 0x2048 //[DataMap.C_Player].m_currentFramePlayer.timeBase 2024/01/19

#define OFFSET_IN_ATTACK 0x073cd740 //[Buttons].in_attack 2024/01/19
#define OFFSET_IN_TOGGLE_DUCK 0x073cd760 //[Buttons].in_toggle_duck 2024/01/19
#define OFFSET_IN_ZOOM 0x073cd8c0 //[Buttons].in_zoom 2024/01/19
#define OFFSET_IN_FORWARD 0x073cd670 //[Buttons].in_forward 2024/01/19
#define OFFSET_IN_JUMP 0x073cd828 //[Buttons].in_jump 2024/01/19
#define OFFSET_IN_DUCK 0x073cd920 //[Buttons].in_duck 2024/01/19
#define OFFSET_IN_BACKWARD 0x073cd698 //[Buttons].in_backward 2024/01/19
#define OFFSET_IN_USE 0x073cd8a0 //[Buttons].in_use 2024/01/19

#define OFFSET_OBSERVER_MODE 0x3454 //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode 2024/01/19
#define OFFSET_OBSERVING_TARGET 0x3460 //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget 2024/01/19

#define OFFSET_GLOW_ENABLE 0x28C //updated OFFSET_GLOW_CONTEXT_ID 7 = enabled, 2 = disabled
#define OFFSET_GLOW_THROUGH_WALLS 0x26c //updated 2 = enabled, 5 = disabled
#define OFFSET_GLOW_FIX 0x268 //updated 1/10/2024
#define OFFSET_HIGHLIGHTGENERICCONTEXT 0x028d //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts 2024/01/19
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE //updated Script_Highlight_SetCurrentContext
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS //updated Script_Highlight_SetVisibilityType 5th mov
#define HIGHLIGHT_SETTINGS 0xB93E050 //updated 1/11/2024 HighlightSettings
#define HIGHLIGHT_TYPE_SIZE 0x34 //updated 1/10/2024

//#define OFFSET_ITEM_GLOW 0x02f0 //m_highlightFunctionBits updated 11/20/2023
//#define GLOW_START_TIME 0x02c8 + 0x30 //m_playerFloatLookStartTime=0x02c8 //updated 11/20/2023
//#define OFFSET_GLOW_T2 0x2dc+ 0x30 //1193322764 = enabled, 0 = disabled
//#define GLOW_LIFE_TIME 0x3A4+ 0x30 // Script_Highlight_SetLifeTime + 4
//#define GLOW_DISTANCE 0x26c // Script_Highlight_SetFarFadeDist
//#define GLOW_TYPE 0x29c // Script_Highlight_GetState + 4
//#define GLOW_COLOR 0x1D0+ 0x30 // Script_CopyHighlightState 15th mov
//#define GLOW_FADE 0x388+ 0x30 // Script_Highlight_GetCurrentInsideOpacity 3rd result of 3 offsets consecutive or first + 8
//#define OFFSET_CROSSHAIR_START 0x1A84 //CPlayer!crosshairTargetStartTime
