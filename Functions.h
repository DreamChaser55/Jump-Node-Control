#pragma once

#include <functional>

#include "ECS.h"

	//move order

string order_data::move::get_string() const
{
	return "Destination: " + format("{:.1f}", destination.x) + " | " + format("{:.1f}", destination.y);
}

void order_data::move::clear(Entity* parent_entity_arg)
{
	//TO DO
}

// find route between origin and destination systems using Dijkstra's algorithm
// returned route DOES NOT INCLUDE the origin system, but INCLUDES the destination system
vector<Entity*> find_route(Entity* origin_system, Entity* destination_system)
{
	unordered_set<Entity*> Unvisited_system_ptrs;
	unordered_map<Entity*, int> System_ptr_distance_map;
	unordered_map<Entity*, Entity*> Previous_system_ptr; // previous system in route from origin

	auto system_count = Systems.size(); // reserve memory in data structures
	Unvisited_system_ptrs.reserve(system_count);
	System_ptr_distance_map.reserve(system_count);
	Previous_system_ptr.reserve(system_count);

	for (Entity* system_entity : Systems) // place initial data in structures
	{
		Unvisited_system_ptrs.emplace(system_entity); // initially all systems are unvisited
		System_ptr_distance_map.emplace(system_entity, INT_MAX); // initially jump distance from origin to all systems is INT_MAX
		Previous_system_ptr.emplace(system_entity, nullptr); // initially all systems do not have previous system in route from origin
	}

	System_ptr_distance_map[origin_system] = 0; // initialize origin system distance to 0

	bool stop = false; //stays false until destination system is found

	while (Unvisited_system_ptrs.empty() == false and stop == false) //if destination is not yet found and there are still unvisited systems
	{
		// find unvisited system with the lowest distance from origin, set as current system
		Entity* current_system = *std::min_element(Unvisited_system_ptrs.begin(), Unvisited_system_ptrs.end(), [&](Entity* system1, Entity* system2)
		{
			return System_ptr_distance_map[system1] < System_ptr_distance_map[system2];
		});

		for (Entity* jn_ptr : current_system->c_star_system_ptr->jump_node_entity_ptrs) // for all jump nodes in the current system
		{
			Entity* neighbour_system = jn_ptr->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr; // find node's destination system - neighbour system

			if (Unvisited_system_ptrs.find(neighbour_system) != Unvisited_system_ptrs.end()) // if neighbour system is unvisited
			{
				int alt_dist = System_ptr_distance_map[current_system] + 1; //determine distance if jumping from current system

				if (alt_dist < System_ptr_distance_map[neighbour_system]) //if this distance lower than current value in the distance map
				{
					//set this distance as new value and add current system as neighbour's previous system in route from origin
					System_ptr_distance_map[neighbour_system] = alt_dist;
					Previous_system_ptr[neighbour_system] = current_system;
				}
			}
		}

		Unvisited_system_ptrs.erase(current_system); // erase current system from unvisited systems

		if (current_system == destination_system) // if current system is the destination system, stop the loop
		{
			stop = true;
		}
	}

	// loop finished, computing route..

	vector<Entity*> route;

	route.emplace_back(destination_system); // first place destination system in route
	Entity* current_system_in_route = destination_system;

	while (true) // then follow Previous_system_ptr map from destination system to origin system, placing systems in route
	{
		Entity* previous_system = Previous_system_ptr[current_system_in_route];
		if (previous_system == origin_system)
		{
			break;
		}
		else
		{
			route.emplace_back(previous_system);
			current_system_in_route = previous_system;
		}
	}

	std::reverse(route.begin(), route.end()); // reverse route (we filled it backwards - from destination system to origin system)

	return route;
}

void order_data::move::execute(Entity* parent_entity_arg)
{
	Entity* destination_closest_system = *std::min_element(Systems.begin(), Systems.end(), [&](const Entity* system1, const Entity* system2)
	{
		double dist_to_system1 = point_squared_distance(destination, system1->c_drawable_ptr->sprite.getPosition());
		double dist_to_system2 = point_squared_distance(destination, system2->c_drawable_ptr->sprite.getPosition());

		return dist_to_system1 < dist_to_system2;
	});

	double distance_from_closest_system = point_distance(destination_closest_system->c_drawable_ptr->sprite.getPosition(), destination);
	if (distance_from_closest_system <= destination_closest_system->c_star_system_ptr->radius) //destination not in interstellar space
	{
		if (destination_closest_system->get_reference().id == parent_entity_arg->c_drawable_ptr->in_system_ptr->get_reference().id) // destination in the same system
		{
			if (parent_entity_arg->c_jump_drive_ptr == nullptr) // if entity does not have a jump drive
			{
				// move sublight
				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight,
																						   order_data::move_sublight{destination});
			}
			else // jump drive detected
			{
				double distance = point_distance(destination, parent_entity_arg->c_drawable_ptr->sprite.getPosition());

				if (distance <= parent_entity_arg->c_jump_drive_ptr->min_range) // too close for jumping
				{
					// move sublight
					parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight,
																		   order_data::move_sublight{destination});
				}
				else if (distance > parent_entity_arg->c_jump_drive_ptr->min_range and distance < parent_entity_arg->c_jump_drive_ptr->max_range) // destination in jump range
				{
					// first check if we are not inside jump inhibition field

					auto& jump_inhib_fields = parent_entity_arg->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs;

					const auto ji_field_source = std::find_if(jump_inhib_fields.begin(), jump_inhib_fields.end(), [&](Entity* ji_field_entity_ptr)
					{
						double dist = point_distance(parent_entity_arg->c_drawable_ptr->sprite.getPosition(), ji_field_entity_ptr->c_drawable_ptr->sprite.getPosition());
						return (dist <= ji_field_entity_ptr->c_jump_inhibition_field_ptr->radius);
					});

					if (ji_field_source != jump_inhib_fields.end()) // we are inside JI field
					{
						Entity* ji_field_source_ptr = *ji_field_source;

						if (point_in_circle(destination,
											ji_field_source_ptr->c_drawable_ptr->sprite.getPosition(),
											ji_field_source_ptr->c_jump_inhibition_field_ptr->radius) == false) // if destination is not inside the same JI field
						{
							// get JI field escape destination

							Vector2d vector_from_field_center = parent_entity_arg->c_drawable_ptr->sprite.getPosition() - ji_field_source_ptr->c_drawable_ptr->sprite.getPosition();

							if (vector_from_field_center.x == 0.0 and vector_from_field_center.y == 0.0)
							{
								// if we are in the exact center of the field, get rotation direction of parent
								vector_from_field_center = angle_rad_to_vector(parent_entity_arg->c_drawable_ptr->sprite.getRotation());
							}

							vector_from_field_center.resize(ji_field_source_ptr->c_jump_inhibition_field_ptr->radius + 100.0);
							Vector2d escape_destination = ji_field_source_ptr->c_drawable_ptr->sprite.getPosition() + vector_from_field_center;

							if (point_distance(escape_destination, destination) < parent_entity_arg->c_jump_drive_ptr->min_range) // if final destination would be too close for jump after moving out of the field
							{
								// move directly to final destination by sublight
								parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight,
																										   order_data::move_sublight(destination));
							}
							else // we can jump after moving out of the field
							{
								// move out of the field by sublight
								parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight,
																										   order_data::move_sublight(escape_destination));

								// then move to final destination
								parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move,
																										   order_data::move(destination));
							}

						}
						else // destination is in the same JI field
						{
							// move sublight
							parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight,
																									   order_data::move_sublight{destination});
						}
					}
					else // we are NOT inside JI field
					{
						// jump directly to final destination
						parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::jump_intrasystem,
																								   order_data::jump_intrasystem{destination});
					}
				}
				else // too far for one jump
				{
					// plan moving to destination in multiple submoves

					Vector2d current_pos = parent_entity_arg->c_drawable_ptr->sprite.getPosition();
					Vector2d jump_vector = destination - current_pos;
					jump_vector.resize(parent_entity_arg->c_jump_drive_ptr->max_range * 0.98);

					while (true) // while we are not yet at final destination
					{
						double current_distance_to_destination = point_distance(current_pos, destination);

						if (current_distance_to_destination >=
							parent_entity_arg->c_jump_drive_ptr->max_range) // if final destination is farther than jump drive max range, plan potential max distance jump (later resolution), continue loop
						{
							current_pos += jump_vector;
							parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move,
																									   order_data::move{current_pos});
						}
						else // final destination is closer than jump drive max range, plan to move to it by single potential jump or sublight travel (later resolution), break loop
						{
							parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move,
																									   order_data::move{destination});
							break;
						}
					}
				}
			}
		}
		else // destination in another system
		{
			// plan intersystem route

			Entity* jump_node = nullptr;
			for (Entity* jn : parent_entity_arg->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_node_entity_ptrs)
			{
				if (jn->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr->get_reference().id == destination_closest_system->get_reference().id)
				{
					jump_node = jn;
					break;
				}
			}

			if (jump_node != nullptr) // jump node to destination found in the current system
			{
				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move, // move to node
																						   order_data::move{jump_node->c_drawable_ptr->sprite.getPosition()});

				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::jump_intersystem, // jump through node
																						   order_data::jump_intersystem{jump_node->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr});

				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move, // move to final destination
																						   order_data::move{destination});
			}
			else // jump node to destination NOT found in the current system
			{
				vector<Entity*> route = find_route(parent_entity_arg->c_drawable_ptr->in_system_ptr, destination_closest_system); // find route using Dijkstra's algorithm

				// starting from current system, follow the route and place orders to move to the node to the next system and jump through
				Entity* current_system = parent_entity_arg->c_drawable_ptr->in_system_ptr;
				for (Entity* sys : route)
				{
					auto& nodes = current_system->c_star_system_ptr->jump_node_entity_ptrs;
					Entity* node = *std::find_if(nodes.begin(), nodes.end(), [&](Entity* nd)
					{
						return nd->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr == sys;
					});

					parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move, // move to node
																							   order_data::move{node->c_drawable_ptr->sprite.getPosition()});

					parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::jump_intersystem, // jump through node
																							   order_data::jump_intersystem{sys});

					current_system = sys;
				}

				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move, // move to final destination
																						   order_data::move{destination});
			}
		}
	}
}

