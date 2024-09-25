use std::path::PathBuf;

use serde::{Deserialize, Serialize};

use crate::love_players::LovePlayer;

#[repr(C)]
#[derive(Clone, Deserialize, Serialize, Debug)]
pub struct EspVisuals {
    pub r#box: bool,
    pub line: bool,
    pub distance: bool,
    pub health_bar: bool,
    pub shield_bar: bool,
    pub name: bool,
}

#[repr(C)]
#[derive(Clone, Deserialize, Serialize, Debug)]
pub struct Loot {
    // rev skull
    pub skull: bool,
    // Backpacks
    pub lightbackpack: bool,
    pub medbackpack: bool,
    pub heavybackpack: bool,
    pub goldbackpack: bool,
    // Shield upgrades
    pub shieldupgrade1: bool, // white
    pub shieldupgrade2: bool, // blue
    pub shieldupgrade3: bool, // purple
    pub shieldupgrade4: bool, // gold
    pub shieldupgrade5: bool, // red
    pub shieldupgradehead1: bool,
    pub shieldupgradehead2: bool,
    pub shieldupgradehead3: bool,
    pub shieldupgradehead4: bool,
    pub shielddown1: bool,
    pub shielddown2: bool,
    pub shielddown3: bool,
    pub shielddown4: bool,
    // heaing and Misc
    pub accelerant: bool,
    pub phoenix: bool,
    pub healthlarge: bool,
    pub healthsmall: bool,
    pub shieldbattsmall: bool,
    pub shieldbattlarge: bool,
    pub mobile_respawn: bool,
    // Ammo
    pub sniperammo: bool,
    pub heavyammo: bool,
    pub lightammo: bool,
    pub energyammo: bool,
    pub shotgunammo: bool,
    // Optics
    pub optic1xhcog: bool,
    pub optic2xhcog: bool,
    pub opticholo1x: bool,
    pub opticholo1x2x: bool,
    pub opticthreat: bool,
    pub optic3xhcog: bool,
    pub optic2x4x: bool,
    pub opticsniper6x: bool,
    pub opticsniper4x8x: bool,
    pub opticsniperthreat: bool,
    // Magazines
    pub sniperammomag1: bool,
    pub energyammomag1: bool,
    pub lightammomag1: bool,
    pub heavyammomag1: bool,
    pub sniperammomag2: bool,
    pub energyammomag2: bool,
    pub lightammomag2: bool,
    pub heavyammomag2: bool,
    pub sniperammomag3: bool,
    pub energyammomag3: bool,
    pub lightammomag3: bool,
    pub heavyammomag3: bool,
    pub sniperammomag4: bool,
    pub energyammomag4: bool,
    pub lightammomag4: bool,
    pub heavyammomag4: bool,
    // Attachments
    pub lasersight1: bool,
    pub lasersight2: bool,
    pub lasersight3: bool,
    pub lasersight4: bool,
    pub stocksniper1: bool,
    pub stocksniper2: bool,
    pub stocksniper3: bool,
    pub stocksniper4: bool,
    pub stockregular1: bool,
    pub stockregular2: bool,
    pub stockregular3: bool,
    pub suppressor1: bool,
    pub suppressor2: bool,
    pub suppressor3: bool,
    pub turbo_charger: bool,
    pub skull_piecer: bool,
    pub hammer_point: bool,
    pub disruptor_rounds: bool,
    pub boosted_loader: bool,
    pub anvil_receiver: bool,
    pub doubletap_trigger: bool,
    pub dual_shell: bool,
    pub kinetic_feeder:bool,
    pub quickdraw_holster:bool,
    pub shotgunbolt1: bool,
    pub shotgunbolt2: bool,
    pub shotgunbolt3: bool,
    pub shotgunbolt4: bool,
    // Nades
    pub grenade_frag: bool,
    pub grenade_arc_star: bool,
    pub grenade_thermite: bool,
    // Supply Drop Weapons
    pub weapon_kraber: bool,
    pub weapon_bow: bool,
    // Shotguns
    pub weapon_mastiff: bool,
    pub weapon_eva8: bool,
    pub weapon_peacekeeper: bool,
    pub weapon_mozambique: bool,
    // Energy weapons
    pub weapon_lstar: bool,
    pub weapon_nemesis: bool,
    pub weapon_havoc: bool,
    pub weapon_devotion: bool,
    pub weapon_triple_take: bool,
    pub weapon_volt: bool,
    // Heavy Weapons
    pub weapon_flatline: bool,
    pub weapon_hemlock: bool,
    pub weapon_3030_repeater: bool,
    pub weapon_rampage: bool,
    pub weapon_car_smg: bool,
    pub weapon_prowler: bool,
    // Light weapons
    pub weapon_p2020: bool,
    pub weapon_re45: bool,
    pub weapon_g7_scout: bool,
    pub weapon_alternator: bool,
    pub weapon_r99: bool,
    pub weapon_spitfire: bool,
    pub weapon_r301: bool,
    // Snipers.. wingman is the odd one...and the bow..
    pub weapon_wingman: bool,
    pub weapon_longbow: bool,
    pub weapon_charge_rifle: bool,
    pub weapon_sentinel: bool,
}

