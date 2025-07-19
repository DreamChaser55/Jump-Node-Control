#pragma once

#include "Engine.h"

class Entity;

class Entity_Ref
{
public:
	Entity* ptr;
	int64_t id;

	bool is_valid() const;
};

namespace Context_Menu
{
	float size_x = float(screen_resolution.x) * 0.07f;
	bool open = false;
	Vector2f screen_px_position; // screen center at (0, 0)
	optional<Entity_Ref> entity_clicked_on;
}

class Selection final
{
public:
	unordered_set<Entity*> selected_entities_ptrs;

	inline void clear() //deselect all entities
	{
		selected_entities_ptrs.clear();
	}

	inline void deselect(Entity* entity_ptr) //deselect specific entity
	{
		selected_entities_ptrs.erase(entity_ptr);
	}

	inline void deselect(vector<Entity*> entity_ptrs) //deselect multiple entities
	{
		for (Entity* entity_ptr : entity_ptrs)
		{
			selected_entities_ptrs.erase(entity_ptr);
		}
	}

	inline void select(Entity* entity_ptr) //select specific entity
	{
		selected_entities_ptrs.emplace(entity_ptr);
	}

	inline void select(vector<Entity*> entity_ptrs) //select multiple entities
	{
		selected_entities_ptrs.insert(entity_ptrs.begin(), entity_ptrs.end());
	}

	inline bool is_selected(Entity* entity_ptr) const
	{
		return selected_entities_ptrs.count(entity_ptr) == 1;
	}
};
Selection selection;

enum class Weapon_Type
{
	Mass_driver, Missile_launcher, Beam_emitter
};

enum class Weapon_Name
{
	Ravager_Cannon, Stinger_Missile, Winter_King_Beam
};

struct TurretTemplate
{
	string name;
	Weapon_Type weapon_type_arg;
	Weapon_Name weapon_name_arg;
	Vector2d position_offset_local_arg;
};

namespace Spacecraft_Component_Definition
{
	struct Jump_Drive
	{
		float charge;
		float charge_rate;
		double min_range;
		double max_range;
		bool intersystem;
	};

	struct Engines
	{
		double max_speed;
		double acceleration;
		double max_angular_velocity;
	};

	struct Jump_Inhibition_Field
	{
		double radius;
	};

	struct Constructor
	{
		float construction_speed_multiplier;
	};
}

struct EntityTemplate
{
	string name;

	vector<Vector2d> collision_polygon_texture_coords;
	Vector2d scaling;
	variant<Texture_Atlas_Subrect*, Animation> graphics;
	Texture_Atlas_Subrect* glowmap_subrect_ptr;

	int crew;
	float max_hull_points;

	vector<TurretTemplate> Turrets;

	optional<Spacecraft_Component_Definition::Engines> engines;
	optional<Spacecraft_Component_Definition::Jump_Drive> jump_drive;
	optional<Spacecraft_Component_Definition::Jump_Inhibition_Field> ji_field;
	optional<Spacecraft_Component_Definition::Constructor> constructor;

};

unordered_map<string, EntityTemplate> entityTemplateMap;

namespace order_data
{
	class move
	{
	public:
		Vector2d destination;

		move()
		{}