bool order_data::move::is_finished(Entity* parent_entity_arg) const
{
	return (holds_alternative<std::nullptr_t>(parent_entity_arg->c_jump_drive_ptr->destination) and
			parent_entity_arg->c_engines_ptr->destination.has_value() == false and
			point_distance(destination, parent_entity_arg->c_drawable_ptr->sprite.getPosition()) < 10.0);
}

	// jump_intrasystem order

string order_data::jump_intrasystem::get_string() const
{
	return "Destination: " + format("{:.10f}", destination.x) + " | " + format("{:.10f}", destination.y);
}

void order_data::jump_intrasystem::clear(Entity* parent_entity_arg)
{
	parent_entity_arg->c_jump_drive_ptr->destination = std::nullptr_t();
}

void order_data::jump_intrasystem::execute(Entity* parent_entity_arg)
{
	parent_entity_arg->c_jump_drive_ptr->destination = destination;
}

bool order_data::jump_intrasystem::is_finished(Entity* parent_entity_arg) const
{
	if (holds_alternative<std::nullptr_t>(parent_entity_arg->c_jump_drive_ptr->destination))
	{
		if (point_distance(destination, parent_entity_arg->c_drawable_ptr->sprite.getPosition()) < 10.0)
		{
			return true; // jump successful
		}
		else // jump into inhibition field
		{
			auto& jump_inhib_fields = parent_entity_arg->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs;

			const auto ji_field_source = std::find_if(jump_inhib_fields.begin(), jump_inhib_fields.end(), [&](Entity* ji_field_entity_ptr)
			{
				auto dist = point_distance(destination, ji_field_entity_ptr->c_drawable_ptr->sprite.getPosition());
				return (dist <= ji_field_entity_ptr->c_jump_inhibition_field_ptr->radius);
			});

			if (ji_field_source != jump_inhib_fields.end())
			{
				Entity* ji_field_source_ptr = *ji_field_source;

				auto intersection_pts_vector = line_segment_circle_overlap_points(parent_entity_arg->c_drawable_ptr->sprite.getPosition(), destination,
																				ji_field_source_ptr->c_drawable_ptr->sprite.getPosition(), ji_field_source_ptr->c_jump_inhibition_field_ptr->radius + 100.0);

				Vector2d& partial_jump_dest = intersection_pts_vector[1];

				if (point_distance(partial_jump_dest, parent_entity_arg->c_drawable_ptr->sprite.getPosition()) > parent_entity_arg->c_jump_drive_ptr->min_range)
				{
					parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::jump_intrasystem, order_data::jump_intrasystem{partial_jump_dest});
				}

				parent_entity_arg->c_spacecraft_ptr->current_order->Suborders.emplace_back(Order_Type::move_sublight, order_data::move_sublight(destination));
			}

			return false;
		}
	}
	else
	{
		return false; // jump in progresss
	}
}

	// jump_intersystem order

string order_data::jump_intersystem::get_string() const
{
	return "Destination system: " + destination_system->c_selectable_ptr->name;
}

void order_data::jump_intersystem::clear(Entity* parent_entity_arg)
{
	parent_entity_arg->c_jump_drive_ptr->destination = std::nullptr_t();
}

void order_data::jump_intersystem::execute(Entity* parent_entity_arg)
{
	parent_entity_arg->c_jump_drive_ptr->destination = destination_system;
}

bool order_data::jump_intersystem::is_finished(Entity* parent_entity_arg) const
{
	return (holds_alternative<std::nullptr_t>(parent_entity_arg->c_jump_drive_ptr->destination) and parent_entity_arg->c_drawable_ptr->in_system_ptr == destination_system);
}

	// colonize order

string order_data::colonize::get_string() const
{
	return "colonize order string";
}

void order_data::colonize::clear(Entity* parent_entity_arg)
{
	print(runtime("order_data::colonize::clear" + NEWL));
}

void order_data::colonize::execute(Entity* parent_entity_arg)
{
	print(runtime("order_data::colonize::execute" + NEWL));
}

bool order_data::colonize::is_finished(Entity* parent_entity_arg) const
{
	print(runtime("order_data::colonize::is_finished" + NEWL));
	return false;
}

	// construct order

string order_data::construct::get_string() const
{
	string location_str_x = format("{:.1f}", location.x);
	string location_str_y = format("{:.1f}", location.y);
	return "Constructing: " + entity_template + " at:" + NEWL + location_str_x + " | " + location_str_y;
}

void order_data::construct::execute(Entity* parent_entity_arg)
{
	if (not construction_started)
	{
		if (parent_entity_arg->c_constructor_ptr != nullptr)
		{
			parent_entity_arg->c_constructor_ptr->Build(entity_template, location);
			construction_started = true;
		}
		else
		{
			print("ERROR: Constructor component does not exist.");
		}

	}
}

bool order_data::construct::is_finished(Entity* parent_entity_arg) const
{
	// The order is considered finished when the constructed entity is no longer under construction
	if (parent_entity_arg->c_constructor_ptr->entity_under_construction_ptr != nullptr)
	{
		// if under_construction is false, return true (finished), if it is true, return false (not finished)
		return parent_entity_arg->c_constructor_ptr->entity_under_construction_ptr->c_spacecraft_ptr->under_construction == false;
	}
	else // if entity_under_construction_ptr == nullptr, return true (finished)
	{
		return true;
	}
}

void order_data::construct::clear(Entity* parent_entity_arg)
{
	// Nullify the ptr to the entity under construction in the Constructor
	parent_entity_arg->c_constructor_ptr->entity_under_construction_ptr = nullptr;
}

	// move_sublight order

string order_data::move_sublight::get_string() const
{
	return "Destination: " + format("{:.1f}", destination.x) + " | " + format("{:.1f}", destination.y);
}

void order_data::move_sublight::clear(Entity* parent_entity_arg)
{
	parent_entity_arg->c_engines_ptr->Stop();
}

void order_data::move_sublight::execute(Entity* parent_entity_arg)
{
	parent_entity_arg->c_engines_ptr->destination = destination;
}

bool order_data::move_sublight::is_finished(Entity* parent_entity_arg) const
{
	return (parent_entity_arg->c_engines_ptr->destination.has_value() == false and
			point_distance(destination, parent_entity_arg->c_drawable_ptr->sprite.getPosition()) < 10.0);
}

	// attack order

string order_data::attack::get_string() const
{
	return "attack order string";
}

void order_data::attack::clear(Entity* parent_entity_arg)
{
	parent_entity_arg->c_weapons_ptr->Stop();
}

void order_data::attack::execute(Entity* parent_entity_arg)
{
	parent_entity_arg->c_weapons_ptr->Attack_Entity(target);
}

bool order_data::attack::is_finished(Entity* parent_entity_arg) const
{
	return not target.is_valid();
}


Turret::Turret(Weapon_Type weapon_type_arg, Weapon_Name weapon_name_arg, float reload_time_arg, Entity* parent_spacecraft_ptr_arg,
			   Vector2d position_offset_local_arg, Texture_Atlas_Subrect* sprite_texture_subrect_arg, Vector2d sprite_scaling_arg) :
	weapon_type(weapon_type_arg),
	weapon_name(weapon_name_arg),
	reload_timer(reload_time_arg),
	reload_time(reload_time_arg),
	parent_spacecraft_ptr(parent_spacecraft_ptr_arg),
	position_offset_local(position_offset_local_arg),
	sprite(Shader_Lighting_Mode::normal_lighting, Colors::White, Draw_Layer::Effects_Higher, sprite_texture_subrect_arg, Blend_Mode::Alpha,
		   SpriteToWorldCoords(position_offset_local_arg, parent_spacecraft_ptr_arg->c_drawable_ptr->sprite), parent_spacecraft_ptr_arg->c_drawable_ptr->sprite.getRotation(), sprite_scaling_arg)
{}

bool Projectile::Step()
{
	sprite.translate(velocity);
	light.position1 = sprite.getPosition();

	--lifetime;

	if (lifetime <= 0)
	{
		return true;
	}

	if (target_entity.is_valid())
	{
		const bool target_hit = target_entity.ptr->c_collidable_ptr->point_collides(sprite.getPosition());

		if (target_hit)
		{
			Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{sprite.getPosition(), get_random_real_in_range(0.0, 3.0), {130.0, 130.0}, Colors::White, name_animation_map["exp_1"],
															Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.1f}, 800.0f});

			target_entity.ptr->c_spacecraft_ptr->hull_points -= damage;

			return true;
		}
	}

	sprite.Enqueue();
	light.Enqueue();

	return false;
}