#[derive(Clone, Deserialize, Serialize, Debug)]
pub struct Config {
    pub(crate) settings: Settings,
    #[serde(skip_serializing_if = "Vec::is_empty", default)]
    pub(crate) love_player: Vec<LovePlayer>,
}

#[repr(C)]
#[derive(Clone, Deserialize, Serialize, Debug)]
pub struct Settings {
    pub load_settings: bool,
    pub no_overlay: bool,
    pub screen_width: u32,
    pub screen_height: u32,
    pub yuan_p: bool,
    pub debug_mode: bool,
    pub keyboard: bool,
    pub gamepad: bool,
    pub aimbot_hot_key_1: i32,
    pub aimbot_hot_key_2: i32,
    pub trigger_bot_hot_key: i32,
    pub flick_bot_hot_key: i32,
    pub quickglow_hot_key: i32,
    pub quickaim_hot_key: i32,
    pub loot_filled_toggle: bool,
    pub player_filled_toggle: bool,
    pub super_glide: bool,
    pub super_grpple:bool,
    pub auto_tapstrafe:bool,
    pub onevone: bool,
    pub tdm_toggle: bool,
    pub item_glow: bool,
    pub player_glow: bool,
    pub player_glow_armor_color: bool,
    pub player_glow_love_user: bool,
    pub weapon_model_glow: bool,
    pub kbd_backlight_control: bool,
    pub bow_charge_rifle_aim: bool,
    pub trigger_bot_shot: bool,
    pub deathbox: bool,
    pub aim_no_recoil: bool,
    pub recoil_pitch: f32,
    pub recoil_yaw: f32,
    pub ads_fov: f32,
    pub non_ads_fov: f32,
    pub flick_fov: f32,
    pub aim: i32,
    pub esp: bool,
    pub esp_visuals: EspVisuals,
    pub mini_map_radar: bool,
    pub mini_map_guides: bool,
    pub mini_map_radar_dot_size1: i32,
    pub mini_map_radar_dot_size2: i32,
    pub main_radar_map: bool,
    pub main_map_radar_dot_size1: i32,
    pub main_map_radar_dot_size2: i32,
    pub aim_dist: f32,
    pub max_dist: f32,
    pub glow_dist: f32,
    pub map_radar_hotkey: i32,
    pub show_aim_target: bool,
    pub game_fps: f32,
    pub calc_game_fps: bool,
    pub no_nade_aim: bool,
    pub firing_range: bool,
    pub bone: i32,
    pub bone_nearest: bool,
    pub bone_auto: bool,
    pub flick_nearest: bool,
    pub headshot_dist: f32,
    pub skynade_dist: f32,
    pub smooth: f32,
    pub smooth_sub: f32,
    pub skynade_smooth: f32,
    pub player_glow_inside_value: u8,
    pub player_glow_outline_size: u8,
    pub glow_r_not: f32,
    pub glow_g_not: f32,
    pub glow_b_not: f32,
    pub glow_r_viz: f32,
    pub glow_g_viz: f32,
    pub glow_b_viz: f32,
    pub glow_r_knocked: f32,
    pub glow_g_knocked: f32,
    pub glow_b_knocked: f32,
    pub loot_filled: u8,
    pub loot_outline: u8,
    pub loot: Loot,
}

impl Default for EspVisuals {
    fn default() -> Self {
        Self {
            r#box: true,
            line: false,
            distance: true,
            health_bar: true,
            shield_bar: true,
            name: false,
        }
    }
}

