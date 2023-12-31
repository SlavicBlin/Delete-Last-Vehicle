#pragma once
#include "backend/reactions/interloper_reaction.hpp"
#include "backend/reactions/reaction.hpp"
#include "enums.hpp"
#include "file_manager.hpp"

#include <bitset>
#include <rage/rlSessionInfo.hpp>
#include <weapon/CAmmoInfo.hpp>
#include <weapon/CWeaponInfo.hpp>
#include <weapon/CAmmoRocketInfo.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#define TYPING_TICKS 3

class CNetGamePlayer;
enum class eNetObjType;

namespace rage
{
	class scrThread;
	class scrProgram;
}

namespace big
{
	struct color
	{
		int r;
		int g;
		int b;
		int a;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(color, r, g, b, a)
	};

	struct remote_player_teleport
	{
		std::int16_t m_player_net_id;
		rage::fvector3 m_position;
	};

	class menu_settings
	{
	public:
		void init(const file& save_file);

		void attempt_save();
		bool load();
		bool write_default_config();

	private:
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();

	private:
		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		int friend_count = 0;
		int player_count = 0;

		CNetGamePlayer* m_syncing_player  = nullptr;
		eNetObjType m_syncing_object_type = (eNetObjType)-1;

		int m_remote_controller_vehicle = -1;
		int m_remote_controlled_vehicle = -1;

		std::unordered_map<uint16_t, remote_player_teleport> m_remote_player_teleports;

		rage::scrThread* m_hunt_the_beast_thread = nullptr;

		rage::scrThread* m_dance_thread   = nullptr;
		rage::scrProgram* m_dance_program = nullptr;

		rage::scrThread* m_mission_creator_thread = nullptr;
;

		rage::scrThread* m_modshop_thread = nullptr;
		bool in_script_vm                 = false;

		struct debug
		{
			struct logs
			{
				bool metric_logs{};
				bool packet_logs{};

				bool stupid_script_native_logs{};

				bool net_event_logs{};

				bool remote_sound_logs{};

				struct script_event
				{
					bool logs = false;

					bool filter_player    = true;
					std::int8_t player_id = -1;

					bool block_all = false; //should not save

					NLOHMANN_DEFINE_TYPE_INTRUSIVE(script_event, logs, filter_player, player_id)
				} script_event{};

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(logs, metric_logs, stupid_script_native_logs, packet_logs, net_event_logs, remote_sound_logs, script_event)
			} logs{};

			struct fuzzer
			{
				bool enabled = false;
				bool enabled_object_types[14];
				bool active                    = false;
				int thread_id                  = 0;
				std::int16_t syncing_object_id = -1;
			} fuzzer{};
			bool window_hook = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(debug, logs, window_hook)
		} debug{};

		struct tunables
		{
			bool disable_phone           = false;
			bool phone_anim              = false;
			bool no_idle_kick            = false;
			bool dont_unload_online_maps = true;
			bool cable_cars              = false;
			bool always_control          = false;
			bool disable_help_text       = false;
			bool remove_speed_limit      = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(tunables, disable_phone, phone_anim, no_idle_kick, dont_unload_online_maps, cable_cars, always_control, disable_help_text, remove_speed_limit)
		} tunables{};

		struct notifications
		{
			struct pair
			{
				bool log    = false;
				bool notify = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(pair, log, notify)
			};

			pair gta_thread_kill{};
			pair gta_thread_start{};

			pair network_player_mgr_init{};
			pair network_player_mgr_shutdown{};

			struct player_join
			{
				bool above_map = true;
				bool log       = false;
				bool notify    = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(player_join, above_map, log, notify)
			} player_join{};

			pair player_leave{};

			pair send_net_info_to_lobby{};
			pair transaction_rate_limit{};
			pair mismatch_sync_type{};
			pair out_of_allowed_range_sync_type{};
			pair invalid_sync{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(notifications, gta_thread_kill, gta_thread_start, network_player_mgr_init, network_player_mgr_shutdown, player_join, player_leave, send_net_info_to_lobby, transaction_rate_limit, mismatch_sync_type, out_of_allowed_range_sync_type, invalid_sync)
		} notifications{};