bool Missile::Step()
{
	if (target_entity.is_valid() == false)
	{
		Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{sprite.getPosition(), get_random_real_in_range(0.0, 3.0), {130.0, 130.0}, Colors::White, name_animation_map["exp_1"],
												Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.1f}, 800.0f});

		return true;
	}

	Vector2d current_pos = sprite.getPosition();
	Vector2d direction_to_target = target_entity.ptr->c_drawable_ptr->sprite.getPosition() - current_pos;

	double angle_to_target = vector_to_angle_rad(direction_to_target);

	double current_rot = sprite.getRotation();
	double angle_difference_signed = angle_difference_rad(angle_to_target, current_rot);

	if (abs(angle_difference_signed) <= turn_rate / 2) //rotation finished
	{
		sprite.setRotation(angle_to_target);
	}
	else //rotation not finished
	{
		sprite.setRotation(current_rot + clamp(angle_difference_signed, -turn_rate, turn_rate));
	}

	Vector2d velocity = angle_rad_to_vector(sprite.getRotation()).get_resized(speed);
	sprite.setPosition(current_pos + velocity);

	Vector2d engine_pos = SpriteToWorldCoords({-0.5, 0.0}, sprite);;
	light.position1 = engine_pos;

	int modulus = int(lifetime) % 2;
	if (modulus == 0)
	{
		Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{engine_pos, get_random_real_in_range(0.0, 5.0), {30.0, 30.0}, Colors::White, name_animation_map["missile_trail_1"],
														Blend_Mode::Alpha, Draw_Layer::Effects_Higher, velocity.get_negated() / 2.0, true, 0.0, Color{0.0f, 0.0f, 0.0f}, 0.0f});
	}

	--lifetime;

	if (lifetime <= 0)
	{
		return true;
	}


	const bool target_hit = target_entity.ptr->c_collidable_ptr->point_collides(sprite.getPosition());

	if (target_hit)
	{
		Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{sprite.getPosition(), get_random_real_in_range(0.0, 3.0), {130.0, 130.0}, Colors::White, name_animation_map["exp_1"],
														Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.1f}, 800.0f});

		target_entity.ptr->c_spacecraft_ptr->hull_points -= damage;

		return true;
	}
	

	sprite.Enqueue();
	light.Enqueue();

	return false;
}

Beam::Beam(Entity_Ref origin_entity_arg,
		   Turret* origin_turret_arg,
		   Entity_Ref target_entity_arg,
		   Vector2f endpoint1_offset_arg,
		   Vector2f endpoint2_offset_arg,
		   float damage_arg,
		   float lifetime_arg,
		   Color light_diffuse_arg,
		   float light_intensity_arg,
		   Color color_arg) :
	origin_entity(origin_entity_arg),
	origin_turret(origin_turret_arg),
	target_entity(target_entity_arg),
	endpoint1_offset(endpoint1_offset_arg),
	endpoint2_offset(endpoint2_offset_arg),
	current_endpoint_offset(endpoint1_offset_arg),
	damage(damage_arg),
	lifetime(lifetime_arg),
	remaining_lifetime(lifetime_arg)
{
	auto origin = SpriteToWorldCoords(origin_turret_arg->position_offset_local, origin_entity_arg.ptr->c_drawable_ptr->sprite);
	auto endpoint = SpriteToWorldCoords(Vector2d(current_endpoint_offset), target_entity_arg.ptr->c_drawable_ptr->sprite);
	auto long_dimension = endpoint - origin;
	auto position = origin + long_dimension / 2.0;
	auto rotation = vector_to_angle_rad(long_dimension);
	auto scaling = Vector2d(long_dimension.get_length(), 5.0);

	sprite = Sprite{Shader_Lighting_Mode::no_lighting, color_arg, Draw_Layer::Effects_Higher, name_texture_atlas_subrect_map["beam"], Blend_Mode::Additive,
	position, rotation, scaling};

	light = Light{false, light_diffuse_arg, light_intensity_arg, 1.5, origin, origin + long_dimension};

	muzzle_glow = Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, Colors::White, Draw_Layer::Effects_Higher, *name_animation_map["redbeamglow"], Blend_Mode::Additive,
	origin, 0.0, {100.0, 100.0}};
}

bool Beam::Step()
{
	remaining_lifetime--;

	if (remaining_lifetime <= 0 or target_entity.is_valid() == false)
	{
		return true;
	}

	float beam_progress = 1.0f - (remaining_lifetime / lifetime);
	current_endpoint_offset = lerp_vectors(endpoint1_offset, endpoint2_offset, beam_progress);

	Vector2d origin = SpriteToWorldCoords(origin_turret->position_offset_local, origin_entity.ptr->c_drawable_ptr->sprite);

	muzzle_glow.setPosition(origin);

	if (muzzle_glow.graphics.index() == 1)
	{
		Animation& animation = get<Animation>(muzzle_glow.graphics);
		animation.advance();
	}

	muzzle_glow.Enqueue();

	Vector2d endpoint = SpriteToWorldCoords(Vector2d(current_endpoint_offset), target_entity.ptr->c_drawable_ptr->sprite);
	const bool target_hit = target_entity.ptr->c_collidable_ptr->point_collides(endpoint);

	Vector2d long_dimension;
	Vector2d position;
	double rotation;
	Vector2d scaling;

	if (target_hit)
	{
		long_dimension = endpoint - origin;
		position = origin + long_dimension / 2.0;

		if (get_random_int_in_range(0, 9) < 3)
		{
			Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{endpoint, get_random_real_in_range(0.0, 3.0), {130.0, 130.0}, Colors::White, name_animation_map["exp_1"],
															Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.1f}, 350.0f});
		}

		target_entity.ptr->c_spacecraft_ptr->hull_points -= damage;
	}
	else
	{
		long_dimension = (endpoint - origin)*2.0;
		position = endpoint;
	}

	rotation = vector_to_angle_rad(long_dimension);
	scaling = Vector2d(long_dimension.get_length(), 12.0);

	sprite.setPosition(position);
	sprite.setRotation(rotation);
	sprite.setScaling(scaling);
	sprite.Enqueue();

	Color outer_color = sprite.color;
	sprite.setScaling(Vector2d(scaling.x, scaling.y/2.2));
	sprite.color = Colors::White;
	sprite.Enqueue();
	sprite.color = outer_color;
	
	light.position1 = origin;
	light.position2 = origin + long_dimension;
	light.Enqueue();
	
	return false;
}

void spawn_spacecraft_grid(Vector2d position, int spawn_num)
{
	vector<Vector2d> collision_polygon_texture_coords; 
	collision_polygon_texture_coords.reserve(41);

	collision_polygon_texture_coords.emplace_back(64, 206);
	collision_polygon_texture_coords.emplace_back(17, 201);
	collision_polygon_texture_coords.emplace_back(1, 192);
	collision_polygon_texture_coords.emplace_back(3, 105);
	collision_polygon_texture_coords.emplace_back(13, 94);
	collision_polygon_texture_coords.emplace_back(155, 101);
	collision_polygon_texture_coords.emplace_back(166, 120);
	collision_polygon_texture_coords.emplace_back(166, 175);
	collision_polygon_texture_coords.emplace_back(174, 174);
	collision_polygon_texture_coords.emplace_back(179, 180);
	collision_polygon_texture_coords.emplace_back(180, 195);
	collision_polygon_texture_coords.emplace_back(199, 195);
	collision_polygon_texture_coords.emplace_back(199, 0);
	collision_polygon_texture_coords.emplace_back(256, 0);
	collision_polygon_texture_coords.emplace_back(255, 188);
	collision_polygon_texture_coords.emplace_back(266, 196);
	collision_polygon_texture_coords.emplace_back(278, 196);
	collision_polygon_texture_coords.emplace_back(279, 176);
	collision_polygon_texture_coords.emplace_back(293, 167);
	collision_polygon_texture_coords.emplace_back(336, 173);
	collision_polygon_texture_coords.emplace_back(336, 144);
	collision_polygon_texture_coords.emplace_back(550, 140);
	collision_polygon_texture_coords.emplace_back(549, 290); //middle
	collision_polygon_texture_coords.emplace_back(336, 287);
	collision_polygon_texture_coords.emplace_back(335, 264);
	collision_polygon_texture_coords.emplace_back(296, 267);
	collision_polygon_texture_coords.emplace_back(278, 259);
	collision_polygon_texture_coords.emplace_back(279, 239);
	collision_polygon_texture_coords.emplace_back(257, 241);
	collision_polygon_texture_coords.emplace_back(254, 421);
	collision_polygon_texture_coords.emplace_back(200, 421);
	collision_polygon_texture_coords.emplace_back(198, 240);
	collision_polygon_texture_coords.emplace_back(179, 239);
	collision_polygon_texture_coords.emplace_back(177, 258);
	collision_polygon_texture_coords.emplace_back(166, 259);
	collision_polygon_texture_coords.emplace_back(166, 320);
	collision_polygon_texture_coords.emplace_back(153, 332);
	collision_polygon_texture_coords.emplace_back(12, 336);
	collision_polygon_texture_coords.emplace_back(1, 328);
	collision_polygon_texture_coords.emplace_back(1, 239);
	collision_polygon_texture_coords.emplace_back(11, 233);
	collision_polygon_texture_coords.emplace_back(61, 231);

	for (auto& polygon : collision_polygon_texture_coords)
	{
		polygon.y += 8.0;
	}

	for (int i = 0; i < spawn_num; ++i)
	{
		for (int j = 0; j < spawn_num; ++j)
		{
			Vector2d spawn_position{position.x + 1000.0*i, position.y + 1000.0*j};

			auto entity_it = Entity::Colony.emplace();
			entity_it->c_drawable_ptr = &*Component::Drawable::Colony.emplace(&*entity_it,
																			  Sprite{Shader_Lighting_Mode::normal_lighting, Colors::White, Draw_Layer::Spacecrafts, name_texture_atlas_subrect_map["faustus"],
																			  Blend_Mode::Alpha, spawn_position, 0, Vector2d(556, 442), name_texture_atlas_subrect_map["faustus_glow"]},
																			  Icon{Colors::Green, Draw_Layer::Spacecrafts, Blend_Mode::Additive,
																			  name_texture_atlas_subrect_map["icon_cruiser"],
																			  Vector2d(screen_resolution.x * 0.036, screen_resolution.y * 0.032)});
			entity_it->c_collidable_ptr = &*Component::Collidable::Colony.emplace(&*entity_it,
																				  CollisionPolygonToSpriteCoords(collision_polygon_texture_coords, {556, 442}));
			entity_it->c_dynamic_ptr = &*Component::Dynamic::Colony.emplace(&*entity_it,
																			Vector2d{0.0, 0.0}, 0);
			entity_it->c_engines_ptr = &*Component::Engines::Colony.emplace(&*entity_it,
																			2, 0.1, deg_to_rad(2));
			entity_it->c_selectable_ptr = &*Component::Selectable::Colony.emplace(&*entity_it,
																				  "USS Marha " + to_string(i) + to_string(j));
			entity_it->c_spacecraft_ptr = &*Component::Spacecraft::Colony.emplace(&*entity_it,
																				  300);
			entity_it->c_weapons_ptr = &*Component::Weapons::Colony.emplace(&*entity_it);

			entity_it->c_weapons_ptr->Turrets.emplace_back(Turret{Weapon_Type::Mass_driver, Weapon_Name::Ravager_Cannon, 30.0f,
														   &*entity_it, Vector2d{0.3, 0.05}, name_texture_atlas_subrect_map["faustus"], Vector2d{50.0, 50.0}});

			entity_it->c_weapons_ptr->Turrets.emplace_back(Turret{Weapon_Type::Mass_driver, Weapon_Name::Ravager_Cannon, 30.0f,
											   &*entity_it, Vector2d{0.3, -0.05}, name_texture_atlas_subrect_map["faustus"], Vector2d{50.0, 50.0}});

			entity_it->c_weapons_ptr->Turrets.emplace_back(Turret{Weapon_Type::Missile_launcher, Weapon_Name::Stinger_Missile, 30.0f,
								   &*entity_it, Vector2d{0.2, 0.0}, name_texture_atlas_subrect_map["faustus"], Vector2d{50.0, 50.0}});

			entity_it->c_weapons_ptr->Turrets.emplace_back(Turret{Weapon_Type::Beam_emitter, Weapon_Name::Winter_King_Beam, 60.0f*8,
					   &*entity_it, Vector2d{-0.2, 0.0}, name_texture_atlas_subrect_map["faustus"], Vector2d{50.0, 50.0}});

			entity_it->c_jump_drive_ptr = &*Component::Jump_Drive::Colony.emplace(&*entity_it,
																				  0.0f, 20.0f / 60.0f, 1000.0, system_radius / 5.0, true);

			if (i == 5 and j == 5)
			{
				entity_it->c_jump_inhibition_field_ptr = &*Component::Jump_Inhibition_Field::Colony.emplace(&*entity_it,
																											1500.0, true);
			}

			entity_it->c_constructor_ptr = &*Component::Constructor::Colony.emplace(&*entity_it);

		}
	}
}

