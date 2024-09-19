#include "FNVHash.h"
#include <unordered_map>
#include <fstream>
#include <unordered_set>

enum class ItemList : uint64_t
{
    lightbackpack = 223,
    medbackpack = 224,
    heavybackpack = 225,
    goldbackpack = 226,
    shieldupgrade1 = 214,
    shieldupgrade2 = 215,
    shieldupgrade3 =216,
    accelerant = 193,
    shieldupgradehead1 = 199,
    shieldupgradehead2 = 200,
    shieldupgradehead3 = 201,
    shieldupgradehead4 = 202,
};

std::unordered_map<std::string, ItemList, FnvHash> itemNameToEnum = {
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
        default: return "unknown";
    }
}