		struct reactions
		{
			// first constructor param is an internal identifier for the event
			// it's never shown in the UI
			reaction bounty{"Bounty", "Blocked Bounty from {}", "{} tried to set a bounty on me!"};
			reaction ceo_kick{"CEO Kick", "Blocked CEO Kick from {}", "{} tried to kick me from my CEO!"};
			reaction ceo_money{"CEO Money", "Blocked CEO Money from {}", "{} tried to drop money on me!"};
			reaction clear_wanted_level{"Clear Wanted Level", "Blocked Clear Wanted Level from {}", "{} tried to clear my wanted level!"};
			reaction crash{"Crash", "Blocked Crash from {}", "{} tried to crash me!"};
			reaction end_session_kick{"End Session Kick", "Blocked End Session Kick from {}", "{} tried to kick me out!"};
			reaction fake_deposit{"Fake Deposit", "Blocked Fake Deposit from {}", "{} tried to show me a fake money notification!"};
			reaction force_mission{"Force Mission", "Blocked Force Mission from {}", "{} tried to force me into a mission!"};
			reaction force_teleport{"Force Teleport", "Blocked Force Teleport from {}", "{} tried to teleport me!"};
			reaction gta_banner{"GTA Banner", "Blocked GTA Banner from {}", "Blocked GTA Banner from {}"}; // please don't enable this
			reaction kick_from_interior{"Kick From Interior", "Blocked Kick From Interior from {}", "{} tried to kick me from my interior!"};
			reaction mc_teleport{"MC Teleport", "Blocked MC Teleport from {}", "{} tried to teleport me!"};
			reaction network_bail{"Network Bail", "Blocked Network Bail from {}", "{} tried to kick me out!"};
			reaction personal_vehicle_destroyed{"Personal Vehicle Destroyed", "Blocked Personal Vehicle Destroyed from {}", "{} tried to show me a fake insurance notification!"};
			reaction remote_off_radar{"Remote Off Radar", "Blocked Remote Off Radar from {}", "{} tried to give me off radar!"};
			reaction rotate_cam{"Rotate Cam", "Blocked Rotate Cam from {}", "{} tried to mess with my camera!"};
			reaction send_to_cutscene{"Send To Cutscene", "Blocked Send To Cutscene from {}", "{} tried to force me into a cutscene!"};
			reaction send_to_location{"Send To Location", "Blocked Send To Location from {}", "{} tried to send me to Cayo Perico!"};
			reaction send_to_interior{"Send To Interior", "Blocked Send To Interior from {}", "{} tried to send me to an interior!"};
			reaction sound_spam{"Sound Spam", "Blocked Sound Spam from {}", "{} tried to spam annoying sounds at me!"};
			reaction spectate_notification{"Spectate Notification", "Blocked Spectate Notification from {}", "Blocked Spectate Notification from {}"};
			reaction give_collectible{"Give Collectible", "Blocked Give Collectible from {}", "{} tried to give me a collectible!"};
			reaction transaction_error{"Transaction Error", "Blocked Transaction Error from {}", "{} tried to show me a transaction error!"};
			reaction tse_freeze{"TSE Freeze", "Blocked TSE Freeze from {}", "{} tried to softlock my game!"};
			reaction tse_sender_mismatch{"TSE Sender Mismatch", "Blocked TSE Sender Mismatch from {}", "Blocked TSE Sender Mismatch from {}"};
			reaction vehicle_kick{"Vehicle Kick", "Blocked Vehicle Kick from {}", "{} tried to kick me from my vehicle!"};
			reaction teleport_to_warehouse{"Teleport To Warehouse", "Blocked Teleport To Warehouse from {}", "{} tried to teleport me to a warehouse!"};
			reaction start_activity{"Start Activity", "Blocked Start Activity from {}", "Blocked Start Activity from {}"};
			reaction start_script{"Start Script", "Blocked Start Script from {}", "Blocked Start Script from {}"};
			reaction null_function_kick{"Null Function Kick", "Blocked Null Function Kick from {}", "{} tried to kick me out!"};
			reaction destroy_personal_vehicle{"Destroy Personal Vehicle", "Blocked Destroy Personal Vehicle from {}", "{} tried to destroy my personal vehicle!"};
			reaction trigger_business_raid{"Trigger Business Raid", "Blocked Trigger Business Raid from {}", "{} tried to trigger a business raid!"};
			reaction turn_into_beast{"Turn Into Beast", "Blocked Turn Into Beast from {}", "{} tried to turn me into the beast!"};
			reaction remote_wanted_level{"Remote Wanted Level", "Blocked Remote Wanted Level from {}", "{} tried to give me a wanted level!"};
			interloper_reaction remote_wanted_level_others{"Remote Wanted Level On Other Players", "{} is attempting to give a wanted level to {}!", "{} is attempting to give a wanted level to {}!", false, false};

			reaction clear_ped_tasks{"Clear Ped Tasks", "Blocked Clear Ped Tasks from {}", "{} tried to freeze me!"};
			reaction remote_ragdoll{"Remote Ragdoll", "Blocked Remote Ragdoll from {}", "{} tried to ragdoll me!"};
			reaction kick_vote{"Kick Vote", "{} is voting to kick you!", "{} is voting to kick me!"};
			reaction report_cash_spawn{"Cash Spawn", "{} is spawning cash!", "{} is spawning cash!"};
			reaction modder_detection{"Modder Detection", "{} is detected as a modder!", "{} is detected as a modder!"};
			reaction game_anti_cheat_modder_detection{"Game Anti-Cheat Modder Detection", "{} is detected as a modder by the game anti-cheat!", "{} is detected as a modder by the game anti-cheat!"};
			reaction request_control_event{"Request Control Event", "Blocked Request Control Event from {}", "{} tried to mess with my vehicle!"};
			reaction report{"Report", "Blocked Report from {}", "{} tried to report me!"};
			reaction spectate{"Spectate", "{} is spectating you", "{} is spectating me!"};
			interloper_reaction spectate_others{"Spectate Others", "{} is spectating {}!", "{} is spectating {}!", false, false};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(reactions, bounty, ceo_money, ceo_kick, clear_wanted_level, crash, end_session_kick, fake_deposit, force_mission, force_teleport, gta_banner, kick_from_interior, mc_teleport, network_bail, personal_vehicle_destroyed, remote_off_radar, rotate_cam, send_to_cutscene, send_to_location, sound_spam, spectate_notification, give_collectible, transaction_error, tse_freeze, tse_sender_mismatch, vehicle_kick, teleport_to_warehouse, trigger_business_raid, start_activity, start_script, null_function_kick, destroy_personal_vehicle, clear_ped_tasks, turn_into_beast, remote_wanted_level, remote_wanted_level_others, remote_ragdoll, kick_vote, report_cash_spawn, modder_detection, game_anti_cheat_modder_detection, request_control_event, report, send_to_interior, spectate, spectate_others)
		} reactions{};