		move(Vector2d destination_a) :
			destination(destination_a)
		{}

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class move_sublight
	{
	public:
		Vector2d destination;

		move_sublight()
		{}

		move_sublight(Vector2d destination_a) :
			destination(destination_a)
		{}

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class jump_intrasystem
	{
	public:
		Vector2d destination;

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class jump_intersystem
	{
	public:
		Entity* destination_system;

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class construct
	{
	public:
		string entity_template; // The template of the entity to build.
		Vector2d location;
		bool construction_started = false; // Flag to avoid starting construction multiple times

		construct(string template_arg, Vector2d location_arg) :
			entity_template(template_arg),
			location(location_arg)
		{}

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class colonize
	{
	public:
		int64_t target_massive_body_offset;

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};

	class attack
	{
	public:
		Entity_Ref target;

		string get_string() const;

		void clear(Entity* parent_entity_arg);

		void execute(Entity* parent_entity_arg);

		bool is_finished(Entity* parent_entity_arg) const;
	};
}

enum class Order_Type
{
	move, move_sublight, jump_intrasystem, jump_intersystem, construct, colonize, attack
};

string get_order_type_string(const Order_Type type)
{
	switch (type)
	{
		case Order_Type::move:
		{
			return "move";
		}
		break;

		case Order_Type::move_sublight:
		{
			return "move_sublight";
		}
		break;

		case Order_Type::construct:
		{
			return "construct";
		}
		break;

		case Order_Type::colonize:
		{
			return "colonize";
		}
		break;

		case Order_Type::jump_intrasystem:
		{
			return "jump_intrasystem";
		}
		break;

		case Order_Type::jump_intersystem:
		{
			return "jump_intersystem";
		}
		break;

		case Order_Type::attack:
		{
			return "attack";
		}
		break;

		default:
		{
			return "ERROR: No order type string!!!";
		}
		break;
	}
}

string get_order_state_string(const Order_Status state)
{
	switch (state)
	{
		case Order_Status::Unopened:
		{
			return "Unopened";
		}
		break;

		case Order_Status::In_Progress:
		{
			return "In Progress";
		}
		break;

		case Order_Status::Finished:
		{
			return "Finished";
		}
		break;
	}
}

class get_order_string
{
public:

	template <typename T>
	string operator()(T& order_data) const
	{
		return order_data.get_string();
	}
};

class order_finish_checker
{
public:
	Entity* parent_entity_ptr;

	order_finish_checker(Entity* parent_entity_arg) : parent_entity_ptr(parent_entity_arg)
	{}

	template <typename T>
	Order_Status operator()(T& order_data)
	{
		if (order_data.is_finished(parent_entity_ptr))
		{
			return Order_Status::Finished;
		}
		else
		{
			return Order_Status::In_Progress;
		}
	}
};

class order_cleaner
{
public:
	Entity* parent_entity_ptr;

	order_cleaner(Entity* parent_entity_arg) : parent_entity_ptr(parent_entity_arg)
	{}

	template <typename T>
	void operator()(T& order_data)
	{
		order_data.clear(parent_entity_ptr);
	}
};

class order_executor
{
public:
	Entity* parent_entity_ptr;

	order_executor(Entity* parent_entity_arg) : parent_entity_ptr(parent_entity_arg)
	{}

	template <typename T>
	void operator()(T& order_data)
	{
		order_data.execute(parent_entity_ptr);
	}
};

class Order final
{
public:
	Order_Type type;
	Order_Status state{Order_Status::Unopened};
	variant<
		order_data::move,
		order_data::move_sublight,
		order_data::jump_intrasystem,
		order_data::jump_intersystem,
		order_data::construct,
		order_data::colonize,
		order_data::attack
			> data;
	deque<Order> Suborders;

	template <typename T>
	Order(const Order_Type type_a, T variant_arg) : type(type_a), data(variant_arg)
	{}

	Order* get_current_order()
	{
		if (Suborders.empty())
		{
			return this;
		}
		else
		{
			return Suborders.front().get_current_order();
		}
	}

	string get_string()
	{
		return get_order_type_string(type) + NEWL +
			visit(get_order_string(), data) + NEWL +
			"Suborders #: " + to_string(Suborders.size()) + NEWL +
			"State: " + get_order_state_string(state);
	}

	void Delete_Finished_Suborders()
	{
		auto is_suborder_finished = [&](Order& suborder)
		{
			return suborder.state == Order_Status::Finished;
		};

		Suborders.erase(remove_if(Suborders.begin(), Suborders.end(), is_suborder_finished),
							  Suborders.end());

		if (Suborders.empty() == false)
		{
			Suborders.front().Delete_Finished_Suborders();
		}
	}

	void Check_if_finished(Entity* parent_entity_ptr)
	{
		order_finish_checker finish_checker{parent_entity_ptr};
		state = visit(finish_checker, data);
	}

	void Clear(Entity* parent_entity_ptr)
	{
		order_cleaner cleaner{parent_entity_ptr};
		visit(cleaner, data);
		state = Order_Status::Finished;
	}

	void Execute(Entity* parent_entity_ptr)
	{
		order_executor executor{parent_entity_ptr};
		visit(executor, data);
		state = Order_Status::In_Progress;
	}
};

class Turret;

class Projectile
{
public:
	Sprite_Translatable_Only sprite;
	Entity_Ref origin_entity;
	Entity_Ref target_entity;
	float damage;
	Vector2d velocity;
	float lifetime;
	Light light;

	Projectile()
	{}

	Projectile(Entity_Ref origin_entity_arg,
			   Entity_Ref target_entity_arg,
			   float damage_arg,
			   Vector2d& velocity_arg,
			   float lifetime_arg,
			   Color sprite_color_arg,
			   Texture_Atlas_Subrect* texture_subrect_arg,
			   Blend_Mode blending_type_arg,
			   Vector2d position_arg,
			   double rotation_arg,
			   Vector2d scaling_arg,
			   Color light_diffuse_arg,
			   float light_intensity_arg) :
		sprite(Shader_Lighting_Mode::no_lighting, sprite_color_arg, Draw_Layer::Effects_Higher, texture_subrect_arg, blending_type_arg,
			   position_arg, rotation_arg, scaling_arg),
		origin_entity(origin_entity_arg),
		target_entity(target_entity_arg),
		damage(damage_arg),
		velocity(velocity_arg),
		lifetime(lifetime_arg),
		light(false, light_diffuse_arg, light_intensity_arg, 1.5, position_arg)
	{}


	bool Step(); // return true if projectile needs to be destroyed

};

class Missile
{
public:
	Sprite sprite;
	Entity_Ref origin_entity;
	Entity_Ref target_entity;
	float damage;
	double speed;
	double turn_rate;
	float lifetime;
	Light light;

	Missile()
	{}

	Missile(Entity_Ref origin_entity_arg,
			   Entity_Ref target_entity_arg,
			   float damage_arg,
			   double speed_arg,
			   double turn_rate_arg,
			   float lifetime_arg,
			   Color sprite_color_arg,
			   Texture_Atlas_Subrect* texture_subrect_arg,
			   Blend_Mode blending_type_arg,
			   Vector2d position_arg,
			   double rotation_arg,
			   Vector2d scaling_arg,
			   Color light_diffuse_arg,
			   float light_intensity_arg) :
		sprite(Shader_Lighting_Mode::no_lighting, sprite_color_arg, Draw_Layer::Effects_Higher, texture_subrect_arg, blending_type_arg,
			   position_arg, rotation_arg, scaling_arg),
		origin_entity(origin_entity_arg),
		target_entity(target_entity_arg),
		damage(damage_arg),
		speed(speed_arg),
		lifetime(lifetime_arg),
		turn_rate(turn_rate_arg),
		light(false, light_diffuse_arg, light_intensity_arg, 1.5, position_arg)
	{}

	bool Step(); // return true if missile needs to be destroyed
};

class Beam
{
public:
	Sprite sprite;
	Entity_Ref origin_entity;
	Turret* origin_turret;
	Entity_Ref target_entity;
	Vector2f endpoint1_offset;
	Vector2f endpoint2_offset;
	Vector2f current_endpoint_offset;
	float damage;
	float lifetime;
	float remaining_lifetime;
	Light light;
	Sprite_Translatable_Only muzzle_glow;

	Beam(Entity_Ref origin_entity_arg,
		 Turret* origin_turret_arg,
		 Entity_Ref target_entity_arg,
		 Vector2f endpoint1_offset_arg,
		 Vector2f endpoint2_offset_arg,
		 float damage_arg,
		 float lifetime_arg,
		 Color light_diffuse_arg,
		 float light_intensity_arg,
		 Color color_arg);

	Beam()
	{}

	bool Step(); // return true if beam needs to be destroyed
};

class Turret
{
public:
	
	Weapon_Type weapon_type;
	Weapon_Name weapon_name;
	float reload_timer;
	float reload_time;
	float hit_points{100};
	Entity* parent_spacecraft_ptr = nullptr;
	Vector2d position_offset_local;
	Entity_Ref target_entity;
	Sprite sprite;

	Turret()
	{}

	Turret(Weapon_Type weapon_type_arg, Weapon_Name weapon_name_arg, float reload_time_arg, Entity* parent_spacecraft_ptr_arg,
		   Vector2d offset_arg, Texture_Atlas_Subrect* sprite_texture_subrect_arg, Vector2d sprite_scaling_arg);
};