impl Default for Loot {
    fn default() -> Self {
        Self {
            // rev skull
            skull: true,
            // Backpacks
            lightbackpack: false,
            medbackpack: true,
            heavybackpack: true,
            goldbackpack: true,
            // Shield upgrades
            shieldupgrade1: false, // white
            shieldupgrade2: false,  // blue
            shieldupgrade3: true,  // purple
            shieldupgrade4: true,  // gold
            shieldupgrade5: true,  // red
            shieldupgradehead1: false,
            shieldupgradehead2: false,
            shieldupgradehead3: false,
            shieldupgradehead4: true,
            shielddown1: false,
            shielddown2: false,
            shielddown3: false,
            shielddown4: true,
            // heaing and Misc
            accelerant: false,
            phoenix: false,
            healthlarge: true,
            healthsmall: false,
            shieldbattsmall: false,
            shieldbattlarge: true,
            mobile_respawn: true,
            // Ammo
            sniperammo: false,
            heavyammo: false,
            lightammo: false,
            energyammo: false,
            shotgunammo: false,
            // Optics
            optic1xhcog: false,
            optic2xhcog: true,
            opticholo1x: false,
            opticholo1x2x: false,
            opticthreat: false,
            optic3xhcog: false,
            optic2x4x: false,
            opticsniper6x: false,
            opticsniper4x8x: false,
            opticsniperthreat: false,
            // Magazines
            sniperammomag1: false,
            energyammomag1: false,
            lightammomag1: false,
            heavyammomag1: false,
            sniperammomag2: false,
            energyammomag2: false,
            lightammomag2: false,
            heavyammomag2: false,
            sniperammomag3: false,
            energyammomag3: false,
            lightammomag3: false,
            heavyammomag3: false,
            sniperammomag4: true,
            energyammomag4: true,
            lightammomag4: true,
            heavyammomag4: true,
            // Attachments
            lasersight1: false,
            lasersight2: false,
            lasersight3: false,
            lasersight4: true,
            stocksniper1: false,
            stocksniper2: false,
            stocksniper3: false,
            stocksniper4: true,
            stockregular1: false,
            stockregular2: false,
            stockregular3: false,
            suppressor1: false,
            suppressor2: false,
            suppressor3: false,
            turbo_charger: true,
            skull_piecer: true,
            hammer_point: false,
            disruptor_rounds: true,
            boosted_loader: false,
            anvil_receiver: false,
            doubletap_trigger: false,
            dual_shell: false,
            kinetic_feeder:false,
            quickdraw_holster:false,
            shotgunbolt1: false,
            shotgunbolt2: false,
            shotgunbolt3: false,
            shotgunbolt4: false,
            // Nades
            grenade_frag: false,
            grenade_arc_star: false,
            grenade_thermite: false,
            // Supply Drop Weapons
            weapon_kraber: true,
            weapon_bow: false,
            // Shotguns
            weapon_mastiff: false,
            weapon_eva8: false,
            weapon_peacekeeper: false,
            weapon_mozambique: false,
            // Energy weapons
            weapon_lstar: false,
            weapon_nemesis: false,
            weapon_havoc: false,
            weapon_devotion: false,
            weapon_triple_take: false,
            weapon_volt: false,
            // Heavy Weapons
            weapon_flatline: false,
            weapon_hemlock: false,
            weapon_3030_repeater: false,
            weapon_rampage: false,
            weapon_car_smg: false,
            weapon_prowler: false,
            // Light weapons
            weapon_p2020: false,
            weapon_re45: false,
            weapon_g7_scout: false,
            weapon_alternator: false,
            weapon_r99: false,
            weapon_spitfire: false,
            weapon_r301: false,
            // Snipers.. wingman is the odd one...
            weapon_wingman: false,
            weapon_longbow: false,
            weapon_charge_rifle: false,
            weapon_sentinel: false,
        }
    }
}