		struct player
		{
			bool spectating            = false;
			bool override_cam_distance = false;
			int cam_distance           = 10;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(player, spectating, override_cam_distance, cam_distance)
		} player{};

		struct player_db
		{
			bool update_player_online_states   = true;
			bool notify_when_online            = false;
			bool notify_when_joinable          = true;
			bool notify_when_unjoinable        = false;
			bool notify_when_offline           = false;
			bool notify_on_session_type_change = false;
			bool notify_on_session_change      = false;
			bool notify_on_spectator_change    = false;
			bool notify_on_become_host         = false;
			bool notify_on_transition_change   = false;
			bool notify_on_mission_change      = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(player_db, update_player_online_states, notify_when_online, notify_when_joinable, notify_when_unjoinable, notify_when_offline, notify_on_session_type_change, notify_on_session_change, notify_on_spectator_change, notify_on_become_host, notify_on_transition_change, notify_on_mission_change)
		} player_db{};

		struct protections
		{
			struct script_events
			{
				bool bounty                     = false;
				bool ceo_money                  = false;
				bool clear_wanted_level         = true;
				bool force_mission              = true;
				bool force_teleport             = true;
				bool gta_banner                 = false;
				bool mc_teleport                = true;
				bool personal_vehicle_destroyed = true;
				bool remote_off_radar           = false;
				bool rotate_cam                 = true;
				bool send_to_cutscene           = true;
				bool send_to_location           = true;
				bool sound_spam                 = false;
				bool spectate                   = false;
				bool give_collectible           = true;
				bool vehicle_kick               = false;
				bool teleport_to_warehouse      = true;
				bool start_activity             = true;
				bool send_sms                   = true;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(script_events, bounty, ceo_money, clear_wanted_level, force_mission, force_teleport, gta_banner, mc_teleport, personal_vehicle_destroyed, remote_off_radar, rotate_cam, send_to_cutscene, send_to_location, sound_spam, spectate, give_collectible, vehicle_kick, teleport_to_warehouse, start_activity, send_sms)
			} script_events{};

			bool rid_join                = false;
			bool receive_pickup          = false;
			bool request_control         = true;
			bool admin_check             = true;
			bool kick_rejoin             = true;
			bool force_relay_connections = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(protections, script_events, rid_join, receive_pickup, request_control, admin_check, kick_rejoin, force_relay_connections)
		} protections{};

		struct self
		{
			struct ipls
			{
				int select = 0;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(ipls, select)
			} ipls{};

			bool allow_ragdoll                = false;
			bool clean_player                 = false;
			bool force_wanted_level           = false;
			bool passive                      = false;
			bool free_cam                     = false;
			bool invisibility                 = false;
			bool local_visibility             = true;
			bool never_wanted                 = false;
			bool no_ragdoll                   = false;
			bool noclip                       = false;
			float noclip_aim_speed_multiplier = 0.25f;
			float noclip_speed_multiplier     = 20.f;
			bool off_radar                    = false;
			bool ghost_org                    = false;
			bool super_run                    = false;
			bool no_collision                 = false;
			bool unlimited_oxygen             = false;
			bool no_water_collision           = false;
			int wanted_level                  = 0;
			bool god_mode                     = false;
			bool part_water                   = false;
			bool proof_bullet                 = false;
			bool proof_fire                   = false;
			bool proof_collision              = false;
			bool proof_melee                  = false;
			bool proof_explosion              = false;
			bool proof_steam                  = false;
			bool proof_drown                  = false;
			bool proof_water                  = false;
			uint32_t proof_mask               = 0;
			bool mobile_radio                 = false;
			bool fast_respawn                 = false;
			bool auto_tp                      = false;
			bool custom_weapon_stop           = true;
			bool prompt_ambient_animations    = false;
			std::string persist_outfit        = "";
			bool persist_outfits_mis          = false;

