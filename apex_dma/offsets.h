//Date 2024/01/26
//GameVersion = v3.0.55.37

#define OFFSET_ENTITYLIST 0x1db2e38 //[Miscellaneous].cl_entitylist updated 2024/01/26
#define OFFSET_LOCAL_ENT 0x2161538 //[Miscellaneous].LocalPlayer updated 2024/01/26
#define OFFSET_NAME_LIST 0xc5e9fd0 //[Miscellaneous].NameList updated 2024/01/26
#define OFFSET_GLOBAL_VARS 0x16d65c0 //[Miscellaneous].GlobalVars updated 2024/01/26
#define OFFSET_MATRIX 0x11a350 //[Miscellaneous].ViewMatrix updated 2024/01/26
#define OFFSET_RENDER 0x73cc3e0 //[Miscellaneous].ViewRender updated 2024/01/26
#define OFFSET_CLIENTSTATE OFFSET_LEVELNAME - 0x1c0   //updated 1/11/2024
#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE + 0xAC //updated 1/10/2024 SignonState (ClientState + 0xAC)
#define OFFSET_INPUT_SYSTEM 0x1755bc0 //[Miscellaneous].InputSystem updated 2024/01/26 
#define OFFSET_LEVELNAME 0x16d6ac0 //[Miscellaneous].LevelName updated 2024/01/26
#define OFFSET_VISIBLE_TIME 0x1950 //[Miscellaneous].CPlayer!lastVisibleTime updated 2024/01/26
#define OFFSET_STUDIOHDR 0xfb0 //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2024/01/26
#define OFFSET_CAMERAPOS 0x1e90 //[Miscellaneous].CPlayer!camera_origin updated 2024/01/26
#define OFFSET_BULLET_SPEED 0x1e5c //[Miscellaneous].CWeaponX!m_flProjectileSpeed updated 2024/01/26
#define OFFSET_BULLET_SCALE 0x1e64 //[Miscellaneous].CWeaponX!m_flProjectileScale updated 2024/01/26
#define OFFSET_CROSSHAIR_LAST 0x1958 //[Miscellaneous].CWeaponX!lastCrosshairTargetTime updated 2024/01/26

#define OFFSET_ITEM_ID 0x1518 //updated 1/10/2024
#define OFFSET_HOST_MAP 0x015eeec0 + 0x58 //[ConVars].host_map + 0x58 updated 2024/01/26
#define OFFSET_WEAPON_NAME 0x1738 //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2024/01/26
#define OFFSET_WEAPON_BITFIELD 0x16c4 //[RecvTable.DT_WeaponX].m_modBitfieldFromPlayer updated 2024/01/26
#define OFFSET_ZOOM_FOV 0x1590 + 0x00b8 //[RecvTable.DT_WeaponX].m_playerData + m_curZoomFOV updated 2024/01/26
#define OFFSET_OFF_WEAPON 0x1904 //[DataMap.C_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2024/01/26
#define OFFSET_PRIMARY_WEAPON 0x18f4 //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons updated 2024/01/26
#define OFFSET_GRAPPLE_ATTACHED 0x0048 //[RecvTable.DT_GrappleData].m_grappleAttached updated 2024/01/26
#define OFFSET_BONES 0x0d60 + 0x48 //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 updated 2024/01/26
#define OFFSET_AMMO 0x1514 //[RecvTable.DT_PropSurvival].m_ammoInClip updated 2024/01/26
#define OFFSET_M_CUSTOMSCRIPTINT 0x1518 //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2024/01/26

#define OFFSET_TEAM 0x030c //[RecvTable.DT_BaseEntity].m_iTeamNum updated 2024/01/26
#define OFFSET_SHIELD 0x01a0 //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2024/01/26
#define OFFSET_MAXSHIELD 0x01a4 //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2024/01/26
#define OFFSET_NAME 0x0449 //[RecvTable.DT_BaseEntity].m_iName updated 2024/01/26
#define OFFSET_SIGN_NAME 0x0440 //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2024/01/26

#define OFFSET_ABS_VELOCITY 0x0170 //[DataMap.C_BaseEntity].m_vecAbsVelocity updated 2024/01/26
#define OFFSET_VIEW_OFFSET 0x00e8 //[DataMap.C_BaseEntity].m_currentFrame.viewOffset updated 2024/01/26
#define OFFSET_ORIGIN 0x017c //[DataMap.C_BaseEntity].m_vecAbsOrigin updated 2024/01/26
#define OFFSET_MODELNAME 0x0030 //[DataMap.C_BaseEntity].m_ModelName updated 2024/01/26

