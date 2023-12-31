/**
 * @file view_spawn_vehicle.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */


Note This will be slightly different as i have removed alot from my build of the menu \\ Do not copy this line 


#include "fiber_pool.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/model_preview/model_preview_service.hpp"
#include "util/vehicle.hpp"
#include "views/view.hpp"

namespace big
{
	void render_spawn_new_vehicle()
	{
		if (ImGui::Checkbox("Preview", &g.spawn_vehicle.preview_vehicle))
		{
			if (!g.spawn_vehicle.preview_vehicle)
			{
				g_model_preview_service->stop_preview();
			}
		}
		ImGui::SameLine();
		components::help_marker("Shows a render of what you have highlighted in front of you.");
		ImGui::SameLine();
		components::command_checkbox<"spawnin">();
		ImGui::SameLine();
		components::command_checkbox<"spawnmaxed">();
		ImGui::SameLine();
		components::command_checkbox<"spawnlast">();

		static char plate_buf[9] = {0};
		strncpy(plate_buf, g.spawn_vehicle.plate.c_str(), 9);

		ImGui::SetNextItemWidth(300.f);
		components::input_text_with_hint("Plate", "Plate Number", plate_buf, sizeof(plate_buf), ImGuiInputTextFlags_None, [] {
			g.spawn_vehicle.plate = plate_buf;
		});

		static int selected_class = -1;
		const auto& class_arr     = g_gta_data_service->vehicle_classes();

		ImGui::SetNextItemWidth(300.f);
		if (ImGui::BeginCombo("Vehicle Class", selected_class == -1 ? "All" : class_arr[selected_class].c_str()))
		{
			if (ImGui::Selectable("All", selected_class == -1))
			{
				selected_class = -1;
			}

			for (int i = 0; i < class_arr.size(); i++)
			{
				if (ImGui::Selectable(class_arr[i].c_str(), selected_class == i))
				{
					selected_class = i;
				}

				if (selected_class == i)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		static char search[64];

		ImGui::SetNextItemWidth(300.f);
		components::input_text_with_hint("Model Name", "Search", search, sizeof(search), ImGuiInputTextFlags_EnterReturnsTrue, [] {
			Vector3 spawn_location = vehicle::get_spawn_location(g.spawn_vehicle.spawn_inside, rage::joaat(search));
			float spawn_heading    = ENTITY::GET_ENTITY_HEADING(self::ped);

			const Vehicle veh = vehicle::spawn(rage::joaat(search), spawn_location, spawn_heading);

			if (veh == 0)
			{
				g_notification_service->push_error("Vehicle", "Unable to spawn vehicle");
			}
			else
			{
				if (g.spawn_vehicle.spawn_maxed)
				{
					vehicle::max_vehicle(veh);
				}

				vehicle::set_plate(veh, plate_buf);

				if (g.spawn_vehicle.spawn_inside)
				{
					vehicle::teleport_into_vehicle(veh);
				}
				if (g.spawn_vehicle.spawn_last)
				{
					vehicle::delete_last_spawned(veh);
				}
			}
		});
		vehicle_map calculated_map{};

		if (g_gta_data_service->vehicles().size() > 0)
		{
			for (auto& item : g_gta_data_service->vehicles())
			{
				const auto& vehicle = item.second;

				std::string display_name         = vehicle.m_display_name;
				std::string display_manufacturer = vehicle.m_display_manufacturer;
				std::string clazz                = vehicle.m_vehicle_class;

				std::transform(display_name.begin(), display_name.end(), display_name.begin(), ::tolower);
				std::transform(display_manufacturer.begin(), display_manufacturer.end(), display_manufacturer.begin(), ::tolower);

				std::string lower_search = search;
				std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), tolower);

				if ((selected_class == -1 || class_arr[selected_class] == clazz) && (display_name.find(lower_search) != std::string::npos || display_manufacturer.find(lower_search) != std::string::npos))
				{
					calculated_map.emplace(item);
				}
			}
		}
		
		static const auto over_30 = (30 * ImGui::GetTextLineHeightWithSpacing() + 2);
		auto calculated_size = calculated_map.size();
		if (calculated_map.size() == 0)
		{
			calculated_size++;
		}
		if (self::veh)
		{
			calculated_size++;
		}
		const auto box_height = calculated_size <= 30 ? (calculated_size * ImGui::GetTextLineHeightWithSpacing() + 2) : over_30;
		if (ImGui::BeginListBox("###vehicles", {300, box_height}))
		{
			if (self::veh)
			{
				static auto veh_hash = 0;

				g_fiber_pool->queue_job([] {
					veh_hash = ENTITY::GET_ENTITY_MODEL(self::veh);
				});

				if (veh_hash)
				{
					const auto& item = g_gta_data_service->vehicle_by_hash(veh_hash);

					components::selectable(std::format("Current Vehicle [{}]", item.m_display_name), false, [] {
						if (self::veh)
						{
							Vector3 spawn_location = vehicle::get_spawn_location(g.spawn_vehicle.spawn_inside, veh_hash);
							float spawn_heading = ENTITY::GET_ENTITY_HEADING(self::ped);

							auto owned_mods = vehicle::get_owned_mods_from_vehicle(self::veh);

							auto veh = vehicle::clone_from_owned_mods(owned_mods, spawn_location, spawn_heading);

							if (veh == 0)
							{
								g_notification_service->push_error("Vehicle", "Unable to spawn vehicle");
							}
							else
							{
								if (g.spawn_vehicle.spawn_maxed)
								{
									vehicle::max_vehicle(veh);
								}

								vehicle::set_plate(veh, plate_buf);

								if (g.spawn_vehicle.spawn_inside)
								{
									vehicle::teleport_into_vehicle(veh);
								}
								if (g.spawn_vehicle.spawn_last)
								{
									vehicle::delete_last_spawned(veh);
								}
							}
						}

						g_model_preview_service->stop_preview();
					});

					if (!g.spawn_vehicle.preview_vehicle || (g.spawn_vehicle.preview_vehicle && !ImGui::IsAnyItemHovered()))
					{
						g_model_preview_service->stop_preview();
					}
					else if (ImGui::IsItemHovered())
					{
						g_fiber_pool->queue_job([] {
							g_model_preview_service->show_vehicle(vehicle::get_owned_mods_from_vehicle(self::veh),
							    g.spawn_vehicle.spawn_maxed);
						});
					}
				}
			}

			if (calculated_map.size() > 0)
			{
				for (auto& item : calculated_map)
				{
					const auto& vehicle = item.second;
					ImGui::PushID(vehicle.m_hash);
					components::selectable(vehicle.m_display_name, false, [&vehicle] {
						const auto spawn_location =
						    vehicle::get_spawn_location(g.spawn_vehicle.spawn_inside, vehicle.m_hash);
						const auto spawn_heading = ENTITY::GET_ENTITY_HEADING(self::ped);

						auto veh = vehicle::spawn(vehicle.m_hash, spawn_location, spawn_heading);

						if (veh == 0)
						{
							g_notification_service->push_error("Vehicle", "Unable to spawn vehicle");
						}
						else
						{
							if (g.spawn_vehicle.spawn_maxed)
							{
								vehicle::max_vehicle(veh);
							}

							vehicle::set_plate(veh, plate_buf);

							if (g.spawn_vehicle.spawn_inside)
							{
								vehicle::teleport_into_vehicle(veh);
							}
							if (g.spawn_vehicle.spawn_last)
							{
								vehicle::delete_last_spawned(veh);
							}
						}

						g_model_preview_service->stop_preview();
						ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
					});
					ImGui::PopID();

					if (!g.spawn_vehicle.preview_vehicle || (g.spawn_vehicle.preview_vehicle && !ImGui::IsAnyItemHovered()))
					{
						g_model_preview_service->stop_preview();
					}
					else if (ImGui::IsItemHovered())
					{
						g_model_preview_service->show_vehicle(vehicle.m_hash, g.spawn_vehicle.spawn_maxed);
					}
				}
			}
			else
			{
				ImGui::TextUnformatted("No vehicles in registry.");
			}
			ImGui::EndListBox();
		}
	}

	void view::spawn_vehicle()
	{
		ImGui::RadioButton("New", &g.spawn_vehicle.spawn_type, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Personal", &g.spawn_vehicle.spawn_type, 1);

		switch (g.spawn_vehicle.spawn_type)
		{
		case 0: render_spawn_new_vehicle(); break;
		case 1: view::pv(); break;
		case 2: view::xml_vehicles(); break;
		}
	}
}