			struct hud
			{
				bool hide_radar                            = false;
				bool hide_ammo                             = false;
				int selected_hud_component                 = 1;
				std::array<bool, 22> hud_components_states = {false};
				bool force_show_hud_element                = false;
				bool force_show_hud                        = false;
				std::atomic_uint_fast8_t typing            = 0;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(hud, hide_radar, hide_ammo, selected_hud_component, hud_components_states, force_show_hud_element, force_show_hud)
			} hud{};
			// do not save below entries
			bool dance_mode = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(self, ipls, allow_ragdoll, clean_player, force_wanted_level, passive, free_cam, invisibility, local_visibility, never_wanted, no_ragdoll, noclip, noclip_aim_speed_multiplier, noclip_speed_multiplier, off_radar, super_run, no_collision, unlimited_oxygen, no_water_collision, wanted_level, god_mode, part_water, proof_bullet, proof_fire, proof_collision, proof_melee, proof_explosion, proof_steam, proof_drown, proof_water, proof_mask, mobile_radio, fast_respawn, auto_tp, hud, custom_weapon_stop, prompt_ambient_animations, persist_outfit, persist_outfits_mis)
		} self{};

		struct session
		{
			struct population_control
			{
				bool ped_enable     = false;
				int ped             = 3;
				bool vehicle_enable = false;
				int vehicle         = 3;
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(population_control, ped_enable, ped, vehicle_enable, vehicle)
			} population_control;

			bool log_chat_messages                 = false;
			bool decloak_players                   = false;
			bool unhide_players_from_player_list   = true;
			bool force_session_host                = false;
			bool force_script_host                 = false;
			bool player_magnet_enabled             = false;
			int player_magnet_count                = 32;
			bool is_team                           = false;
			bool join_in_sctv_slots                = false;
			bool lock_session                      = false;
			bool allow_friends_into_locked_session = false;
			bool trust_friends                     = false;
			bool trust_session                     = false;

			const char chat_command_prefix = '/';
			const char chat_output_prefix  = '>';

			bool chat_commands                                   = false;
			CommandAccessLevel chat_command_default_access_level = CommandAccessLevel::NONE;

			bool kick_chat_spammers          = false;
			bool kick_host_when_forcing_host = false;

			bool explosion_karma = false;
			bool damage_karma    = false;

			bool disable_traffic = false;
			bool disable_peds    = false;
			bool force_thunder   = false;

			bool block_ceo_money      = false;
			bool randomize_ceo_colors = false;
			bool block_jobs           = false;
			bool block_muggers        = false;
			bool block_ceo_raids      = false;

			int send_to_apartment_idx = 1;
			int send_to_warehouse_idx = 1;

			// not to be saved
			bool join_queued = false;
			rage::rlSessionInfo info;
			bool never_wanted_all = false;
			bool off_radar_all    = false;
			bool semi_godmode_all = false;
			bool wanted_level_all = false;
			bool vehicle_fix_all  = false;

			bool show_cheating_message = false;
			bool anonymous_bounty      = true;

			bool fast_join = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(session, population_control, log_chat_messages, decloak_players, force_session_host, force_script_host, player_magnet_enabled, player_magnet_count, is_team, join_in_sctv_slots, kick_chat_spammers, kick_host_when_forcing_host, explosion_karma, damage_karma, disable_traffic, disable_peds, force_thunder, block_ceo_money, randomize_ceo_colors, block_jobs, block_muggers, block_ceo_raids, send_to_apartment_idx, send_to_warehouse_idx, chat_commands, chat_command_default_access_level, show_cheating_message, anonymous_bounty, lock_session, fast_join, unhide_players_from_player_list, allow_friends_into_locked_session, trust_friends)
		} session{};

		struct settings
		{
			bool dev_dlc = false;

			struct hotkeys
			{
				bool editing_menu_toggle = false;
				int menu_toggle          = VK_INSERT;
				std::atomic<bool> is_mp_chat_active;
				int teleport_waypoint    = 0;
				int teleport_objective   = 0;
				int teleport_pv          = 0;
				int teleport_selected    = 0;
				int noclip               = 0;
				int vehicle_flymode      = 0;
				int bringvehicle         = 0;
				int invis                = 0;
				int heal                 = 0;
				int fill_inventory       = 0;
				int skip_cutscene        = 0;
				int freecam              = 0;
				int superrun             = 0;
				int passive              = 0;
				int invisveh             = 0;
				int localinvisveh        = 0;
				int fill_ammo            = 0;
				int fast_quit            = 0;
				int clear_wanted         = 0;
				int cmd_excecutor        = 'U';

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(hotkeys, editing_menu_toggle, menu_toggle, teleport_waypoint, teleport_objective, teleport_pv, teleport_selected, noclip, vehicle_flymode, bringvehicle, invis, heal, fill_inventory, skip_cutscene, freecam, superrun, passive, invisveh, localinvisveh, fill_ammo, fast_quit, clear_wanted, cmd_excecutor)
			} hotkeys{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(settings, hotkeys, dev_dlc)
		} settings{};

		struct spawn_vehicle
		{
			bool preview_vehicle = false;
			bool spawn_inside    = false;
			bool spawn_maxed     = false;
			bool spawn_last      = false;
			std::string plate    = "";
			int spawn_type       = 0;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(spawn_vehicle, preview_vehicle, spawn_inside, spawn_last, spawn_maxed, plate, spawn_type)
		} spawn_vehicle{};
		 

