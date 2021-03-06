#include <iostream>
#include <fstream>
#include <filesystem>

#include <cppcodec/base64_rfc4648.hpp>

#include "./Config.h"

#include "./Utility/File.h"

namespace fs = std::filesystem;
using base64 = cppcodec::base64_rfc4648;

Config::Config() {
	fs::path dir = GetCheatFolder();

	if (!fs::exists(dir))
		fs::create_directory(dir);

	fs::path file("config.json");
	fs::path config = dir / file;

	std::string path{ config.u8string() };

	if (!LoadFromFile(path.c_str()))
		DumpToFile(path.c_str());
}

bool Config::LoadFromFile(const char* fileName) {
	struct stat info;

	if (stat(fileName, &info) != 0) {
		return false;
	}
	else if (info.st_mode & S_IFDIR) {
		return false;
	}

	std::string src;

	if (!ReadFileToString(fileName, src))
		return false;

	std::vector<uint8_t> bsfDecoded = base64::decode(src);
	std::string bsfString(bsfDecoded.begin(), bsfDecoded.end());

	data = nlohmann::json::parse(bsfString);

	if (data["aimbot"]["enabled"].is_boolean()) aim.isEnabled = data["aimbot"]["enabled"].get<bool>();
	if (data["aimbot"]["friendly_fire"].is_boolean()) aim.friendlyFire = data["aimbot"]["friendly_fire"].get<bool>();
	if (data["aimbot"]["ignore_flash"].is_boolean()) aim.ignoreFlash = data["aimbot"]["ignore_flash"].get<bool>();
	if (data["aimbot"]["ignore_smoke"].is_boolean()) aim.ignoreSmoke = data["aimbot"]["ignore_smoke"].get<bool>();
	if (data["aimbot"]["silent_aim"].is_boolean()) aim.silentAim = data["aimbot"]["silent_aim"].get<bool>();
	if (data["aimbot"]["auto_scope"].is_boolean()) aim.autoScope = data["aimbot"]["auto_scope"].get<bool>();
	if (data["aimbot"]["auto_shoot"].is_boolean()) aim.autoShoot = data["aimbot"]["auto_shoot"].get<bool>();
	if (data["aimbot"]["fov"].is_number_float()) aim.fieldOfView = std::clamp(data["aimbot"]["fov"].get<float>(), 0.f, 10.f);
	if (data["aimbot"]["smooth"].is_number_float()) aim.smoothAmount = std::clamp(data["aimbot"]["smooth"].get<float>(), 1.f, 10.f);

	if (data["visuals"]["players"]["enabled"].is_boolean()) visuals.players.isEnabled = data["visuals"]["players"]["enabled"].get<bool>();
	if (data["visuals"]["players"]["outlined"].is_boolean()) visuals.players.isOutlined = data["visuals"]["players"]["outlined"].get<bool>();
	if (data["visuals"]["players"]["health"].is_boolean()) visuals.players.hasHealth = data["visuals"]["players"]["health"].get<bool>();
	if (data["visuals"]["players"]["armor"].is_boolean()) visuals.players.hasArmor = data["visuals"]["players"]["armor"].get<bool>();
	if (data["visuals"]["players"]["name"].is_boolean()) visuals.players.hasName = data["visuals"]["players"]["name"].get<bool>();
	if (data["visuals"]["players"]["distance"].is_boolean()) visuals.players.hasDistance = data["visuals"]["players"]["distance"].get<bool>();

	if (data["visuals"]["entities"]["bomb"].is_boolean()) visuals.entities.showBomb = data["visuals"]["entities"]["bomb"].get<bool>();
	if (data["visuals"]["entities"]["dropped_weapon"].is_boolean()) visuals.entities.showDroppedWeapons = data["visuals"]["entities"]["dropped_weapon"].get<bool>();

	if (data["visuals"]["chams"]["player_material"].is_number_integer()) visuals.chams.playerMaterial = data["visuals"]["chams"]["player_material"].get<int>();
	if (data["visuals"]["chams"]["weapon_material"].is_number_integer()) visuals.chams.weaponMaterial = data["visuals"]["chams"]["weapon_material"].get<int>();
	if (data["visuals"]["chams"]["arms_material"].is_number_integer()) visuals.chams.armsMaterial = data["visuals"]["chams"]["arms_material"].get<int>();

	if (data["visuals"]["glow"]["show_players"].is_boolean()) visuals.glow.showPlayers = data["visuals"]["glow"]["show_players"].get<bool>();
	if (data["visuals"]["glow"]["show_defuse_kits"].is_boolean()) visuals.glow.showDefuseKits = data["visuals"]["glow"]["show_defuse_kits"].get<bool>();
	if (data["visuals"]["glow"]["show_planted_c4"].is_boolean()) visuals.glow.showPlantedC4 = data["visuals"]["glow"]["show_planted_c4"].get<bool>();
	if (data["visuals"]["glow"]["show_dropped_weapons"].is_boolean()) visuals.glow.showDroppedWeapons = data["visuals"]["glow"]["show_dropped_weapons"].get<bool>();
	if (data["visuals"]["glow"]["show_chickens"].is_boolean()) visuals.glow.showChickens = data["visuals"]["glow"]["show_chickens"].get<bool>();
	if (data["visuals"]["glow"]["bloom_amount"].is_number_float()) visuals.glow.bloomAmount = std::clamp(data["visuals"]["glow"]["bloom_amount"].get<float>(), 0.f, 1.f);

	if (data["visuals"]["enemy_only"].is_boolean()) visuals.isOnlyEnemy = data["visuals"]["enemy_only"].get<bool>();
	if (data["visuals"]["on_death"].is_boolean()) visuals.isOnDeath = data["visuals"]["on_death"].get<bool>();
	if (data["visuals"]["snap_lines"].is_boolean()) visuals.hasSnapLines = data["visuals"]["snap_lines"].get<bool>();

	if (data["misc"]["bunny_hop"].is_boolean()) misc.bunnyHop = data["misc"]["bunny_hop"].get<bool>();

	return true;
}