void spawn_spacecraft()
{
	spawn_spacecraft_grid(Systems[0]->c_drawable_ptr->sprite.getPosition() + Vector2d(system_radius/5, system_radius/5), 10);
}

void SpawnEntities()
{
	// *** Spawn Entities *** //
	print(runtime(NEWL + "Spawning entities: " + NEWL));

	double gal_size = system_radius * 4.5 * sqrt(max(num_of_systems, 5));
	galaxy_size.x = gal_size;
	galaxy_size.y = gal_size;

	spawn_systems();
	spawn_celestial_bodies();
	spawn_spacecraft();

	vector<Vector2d> collision_polygon_local;
	collision_polygon_local.reserve(41);

	collision_polygon_local.emplace_back(64, 206);
	collision_polygon_local.emplace_back(17, 201);
	collision_polygon_local.emplace_back(1, 192);
	collision_polygon_local.emplace_back(3, 105);
	collision_polygon_local.emplace_back(13, 94);
	collision_polygon_local.emplace_back(155, 101);
	collision_polygon_local.emplace_back(166, 120);
	collision_polygon_local.emplace_back(166, 175);
	collision_polygon_local.emplace_back(174, 174);
	collision_polygon_local.emplace_back(179, 180);
	collision_polygon_local.emplace_back(180, 195);
	collision_polygon_local.emplace_back(199, 195);
	collision_polygon_local.emplace_back(199, 0);
	collision_polygon_local.emplace_back(256, 0);
	collision_polygon_local.emplace_back(255, 188);
	collision_polygon_local.emplace_back(266, 196);
	collision_polygon_local.emplace_back(278, 196);
	collision_polygon_local.emplace_back(279, 176);
	collision_polygon_local.emplace_back(293, 167);
	collision_polygon_local.emplace_back(336, 173);
	collision_polygon_local.emplace_back(336, 144);
	collision_polygon_local.emplace_back(550, 140);
	collision_polygon_local.emplace_back(549, 290);//stred
	collision_polygon_local.emplace_back(336, 287);
	collision_polygon_local.emplace_back(335, 264);
	collision_polygon_local.emplace_back(296, 267);
	collision_polygon_local.emplace_back(278, 259);
	collision_polygon_local.emplace_back(279, 239);
	collision_polygon_local.emplace_back(257, 241);
	collision_polygon_local.emplace_back(254, 421);
	collision_polygon_local.emplace_back(200, 421);
	collision_polygon_local.emplace_back(198, 240);
	collision_polygon_local.emplace_back(179, 239);
	collision_polygon_local.emplace_back(177, 258);
	collision_polygon_local.emplace_back(166, 259);
	collision_polygon_local.emplace_back(166, 320);
	collision_polygon_local.emplace_back(153, 332);
	collision_polygon_local.emplace_back(12, 336);
	collision_polygon_local.emplace_back(1, 328);
	collision_polygon_local.emplace_back(1, 239);
	collision_polygon_local.emplace_back(11, 233);
	collision_polygon_local.emplace_back(61, 231);
}

void Initialize_game()
{
	print(runtime(NEWL + "Initialize_game()" + NEWL));

	// Initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(D3D11_Device, D3D11_Device_Context);

	auto& io = ImGui::GetIO();
	io.IniFilename = NULL;
	io.MouseDrawCursor = true;

	io.Fonts->AddFontFromFileTTF("arial.ttf", float(screen_resolution.y) * 0.0156f);

	LoadResources();

	//Restart timers
	Stopwatch_FPS_limit.restart();
	Second.restart();

	camera.refresh();

	mouse.sprite = Sprite{Shader_Lighting_Mode::no_lighting, Color(1.0f, 1.0f, 1.0f, 1.0f), Draw_Layer::GUI_Higher, name_texture_atlas_subrect_map["marker2"], Blend_Mode::Additive,
		mouse.world_position, 0, mouse.scaling};

	background_sprite = Sprite{Shader_Lighting_Mode::no_lighting, Color(1.0f, 1.0f, 1.0f, 1.0f), Draw_Layer::Background_and_Universe, name_texture_atlas_subrect_map["background_milkyway_4K"], Blend_Mode::Opaque,
		camera.position, 0.0, camera.scaling};

	/// Faustus EntityTemplate

	EntityTemplate faustus;
	faustus.name = "GTSC Faustus";

	faustus.collision_polygon_texture_coords.reserve(41);

	faustus.collision_polygon_texture_coords.emplace_back(64, 206);
	faustus.collision_polygon_texture_coords.emplace_back(17, 201);
	faustus.collision_polygon_texture_coords.emplace_back(1, 192);
	faustus.collision_polygon_texture_coords.emplace_back(3, 105);
	faustus.collision_polygon_texture_coords.emplace_back(13, 94);
	faustus.collision_polygon_texture_coords.emplace_back(155, 101);
	faustus.collision_polygon_texture_coords.emplace_back(166, 120);
	faustus.collision_polygon_texture_coords.emplace_back(166, 175);
	faustus.collision_polygon_texture_coords.emplace_back(174, 174);
	faustus.collision_polygon_texture_coords.emplace_back(179, 180);
	faustus.collision_polygon_texture_coords.emplace_back(180, 195);
	faustus.collision_polygon_texture_coords.emplace_back(199, 195);
	faustus.collision_polygon_texture_coords.emplace_back(199, 0);
	faustus.collision_polygon_texture_coords.emplace_back(256, 0);
	faustus.collision_polygon_texture_coords.emplace_back(255, 188);
	faustus.collision_polygon_texture_coords.emplace_back(266, 196);
	faustus.collision_polygon_texture_coords.emplace_back(278, 196);
	faustus.collision_polygon_texture_coords.emplace_back(279, 176);
	faustus.collision_polygon_texture_coords.emplace_back(293, 167);
	faustus.collision_polygon_texture_coords.emplace_back(336, 173);
	faustus.collision_polygon_texture_coords.emplace_back(336, 144);
	faustus.collision_polygon_texture_coords.emplace_back(550, 140);
	faustus.collision_polygon_texture_coords.emplace_back(549, 290); //middle
	faustus.collision_polygon_texture_coords.emplace_back(336, 287);
	faustus.collision_polygon_texture_coords.emplace_back(335, 264);
	faustus.collision_polygon_texture_coords.emplace_back(296, 267);
	faustus.collision_polygon_texture_coords.emplace_back(278, 259);
	faustus.collision_polygon_texture_coords.emplace_back(279, 239);
	faustus.collision_polygon_texture_coords.emplace_back(257, 241);
	faustus.collision_polygon_texture_coords.emplace_back(254, 421);
	faustus.collision_polygon_texture_coords.emplace_back(200, 421);
	faustus.collision_polygon_texture_coords.emplace_back(198, 240);
	faustus.collision_polygon_texture_coords.emplace_back(179, 239);
	faustus.collision_polygon_texture_coords.emplace_back(177, 258);
	faustus.collision_polygon_texture_coords.emplace_back(166, 259);
	faustus.collision_polygon_texture_coords.emplace_back(166, 320);
	faustus.collision_polygon_texture_coords.emplace_back(153, 332);
	faustus.collision_polygon_texture_coords.emplace_back(12, 336);
	faustus.collision_polygon_texture_coords.emplace_back(1, 328);
	faustus.collision_polygon_texture_coords.emplace_back(1, 239);
	faustus.collision_polygon_texture_coords.emplace_back(11, 233);
	faustus.collision_polygon_texture_coords.emplace_back(61, 231);

	faustus.scaling = Vector2d(556, 442);
	faustus.graphics = name_texture_atlas_subrect_map["faustus"];
	faustus.glowmap_subrect_ptr = name_texture_atlas_subrect_map["faustus_glow"];
	faustus.crew = 300;
	faustus.max_hull_points = 1000.0f;

	Spacecraft_Component_Definition::Jump_Drive jd;
	jd.charge = 0.0f;
	jd.charge_rate = 20.0f / 60.0f;
	jd.min_range = 1000.0;
	jd.max_range = system_radius / 5.0;
	jd.intersystem = true;
	faustus.jump_drive = jd;

	Spacecraft_Component_Definition::Engines engines;
	engines.max_speed = 2.0;
	engines.acceleration = 0.1;
	engines.max_angular_velocity = deg_to_rad(2);
	faustus.engines = engines;

	Spacecraft_Component_Definition::Jump_Inhibition_Field ji_field;
	ji_field.radius = 1500.0;
	faustus.ji_field = ji_field;

	Spacecraft_Component_Definition::Constructor constructor;
	constructor.construction_speed_multiplier = 1.0f;
	faustus.constructor = constructor;

	TurretTemplate turret1;
	turret1.name = "turret1MD";
	turret1.weapon_name_arg = Weapon_Name::Ravager_Cannon;
	turret1.weapon_type_arg = Weapon_Type::Mass_driver;
	turret1.position_offset_local_arg = Vector2d{0.3, 0.05};

	TurretTemplate turret2;
	turret2.name = "turret2MD";
	turret2.weapon_name_arg = Weapon_Name::Ravager_Cannon;
	turret2.weapon_type_arg = Weapon_Type::Mass_driver;
	turret2.position_offset_local_arg = Vector2d{0.3, -0.05};

	TurretTemplate turret3;
	turret3.name = "turret3ML";
	turret3.weapon_name_arg = Weapon_Name::Stinger_Missile;
	turret3.weapon_type_arg = Weapon_Type::Missile_launcher;
	turret3.position_offset_local_arg = Vector2d{0.2, 0.0};

	TurretTemplate turret4;
	turret4.name = "turret4BE";
	turret4.weapon_name_arg = Weapon_Name::Winter_King_Beam;
	turret4.weapon_type_arg = Weapon_Type::Beam_emitter;
	turret4.position_offset_local_arg = Vector2d{-0.2, 0.0};

	faustus.Turrets.emplace_back(turret1);
	faustus.Turrets.emplace_back(turret2);
	faustus.Turrets.emplace_back(turret3);
	faustus.Turrets.emplace_back(turret4);

	entityTemplateMap["GTSC Faustus"] = faustus;

	///

	SpawnEntities();

	camera.position = Systems[0]->c_drawable_ptr->sprite.getPosition() + Vector2d(system_radius / 5, system_radius / 5);
	camera.position_leading = camera.position;
}