		struct clone_pv
		{
			bool preview_vehicle = false;
			bool spawn_inside    = false;
			bool spawn_clone     = false;
			bool spawn_maxed     = false;
			bool clone_plate     = false;
			std::string plate    = "";
			std::string garage   = "";

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(clone_pv, preview_vehicle, spawn_inside, spawn_clone, spawn_maxed, clone_plate, plate, garage)
		} clone_pv{};

		struct persist_car
		{
			bool preview_vehicle                   = false;
			bool spawn_inside                      = false;
			std::string persist_vehicle_sub_folder = "";

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(persist_car, preview_vehicle, spawn_inside, persist_vehicle_sub_folder)
		} persist_car{};

		struct world
		{
			struct train
			{
				bool drive_train  = false;
				bool derail_train = false;
			} train{};

			struct water
			{
				bool part_water = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(water, part_water)
			} water{};

			struct gravity
			{
				bool modify_gravity   = false;
				float current_gravity = 9.8f;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(gravity, modify_gravity, current_gravity)
			} gravity{};

			struct ocean
			{
				bool modify_ocean   = false;
				bool disable_ocean	= false;
				int ocean_opacity	= 100;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(ocean, modify_ocean, disable_ocean, ocean_opacity)
			} ocean{};

			struct waypoint_n_objective
			{
				bool waypoint_beacon            = false;
				bool objective_beacon           = false;
				float waypoint_beacon_color[3]  = {1, 0, 1};
				float objective_beacon_color[3] = {1, 1, 0};

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(waypoint_n_objective, waypoint_beacon, objective_beacon, objective_beacon_color, waypoint_beacon_color)
			} waypoint_n_objective{};

			struct spawn_ped
			{
				bool preview_ped       = false;
				bool spawn_invincible  = false;
				bool spawn_invisible   = false;
				bool spawn_as_attacker = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(spawn_ped, preview_ped, spawn_invincible, spawn_invisible, spawn_as_attacker)
			} spawn_ped{};

			struct custom_time
			{
				bool override_time = {};
				int hour{}, minute{}, second{};

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(custom_time, override_time, hour, minute, second)
			} custom_time;


			struct blackhole
			{
				bool enable           = false;
				bool include_peds     = false;
				bool include_vehicles = false;
				float scale           = 6.f;
				float color[3]        = {1, 1, 1};
				int alpha             = 150;
				rage::fvector3 pos;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(blackhole, include_peds, include_vehicles, color, alpha)
			} blackhole{};

			struct nearby
			{
				bool ignore     = false;
				bool ped_rain   = false;
				bool veh_rain   = false;
				bool high_alert = false;
				bool ped_rush   = false;
				bool combative  = false;

				struct auto_disarm
				{
					bool enable     = false;
					bool neutralize = false;
					NLOHMANN_DEFINE_TYPE_INTRUSIVE(auto_disarm, enable, neutralize)
				} auto_disarm{};

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(nearby, ignore, ped_rain, veh_rain, high_alert, ped_rush, combative, auto_disarm)
			} nearby{};

			struct model_swapper
			{
				std::vector<std::pair<std::string, std::string>> models;
				std::mutex m;
				bool update = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(model_swapper, models)
			} model_swapper{};

			bool override_weather = false;
			int local_weather     = 0;

			bool blackout    = false;
			bool ground_snow = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(world, water, spawn_ped, custom_time, blackhole, model_swapper, nearby, local_weather, override_weather, blackout, ground_snow)
		} world{};

		struct spoofing
		{
			int pool_type              = 0;
			bool hide_from_player_list = false;

			bool spoof_blip = false;
			int blip_type   = 0; // actual blip type + 1

			bool spoof_rank = false;
			int rank        = 1;

			bool spoof_job_points = false;
			int job_points        = 1;

			bool spoof_kd_ratio = false;
			float kd_ratio      = 2.0f;

			bool spoof_bad_sport = false;
			int badsport_type    = 0;

			bool spoof_player_model   = false;
			std::string player_model  = "";
			std::string player_outfit = "";

			bool spoof_hide_god      = false;
			bool spoof_hide_spectate = true;

			bool spoof_crew_data = false;
			std::string crew_tag = "";
			bool rockstar_crew   = false;
			bool square_crew_tag = false;

			bool spoof_session_region_type      = false;
			int session_region_type             = 0;
			bool spoof_session_language         = false;
			int session_language                = 0;
			bool spoof_session_player_count     = false;
			int session_player_count            = 25;
			bool spoof_session_bad_sport_status = false;
			bool session_bad_sport              = false;

			bool override_game_hashes = false;
			std::string game_checksum_data_b64 = "";
			int game_dlc_checksum = -1;
			int last_game_version = -1;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(spoofing, hide_from_player_list, pool_type, spoof_blip, blip_type, spoof_rank, rank, spoof_job_points, job_points, spoof_kd_ratio, kd_ratio, spoof_bad_sport, badsport_type, spoof_player_model, player_model, spoof_hide_god, spoof_hide_spectate, spoof_crew_data, crew_tag, rockstar_crew, square_crew_tag, spoof_session_region_type, session_region_type, spoof_session_language, session_language, spoof_session_player_count, session_player_count, spoof_session_bad_sport_status, session_bad_sport, override_game_hashes, game_checksum_data_b64, game_dlc_checksum, last_game_version)
		} spoofing{};