impl Default for Settings {
    fn default() -> Self {
        Self {
            // CONFIG AREA, you can change default values below.
            // Enable Loading of setting file automaticly.
            load_settings: true,
            no_overlay: true,
            screen_width: 1920,
            screen_height: 1080,
            yuan_p: false,
            debug_mode: false, //Used to close menu,show debug info
            // Gamepad or Keyboard config, Only one true at once or it wont work.
            keyboard: true,
            gamepad: false,
            aimbot_hot_key_1: 108,
            aimbot_hot_key_2: 79,
            // Done with Gamepad or Keyboard config
            trigger_bot_hot_key: 81,
            flick_bot_hot_key: 111,
            quickglow_hot_key: 93,
            quickaim_hot_key: 94,
            trigger_bot_shot: true,
            // Terminal Stuff
            loot_filled_toggle: false,
            player_filled_toggle: false,
            super_glide: true,
            super_grpple: true,
            auto_tapstrafe: true,
            // end Terminal Stuff
            onevone: false,
            tdm_toggle: false,
            item_glow: true,
            player_glow: true,
            player_glow_armor_color: false,
            player_glow_love_user: true,
            weapon_model_glow: true,
            kbd_backlight_control: false,
            bow_charge_rifle_aim: false,
            deathbox: true,
            aim_no_recoil: false,
            recoil_pitch: 50.0,
            recoil_yaw: 30.0,
            ads_fov: 7.0, // Fov you want to use while aiming
            non_ads_fov: 16.0,
            flick_fov: 30.0,
            aim: 2, // 0 no aim, 1 aim with no vis check, 2 aim with vis check
            esp: false,
            esp_visuals: EspVisuals::default(),
            mini_map_radar: true,
            mini_map_guides: true,
            mini_map_radar_dot_size1: 5,
            mini_map_radar_dot_size2: 1,
            main_radar_map: false, // if the Main Map Radar is enabled
            main_map_radar_dot_size1: 5,
            main_map_radar_dot_size2: 5,
            aim_dist: 200.0 * 40.0,
            max_dist: 3800.0 * 40.0, // Max Distance of ESP 3800 is full map
            map_radar_hotkey: 99,
            show_aim_target: true,
            game_fps: 143.0,       // Game FPS for aim prediction
            calc_game_fps: false, // Automatic calculation of game fps
            // aimbot for nades on or off
            no_nade_aim: true,  //Right click to nade-aim to true
            firing_range: false,
            bone: 2, // bone 0 head, 1 neck, 2 chest, 3 dick shot
            bone_nearest: false,
            bone_auto: true,
            flick_nearest: false,
            headshot_dist: 200.0 * 40.0,
            skynade_dist: 120.0 * 40.0,
            glow_dist: 200.0 * 40.0,
            smooth: 120.0, // min 85 no beaming, 100 somewhat beam people, 125 should be safe
            smooth_sub: 120.0,
            skynade_smooth: 120.0 * 1.2,
            // Player Glow Color and Brightness.
            // inside fill
            player_glow_inside_value: 14, // 0 = no fill, 14 = full fill
            player_glow_outline_size: 32, // 0-255
            // Not Visable
            glow_r_not: 0.0, // Red 0-1, higher is brighter color.
            glow_g_not: 1.0,
            glow_b_not: 0.0,
            // Visable
            glow_r_viz: 1.0,
            glow_g_viz: 0.0,
            glow_b_viz: 0.0,
            // Knocked
            glow_r_knocked: 0.80,
            glow_g_knocked: 0.78,
            glow_b_knocked: 0.45,

            // Item Configs
            // loot Fill
            loot_filled: 0, // 0 no fill, 14 100% fill
            loot_outline: 32,

            loot: Loot::default(),
        }
    }
}

impl Default for Config {
    fn default() -> Self {
        Self {
            settings: Default::default(),
            love_player: Default::default(),
        }
    }
}

pub fn get_config_path() -> PathBuf {
    let base_path = std::env::current_dir().expect("Failed to determine the current directory");
    let configuration_directory = base_path;
    configuration_directory.join("settings.toml")
}

pub fn get_configuration() -> Result<Config, config::ConfigError> {
    let settings = config::Config::builder()
        .add_source(config::Config::try_from::<Config>(&Config::default())?)
        .add_source(config::File::from(get_config_path()))
        .add_source(config::Environment::with_prefix("APP"))
        .build()?;

    settings.try_deserialize::<Config>()
}

pub fn save_configuration(config_state: Config) -> Result<(), std::io::Error> {
    use std::fs;
    use std::io::Write;

    let mut config_write = fs::OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open(get_config_path())?;
    let toml_con = toml::to_string(&config_state).unwrap();
    write!(config_write, "{}", toml_con)?;
    Ok(())
}