#define OFFSET_HEALTH 0x02fc //[RecvTable.DT_Player].m_iHealth updated updated 2024/01/26
#define OFFSET_ARMORTYPE 0x45cc //[RecvTable.DT_Player].m_armorType updated updated 2024/01/26
#define OFFSET_ZOOMING 0x1b91 //[RecvTable.DT_Player].m_bZooming updated 2024/01/26
#define OFFSET_PLATFORM_UID 0x2508 //[RecvTable.DT_Player].m_platformUserId updated 2024/01/26
#define OFFSET_SKYDRIVESTATE 0x462c //[RecvTable.DT_Player].m_skydiveState updated 2024/01/26
#define OFFSET_GRAPPLE 0x2c08 //[RecvTable.DT_Player].m_grapple updated 2024/01/26
#define OFFSET_GRAPPLE_ACTIVE 0x2c98 //[RecvTable.DT_Player].m_grappleActive updated 2024/01/26
#define OFFSET_LIFE_STATE 0x0658 //[RecvTable.DT_Player].m_lifeState updated 2024/01/26
#define OFFSET_BLEED_OUT_STATE 0x26a0 //[RecvTable.DT_Player].m_bleedoutState updated 2024/01/26
#define OFFSET_VIEWANGLES 0x24f4 - 0x14 //[RecvTable.DT_Player].m_ammoPoolCapacity - 0x14 updated 2024/01/26
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10 //updated 1/11/2024
#define OFFSET_ACTIVE_WEAPON 0x1888 + 0x0058 //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons updated 2024/01/26
#define OFFSET_VIEW_MODELS 0x2ce0 //[RecvTable.DT_Player].m_hViewModels updated 2024/01/26

#define OFFSET_TRAVERSAL_PROGRESS 0x2abc //[DataMap.C_Player].m_traversalProgress updated 2024/01/26
#define OFFSET_TRAVERSAL_STARTTIME 0x2ac0 //[DataMap.C_Player].m_traversalStartTime updated 2024/01/26
#define OFFSET_FLAGS 0x00c8 //[DataMap.C_Player].m_fFlags updated 2024/01/26
#define OFFSET_WALLRUNSTART 0x3524 //[DataMap.C_Player].m_wallRunStartTime updated 2024/01/26
#define OFFSET_WALLRUNCLEAR 0x3528 //[DataMap.C_Player].m_wallRunClearTime float updated 2024/01/26
#define OFFSET_IN_DUCKSTATE 0x29ac //[DataMap.C_Player].m_duckState updated 2024/01/26
#define OFFSET_AIMPUNCH 0x23f8 //[DataMap.C_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2024/01/26
#define OFFSET_YAW 0x21fc - 0x8 //[DataMap.C_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2024/01/26
#define OFFSET_TIME_BASE 0x2048 //[DataMap.C_Player].m_currentFramePlayer.timeBase updated 2024/01/26

#define OFFSET_IN_ATTACK 0x073cd6a0 //[Buttons].in_attack updated 2024/01/26
#define OFFSET_IN_TOGGLE_DUCK 0x073cd6e0 //[Buttons].in_toggle_duck updated 2024/01/26
#define OFFSET_IN_ZOOM 0x073cd840 //[Buttons].in_zoom updated 2024/01/26
#define OFFSET_IN_FORWARD 0x073cd5f0 //[Buttons].in_forward updated 2024/01/26
#define OFFSET_IN_JUMP 0x073cd7a0 //[Buttons].in_jump updated 2024/01/26
#define OFFSET_IN_DUCK 0x073cd8a0 //[Buttons].in_duck updated 2024/01/26
#define OFFSET_IN_BACKWARD 0x073cd618 //[Buttons].in_backward updated 2024/01/26
#define OFFSET_IN_USE 0x073cd820 //[Buttons].in_use updated 2024/01/26

#define OFFSET_OBSERVER_MODE 0x3454 //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode updated 2024/01/26
#define OFFSET_OBSERVING_TARGET 0x3460 //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget updated 2024/01/26

#define OFFSET_GLOW_ENABLE 0x28C //updated OFFSET_GLOW_CONTEXT_ID 7 = enabled, 2 = disabled
#define OFFSET_GLOW_THROUGH_WALLS 0x26c //updated 2 = enabled, 5 = disabled
#define OFFSET_GLOW_FIX 0x268 //updated 1/10/2024
#define OFFSET_HIGHLIGHTGENERICCONTEXT 0x028d //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts updated 2024/01/26
#define OFFSET_GLOW_CONTEXT_ID OFFSET_GLOW_ENABLE //updated Script_Highlight_SetCurrentContext
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS //updated
#define HIGHLIGHT_SETTINGS 0xB93DFD0 //updated 1/11/2024 HighlightSettings
#define HIGHLIGHT_TYPE_SIZE 0x34 //updated 1/10/2024