		struct vehicle
		{
			struct speedo_meter
			{
				float x = .9f;
				float y = .72f;

				bool enabled           = false;
				bool left_side         = false;
				bool show_current_gear = true;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(speedo_meter, x, y, enabled, left_side, show_current_gear)
			} speedo_meter{};

			struct fly
			{
				bool dont_stop    = false;
				bool enabled      = false;
				bool no_collision = false;
				bool stop_on_exit = false;
				float speed       = 100;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(fly, dont_stop, enabled, no_collision, stop_on_exit, speed)
			} fly{};

			struct rainbow_paint
			{
				RainbowPaintType type = RainbowPaintType::Off;
				bool neon             = false;
				bool primary          = false;
				bool secondary        = false;
				bool smoke            = false;
				int speed             = 1;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(rainbow_paint, type, neon, primary, secondary, smoke, speed)
			} rainbow_paint{};

			SpeedUnit speed_unit = SpeedUnit::MIPH;

			bool god_mode        = false;
			bool proof_bullet    = false;
			bool proof_fire      = false;
			bool proof_collision = false;
			bool proof_melee     = false;
			bool proof_explosion = false;
			bool proof_steam     = false;
			bool proof_water     = false;
			uint32_t proof_mask  = 0;

			AutoDriveDestination auto_drive_destination = AutoDriveDestination::STOPPED;
			AutoDriveStyle auto_drive_style             = AutoDriveStyle::LAW_ABIDING;
			float auto_drive_speed                      = 1;
			bool auto_turn_signals                      = false;
			eBoostBehaviors boost_behavior              = eBoostBehaviors::DEFAULT;
			bool drive_on_water                         = false;
			bool horn_boost                             = false;
			bool instant_brake                          = false;
			bool block_homing                           = false;
			bool bypass_speed                           = false;
			bool ls_customs                             = false; // don't save this to disk
			bool seatbelt                               = false;
			bool turn_signals                           = false;
			bool vehicle_jump                           = false;
			bool keep_vehicle_repaired                  = false;
			bool no_water_collision                     = false;
			bool disable_engine_auto_start              = false;
			bool change_engine_state_immediately        = false;
			bool keep_engine_running                    = false;
			bool keep_vehicle_clean                     = false;
			bool vehinvisibility                        = false;
			bool localveh_visibility                    = false;
			bool keep_on_ground                         = false;
			bool no_collision                           = false;
			bool unlimited_weapons                      = false;
			bool siren_mute                             = false;
			bool all_vehs_in_heists                     = false;

			struct vehicle_ammo_special
			{
				bool enabled                       = false;
				eAmmoSpecialType type              = eAmmoSpecialType::None;
				eExplosionTag explosion_tag        = eExplosionTag::EXP_TAG_ROGUE_CANNON;
				float speed                        = 2000;
				float time_between_shots           = 0.04;
				float alternate_wait_time          = -1;
				float weapon_range                 = 250;
				float rocket_time_between_shots    = 0.66;
				float rocket_alternate_wait_time   = 0.66;
				float rocket_lock_on_range         = 500;
				float rocket_range                 = 1000;
				float rocket_reload_time           = -1;
				eExplosionTag rocket_explosion_tag = eExplosionTag::TANKSHELL;
				float rocket_lifetime              = 15;
				float rocket_launch_speed          = 1200;
				float rocket_time_before_homing    = 0.75;
				bool rocket_improve_tracking       = true;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(vehicle_ammo_special, enabled, type, explosion_tag, speed, time_between_shots, alternate_wait_time, weapon_range, rocket_time_between_shots, rocket_alternate_wait_time, rocket_lock_on_range, rocket_range, rocket_reload_time, rocket_explosion_tag, rocket_lifetime, rocket_launch_speed, rocket_time_before_homing, rocket_improve_tracking)
			} vehicle_ammo_special{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(vehicle, speedo_meter, fly, rainbow_paint, speed_unit, god_mode, proof_bullet, proof_fire, proof_collision, proof_melee, proof_explosion, proof_steam, proof_water, proof_mask, auto_drive_destination, auto_drive_style, auto_drive_speed, auto_turn_signals, boost_behavior, drive_on_water, horn_boost, instant_brake, block_homing, bypass_speed, seatbelt, turn_signals, vehicle_jump, keep_vehicle_repaired, no_water_collision, disable_engine_auto_start, change_engine_state_immediately, keep_engine_running, keep_vehicle_clean, vehinvisibility, localveh_visibility, keep_on_ground, no_collision, unlimited_weapons, siren_mute, all_vehs_in_heists, vehicle_ammo_special)
		} vehicle{};

		struct weapons
		{
			struct ammo_special
			{
				bool toggle                 = false;
				eAmmoSpecialType type       = eAmmoSpecialType::None;
				eExplosionTag explosion_tag = eExplosionTag::DONTCARE;