void apply_function_to_suborders_and_order(std::function<void(Order&)> func, Order& order)
{
	if (order.Suborders.empty() == false)
	{
		for (Order& suborder : order.Suborders)
		{
			apply_function_to_suborders_and_order(func, suborder);
		}
	}

	func(order);
}

void DrawGUI()
{
	jump_node_lines.Enqueue(); //jump node lines

	//Mouse
	mouse.sprite.setPosition(mouse.world_position);
	mouse.sprite.setScaling(mouse.scaling);
	//mouse.sprite.Enqueue(); // imgui takes care of mouse drawing now

	//Background
	background_sprite.setPosition(camera.position);
	background_sprite.setScaling(camera.scaling);
	background_sprite.Enqueue();


	if (debug)	// debug draw
	{
		ImGui::SetNextWindowPos({0.0f, 0.0f});

		ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text(("FPS: " + format("{:.2f}", average_fps) + " Frame time: " + format("{:.2f}", double(average_frame_time.count()) / 1000000.0)).c_str());
		ImGui::Text(("Cam coords: " + format("{:.3f}", camera.position.x / 1000.0) + " | " +
					 format("{:.3f}", camera.position.y / 1000.0) + " km" + NEWL).c_str());
		ImGui::Text(("Cam size.x: " + format("{:.3f}", camera.relative_rectangle.size.x / 1000.0) + " | " +
					 format("{:.3f}", camera.relative_rectangle.size.y / 1000.0)).c_str());
		ImGui::Text(("Cam zoom: " + format("{:.3f}", camera.get_zoom())).c_str());

		ImGui::Text(("Screen mouse pos in pixels: " + format("{:.3f}", mouse.screen_position_in_pixels.x) +
					 " | " + format("{:.3f}", mouse.screen_position_in_pixels.y)).c_str());
		ImGui::Text(("Screen mouse pos ImGui: " + format("{:.3f}", mouse.screen_position_for_imgui.x) +
					 " | " + format("{:.3f}", mouse.screen_position_for_imgui.y)).c_str());
		ImGui::Text(("Mouse pos world: " + format("{:.3f}", mouse.world_position.x) +
					 " | " + format("{:.3f}", mouse.world_position.y)).c_str());

		ImGui::Text(("Particles: " + to_string(Systems[0]->c_star_system_ptr->Particles.size())).c_str());
		ImGui::Text(("Lights: " + to_string(number_of_active_lights)).c_str());

		ImGui::End();
	}

	if (Context_Menu::open)
	{
		Vector2f context_menu_imgui_pixel_position = ScreenPixelToImguiCoords(Context_Menu::screen_px_position);

		ImGui::SetNextWindowPos({context_menu_imgui_pixel_position.x,
								 context_menu_imgui_pixel_position.y});
		ImGui::SetNextWindowSize({Context_Menu::size_x, Context_Menu::size_x * 2.0f});
		ImGui::Begin("Context_Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
					 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::End();
	}

	if (selection.selected_entities_ptrs.empty() == false) //something is selected
	{
		float sidebar_size_x = float(screen_resolution.x) * 0.2f;
		ImGui::SetNextWindowPos({float(screen_resolution.x) - sidebar_size_x, 0.0f});
		ImGui::SetNextWindowSize({sidebar_size_x, float(screen_resolution.y)});
		ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::End();

		if (selection.selected_entities_ptrs.size() == 1) //one entity is selected
		{
			Entity* selected_entity_ptr = *selection.selected_entities_ptrs.begin();
			selected_entity_ptr->c_selectable_ptr->Draw_Sidebar_Info();

			if (Context_Menu::open)
			{
				if (selected_entity_ptr->c_spacecraft_ptr)
				{
					if (Context_Menu::entity_clicked_on) // context menu on entity
					{
						ImGui::Begin("Context_Menu");
						ImGui::Text(Context_Menu::entity_clicked_on->ptr->c_selectable_ptr->name.c_str());
						ImGui::End();

						if (Context_Menu::entity_clicked_on->ptr->c_spacecraft_ptr) // if the context menu entity is a spacecraft
						{
							if (selected_entity_ptr->c_weapons_ptr)
							{
								ImGui::Begin("Context_Menu");
								if (ImGui::Button("Attack"))
								{
									if (keyboard.Key_LSHIFT.pressed_down == false) // if left SHIFT not pressed down, first clear all orders from the selected spacecraft
									{
										selected_entity_ptr->c_spacecraft_ptr->Clear_Orders();
									}

									selected_entity_ptr->c_spacecraft_ptr->Orders.emplace_back(Order_Type::attack,
																							   order_data::attack{Context_Menu::entity_clicked_on.value()});

									Context_Menu::open = false;
								}
								ImGui::End();
							}
						}
					}
					else // context menu on empty space
					{
						Vector2d context_world_position = ScreenPixelToWorld(Context_Menu::screen_px_position);
						string context_menu_string = format(runtime("Empty space:" + NEWL + "{:.1f}" + NEWL + "{:.1f}"),
															context_world_position.x,
															context_world_position.y);

						ImGui::Begin("Context_Menu");
						ImGui::Text(context_menu_string.c_str());
						ImGui::End();

						if (selected_entity_ptr->c_engines_ptr or selected_entity_ptr->c_jump_drive_ptr)
						{
							ImGui::Begin("Context_Menu");
							if (ImGui::Button("Move Here"))
							{
								if (keyboard.Key_LSHIFT.pressed_down == false) // if left SHIFT not pressed down, first clear all orders from the selected spacecraft
								{
									selected_entity_ptr->c_spacecraft_ptr->Clear_Orders();
								}

								selected_entity_ptr->c_spacecraft_ptr->Orders.emplace_back(Order_Type::move,
																						   order_data::move{ScreenPixelToWorld(Context_Menu::screen_px_position)});

								Context_Menu::open = false;
							}
							ImGui::End();
						}
					}
				}
			}
		}
		else //multiple entities selected
		{
			ImGui::Begin("Sidebar");
			ImGui::Text((to_string(selection.selected_entities_ptrs.size()) + " SELECTED").c_str());
			ImGui::End();
		}

		for (Entity* entity_ptr : selection.selected_entities_ptrs)
		{
			entity_ptr->c_selectable_ptr->Draw_Selection_Brackets();

			if (entity_ptr->c_spacecraft_ptr != nullptr)
			{
				// draw destination markers

				vector<Vector2d> linestrip_vertices;
				linestrip_vertices.emplace_back(entity_ptr->c_drawable_ptr->sprite.getPosition());

				vector<Color> linestrip_vertices_colors;
				linestrip_vertices_colors.emplace_back(Colors::White);

				class Destination_marker_drawer
				{
				public:
					vector<Vector2d>* vertices_ptr;
					vector<Color>* vertices_colors_ptr;

					void operator()(Order& order)
					{
						switch (order.type)
						{
							case Order_Type::move:
							{
								auto& move_data = get<order_data::move>(order.data);
								Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, {0.2f, 0.2f, 0.2f, 1.0f}, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"], Blend_Mode::Alpha,
								move_data.destination, 0.0, {35.0, 35.0}}.Enqueue();

								vertices_ptr->emplace_back(move_data.destination);
								vertices_colors_ptr->emplace_back(Color{0.2f, 0.2f, 0.2f, 1.0f});
							}
							break;

							case Order_Type::move_sublight:
							{
								auto& move_sublight_data = get<order_data::move_sublight>(order.data);
								Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, {0.1f, 1.0f, 0.1f, 1.0f}, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"], Blend_Mode::Alpha,
								move_sublight_data.destination, 0.0, {35.0, 35.0}}.Enqueue();

								vertices_ptr->emplace_back(move_sublight_data.destination);
								vertices_colors_ptr->emplace_back(Color{0.1f, 1.0f, 0.1f, 1.0f});
							}
							break;

							case Order_Type::jump_intrasystem:
							{
								auto& jump_intrasystem_data = get<order_data::jump_intrasystem>(order.data);
								Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, {0.1f, 0.1f, 1.0f, 1.0f}, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"], Blend_Mode::Alpha,
								jump_intrasystem_data.destination, 0.0, {35.0, 35.0}}.Enqueue();

								vertices_ptr->emplace_back(jump_intrasystem_data.destination);
								vertices_colors_ptr->emplace_back(Color{0.1f, 0.1f, 1.0f, 1.0f});
							}
							break;

							case Order_Type::jump_intersystem:
							{
								auto& jump_intersystem_data = get<order_data::jump_intersystem>(order.data);
								Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, {0.1f, 1.0f, 1.0f, 1.0f}, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"], Blend_Mode::Alpha,
								jump_intersystem_data.destination_system->c_drawable_ptr->sprite.getPosition(), 0.0, {system_radius/5.0, system_radius / 5.0}}.Enqueue();

								vertices_ptr->emplace_back(jump_intersystem_data.destination_system->c_drawable_ptr->sprite.getPosition());
								vertices_colors_ptr->emplace_back(Color{0.1f, 1.0f, 1.0f, 1.0f});
							}
							break;

							case Order_Type::attack:
							{
								auto& attack_data = get<order_data::attack>(order.data);
								Sprite_Translatable_Only{Shader_Lighting_Mode::no_lighting, Colors::Red, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"], Blend_Mode::Alpha,
								attack_data.target.ptr->c_drawable_ptr->sprite.getPosition(), 0.0, {40.0, 40.0}}.Enqueue();

								vertices_ptr->emplace_back(attack_data.target.ptr->c_drawable_ptr->sprite.getPosition());
								vertices_colors_ptr->emplace_back(Colors::Red);
							}
							break;
						}
					}
				};


				for (Order& order : entity_ptr->c_spacecraft_ptr->Orders)
				{
					apply_function_to_suborders_and_order(Destination_marker_drawer{&linestrip_vertices, &linestrip_vertices_colors}, order);
				}

				Linelist{Draw_Layer::GUI_Lower, Blend_Mode::Additive, linestrip_to_linelist(linestrip_vertices), linestrip_colors_to_linelist(linestrip_vertices_colors)}.Enqueue();

				if (entity_ptr->c_jump_drive_ptr != nullptr)
				{
					Linelist{Colors::White, Draw_Layer::GUI_Lower, Blend_Mode::Additive,
						GetCircleVertices(entity_ptr->c_drawable_ptr->sprite.getPosition(), entity_ptr->c_jump_drive_ptr->min_range, 256)}.Enqueue();

					Linelist{Colors::White, Draw_Layer::GUI_Lower, Blend_Mode::Additive,
						GetCircleVertices(entity_ptr->c_drawable_ptr->sprite.getPosition(), entity_ptr->c_jump_drive_ptr->max_range, 256)}.Enqueue();
				}
			}
		}
	}
}

