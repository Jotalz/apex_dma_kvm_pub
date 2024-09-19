#include "FNVHash.h"
#include <unordered_map>
#include <fstream>
#include <unordered_set>

enum class ItemList : uint64_t
{
    weapon_kraber = 1,
    weapon_mastiff = 2,
    weapon_lstar = 7,
    weapon_havoc = 13,
    weapon_devotion = 19,
    weapon_triple_take = 24,
    weapon_flatline = 29,
    weapon_hemlock = 34,
    weapon_g7_scout = 39,
    weapon_alternator = 44,
    weapon_r99 = 49,
    weapon_prowler = 55,
    weapon_volt = 60,
    weapon_longbow = 65,
    weapon_charge_rifle = 70,
    weapon_spitfire = 75,
    weapon_r301 = 80,
    weapon_eva8 = 85,
    weapon_peacekeeper = 90,
    weapon_mozambique = 95,
    weapon_wingman = 108,
    weapon_p2020 = 113,
    weapon_re45 = 122,
    weapon_sentinel = 127,
    weapon_bow = 132,
    weapon_3030_repeater = 133,
    weapon_nemesis = 139,
    lightammo = 144,
    energyammo = 145,
    shotgunammo = 146,
    heavyammo = 147,
    sniperammo = 148,
    weapon_rampage = 150,
    weapon_car_smg = 155,
    accelerant = 193,
    phoenix = 194,
    healthlarge = 195,
    healthsmall = 196,
    shieldbattlarge = 197,
    shieldbattsmall = 198,
    shieldupgradehead1 = 199,
    shieldupgradehead2 = 200,
    shieldupgradehead3 = 201,
    shieldupgradehead4 = 202,
    shieldupgrade1 = 214,
    shieldupgrade2 = 215,
    shieldupgrade3 =216,
    shielddown1 = 219,
    shielddown2 = 220,
    shielddown3 =221,
    shielddown4 = 222,
    lightbackpack = 223,
    medbackpack = 224,
    heavybackpack = 225,
    goldbackpack = 226,
    grenade_thermite = 228,
    grenade_frag = 229,
    grenade_arc_star = 230,
    optic1xhcog = 231,
    optic2xhcog = 232,
    opticholo1x = 233,
    opticholo1x2x = 234,
    opticthreat = 235,
    optic3xhcog = 236,
    optic2x4x = 237,
    opticsniper6x = 238,
    opticsniper4x8x = 239,
    opticsniperthreat = 240,
    suppressor1 = 241,
    suppressor2 = 242,
    suppressor3 = 243,
    lasersight1 = 245,
    lasersight2 = 246,
    lasersight3 = 247,
    lightammomag1 = 248,
    lightammomag2 = 249,
    lightammomag3 = 250,
    lightammomag4 = 251,
    heavyammomag1 = 252,
    heavyammomag2 = 253,
    heavyammomag3 = 254,
    heavyammomag4 = 255,
    energyammomag1 = 256,
    energyammomag2 = 257,
    energyammomag3 = 258,
    energyammomag4 = 259,
    sniperammomag1 = 260,
    sniperammomag2 = 261,
    sniperammomag3 = 262,
    sniperammomag4 = 263,
    shotgunbolt1 = 264,
    shotgunbolt2 = 265,
    shotgunbolt3 = 266,
    shotgunbolt4 = 267,
    stockregular1 = 268,
    stockregular2 = 269,
    stockregular3 =270,
    stocksniper1 = 271,
    stocksniper2 = 272,
    stocksniper3 = 273,
    turbo_charger = 274,
    skull_piecer = 276,
    hammer_point = 280,
    disruptor_rounds = 281,
    boosted_loader = 288,
};