				NLOHMANN_JSON_SERIALIZE_ENUM(eAmmoSpecialType,
				    {
				        {eAmmoSpecialType::None, "none"},
				        {eAmmoSpecialType::ArmorPiercing, "armor_piercing"},
				        {eAmmoSpecialType::Explosive, "explosive"},
				        {eAmmoSpecialType::FMJ, "fmj"},
				        {eAmmoSpecialType::HollowPoint, "hollow_point"},
				        {eAmmoSpecialType::Incendiary, "incendiary"},
				        {eAmmoSpecialType::Tracer, "tracer"},
				    })
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(ammo_special, toggle, type, explosion_tag)
			} ammo_special{};

			struct gravity_gun
			{
				bool launch_on_release = false;
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(gravity_gun, launch_on_release)
			} gravity_gun{};

			struct paintgun
			{
				bool rainbow = false;
				float speed  = 1.f;
				float col[4] = {0.f, 0.f, 1.f, 1.f};
				NLOHMANN_DEFINE_TYPE_INTRUSIVE(paintgun, rainbow, col)
			} paintgun{};

			CustomWeapon custom_weapon    = CustomWeapon::NONE;
			bool infinite_ammo            = false;
			bool always_full_ammo         = false;
			bool infinite_mag             = false;
			float increased_damage        = 1;
			bool increase_damage          = false;
			bool no_recoil                = false;
			bool no_spread                = false;
			std::string vehicle_gun_model = "bus";
			bool increased_c4_limit       = false;
			bool increased_flare_limit    = false;
			bool rapid_fire               = false;
			bool interior_weapon          = false;
			bool infinite_range           = false;
			bool enable_weapon_hotkeys    = false;
			std::map<int, std::vector<uint32_t>> weapon_hotkeys{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(weapons, ammo_special, custom_weapon, infinite_ammo, always_full_ammo, infinite_mag, increased_damage, increase_damage, no_recoil, no_spread, vehicle_gun_model, increased_c4_limit, increased_flare_limit, rapid_fire, gravity_gun, paintgun, interior_weapon, infinite_range, enable_weapon_hotkeys, weapon_hotkeys)
		} weapons{};

		struct window
		{
			bool about  = false;
			bool main   = true;
			bool users  = true;
			bool player = true;
			bool demo   = false;

			ImU32 background_color = 3357612055;
			ImU32 text_color       = 4294967295;
			ImU32 button_color     = 4293353517;
			ImU32 frame_color      = 2939499829;
			float gui_scale        = 1.f;

			ImFont* font_title     = nullptr;
			ImFont* font_sub_title = nullptr;
			ImFont* font_small     = nullptr;
			ImFont* font_icon      = nullptr;

			struct ingame_overlay
			{
				bool opened                = true;
				bool show_with_menu_opened = true;
				int corner                 = 0;

				bool show_watermark        = false;
				bool show_fps              = true;
				bool show_position         = true;
				bool show_indicators       = true;
				bool show_players          = true;
				bool show_replay_interface = false;
				bool show_game_versions    = false;

				struct ingame_overlay_indicators
				{
					bool show_player_godmode   = true;
					bool show_off_radar        = true;
					bool show_vehicle_godmode  = true;
					bool show_never_wanted     = true;
					bool show_infinite_ammo    = false;
					bool show_always_full_ammo = false;
					bool show_infinite_mag     = false;
					bool show_invisibility     = false;

					NLOHMANN_DEFINE_TYPE_INTRUSIVE(ingame_overlay_indicators, show_player_godmode, show_off_radar, show_vehicle_godmode, show_never_wanted, show_always_full_ammo, show_infinite_ammo, show_infinite_mag, show_invisibility)
				} ingame_overlay_indicators{};

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(ingame_overlay, ingame_overlay_indicators, opened, corner, show_watermark, show_with_menu_opened, show_fps, show_indicators, show_players, show_replay_interface, show_position, show_game_versions)
			} ingame_overlay{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(window, background_color, main, users, player, ingame_overlay, text_color, button_color, frame_color, demo, gui_scale)
		} window{};

		struct context_menu
		{
			bool enabled = false;

			uint8_t allowed_entity_types = static_cast<uint8_t>(ContextEntityType::PED) | static_cast<uint8_t>(ContextEntityType::PLAYER) | static_cast<uint8_t>(ContextEntityType::VEHICLE) | static_cast<uint8_t>(ContextEntityType::OBJECT);

			ImU32 selected_option_color = 4278255360;

			bool bounding_box_enabled = true;
			ImU32 bounding_box_color  = 4278255360;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(context_menu, enabled, allowed_entity_types, selected_option_color, bounding_box_enabled, bounding_box_color)
		} context_menu{};

		struct chat
		{
			bool local       = false;
			bool auto_scroll = true;
			bool show_spam   = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(chat, local, auto_scroll, show_spam)
		} chat{};

		struct outfit_editor
		{
			int componentId = 0;
			int drawableId  = 0;
			int textureId   = 0;
			int paletteId   = 0;