void Step()
{
	Process_ECS();
	PS_constant_buffer.number_of_active_lights.x = number_of_active_lights;
	DrawGUI();

	bool demo_window = true;
	ImGui::ShowDemoWindow(&demo_window);
}

void Process_Mouse_Buttons()
{
	auto& imgui_io = ImGui::GetIO();

	if (imgui_io.WantCaptureMouse == false)
	{
		// mouse wheel camera zooming

		double zoom_multiplier = pow(1.5, double(mouse.wheel_turns)); //lZ (wheel turns) returns negative if zooming out (wheel towards the player), but zoom_multiplier needs to be positive
		if (zoom_multiplier != 1) // changing zoom
		{
			// zoom to mouse position

			Vector2d vector_to_mouse = mouse.world_position - camera.position_leading;
			camera.position_leading += vector_to_mouse - vector_to_mouse * zoom_multiplier;
			camera.zoom_leading *= zoom_multiplier;
		}
	}

	camera.Step();
	mouse.scaling = Vector2d(30.0, 30.0) * camera.get_zoom(); // set mouse size according to camera zoom

	if (mouse.Left_Button.key_event == Key_Event::Pressed) //click select
	{
		if (imgui_io.WantCaptureMouse == false)
		{
			Context_Menu::open = false;

			selection_rectangle.topleft.x = mouse.world_position.x;
			selection_rectangle.topleft.y = mouse.world_position.y;
		}
	}
	else if (mouse.Left_Button.key_event == Key_Event::Released)
	{
		if (imgui_io.WantCaptureMouse == false)
		{
			const Vector2f screen_space_rect_topleft = WorldToScreenCoords(Vector2d{selection_rectangle.topleft.x, selection_rectangle.topleft.y});
			if ((screen_space_rect_topleft - mouse.screen_position_normalized).get_length() > 0.05f)	//rectangle select - check selection rectangle minimum size
			{
				vector<Entity*> entities_in_rect;
				for (Entity& entity : Entity::Colony)
				{
					if (entity.c_selectable_ptr != nullptr)
					{
						if (selection_rectangle.contains_point(entity.c_drawable_ptr->sprite.getPosition()))
						{
							entities_in_rect.emplace_back(&entity);
						}
					}
				}

				if (keyboard.Key_LSHIFT.pressed_down == false)
				{
					selection.clear();
					selection.select(entities_in_rect);
				}
				else //SHIFT pressed down
				{
					if (std::all_of(entities_in_rect.begin(), entities_in_rect.end(), [&](Entity* entity_ptr) //if all entities in rect already selected
					{
						return selection.is_selected(entity_ptr);
					}))
					{
						selection.deselect(entities_in_rect); //deselect them
					}
					else // at least one entity in rect is not selected
					{
						selection.select(entities_in_rect);
					}
				}
			}
			else //not rectangle select
			{
				for (Entity& entity : Entity::Colony)
				{
					if (entity.c_collidable_ptr != nullptr)
					{
						const bool is_clicked_on = entity.c_collidable_ptr->point_collides(mouse.world_position);
						if (is_clicked_on)
						{
							if (entity.c_selectable_ptr != nullptr)
							{
								if (keyboard.Key_LSHIFT.pressed_down == true)
								{
									if (selection.is_selected(&entity))
									{
										selection.deselect(&entity);
									}
									else // not selected
									{
										selection.select(&entity);
									}
								}
								else // NOT shift
								{
									selection.clear();
									selection.select(&entity);
								}
							}
						}
					}
				}
			}
		}
	}

	if (mouse.Right_Button.key_event == Key_Event::Pressed)
	{
		if (imgui_io.WantCaptureMouse == false)
		{
			Context_Menu::open = false;
		}
	}
	else if (mouse.Right_Button.key_event == Key_Event::Released) // right mouse button click: movement and attack orders
	{
		if (imgui_io.WantCaptureMouse == false)
		{
			if (keyboard.Key_LSHIFT.pressed_down == false) // if left SHIFT not pressed down, first clear all orders from selected entities
			{
				for (Entity* selected_entity : selection.selected_entities_ptrs)
				{
					if (selected_entity->c_spacecraft_ptr != nullptr)
					{
						selected_entity->c_spacecraft_ptr->Clear_Orders();
					}
				}
			}

			Entity* spacecraft_clicked_on = nullptr; // find if player right-clicked on any spacecraft

			for (Entity& entity : Entity::Colony)
			{
				if (entity.c_collidable_ptr != nullptr and entity.c_spacecraft_ptr != nullptr)
				{
					const bool is_clicked_on = entity.c_collidable_ptr->point_collides(mouse.world_position);
					if (is_clicked_on)
					{
						spacecraft_clicked_on = &entity;
						break;
					}
				}
			}

			if (spacecraft_clicked_on == nullptr) // no spacecraft clicked on, give move order to selected entities
			{
				for (Entity* selected_entity : selection.selected_entities_ptrs)
				{
					if (selected_entity->c_spacecraft_ptr != nullptr)
					{
						selected_entity->c_spacecraft_ptr->Orders.emplace_back(Order_Type::move,
																			   order_data::move{mouse.world_position});
					}
				}
			}
			else // spacecraft clicked on, give attack order to selected entities
			{
				for (Entity* selected_entity : selection.selected_entities_ptrs)
				{
					if (selected_entity->c_weapons_ptr != nullptr)
					{
						selected_entity->c_spacecraft_ptr->Orders.emplace_back(Order_Type::attack,
																			   order_data::attack{spacecraft_clicked_on->get_reference()});
					}
				}
			}


		}
	}

	// mouse wheel click: context menu
	if (mouse.Wheel.key_event == Key_Event::Released)
	{
		if (imgui_io.WantCaptureMouse == false)
		{
			Context_Menu::open = true;
			Context_Menu::entity_clicked_on.reset();
			Context_Menu::screen_px_position = mouse.screen_position_in_pixels;

			for (Entity& entity : Entity::Colony)
			{
				if (entity.c_collidable_ptr and not entity.c_star_system_ptr)
				{
					const bool is_clicked_on = entity.c_collidable_ptr->point_collides(mouse.world_position);
					if (is_clicked_on)
					{
						Context_Menu::entity_clicked_on = entity.get_reference();
					}

				}
			}



		}
	}

	if (mouse.Left_Button.pressed_down)	// mouse left down
	{
		
		if (imgui_io.WantCaptureMouse == false)
		{
			selection_rectangle.size.x = mouse.world_position.x - selection_rectangle.topleft.x;
			selection_rectangle.size.y = mouse.world_position.y - selection_rectangle.topleft.y;

			if (selection_rectangle.size.y != 0.0 or selection_rectangle.size.x != 0.0)
			{
				Linelist line(Colors::Blue, Draw_Layer::GUI_Lower, Blend_Mode::Opaque);
				vector<Vector2d>& vertices = line.vertices;

				vertices.reserve(8);

				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x, selection_rectangle.topleft.y});
				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x + selection_rectangle.size.x, selection_rectangle.topleft.y});

				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x + selection_rectangle.size.x, selection_rectangle.topleft.y});
				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x + selection_rectangle.size.x, selection_rectangle.topleft.y + selection_rectangle.size.y});

				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x + selection_rectangle.size.x, selection_rectangle.topleft.y + selection_rectangle.size.y});
				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x, selection_rectangle.topleft.y + selection_rectangle.size.y});

				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x, selection_rectangle.topleft.y + selection_rectangle.size.y});
				vertices.emplace_back(Vector2d{selection_rectangle.topleft.x, selection_rectangle.topleft.y});

				line.Enqueue();
			}
		}


	}
	else //left mouse button not pressed down
	{
		selection_rectangle.topleft.x = mouse.world_position.x;
		selection_rectangle.topleft.y = mouse.world_position.y;

		selection_rectangle.size.y = 0;
		selection_rectangle.size.x = 0;
	}
}