std::unordered_map<std::string, ItemList, FnvHash> itemNameToEnum = {
    {"mdl/weapons/at_rifle/w_at_rifle.rmdl",ItemList::weapon_kraber},
    {"mdl/techart/mshop/weapons/class/shotgun/mastiff/mastiff_base_w.rmdl",ItemList::weapon_mastiff},
    {"mdl/weapons/lstar/w_lstar.rmdl",ItemList::weapon_lstar},
    {"mdl/weapons/beam_ar/w_beam_ar_legendary_01.rmdl",ItemList::weapon_havoc},
    {"mdl/weapons/hemlock_br/w_hemlock_br_holloween_2019.rmdl",ItemList::weapon_devotion},
    {"mdl/weapons/doubletake/w_doubletake.rmdl",ItemList::weapon_triple_take},
    {"mdl/techart/mshop/weapons/class/assault/flatline/flatline_base_w.rmdl",ItemList::weapon_flatline},
    {"mdl/weapons/m1a1_hemlok/w_hemlok_legendary_01.rmdl",ItemList::weapon_hemlock},
    {"mdl/techart/mshop/weapons/class/assault/g7/g7_base_w.rmdl",ItemList::weapon_g7_scout},
    {"mdl/weapons/alternator_smg/w_alternator_smg.rmdl",ItemList::weapon_alternator},
    {"mdl/weapons/r97/w_r97.rmdl",ItemList::weapon_r99},
    {"mdl/techart/mshop/weapons/class/smg/prowler/prowler_base_w.rmdl",ItemList::weapon_prowler},
    {"mdl/weapons/hemlok_smg/w_hemlok_smg.rmdl",ItemList::weapon_volt},
    {"mdl/weapons/rspn101_dmr/w_rspn101_dmr.rmdl",ItemList::weapon_longbow},
    {"mdl/weapons/defender/w_defender_legendary_04.rmdl",ItemList::weapon_charge_rifle},
    {"mdl/weapons/lmg_hemlok/w_lmg_hemlok.rmdl",ItemList::weapon_spitfire},
    {"mdl/weapons/rspn101/w_rspn101_legendary_01.rmdl",ItemList::weapon_r301},
    {"mdl/weapons/w1128/w_w1128.rmdl",ItemList::weapon_eva8},
    {"mdl/weapons/peacekeeper/w_peacekeeper.rmdl",ItemList::weapon_peacekeeper},
    {"mdl/weapons/pstl_sa3/w_pstl_sa3.rmdl",ItemList::weapon_mozambique},
    {"mdl/weapons/b3wing/w_b3wing_legendary_01.rmdl",ItemList::weapon_wingman},
    {"mdl/weapons/p2011/w_p2011.rmdl",ItemList::weapon_p2020},
    {"mdl/weapons/p2011_auto/w_p2011_auto.rmdl",ItemList::weapon_re45},
    {"mdl/weapons/sentinel/w_sentinel.rmdl",ItemList::weapon_sentinel},
    {"mdl/weapons/compound_bow/w_compound_bow.rmdl",ItemList::weapon_bow},
    {"mdl/weapons/3030repeater/w_3030repeater.rmdl",ItemList::weapon_3030_repeater},
    {"mdl/techart/mshop/weapons/class/assault/nemesis/nemesis_base_w.rmdl",ItemList::weapon_nemesis},
    {"mdl/weapons_r5/loot/_master/w_loot_wep_ammo_sc.rmdl",ItemList::lightammo},
    {"mdl/weapons_r5/loot/_master/w_loot_wep_ammo_nrg.rmdl",ItemList::energyammo},
    {"mdl/weapons_r5/loot/_master/w_loot_wep_ammo_shg.rmdl",ItemList::shotgunammo},
    {"mdl/weapons_r5/loot/_master/w_loot_wep_ammo_hc.rmdl",ItemList::heavyammo},
    {"mdl/weapons_r5/loot/_master/w_loot_wep_ammo_sniper.rmdl",ItemList::sniperammo},
    {"mdl/techart/mshop/weapons/class/lmg/dragon/dragon_base_w.rmdl",ItemList::weapon_rampage},
    {"mdl/techart/mshop/weapons/class/smg/car/car_base_w.rmdl",ItemList::weapon_car_smg},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_ultimate_accelerant.rmdl",ItemList::accelerant},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_phoenix_kit_v1.rmdl",ItemList::phoenix},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_health_main_large.rmdl",ItemList::healthlarge},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_health_main_small.rmdl",ItemList::healthsmall},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_large.rmdl",ItemList::shieldbattlarge},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_small.rmdl",ItemList::shieldbattsmall},
    {"mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl",ItemList::shieldupgradehead1},
    { "mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl",ItemList::shieldupgradehead2},
    {"mdl/humans_r5/loot/w_loot_char_backpack_light.rmdl", ItemList::lightbackpack},
    {"mdl/humans_r5/loot/w_loot_char_backpack_medium.rmdl", ItemList::medbackpack},
    {"mdl/humans_r5/loot/w_loot_char_backpack_heavy.rmdl", ItemList::heavybackpack},
    {"mdl/humans_r5/loot/w_loot_char_backpack_heavy.rmdl", ItemList::goldbackpack},
    {"mdl/props/loot_wep_iso_armor/w_loot_wep_iso_armor_core_01.rmdl", ItemList::shieldupgrade1},
    {"mdl/props/loot_wep_iso_armor/w_loot_wep_iso_armor_core_01.rmdl", ItemList::shieldupgrade2},
    {"mdl/props/loot_wep_iso_armor/w_loot_wep_iso_armor_core_01.rmdl", ItemList::shieldupgrade3},
    {"mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl", ItemList::shieldupgradehead1},
    {"mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl", ItemList::shieldupgradehead2},
    {"mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl", ItemList::shieldupgradehead3},
    {"mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl", ItemList::shieldupgradehead4},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_ultimate_accelerant.rmdl", ItemList::accelerant},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_phoenix_kit_v1.rmdl", ItemList::phoenix},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_health_main_large.rmdl", ItemList::healthlarge},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_health_main_small.rmdl", ItemList::healthsmall},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_large.rmdl", ItemList::shieldbattlarge},
    {"mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_small.rmdl", ItemList::shieldbattsmall},
};