			int propId         = 0;
			int propDrawableId = 0;
			int propTextureId  = 0;
		} outfit_editor{};

		struct esp
		{
			bool enabled                    = true;
			float global_render_distance[2] = {0.f, 600.f};
			float tracer_render_distance[2] = {200.f, 600.f};
			float box_render_distance[2]    = {0.f, 150.f};
			bool tracer                     = true;
			float tracer_draw_position[2]   = {0.5f, 1.f};
			bool box                        = true;
			bool health                     = true;
			bool armor                      = true;
			bool god                        = true;
			bool distance                   = true;
			bool name                       = true;
			bool change_esp_color_from_dist = false;
			bool scale_health_from_dist     = false;
			bool scale_armor_from_dist      = false;
			bool object_esp                 = false;
			bool show_gs_cache_boxes        = false;
			float distance_threshold[2]     = {100.f, 200.f};
			ImU32 enemy_color               = 4281479904;
			ImU32 enemy_near_color          = 4283794943;
			ImU32 default_color             = 4285713522;
			ImU32 friend_color              = 4293244509;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(esp, enabled, global_render_distance, tracer_render_distance, box_render_distance, tracer, tracer_draw_position, box, health, armor, god, distance, name, change_esp_color_from_dist, scale_health_from_dist, scale_armor_from_dist, object_esp, show_gs_cache_boxes, distance_threshold, enemy_color, enemy_near_color, default_color, friend_color)
		} esp{};

		struct session_browser
		{
			bool region_filter_enabled = true;
			int region_filter          = 0;

			bool language_filter_enabled = false;
			int language_filter          = 0;

			bool pool_filter_enabled = false;
			int pool_filter          = 0;

			bool player_count_filter_enabled = false;
			int player_count_filter_minimum  = 0;
			int player_count_filter_maximum  = 32;

			int sort_method    = 0;
			int sort_direction = 0;

			bool replace_game_matchmaking = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(session_browser, region_filter_enabled, region_filter, language_filter_enabled, language_filter, player_count_filter_enabled, player_count_filter_minimum, player_count_filter_maximum, sort_method, sort_direction, replace_game_matchmaking, pool_filter_enabled, pool_filter)
		} session_browser{};

		struct ugc
		{
			bool infinite_model_memory;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(ugc, infinite_model_memory)
		} ugc{};

		struct stat_editor
		{
			struct stat
			{
				int radio_button_index      = 0;
				std::string int_text        = "";
				std::string int_value       = "";
				bool int_read               = false;
				std::string bool_text       = "";
				std::string bool_value      = "";
				bool bool_read              = false;
				std::string float_text      = "";
				std::string float_value     = "";
				bool float_read             = false;
				std::string increment_text  = "";
				std::string increment_value = "";
				bool increment_loop_write   = false;
				std::string date_text       = "";
				std::string date_value      = "";
				bool date_read              = false;
				std::string string_text     = "";
				std::string string_value    = "";
				bool string_read            = false;
				std::string label_text      = "";
				std::string label_value     = "";
				std::string user_id_text    = "";
				std::string user_id_value   = "";
				bool user_id_read           = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(stat, radio_button_index, int_text, int_value, int_read, bool_text, bool_value, bool_read, float_text, float_value, float_read, increment_text, increment_value, increment_loop_write, date_text, date_value, date_read, string_text, string_value, string_read, label_text, label_value, user_id_text, user_id_value, user_id_read)
			} stat{};

			struct packed_stat
			{
				int radio_button_index = 0;

				std::string int_text  = "";
				std::string int_value = "";
				bool int_read         = false;

				std::string bool_text  = "";
				std::string bool_value = "";
				bool bool_read         = false;

				NLOHMANN_DEFINE_TYPE_INTRUSIVE(packed_stat, radio_button_index, int_text, int_value, int_read, bool_text, bool_value, bool_read)
			} packed_stat{};

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(stat_editor, stat, packed_stat)
		} stat_editor{};

		struct lua
		{
			bool enable_auto_reload_changed_scripts = false;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(lua, enable_auto_reload_changed_scripts)
		} lua{};

		struct persist_weapons
		{
			bool enabled = false;
			std::string weapon_loadout_file;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(persist_weapons, enabled, weapon_loadout_file)
		} persist_weapons{};

		struct vfx
		{
			bool enable_custom_sky_color = false;

			float azimuth_east[4]       = {1, 0, 0, 0};
			float azimuth_west[4]       = {1, 0, 0, 0};
			float azimuth_transition[4] = {1, 0, 0, 0};
			float zenith[4]             = {1, 0, 0, 0};

			float stars_intensity = 1;

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(vfx, enable_custom_sky_color, azimuth_east, azimuth_west, azimuth_transition, zenith, stars_intensity)
		} vfx{};

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, debug, tunables, notifications, player, player_db, protections, self, session, settings, spawn_vehicle, clone_pv, persist_car, spoofing, vehicle, weapons, window, context_menu, esp, session_browser, ugc, reactions, world, stat_editor, lua, persist_weapons, vfx)
	};

	inline auto g = menu_settings();
}