constexpr double get_turn_radius(const double max_angular_speed, const double max_speed)
{
	return ((360.0 / rad_to_deg(max_angular_speed)) * max_speed) / (2.0 * PI);
}

constexpr double get_stop_radius(const double turn_radius)
{
	return turn_radius * 0.1;
}

void Process_Keyboard_Input()
{
	if (keyboard.Key_ESCAPE.key_event == Key_Event::Pressed)
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	if (keyboard.Key_C.key_event == Key_Event::Pressed)
	{
		print("Crashing");
		std::cin.ignore();
	}
	if (keyboard.Key_ADD.key_event == Key_Event::Pressed)
	{
		camera.zoom_leading *= 0.5;
	}
	if (keyboard.Key_SUBTRACT.key_event == Key_Event::Pressed)
	{
		camera.zoom_leading *= 2.0;
	}

	if (keyboard.Key_D.key_event == Key_Event::Pressed)	// debug
	{
		debug = not debug;
	}

	if (keyboard.Key_P.key_event == Key_Event::Pressed)	// pause
	{
		paused = true;
	}

	if (keyboard.Key_E.key_event == Key_Event::Pressed)	// explosion
	{
		Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{mouse.world_position, get_random_real_in_range(0.0, 3.0), {100.0, 100.0}, Colors::White, name_animation_map["exp_1"],
															Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.3f}, 800.0f});
		get<Animation>(Systems[0]->c_star_system_ptr->Effects.back().sprite.graphics).looping = true;
	}

	if (keyboard.Key_R.key_event == Key_Event::Pressed)	// explosion nonlooping
	{
		Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{mouse.world_position, get_random_real_in_range(0.0, 3.0), {100.0, 100.0}, Colors::White, name_animation_map["exp_1"],
															Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.3f}, 800.0f});
	}

	if (keyboard.Key_A.key_event == Key_Event::Pressed)	// attack
	{
		auto& spacecraft_entities = Systems[0]->c_star_system_ptr->spacecraft_entity_ptrs;

		for (int i = 0; i < spacecraft_entities.size() - 1; ++i)
		{
			spacecraft_entities[i]->c_spacecraft_ptr->Orders.emplace_back(Order_Type::attack,
																  order_data::attack{spacecraft_entities[i + 1]->get_reference()});
		}

	}

	if (keyboard.Key_S.key_event == Key_Event::Pressed)	// stop
	{
		for (Entity* entity : selection.selected_entities_ptrs)
		{
			if (entity->c_spacecraft_ptr != nullptr)
			{
				entity->c_spacecraft_ptr->Clear_Orders();
			}

			if (entity->c_weapons_ptr != nullptr)
			{
				entity->c_weapons_ptr->Stop();
			}

			if (entity->c_engines_ptr != nullptr)
			{
				entity->c_engines_ptr->Stop();
			}
		}
	}

	if (keyboard.Key_B.key_event == Key_Event::Pressed)
	{
		for (Entity* selected_entity : selection.selected_entities_ptrs)
		{
			if (selected_entity->c_spacecraft_ptr != nullptr and selected_entity->c_constructor_ptr != nullptr)
			{
				selected_entity->c_spacecraft_ptr->Orders.emplace_back(Order_Type::construct,
																	   order_data::construct{"GTSC Faustus",
																	   mouse.world_position});
			}
		}
	}
}

void Process_Mouse_Movement()
{
	// Camera movement if mouse at border

	const double mouse_pos_relative_x = mouse.screen_position_in_pixels.x / (double(screen_resolution.x) * 0.5);

	if (mouse_pos_relative_x < -0.95)
	{
		camera.position_leading.x -= camera.get_zoom() * camera_scrolling_speed;
		Context_Menu::open = false;
	}
	else if (mouse_pos_relative_x > 0.95)
	{
		camera.position_leading.x += camera.get_zoom() * camera_scrolling_speed;
		Context_Menu::open = false;
	}

	const double mouse_pos_relative_y = mouse.screen_position_in_pixels.y / (double(screen_resolution.y) * 0.5);

	if (mouse_pos_relative_y < -0.95)
	{
		camera.position_leading.y -= camera.get_zoom() * camera_scrolling_speed;
		Context_Menu::open = false;
	}
	else if (mouse_pos_relative_y > 0.95)
	{
		camera.position_leading.y += camera.get_zoom() * camera_scrolling_speed;
		Context_Menu::open = false;
	}
}

void Process_Mouse_Input()
{
	Process_Mouse_Movement();
	Process_Mouse_Buttons();
}

void Refresh_Input()
{
	keyboard.Refresh_Key_State();
	mouse.Refresh_Key_State();
	mouse.Refresh_Movement();
}

void Process_Input()
{
	Process_Keyboard_Input();
	Process_Mouse_Input();
}

//returns random system name - you dont want unique for large number of systems
string name_system(bool unique)
{
	bool already_exists;
	string name;
	do
	{
		already_exists = false;

		name = magnitudes[get_random_int_in_range<int>(0, magnitudes.size() - 1)] + " " + constellations[get_random_int_in_range<int>(0, constellations.size() - 1)];

		if (unique)
		{
			for (Entity* star_system_ptr : Systems)
			{
				if (name == star_system_ptr->c_selectable_ptr->name)
				{
					already_exists = true;
					break;
				}
			}
		}
	}
	while (already_exists == true);
	return name;
}