std::string getEnumName(ItemList itemEnum) {
    switch (itemEnum) {
        case ItemList::lightbackpack: return "lightbackpack";
        case ItemList::medbackpack: return "medbackpack";
        case ItemList::heavybackpack: return "heavybackpack";
        case ItemList::goldbackpack: return "goldbackpack";
        case ItemList::shieldupgrade1: return "shieldupgrade1";
        case ItemList::shieldupgrade2: return "shieldupgrade2";
        case ItemList::shieldupgrade3: return "shieldupgrade3";
        case ItemList::shieldupgradehead1: return "shieldupgradehead1";
        case ItemList::shieldupgradehead2: return "shieldupgradehead2";
        case ItemList::shieldupgradehead3: return "shieldupgradehead3";
        case ItemList::shieldupgradehead4: return "shieldupgradehead4";
        case ItemList::accelerant: return "accelerant";
        case ItemList::phoenix: return "phoenix";
        case ItemList::healthlarge: return "healthlarge";
        case ItemList::healthsmall: return "healthsmall";
        case ItemList::shieldbattlarge: return "shieldbattlarge";
        case ItemList::shieldbattsmall: return "shieldbattsmall";
        case ItemList::weapon_kraber: return "weapon_kraber";
        case ItemList::weapon_mastiff: return "weapon_mastiff";
        case ItemList::weapon_lstar: return "weapon_lstar";
        case ItemList::weapon_havoc: return "weapon_havoc";
        case ItemList::weapon_devotion: return "weapon_devotion";
        case ItemList::weapon_triple_take: return "weapon_triple_take";
        case ItemList::weapon_flatline: return "weapon_flatline";
        case ItemList::weapon_hemlock: return "weapon_hemlock";
        case ItemList::weapon_g7_scout: return "weapon_g7_scout";
        case ItemList::weapon_alternator: return "weapon_alternator";
        case ItemList::weapon_r99: return "weapon_r99";
        case ItemList::weapon_prowler: return "weapon_prowler";
        case ItemList::weapon_volt: return "weapon_volt";
        case ItemList::weapon_longbow: return "weapon_longbow";
        case ItemList::weapon_charge_rifle: return "weapon_charge_rifle";
        case ItemList::weapon_spitfire: return "weapon_spitfire";
        case ItemList::weapon_r301: return "weapon_r301";
        case ItemList::weapon_eva8: return "weapon_eva8";
        case ItemList::weapon_peacekeeper: return "weapon_peacekeeper";
        case ItemList::weapon_mozambique: return "weapon_mozambique";
        case ItemList::weapon_wingman: return "weapon_wingman";
        case ItemList::weapon_p2020: return "weapon_p2020";
        case ItemList::weapon_re45: return "weapon_re45";
        case ItemList::weapon_sentinel: return "weapon_sentinel";
        case ItemList::weapon_bow: return "weapon_bow";
        case ItemList::weapon_3030_repeater: return "weapon_3030_repeater";
        case ItemList::weapon_nemesis: return "weapon_nemesis";
        case ItemList::lightammo: return "lightammo";
        case ItemList::energyammo: return "energyammo";
        case ItemList::heavyammo: return "heavyammo";
        case ItemList::shotgunammo: return "shotgunammo";
        case ItemList::sniperammo: return "sniperammo";
        case ItemList::weapon_rampage: return "weapon_rampage";
        case ItemList::weapon_car_smg: return "weapon_car_smg";
        default: return "unknown";
    }
}