void Config::DumpToFile(const char* fileName) {
	nlohmann::json od = {
		{"aimbot", {
			{"enabled", aim.isEnabled},
			{"friendly_fire", aim.friendlyFire},
			{"ignore_flash", aim.ignoreFlash},
			{"ignore_smoke", aim.ignoreSmoke},
			{"silent_aim", aim.silentAim},
			{"auto_scope", aim.autoScope},
			{"auto_shoot", aim.autoShoot},
			{"fov", aim.fieldOfView},
			{"smooth", aim.smoothAmount}
		}},
		{"visuals", {
			{"players", {
				{"enabled", visuals.players.isEnabled},
				{"outlined", visuals.players.isOutlined},
				{"health", visuals.players.hasHealth},
				{"armor", visuals.players.hasArmor},
				{"name", visuals.players.hasName},
				{"distance", visuals.players.hasDistance}
			}},
			{"entities", {
				{"bomb", visuals.entities.showBomb},
				{"dropped_weapon", visuals.entities.showDroppedWeapons}
			}},
			{"chams", {
				{"player_material", visuals.chams.playerMaterial},
				{"weapon_material", visuals.chams.weaponMaterial},
				{"arms_material", visuals.chams.armsMaterial}
			}},
			{"glow", {
				{"show_players", visuals.glow.showPlayers},
				{"show_defuse_kits", visuals.glow.showDefuseKits},
				{"show_planted_c4", visuals.glow.showPlantedC4},
				{"show_dropped_weapons", visuals.glow.showDroppedWeapons},
				{"show_chickens", visuals.glow.showChickens},
				{"bloom_amount", visuals.glow.bloomAmount}
			}},
			{"enemy_only", visuals.isOnlyEnemy},
			{"on_death", visuals.isOnDeath},
			{"snap_lines", visuals.hasSnapLines}
		}},
		{"misc", {
			{"bunny_hop", misc.bunnyHop}
		}}
	};

	std::ofstream out(fileName);
	out << base64::encode(od.dump());
	out.close();
}