void create_jump_lane(Entity* origin_system_entity_ptr, Entity* destination_system_entity_ptr)
{
	//check if connection to the target system already exists..

	auto is_leading_to_dest_system = [=](Entity* jump_node_entity)
	{
		return jump_node_entity->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr == destination_system_entity_ptr;
	};

	bool already_exists = any_of(origin_system_entity_ptr->c_star_system_ptr->jump_node_entity_ptrs.begin(),
								 origin_system_entity_ptr->c_star_system_ptr->jump_node_entity_ptrs.end(),
								 is_leading_to_dest_system);

	if (already_exists == false) //if not, proceed to create connection..
	{
		//find random positions in origin and destination systems
		auto position_origin_node = random_point_inside_circle(origin_system_entity_ptr->c_drawable_ptr->sprite.getPosition(),
													origin_system_entity_ptr->c_star_system_ptr->radius,
													origin_system_entity_ptr->c_star_system_ptr->radius / 20);

		auto position_destination_node = random_point_inside_circle(destination_system_entity_ptr->c_drawable_ptr->sprite.getPosition(),
													destination_system_entity_ptr->c_star_system_ptr->radius,
													destination_system_entity_ptr->c_star_system_ptr->radius / 20);

			//spawn jump nodes

		//origin node
		auto origin_jump_node_entity_it = Entity::Colony.emplace();
		origin_jump_node_entity_it->c_drawable_ptr =
			&*Component::Drawable::Colony.emplace(&*origin_jump_node_entity_it,
												  Sprite{Shader_Lighting_Mode::no_lighting, {0.25f, 0.25f, 0.25f, 1.0f}, Draw_Layer::Background_and_Universe, *name_animation_map["warpmap"], Blend_Mode::Additive,
													position_origin_node, 0, Vector2d(jump_node_radius, jump_node_radius)},
												  Icon{Colors::Green, Draw_Layer::Background_and_Universe, Blend_Mode::Additive, name_texture_atlas_subrect_map["icon_node"], Vector2d(20, 20)});
		origin_jump_node_entity_it->c_collidable_ptr =
			&*Component::Collidable::Colony.emplace(&*origin_jump_node_entity_it,
													jump_node_radius);
		origin_jump_node_entity_it->c_selectable_ptr =
			&*Component::Selectable::Colony.emplace(&*origin_jump_node_entity_it,
													"Jump Node");
		origin_jump_node_entity_it->c_jump_node_ptr =
			&*Component::Jump_Node::Colony.emplace(&*origin_jump_node_entity_it,
													100);
		origin_jump_node_entity_it->c_jump_inhibition_field_ptr =
			&*Component::Jump_Inhibition_Field::Colony.emplace(&*origin_jump_node_entity_it,
															   5000.0, true
			);

		origin_system_entity_ptr->c_star_system_ptr->jump_node_entity_ptrs.emplace_back(&*origin_jump_node_entity_it);

		
		//destination node
		auto destination_jump_node_entity_it = Entity::Colony.emplace();
		destination_jump_node_entity_it->c_drawable_ptr =
			&*Component::Drawable::Colony.emplace(&*destination_jump_node_entity_it,
													Sprite{Shader_Lighting_Mode::no_lighting, {0.25f, 0.25f, 0.25f, 1.0f}, Draw_Layer::Background_and_Universe, *name_animation_map["warpmap"], Blend_Mode::Additive,
													position_destination_node, 0, Vector2d(jump_node_radius, jump_node_radius)},
												  Icon{Colors::Green, Draw_Layer::Background_and_Universe, Blend_Mode::Additive, name_texture_atlas_subrect_map["icon_node"], Vector2d(20, 20)});
		destination_jump_node_entity_it->c_collidable_ptr =
			&*Component::Collidable::Colony.emplace(&*destination_jump_node_entity_it,
													jump_node_radius);
		destination_jump_node_entity_it->c_selectable_ptr =
			&*Component::Selectable::Colony.emplace(&*destination_jump_node_entity_it,
													"Jump Node");
		destination_jump_node_entity_it->c_jump_node_ptr =
			&*Component::Jump_Node::Colony.emplace(&*destination_jump_node_entity_it,
													100);
		destination_jump_node_entity_it->c_jump_inhibition_field_ptr =
			&*Component::Jump_Inhibition_Field::Colony.emplace(&*destination_jump_node_entity_it,
															   5000.0, true
			);

		destination_system_entity_ptr->c_star_system_ptr->jump_node_entity_ptrs.emplace_back(&*destination_jump_node_entity_it);
		

		//connect the jump nodes
		origin_jump_node_entity_it->c_jump_node_ptr->destination_node_ptr = &*destination_jump_node_entity_it;
		destination_jump_node_entity_it->c_jump_node_ptr->destination_node_ptr = &*origin_jump_node_entity_it;

		//emplace jump node lines vertices
		jump_node_lines.vertices.emplace_back(origin_jump_node_entity_it->c_drawable_ptr->sprite.getPosition());
		jump_node_lines.vertices.emplace_back(destination_jump_node_entity_it->c_drawable_ptr->sprite.getPosition());
	}
}

void spawn_systems()
{
	class SystemSortingClass //define a sorting class that accepts point coordinates and returns true if system at first offset is closer
	{
	public:
		Vector2d point;

		SystemSortingClass(Vector2d point_arg) : point(point_arg)
		{}

		bool operator() (Entity* system1, Entity* system2)
		{
			return point_squared_distance(point, system1->c_drawable_ptr->sprite.getPosition()) <
				   point_squared_distance(point, system2->c_drawable_ptr->sprite.getPosition());
		}
	};

	//Entity::Colony.reserve(num_of_systems * 2 + 1);

	vector<Entity*> system_entity_ptrs;
	system_entity_ptrs.reserve(num_of_systems);

	double min_distance = AU * 2.5; //minimum and maximum distance between systems
	double max_distance = AU * 10;
	Vector2d generated_pos;
	bool bad_position;

	for (int i = 0; i < num_of_systems; ++i)
	{
		do
		{
			generated_pos.x = get_random_real_in_range(-(galaxy_size.x / 2), galaxy_size.x / 2); //generate new random coordinates
			generated_pos.y = get_random_real_in_range(-(galaxy_size.y / 2), galaxy_size.y / 2);

			bad_position = false;

			if (generated_pos.x < -(galaxy_size.x / 2) + min_distance or
				generated_pos.x >(galaxy_size.x / 2 - min_distance) or
				generated_pos.y < -(galaxy_size.y / 2) + min_distance or
				generated_pos.y >(galaxy_size.y / 2 - min_distance)) //check if too close to boundary
			{
				bad_position = true;
			}
			else if (Systems.empty() == false) //check if too close to other systems, if they exist
			{
				sort(system_entity_ptrs.begin(), system_entity_ptrs.end(), SystemSortingClass(generated_pos));

				Vector2d closest_system_pos = system_entity_ptrs[0]->c_drawable_ptr->sprite.getPosition();

				auto distance_to_closest = point_distance(generated_pos, closest_system_pos);
				if (distance_to_closest < min_distance or distance_to_closest > max_distance)
				{
					bad_position = true;
				}
			}

		}
		while (bad_position); //if its too close, repeat coordinate generation and check again

		//spawn system
		
		auto entity_it = Entity::Colony.emplace();
		entity_it->c_drawable_ptr =
			&*Component::Drawable::Colony.emplace(&*entity_it,
													Sprite{Shader_Lighting_Mode::no_lighting, Color(0.0f, 0.5f, 1.0f, 1.0f), Draw_Layer::Background_and_Universe,
													name_texture_atlas_subrect_map["hex"], Blend_Mode::Alpha,
													generated_pos, 0.0, Vector2d(2.1 * AU, 2.1 * AU * 1.15445321307779)});
		entity_it->c_collidable_ptr =
			&*Component::Collidable::Colony.emplace(&*entity_it,
													collision_square);
		entity_it->c_selectable_ptr =
			&*Component::Selectable::Colony.emplace(&*entity_it,
													name_system(false));
		entity_it->c_star_system_ptr =
			&*Component::Star_System::Colony.emplace(&*entity_it,
													AU);

		

		Entity* just_created_system = &*entity_it;

		system_entity_ptrs.emplace_back(just_created_system);

		if (Systems.size() > 1)
		{
			create_jump_lane(just_created_system, system_entity_ptrs[0]); //connect to closest system

			if (Systems.size() > 2)
			{
				if (get_random_real_in_range(0.0f, 1.0f) < 0.333f and
					point_distance(just_created_system->c_drawable_ptr->sprite.getPosition(), system_entity_ptrs[1]->c_drawable_ptr->sprite.getPosition()) < max_distance)
				{
					create_jump_lane(just_created_system, system_entity_ptrs[1]); //connect to second closest system
				}
			}
		}
	}

	print(runtime("	Spawning systems finished. Final # of System entities: " + to_string(system_entity_ptrs.size()) + NEWL));
}

void spawn_celestial_bodies()
{
	for (Entity* system_entity_ptr : Systems)
	{
		Vector2d system_center_pos = system_entity_ptr->c_drawable_ptr->sprite.getPosition();

		int num_stars = 1;
		for (int i = 0; i < num_stars; ++i)
		{
			auto star_entity = Entity::Colony.emplace();
			star_entity->c_drawable_ptr = &*Component::Drawable::Colony.emplace(&*star_entity,
																				Sprite{Shader_Lighting_Mode::normal_lighting, Colors::White, Draw_Layer::Background_and_Universe,
																				name_texture_atlas_subrect_map["star_yellow"], Blend_Mode::Alpha,
																				system_center_pos, 0, Vector2d(695700000.0 * 2.0, 695700000.0 * 2.0)},
																				Icon{Colors::Green, Draw_Layer::Background_and_Universe, Blend_Mode::Additive, name_texture_atlas_subrect_map["icon_planet"],
																				Vector2d(20, 20)});
			star_entity->c_collidable_ptr = &*Component::Collidable::Colony.emplace(&*star_entity,
																				collision_square);
			star_entity->c_selectable_ptr = &*Component::Selectable::Colony.emplace(&*star_entity,
																					"Star");
			star_entity->c_star_ptr = &*Component::Star::Colony.emplace(&*star_entity,
																		Light{false,  Color(1.0f, 1.0f, 1.0f), AU * 80'000.0f, 1.5f, system_center_pos});
			system_entity_ptr->c_star_system_ptr->star_entity_ptrs.emplace_back(&*star_entity);
		}

		int num_planets = get_random_int_in_range(0, 10);
		for (int i = 0; i < num_planets; ++i)
		{
			auto planet_pos = random_point_inside_circle(system_center_pos, system_entity_ptr->c_star_system_ptr->radius, system_entity_ptr->c_star_system_ptr->radius / 20);

			auto planet_entity = Entity::Colony.emplace();
			planet_entity->c_drawable_ptr = &*Component::Drawable::Colony.emplace(&*planet_entity,
																				  Sprite{Shader_Lighting_Mode::normal_lighting, Colors::White, Draw_Layer::Background_and_Universe,
																				  name_texture_atlas_subrect_map["planet"], Blend_Mode::Alpha,
																				  planet_pos, 0, Vector2d(20000000.0, 20000000.0)},
																				  Icon{Colors::Green, Draw_Layer::Background_and_Universe, Blend_Mode::Additive, name_texture_atlas_subrect_map["icon_planet"],
																				  Vector2d(20, 20)});
			planet_entity->c_collidable_ptr = &*Component::Collidable::Colony.emplace(&*planet_entity,
																					collision_square);
			planet_entity->c_selectable_ptr = &*Component::Selectable::Colony.emplace(&*planet_entity,
																					"Planet");
			planet_entity->c_planet_ptr = &*Component::Planet::Colony.emplace(&*planet_entity, 0);


			system_entity_ptr->c_star_system_ptr->planet_entity_ptrs.emplace_back(&*planet_entity);
		}
	}
}