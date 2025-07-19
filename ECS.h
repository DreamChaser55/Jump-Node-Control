#include <typeinfo>
#include <D:\Programming\CPP\Sources\plf_colony-master\plf_colony.h>
#include "Classes.h"

using plf::colony;

class Entity;
class Order;

vector<Entity*> Systems;

class Component_Base
{
public:
	Entity* parent_entity_ptr;

	Component_Base(Entity* parent_entity_ptr_arg) :
		parent_entity_ptr(parent_entity_ptr_arg)
	{}
};

namespace Component // add new component here
{
	class Drawable : public Component_Base
	{
	public:
		Sprite sprite;
		optional<Icon> icon;
		Active_Sprite active_sprite = Active_Sprite::Normal;
		Entity* in_system_ptr = nullptr;

		Drawable(Entity* parent_entity_ptr_arg,
				 Sprite sprite_arg,
				 optional<Icon> icon_arg = std::nullopt);

		void Draw();

		void Destroy();

		static colony<Drawable> Colony;
	};
	colony<Drawable> Drawable::Colony;


	class Collidable : public Component_Base
	{
	public:
		variant<double, vector<Vector2d>> radius_or_polygon;

		Collidable(Entity* parent_entity_ptr_arg,
				   variant<double, vector<Vector2d>> radius_or_polygon_arg) :
			Component_Base(parent_entity_ptr_arg),
			radius_or_polygon(radius_or_polygon_arg)
		{}

		bool point_collides(Vector2d point);

		static colony<Collidable> Colony;
	};
	colony<Collidable> Collidable::Colony;

	class Dynamic : public Component_Base
	{
	public:
		Vector2d velocity;
		double angular_velocity;

		Dynamic(Entity* parent_entity_ptr_arg,
				Vector2d velocity_arg,
				double angular_velocity_arg) :
			Component_Base(parent_entity_ptr_arg),
			velocity(velocity_arg),
			angular_velocity(angular_velocity_arg)
		{}

		void Step();

		static colony<Dynamic> Colony;
	};
	colony<Dynamic> Dynamic::Colony;

	class Engines : public Component_Base
	{
	public:
		optional<Vector2d> destination;
		double max_speed;
		double acceleration;
		double max_angular_velocity;

		Engines(Entity* parent_entity_ptr_arg,
				double max_speed_arg,
				double acceleration_arg,
				double max_angular_speed_arg) :
			Component_Base(parent_entity_ptr_arg),
			max_speed(max_speed_arg),
			acceleration(acceleration_arg),
			max_angular_velocity(max_angular_speed_arg)
		{}

		void Step();

		void Stop();

		static colony<Engines> Colony;
	};
	colony<Engines> Engines::Colony;

	class Weapons : public Component_Base
	{
	public:
		vector<Turret> Turrets;

		Weapons(Entity* parent_entity_ptr_arg) :
			Component_Base(parent_entity_ptr_arg)
		{}

		void Step();

		void Attack_Entity(Entity_Ref& entity_ref);

		void Stop();

		static colony<Weapons> Colony;
	};
	colony<Weapons> Weapons::Colony;

	class Selectable : public Component_Base
	{
	public:
		string name;

		Selectable(Entity* parent_entity_ptr_arg,
				   string name_arg) :
			Component_Base(parent_entity_ptr_arg),
			name(name_arg)
		{}

		void Draw_Sidebar_Info();

		void Draw_Selection_Brackets();

		void Destroy();

		static colony<Selectable> Colony;
	};
	colony<Selectable> Selectable::Colony;

	class Star_System : public Component_Base
	{
	public:
		double radius;

		vector<Entity*> drawable_entity_ptrs;
		vector<Entity*> star_entity_ptrs;
		vector<Entity*> planet_entity_ptrs;
		vector<Entity*> jump_node_entity_ptrs;
		vector<Entity*> spacecraft_entity_ptrs;
		vector<Entity*> jump_inhibition_entity_ptrs;

		vector<Particle> Particles;
		vector<Effect> Effects;

		vector<Projectile> Projectiles;
		vector<Missile> Missiles;
		vector<Beam> Beams;

		Star_System(Entity* parent_entity_ptr_arg,
					double radius_arg);

		void Draw();

		void Process_Particles();

		void Process_Effects();

		void Process_Ordnance();

		static colony<Star_System> Colony;
	};
	colony<Star_System> Star_System::Colony;

	class Jump_Node : public Component_Base
	{
	public:
		float stability;
		Entity* destination_node_ptr = nullptr;

		Jump_Node(Entity* parent_entity_ptr_arg,
				  float stability_arg) :
			Component_Base(parent_entity_ptr_arg),
			stability(stability_arg)
		{}

		static colony<Jump_Node> Colony;
	};
	colony<Jump_Node> Jump_Node::Colony;

	class Star : public Component_Base
	{
	public:
		Light light;

		Star(Entity* parent_entity_ptr_arg,
			 Light light_arg) :
			Component_Base(parent_entity_ptr_arg),
			light(light_arg)
		{}

		static colony<Star> Colony;
	};
	colony<Star> Star::Colony;

	class Planet : public Component_Base
	{
	public:
		uint64_t population;

		Planet(Entity* parent_entity_ptr_arg,
			   uint64_t population_arg) :
			Component_Base(parent_entity_ptr_arg),
			population(population_arg)
		{}

		static colony<Planet> Colony;
	};
	colony<Planet> Planet::Colony;

	class Spacecraft : public Component_Base
	{
	public:
		int crew = 0;
		float hull_points = 1000.0f;
		float max_hull_points = 1000.0f;
		bool under_construction = false;
		deque<Order> Orders;
		Order* current_order = nullptr;

		Spacecraft(Entity* parent_entity_ptr_arg,
				   int crew_arg);

		void Clear_Orders();

		void Process_orders();

		bool Step() const;

		void Destroy();

		static colony<Spacecraft> Colony;

	private:
		Order* get_current_order();
	};
	colony<Spacecraft> Spacecraft::Colony;


	class Jump_Drive : public Component_Base
	{
	public:
		float charge;
		float charge_rate;
		double min_range;
		double max_range;
		bool intersystem;

		variant<nullptr_t, Vector2d, Entity*> destination;

		Jump_Drive(Entity* parent_entity_ptr_arg,
							   float charge_arg,
							   float charge_rate_arg,
							   double min_range_arg,
							   double max_range_arg,
				   bool intersystem_arg);

		void Step();

		static colony<Jump_Drive> Colony;
	};
	colony<Jump_Drive> Jump_Drive::Colony;


	class Jump_Inhibition_Field : public Component_Base
	{
	public:
		double radius;
		bool active = true;

		Jump_Inhibition_Field(Entity* parent_entity_ptr_arg,
				   double radius_arg,
				   bool active_arg);

		void Draw();

		void Destroy();

		static colony<Jump_Inhibition_Field> Colony;
	};
	colony<Jump_Inhibition_Field> Jump_Inhibition_Field::Colony;

	class Constructor : public Component_Base
	{
	public:
		Entity* entity_under_construction_ptr = nullptr; // Optional: could be used for visualization or targeting
		float construction_speed_multiplier = 1.0f;

		Constructor(Entity* parent_entity_ptr_arg) :
			Component_Base(parent_entity_ptr_arg)
		{}

		void Build(string entity_template_arg, Vector2d location);

		void Step();

		static colony<Constructor> Colony;
	};
	colony<Constructor> Component::Constructor::Colony;
}

class Entity final
{
public:
	static int64_t id_counter;
	int64_t id;

	Component::Drawable*				c_drawable_ptr				= nullptr; // add new component here
	Component::Collidable*				c_collidable_ptr			= nullptr;
	Component::Dynamic*					c_dynamic_ptr				= nullptr;
	Component::Engines*					c_engines_ptr				= nullptr;
	Component::Selectable*				c_selectable_ptr			= nullptr;
	Component::Star_System*				c_star_system_ptr			= nullptr;
	Component::Jump_Node*				c_jump_node_ptr				= nullptr;
	Component::Star*					c_star_ptr					= nullptr;
	Component::Planet*					c_planet_ptr				= nullptr;
	Component::Spacecraft*				c_spacecraft_ptr			= nullptr;
	Component::Weapons*					c_weapons_ptr				= nullptr;
	Component::Jump_Drive*				c_jump_drive_ptr		    = nullptr;
	Component::Jump_Inhibition_Field*	c_jump_inhibition_field_ptr	= nullptr;
	Component::Constructor*				c_constructor_ptr			= nullptr;

	Entity() :
		id(++id_counter)
	{
		Valid_ids.insert(id);
	}

	Entity_Ref get_reference()
	{
		return Entity_Ref{this, id};
	}

	bool Step();

	void Destroy_Components();
	void Erase_Components();

	static colony<Entity> Colony;
	static unordered_set<int64_t> Valid_ids;
};
int64_t Entity::id_counter = 0;
colony<Entity> Entity::Colony;
unordered_set<int64_t> Entity::Valid_ids;


//Definitions

void Component::Drawable::Draw()
{
	if (sprite.graphics.index() == 1)
	{
		Animation& animation = get<Animation>(sprite.graphics);
		animation.advance();
	}

	if (icon)
	{
		//icon->sprite.texture_subrect.advance();

		if (icon->sprite.graphics.index() == 1)
		{
			Animation& animation = get<Animation>(icon->sprite.graphics);
			animation.advance();
		}

		if (camera.zoom_changed)
		{
			icon->sprite.setScaling(icon->size_on_screen * camera.get_zoom());
		}

		if (camera.scaling.x < icon_culling_view_size)
		{
			if (icon->sprite.getScaling().get_length() > sprite.getScaling().get_length()) //if icon sprite is larger than drawable sprite
			{
				active_sprite = Active_Sprite::Icon; //draw only icon sprite
			}
			else //icon is smaller than drawable
			{
				active_sprite = Active_Sprite::Normal; //draw only drawable sprite
			}
		}
		else //draw nothing
		{
			active_sprite = Active_Sprite::None;
		}
	}

	if (in_system_ptr == nullptr) //system is NOT drawing this drawable
	{
		switch (active_sprite) //draw previously determined active sprite - normal sprite or icon (or nothing)
		{
			case Active_Sprite::Normal:
				sprite.Enqueue();
				break;

			case Active_Sprite::Icon:
				icon->sprite.Enqueue();
				break;
		}
	}
}

void Component::Drawable::Destroy()
{
	if (in_system_ptr != nullptr) //if system is drawing this drawable
	{
		auto& drawable_entity_ptrs = in_system_ptr->c_star_system_ptr->drawable_entity_ptrs; // remove from system drawable entities

		auto iter = drawable_entity_ptrs.begin();
		while (true)
		{
			if (*iter == parent_entity_ptr)
			{
				drawable_entity_ptrs.erase(iter);
				break;
			}

			++iter;
		}
	}
}

bool Component::Collidable::point_collides(Vector2d point) 
{
	Sprite& sprite = parent_entity_ptr->c_drawable_ptr->sprite;

	if (holds_alternative<vector<Vector2d>>(radius_or_polygon)) //polygon
	{
		bool is_collision_valid = point_in_polygon(get<vector<Vector2d>>(radius_or_polygon),
												   WorldToSpriteCoords(point, sprite));
		return is_collision_valid;
	}
	else //radius
	{
		bool is_collision_valid = point_distance(sprite.getPosition(), point) < get<double>(radius_or_polygon);
		return is_collision_valid;
	}
}

void Component::Dynamic::Step()
{
	//if we are moving or rotating, change position of drawable sprite (and icon, if there is one)

	Sprite& drawable_sprite = parent_entity_ptr->c_drawable_ptr->sprite;

	if (velocity.x != 0.0 or velocity.y != 0.0)
	{
		Vector2d new_position = drawable_sprite.getPosition() + velocity;

		drawable_sprite.setPosition(new_position);

		optional<Icon>& icon_opt = parent_entity_ptr->c_drawable_ptr->icon;

		if (icon_opt)
		{
			icon_opt->sprite.setPosition(new_position);
		}
	}

	if (angular_velocity != 0.0)
	{
		drawable_sprite.setRotation(drawable_sprite.getRotation() + angular_velocity);
	}
}

void Component::Engines::Stop()
{
	parent_entity_ptr->c_dynamic_ptr->velocity = Vector2d(0.0, 0.0);
	parent_entity_ptr->c_dynamic_ptr->angular_velocity = 0;
	destination = std::nullopt;
}

void Component::Engines::Step()
{
	if (destination) //destination is set
	{
		Vector2d current_position = parent_entity_ptr->c_drawable_ptr->sprite.getPosition();

		if (point_distance(destination.value(), current_position) <= max_speed/2) //movement finished
		{
			Stop();
		}
		else //movement not finished
		{
			Vector2d accel_vector = (destination.value() - current_position).get_resized(acceleration);
			parent_entity_ptr->c_dynamic_ptr->velocity += accel_vector;

			if (parent_entity_ptr->c_dynamic_ptr->velocity.get_length() > max_speed)
			{
				parent_entity_ptr->c_dynamic_ptr->velocity.resize(max_speed);
			}

			double accel_angle = vector_to_angle_rad(accel_vector);
			double angle_difference_signed = angle_difference_rad(accel_angle, parent_entity_ptr->c_drawable_ptr->sprite.getRotation());

			if (abs(angle_difference_signed) <= max_angular_velocity / 2) //rotation finished
			{
				parent_entity_ptr->c_dynamic_ptr->angular_velocity = 0;
			}
			else //rotation not finished
			{
				parent_entity_ptr->c_dynamic_ptr->angular_velocity += angle_difference_signed;

				if (abs(parent_entity_ptr->c_dynamic_ptr->angular_velocity) > max_angular_velocity)
				{
					parent_entity_ptr->c_dynamic_ptr->angular_velocity = clamp(parent_entity_ptr->c_dynamic_ptr->angular_velocity, -max_angular_velocity, max_angular_velocity);
				}
			}

			//draw destination marker
			Sprite{Shader_Lighting_Mode::no_lighting, Colors::Aqua, Draw_Layer::GUI_Lower, name_texture_atlas_subrect_map["marker"],
				Blend_Mode::Alpha, destination.value(), 0.0,{50.0, 50.0}}.Enqueue();
		}
	}

	Sprite& sprite_ref = parent_entity_ptr->c_drawable_ptr->sprite;
	Vector2d velocity_vect = angle_rad_to_vector(sprite_ref.getRotation());
	velocity_vect.normalize();
	velocity_vect.negate();
	velocity_vect *= 5.0;

	if (parent_entity_ptr->c_drawable_ptr->active_sprite == Active_Sprite::Normal)
	{
		for (int i = 0; i < 12; ++i)
		{
			double y_coord = get_random_real_in_range(-0.28, 0.28);
			Vector2d velocity_vect_scaled = velocity_vect * get_random_real_in_range(0.75, 1.25);

			parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Particles.emplace_back(Particle{SpriteToWorldCoords({-0.5, y_coord}, sprite_ref), {50.0, 50.0}, Color{0.25f, 0.30f, 1.0f, 1.0f},
																										Draw_Layer::Effects_Higher, 10, velocity_vect_scaled});
		}

		Light{false, Color{0.25f, 0.30f, 1.0f}, 600.0f, 1.8f, SpriteToWorldCoords({-0.5, -0.28}, sprite_ref), SpriteToWorldCoords({-0.5, 0.28}, sprite_ref)}.Enqueue();
	}
}

void draw_order_treenode(Order& order)
{
	auto order_str = order.get_string();

	ImVec4 color;

	switch (order.type)
	{
		case Order_Type::move:
		{
			color = {1.0, 1.0, 1.0, 1.0};
		}
		break;

		case Order_Type::move_sublight:
		{
			color = {0.5, 1.0, 0.5, 1.0};
		}
		break;

		case Order_Type::jump_intrasystem:
		{
			color = {0.5, 0.5, 1.0, 1.0};
		}
		break;

		case Order_Type::jump_intersystem:
		{
			color = {0.3, 1.0, 1.0, 1.0};
		}
		break;

		case Order_Type::attack:
		{
			color = {1.0, 0.0, 0.0, 1.0};
		}
		break;

		case Order_Type::construct:
		{
			color = {0.0, 1.0, 0.0, 1.0};
		}
		break;
	}

	ImGui::PushStyleColor(ImGuiCol_Text, color);

	if (ImGui::TreeNode(order_str.c_str()))
	{
		ImGui::PopStyleColor();

		if (order.Suborders.empty() == false)
		{
			for (Order& suborder : order.Suborders)
			{
				draw_order_treenode(suborder);
			}
		}

		ImGui::TreePop();
	}
	else
	{
		ImGui::PopStyleColor();
	}
}

void Component::Selectable::Draw_Sidebar_Info()
{
	ImGui::Begin("Sidebar");

	float sidebar_width = float(screen_resolution.x) * 0.2f;
	const char* name_c_str = parent_entity_ptr->c_selectable_ptr->name.c_str();
	float text_width = ImGui::CalcTextSize(name_c_str).x;

	ImGui::SetCursorPosX((sidebar_width - text_width) * 0.5f);

	ImGui::TextColored({1.0f, 0.5f, 0.25f, 1.0f}, name_c_str);
	ImGui::Text(" ");

	Drawable* drawable = parent_entity_ptr->c_drawable_ptr;

	ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Position:");
	ImGui::SameLine();
	ImGui::Text((format("{:.1f}", drawable->sprite.getPosition().x) + " | " + format("{:.1f}", drawable->sprite.getPosition().y) + NEWL).c_str());

	ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Orientation:");
	ImGui::SameLine();
	ImGui::Text((format("{:.3f}", drawable->sprite.getRotation()) + NEWL).c_str());

	ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "In System:");
	ImGui::SameLine();

	if (drawable->in_system_ptr != nullptr)
	{
		ImGui::Text(drawable->in_system_ptr->c_selectable_ptr->name.c_str());
	}
	else
	{
		ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "(none)");
	}
	
	if (parent_entity_ptr->c_dynamic_ptr != nullptr)
	{
		ImGui::TextColored({0.65f, 0.65f, 1.0f, 1.0f}, "Velocity:");
		ImGui::SameLine();
		ImGui::Text((format("{:.3f}", parent_entity_ptr->c_dynamic_ptr->velocity.get_length())).c_str());
	}

	if (parent_entity_ptr->c_engines_ptr != nullptr)
	{
		ImGui::TextColored({0.65f, 0.65f, 1.0f, 1.0f}, "Sublight Destination:");
		ImGui::SameLine();

		optional<Vector2d>& destination_opt = parent_entity_ptr->c_engines_ptr->destination;
		if (destination_opt)
		{
			Vector2d destination = destination_opt.value();
			ImGui::Text((format("{:.1f}", destination.x) + " | " + format("{:.1f}", destination.y)).c_str());
		}
		else
		{
			ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "(none)");
		}
	}

	if (parent_entity_ptr->c_jump_drive_ptr != nullptr)
	{
		ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, "Jump drive charge:");
		ImGui::SameLine();
		ImGui::Text(format("{:.2f}", parent_entity_ptr->c_jump_drive_ptr->charge).c_str());

		ImGui::TextColored({0.0f, 0.0f, 1.0f, 1.0f}, "Jump Destination:");
		ImGui::SameLine();

		variant<std::nullptr_t, Vector2d, Entity*>& destination_variant = parent_entity_ptr->c_jump_drive_ptr->destination;
		if (holds_alternative<Vector2d>(destination_variant))
		{
			Vector2d& destination = get<Vector2d>(destination_variant);
			ImGui::Text((format("{:.1f}", destination.x) + " | " + format("{:.1f}", destination.y)).c_str());
		}
		else if (holds_alternative<Entity*>(destination_variant))
		{
			Entity* dest_system = get<Entity*>(destination_variant);
			ImGui::Text(dest_system->c_selectable_ptr->name.c_str());
		}
		else // nullptr_t
		{
			ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "(none)");
		}
	}

	if (parent_entity_ptr->c_spacecraft_ptr != nullptr)
	{
		ImGui::TextColored({1.0f, 0.2f, 0.2f, 1.0f}, "Crew:");
		ImGui::SameLine();
		ImGui::Text(to_string(parent_entity_ptr->c_spacecraft_ptr->crew).c_str());

		ImGui::TextColored({1.0f, 0.2f, 0.2f, 1.0f}, "HP:");
		ImGui::SameLine();
		ImGui::Text(format("{:.2f}", parent_entity_ptr->c_spacecraft_ptr->hull_points).c_str());

		if (ImGui::CollapsingHeader("Orders"))
		{
			if (parent_entity_ptr->c_spacecraft_ptr->Orders.empty() == false)
			{
				for (Order& order : parent_entity_ptr->c_spacecraft_ptr->Orders)
				{
					draw_order_treenode(order);
				}
			}
			else
			{
				ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.0f}, "(none)");
			}
		}
	}

	if (parent_entity_ptr->c_constructor_ptr != nullptr)
	{
		ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Construction speed mult:");
		ImGui::SameLine();
		ImGui::Text(format("{:.2f}", parent_entity_ptr->c_constructor_ptr->construction_speed_multiplier).c_str());

		ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Constructee:");
		ImGui::SameLine();

		string constructee;
		if (parent_entity_ptr->c_constructor_ptr->entity_under_construction_ptr != nullptr)
		{
			constructee = parent_entity_ptr->c_constructor_ptr->entity_under_construction_ptr->c_selectable_ptr->name;
		}
		else
		{
			constructee = "(none)";
		}

		ImGui::Text(constructee.c_str());
	}

	if (parent_entity_ptr->c_star_system_ptr != nullptr)
	{
		int num_stars = parent_entity_ptr->c_star_system_ptr->star_entity_ptrs.size();
		int num_jump_nodes = parent_entity_ptr->c_star_system_ptr->jump_node_entity_ptrs.size();
		int num_planets = parent_entity_ptr->c_star_system_ptr->planet_entity_ptrs.size();
		int num_spacecrafts = parent_entity_ptr->c_star_system_ptr->spacecraft_entity_ptrs.size();
		int num_drawables = parent_entity_ptr->c_star_system_ptr->drawable_entity_ptrs.size();

		int num_projectiles = parent_entity_ptr->c_star_system_ptr->Projectiles.size();
		int num_missiles = parent_entity_ptr->c_star_system_ptr->Missiles.size();
		int num_beams = parent_entity_ptr->c_star_system_ptr->Beams.size();

		int num_effects = parent_entity_ptr->c_star_system_ptr->Effects.size();
		int num_particles = parent_entity_ptr->c_star_system_ptr->Particles.size();

		string objects_str = "Stars: " + to_string(num_stars) + NEWL +
			"Jump Nodes: " + to_string(num_jump_nodes) + NEWL +
			"Planets: " + to_string(num_planets) + NEWL +
			"Spacecrafts: " + to_string(num_spacecrafts) + NEWL +
			"Drawables: " + to_string(num_drawables) + NEWL +

			"Projectiles: " + to_string(num_projectiles) + NEWL +
			"Missiles: " + to_string(num_missiles) + NEWL +
			"Beams: " + to_string(num_beams) + NEWL +

			"Effects: " + to_string(num_effects) + NEWL +
			"Particles: " + to_string(num_particles);

		ImGui::Text(objects_str.c_str());
	}

	ImGui::End();
}

void Component::Selectable::Draw_Selection_Brackets()
{
	if (parent_entity_ptr->c_drawable_ptr->active_sprite != Active_Sprite::None)
	{
		Vector2d position;
		double scale_factor;

		if (parent_entity_ptr->c_drawable_ptr->icon and parent_entity_ptr->c_drawable_ptr->active_sprite == Active_Sprite::Icon)
		{
			position = parent_entity_ptr->c_drawable_ptr->icon->sprite.getPosition();
			scale_factor = parent_entity_ptr->c_drawable_ptr->icon->sprite.getScaling().get_length() / 4;
		}
		else
		{
			position = parent_entity_ptr->c_drawable_ptr->sprite.getPosition();
			scale_factor = parent_entity_ptr->c_drawable_ptr->sprite.getScaling().get_length() / 4;
		}


		Linelist line(Colors::Green, Draw_Layer::GUI_Lower, Blend_Mode::Opaque);
		vector<Vector2d>& vertices = line.vertices;

		vertices.reserve(16);

		vertices.emplace_back(position.x - 2 * scale_factor, position.y - scale_factor);
		vertices.emplace_back(position.x - 2 * scale_factor, position.y - 2 * scale_factor);
		vertices.emplace_back(position.x - 2 * scale_factor, position.y - 2 * scale_factor);
		vertices.emplace_back(position.x - scale_factor, position.y - 2 * scale_factor);

		vertices.emplace_back(position.x + scale_factor, position.y - 2 * scale_factor);
		vertices.emplace_back(position.x + 2 * scale_factor, position.y - 2 * scale_factor);
		vertices.emplace_back(position.x + 2 * scale_factor, position.y - 2 * scale_factor);
		vertices.emplace_back(position.x + 2 * scale_factor, position.y - scale_factor);

		vertices.emplace_back(position.x + 2 * scale_factor, position.y + scale_factor);
		vertices.emplace_back(position.x + 2 * scale_factor, position.y + 2 * scale_factor);
		vertices.emplace_back(position.x + 2 * scale_factor, position.y + 2 * scale_factor);
		vertices.emplace_back(position.x + scale_factor, position.y + 2 * scale_factor);

		vertices.emplace_back(position.x - scale_factor, position.y + 2 * scale_factor);
		vertices.emplace_back(position.x - 2 * scale_factor, position.y + 2 * scale_factor);
		vertices.emplace_back(position.x - 2 * scale_factor, position.y + 2 * scale_factor);
		vertices.emplace_back(position.x - 2 * scale_factor, position.y + scale_factor);

		line.Enqueue();
	}
}

void Component::Selectable::Destroy()
{
	selection.deselect(parent_entity_ptr); // deselect
}

bool Entity_Ref::is_valid() const
{
	return Entity::Valid_ids.count(id) == 1;
}

Component::Star_System::Star_System(Entity* parent_entity_ptr_arg,
									double radius_arg) :
	Component_Base(parent_entity_ptr_arg),
	radius(radius_arg)
{
	Systems.emplace_back(parent_entity_ptr);
}

void Component::Star_System::Draw()
{
	if (parent_entity_ptr->c_drawable_ptr->sprite.get_sprite_bounding_box(true).intersects_rect(camera.world_rectangle))
	{
		//draw starlight
		for (Entity* star_ptr : star_entity_ptrs)
		{
			star_ptr->c_star_ptr->light.Enqueue();
		}

		//draw system drawables
		for (Entity* entity_ptr : drawable_entity_ptrs)
		{
			switch (entity_ptr->c_drawable_ptr->active_sprite)
			{
				case Active_Sprite::Normal:
					entity_ptr->c_drawable_ptr->sprite.Enqueue();
					break;

				case Active_Sprite::Icon:
					entity_ptr->c_drawable_ptr->icon->sprite.Enqueue();
					break;
			}
		}
	}

	// draw star system radius circle
	int64_t circle_resolution = int64_t(radius / camera.relative_rectangle.size.x * 128);
	int64_t circle_resolution_clamped = clamp<int64_t>(circle_resolution, 12, 256);
	vector<Vector2d> radius_circle_vertices = GetCircleVertices(parent_entity_ptr->c_drawable_ptr->sprite.getPosition(), radius, circle_resolution_clamped);
	Linelist radius_circle{Colors::AliceBlue, Draw_Layer::Background_and_Universe, Blend_Mode::Opaque, radius_circle_vertices};
	radius_circle.Enqueue();
}

void Component::Star_System::Process_Particles()
{
	Particles.erase(remove_if(Particles.begin(), Particles.end(),
	[&](Particle& particle)
	{
		particle.Step();
		return particle.lifetime <= 0;
	}),
					Particles.end());

	for (Particle& particle : Particles)
	{
		particle.sprite.Enqueue();
	}
}

void Component::Star_System::Process_Effects()
{
	Effects.erase(remove_if(Effects.begin(), Effects.end(),
							  [&](Effect& effect)
	{
		effect.Step();
		return get<Animation>(effect.sprite.graphics).to_be_deleted;
	}),
					Effects.end());

	for (Effect& effect : Effects)
	{
		effect.sprite.Enqueue();
		effect.light.Enqueue();
	}
}

void Component::Star_System::Process_Ordnance()
{
	// Projectiles
	Projectiles.erase(remove_if(Projectiles.begin(), Projectiles.end(),
							[&](Projectile& projectile)
	{
		return projectile.Step();
	}),
					  Projectiles.end());

	// Missiles
	Missiles.erase(remove_if(Missiles.begin(), Missiles.end(),
								[&](Missile& missile)
	{
		return missile.Step();
	}),
				   Missiles.end());

	// Beams
	Beams.erase(remove_if(Beams.begin(), Beams.end(),
							 [&](Beam& beam)
	{
		return beam.Step();
	}),
				   Beams.end());
}


Component::Spacecraft::Spacecraft(Entity* parent_entity_ptr_arg,
												   int crew_arg) :
	Component_Base(parent_entity_ptr_arg),
	crew(crew_arg)
{
	if (parent_entity_ptr->c_drawable_ptr->in_system_ptr != nullptr)
	{
		parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->spacecraft_entity_ptrs.emplace_back(parent_entity_ptr);
	}
}

Component::Drawable::Drawable(Entity* parent_entity_ptr_arg,
							  Sprite sprite_arg,
							  optional<Icon> icon_arg) :
	Component_Base(parent_entity_ptr_arg),
	sprite(sprite_arg),
	icon(icon_arg)
{
	if (Systems.empty() == false)
	{
		Vector2d position = sprite_arg.getPosition();

		Entity* closest_system_ptr = *std::min_element(Systems.begin(), Systems.end(), [&](Entity* system_ent1, Entity* system_ent2)
		{
			return	point_squared_distance(system_ent1->c_drawable_ptr->sprite.getPosition(), position) <
				point_squared_distance(system_ent2->c_drawable_ptr->sprite.getPosition(), position);
		});

		if (point_distance(closest_system_ptr->c_drawable_ptr->sprite.getPosition(), position) <= closest_system_ptr->c_star_system_ptr->radius)
		{
			in_system_ptr = closest_system_ptr;
			closest_system_ptr->c_star_system_ptr->drawable_entity_ptrs.emplace_back(parent_entity_ptr);
		}
	}

	if (icon_arg)
	{
		icon->sprite.setPosition(sprite_arg.getPosition());
	}
}

void Component::Weapons::Step()
{
	for (Turret& turret : Turrets)
	{
		Vector2d turret_position = SpriteToWorldCoords(turret.position_offset_local, parent_entity_ptr->c_drawable_ptr->sprite);

		turret.sprite.setPosition(turret_position);

		--turret.reload_timer;
		turret.reload_timer = clamp(turret.reload_timer, 0.0f, turret.reload_time);
		

		if (turret.target_entity.ptr != nullptr)
		{
			if (turret.target_entity.is_valid())
			{
				Vector2d target_position = turret.target_entity.ptr->c_drawable_ptr->sprite.getPosition();
				Vector2d vector_to_target = target_position - turret_position;
				double angle_to_target = vector_to_angle_rad(vector_to_target);

				if (turret.reload_timer == 0)
				{
					switch (turret.weapon_type)
					{
						case Weapon_Type::Mass_driver:
						{
							turret.sprite.setRotation(angle_to_target);

							auto& projectiles_vect = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Projectiles;

							switch (turret.weapon_name)
							{
								case Weapon_Name::Ravager_Cannon:
								{
									parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Effects.emplace_back(
										Effect{turret_position, get_random_real_in_range(0.0, 3.0), {120.0, 120.0}, Color{0.2f, 0.2f, 1.0f, 1.0f}, name_animation_map["muzzle_flash"],
										Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{0.2f, 0.2f, 1.0f}, 800.0f}
									);

									Vector2d velocity = vector_to_target.get_resized(8.0);
									double direction = vector_to_angle_rad(velocity);
									projectiles_vect.emplace_back(
										Projectile{parent_entity_ptr->get_reference(), turret.target_entity, 10.0f, velocity, 60.0f * 10, Colors::White, name_texture_atlas_subrect_map["mass_driver"],
										Blend_Mode::Additive, turret_position, direction, Vector2d{50.0, 50.0}, Color{0.2f, 0.2f, 1.0f}, 1000.0f}
									);
								}
								break;
							}
						}
						break;


						case Weapon_Type::Missile_launcher:
						{
							turret.sprite.setRotation(parent_entity_ptr->c_drawable_ptr->sprite.getRotation());

							auto& missiles_vect = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Missiles;

							switch (turret.weapon_name)
							{
								case Weapon_Name::Stinger_Missile:
								{
									parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Effects.emplace_back(
										Effect{turret_position, get_random_real_in_range(0.0, 3.0), {120.0, 120.0}, {1.0f, 0.9f, 0.5f, 1.0f}, name_animation_map["muzzle_flash"],
										Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.9f, 0.5f}, 800.0f}
									);

									missiles_vect.emplace_back(
										Missile{parent_entity_ptr->get_reference(), turret.target_entity, 10.0f, 8.0, deg_to_rad(2.0), 60.0f * 10, Colors::White, name_texture_atlas_subrect_map["missile"],
										Blend_Mode::Alpha, turret_position, parent_entity_ptr->c_drawable_ptr->sprite.getRotation(), Vector2d{330, 63}/8.0, Color{1.0f, 1.0f, 0.8f}, 800.0f}
									);
								
								}
								break;
							}
						}
						break;

						case Weapon_Type::Beam_emitter:
						{
							turret.sprite.setRotation(angle_to_target);

							auto& beams_vect = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->Beams;

							switch (turret.weapon_name)
							{
								case Weapon_Name::Winter_King_Beam:
								{
									Vector2f endpoint1{get_random_real_in_range(-0.5f, 0.5f), get_random_real_in_range(-0.5f, 0.5f)};
									Vector2f endpoint2{get_random_real_in_range(-0.5f, 0.5f), get_random_real_in_range(-0.5f, 0.5f)};

									beams_vect.emplace_back(
										Beam{parent_entity_ptr->get_reference(), &turret, turret.target_entity, endpoint1, endpoint2, 1.0f, 60.0f * 5,
										Color{1.0f, 0.1f, 0.1f}, 1000.0f, Colors::Red}
									);
								}
								break;
							}
						}
						break;
					}

					turret.reload_timer = turret.reload_time;
				}
			}
			else // target entity is invalid, set target pointer to null
			{
				turret.target_entity.ptr = nullptr;
			}
		}

		if (turret.target_entity.ptr == nullptr)
		{
			turret.sprite.setRotation(parent_entity_ptr->c_drawable_ptr->sprite.getRotation());
		}

		if (parent_entity_ptr->c_drawable_ptr->active_sprite == Active_Sprite::Normal) // only draw Turrets when drawing the main sprite normally (not just Icon)
		{
			turret.sprite.Enqueue();
		}
	}
}

void Component::Weapons::Attack_Entity(Entity_Ref& entity_ref)
{
	for (Turret& turret : Turrets)
	{
		turret.target_entity = entity_ref;
	}
}

void Component::Weapons::Stop()
{
	for (Turret& turret : Turrets)
	{
		turret.target_entity.ptr = nullptr;
		turret.target_entity.id = 0;
	}
}

Component::Jump_Drive::Jump_Drive(Entity* parent_entity_ptr_arg,
								  float charge_arg,
								  float charge_rate_arg,
								  double min_range_arg,
								  double max_range_arg,
								  bool intersystem_arg) :
	Component_Base(parent_entity_ptr_arg),
	charge(charge_arg),
	charge_rate(charge_rate_arg),
	min_range(min_range_arg),
	max_range(max_range_arg),
	intersystem(intersystem_arg)
{}

void Component::Jump_Drive::Step()
{
	if (charge < 100.0f) // recharging
	{
		charge += charge_rate;
	}
	
	charge = clamp(charge, 0.0f, 100.0f);

	if (holds_alternative<Vector2d>(destination)) // intrasystem jump destination is set
	{
		Vector2d current_position = parent_entity_ptr->c_drawable_ptr->sprite.getPosition();
		Vector2d destination_position = get<Vector2d>(destination);

		double destination_distance_from_system_center = point_distance(parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_drawable_ptr->sprite.getPosition(), destination_position);

		if (destination_distance_from_system_center > parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->radius) // destination outside current system
		{
			destination = std::nullptr_t();
		}
		else // not outside current system
		{
			auto distance = point_distance(current_position, destination_position);

			auto& jump_inhib_fields = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs;
			bool destination_inhibited = std::any_of(jump_inhib_fields.begin(), jump_inhib_fields.end(), [&](Entity* ji_field_entity_ptr)
			{
				auto dist = point_distance(destination_position, ji_field_entity_ptr->c_drawable_ptr->sprite.getPosition());
				return (dist <= ji_field_entity_ptr->c_jump_inhibition_field_ptr->radius);
			});

			if (destination_inhibited or
				distance < min_range or
				distance > max_range) // destination NOT valid
			{
				destination = std::nullptr_t();
			}
			else if (charge == 100.0f) // destination is valid and drive charged
			{
				parent_entity_ptr->c_drawable_ptr->sprite.setPosition(destination_position);

				optional<Icon>& icon_opt = parent_entity_ptr->c_drawable_ptr->icon;

				if (icon_opt)
				{
					icon_opt->sprite.setPosition(destination_position);
				}

				destination = std::nullptr_t();
				charge = 0.0f;
			}
			
		}
	}
	else if (holds_alternative<Entity*>(destination)) // intersystem jump
	{
		Entity* destination_system = get<Entity*>(destination);


		Entity* origin_jump_node_ptr = nullptr;
		for (Entity* jump_node : parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_node_entity_ptrs)
		{
			if (jump_node->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->in_system_ptr == destination_system)
			{
				origin_jump_node_ptr = jump_node;
				break;
			}
		}

		if (origin_jump_node_ptr != nullptr and origin_jump_node_ptr->c_collidable_ptr->point_collides(parent_entity_ptr->c_drawable_ptr->sprite.getPosition()))
		{
			if (charge == 100.0f)
			{
				// intersystem jump

				Vector2d destination_node_pos = origin_jump_node_ptr->c_jump_node_ptr->destination_node_ptr->c_drawable_ptr->sprite.getPosition();
				parent_entity_ptr->c_drawable_ptr->sprite.setPosition(destination_node_pos);

				optional<Icon>& icon_opt = parent_entity_ptr->c_drawable_ptr->icon;

				if (icon_opt)
				{
					icon_opt->sprite.setPosition(destination_node_pos);
				}

				destination = std::nullptr_t();
				charge = 0.0f;

				// change drawable, spacecraft and jump inhibition entity ptrs in star system entities

				{
					auto& drawable_entity_ptrs = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->drawable_entity_ptrs; // remove from system drawable entities

					auto iter = drawable_entity_ptrs.begin();
					while (true)
					{
						if (*iter == parent_entity_ptr)
						{
							drawable_entity_ptrs.erase(iter);
							break;
						}

						++iter;
					}
				}

				if (parent_entity_ptr->c_spacecraft_ptr != nullptr)
				{
					auto& spacecraft_entity_ptrs = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->spacecraft_entity_ptrs; // remove from system spacecraft entities

					auto iter = spacecraft_entity_ptrs.begin();
					while (true)
					{
						if (*iter == parent_entity_ptr)
						{
							spacecraft_entity_ptrs.erase(iter);
							break;
						}

						++iter;
					}
				}

				if (parent_entity_ptr->c_jump_inhibition_field_ptr != nullptr)
				{
					auto& jump_inhib_field_entity_ptrs = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs; // remove from system jump inhib field entities

					auto iter = jump_inhib_field_entity_ptrs.begin();
					while (true)
					{
						if (*iter == parent_entity_ptr)
						{
							jump_inhib_field_entity_ptrs.erase(iter);
							break;
						}

						++iter;
					}
				}

				parent_entity_ptr->c_drawable_ptr->in_system_ptr = destination_system; // change in system ptr

				// add itself to new system pointer vectors

				destination_system->c_star_system_ptr->drawable_entity_ptrs.emplace_back(parent_entity_ptr);

				if (parent_entity_ptr->c_spacecraft_ptr != nullptr)
				{
					destination_system->c_star_system_ptr->spacecraft_entity_ptrs.emplace_back(parent_entity_ptr);
				}
				
				if (parent_entity_ptr->c_jump_inhibition_field_ptr != nullptr)
				{
					destination_system->c_star_system_ptr->jump_inhibition_entity_ptrs.emplace_back(parent_entity_ptr);
				}
			}
		}
	}
}

bool Component::Spacecraft::Step() const // optimize
{
	return hull_points <= 0.0f; // if destroyed, return true
}

void Component::Spacecraft::Destroy()
{
	if (parent_entity_ptr->c_drawable_ptr->in_system_ptr != nullptr) // if in system
	{
		auto& system_spacecraft_entity_ptrs = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->spacecraft_entity_ptrs; // remove from system spacecraft entities

		auto iter = system_spacecraft_entity_ptrs.begin();
		while (true)
		{
			if (*iter == parent_entity_ptr)
			{
				system_spacecraft_entity_ptrs.erase(iter);
				break;
			}

			++iter;
		}
	}

	// spawn explosion effect
	double largest_dimension = std::max(parent_entity_ptr->c_drawable_ptr->sprite.getScaling().x,
										parent_entity_ptr->c_drawable_ptr->sprite.getScaling().y);
	double exp_size = largest_dimension * 4.0;
	Systems[0]->c_star_system_ptr->Effects.emplace_back(Effect{parent_entity_ptr->c_drawable_ptr->sprite.getPosition(), get_random_real_in_range(0.0, 3.0), {exp_size, exp_size}, Colors::White, name_animation_map["exp_1"],
									Blend_Mode::Additive, Draw_Layer::Effects_Higher, {0.0, 0.0}, false, 1.5, Color{1.0f, 0.8f, 0.1f}, 12000.0f});
}

void Component::Spacecraft::Process_orders()
{
	current_order = get_current_order();

	if (current_order != nullptr)
	{
		switch (current_order->state)
		{
			case Order_Status::Unopened:
			{
				current_order->Execute(parent_entity_ptr);
			}
			break;

			case Order_Status::In_Progress:
			{
				current_order->Check_if_finished(parent_entity_ptr);
			}
			break;

			//case Order_Status::Finished:
			//{

			//}
			//break;
		}

		Order& toplevel_standing_order = Orders.front();

		if (toplevel_standing_order.state == Order_Status::Finished)
		{
			Orders.erase(Orders.begin());
		}
		else
		{
			toplevel_standing_order.Delete_Finished_Suborders();
		}
	}
}

void Component::Spacecraft::Clear_Orders() // We MUST use this function to properly clear all orders, DO NOT just call Spacecraft::Orders.clear()!!!
{
	if (Orders.empty() == false)
	{
		Order* current_order_ptr = get_current_order();
		current_order_ptr->Clear(parent_entity_ptr);
		Orders.clear();
	}
}

Order* Component::Spacecraft::get_current_order()
{
	if (Orders.empty() == false)
	{
		return Orders.front().get_current_order();
	}
	else
	{
		return nullptr;
	}
}

void Component::Jump_Inhibition_Field::Draw()
{
	Linelist{Colors::Orange, Draw_Layer::GUI_Lower, Blend_Mode::Additive,
		GetCircleVertices(parent_entity_ptr->c_drawable_ptr->sprite.getPosition(), radius, 128)}.Enqueue();
}

Component::Jump_Inhibition_Field::Jump_Inhibition_Field(Entity* parent_entity_ptr_arg,
														double radius_arg,
														bool active_arg) :
	Component_Base(parent_entity_ptr_arg),
	radius(radius_arg),
	active(active_arg)
{
	parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs.emplace_back(parent_entity_ptr);
}

void Component::Jump_Inhibition_Field::Destroy()
{
	auto& jump_inhibition_entity_ptrs = parent_entity_ptr->c_drawable_ptr->in_system_ptr->c_star_system_ptr->jump_inhibition_entity_ptrs;
	auto iter = jump_inhibition_entity_ptrs.begin();
	while (true)
	{
		if (*iter == parent_entity_ptr)
		{
			jump_inhibition_entity_ptrs.erase(iter);
			break;
		}

		++iter;
	}
}

// Component::Constructor

void Component::Constructor::Build(string entity_template_arg, Vector2d location)
{
	// Get info from the template
	EntityTemplate entity_template = entityTemplateMap.at(entity_template_arg);

	auto entity_it = Entity::Colony.emplace();
	entity_under_construction_ptr = &*entity_it;
	entity_it->c_drawable_ptr = &*Component::Drawable::Colony.emplace(&*entity_it,
																		Sprite{Shader_Lighting_Mode::normal_lighting, Colors::White, Draw_Layer::Spacecrafts, entity_template.graphics,
																		Blend_Mode::Alpha, location, 0, entity_template.scaling, entity_template.glowmap_subrect_ptr},
																		Icon{Colors::Green, Draw_Layer::Spacecrafts, Blend_Mode::Additive, name_texture_atlas_subrect_map["icon_cruiser"],
																		Vector2d(screen_resolution.x * 0.036, screen_resolution.y * 0.032)});
	entity_it->c_collidable_ptr = &*Component::Collidable::Colony.emplace(&*entity_it,
																			CollisionPolygonToSpriteCoords(entity_template.collision_polygon_texture_coords, {556, 442}));
	entity_it->c_dynamic_ptr = &*Component::Dynamic::Colony.emplace(&*entity_it,
																	Vector2d{0.0, 0.0}, 0);
	if (entity_template.engines)
	{
		entity_it->c_engines_ptr = &*Component::Engines::Colony.emplace(&*entity_it,
																		entity_template.engines->max_speed,
																		entity_template.engines->acceleration,
																		entity_template.engines->max_angular_velocity);
	}
	entity_it->c_selectable_ptr = &*Component::Selectable::Colony.emplace(&*entity_it,
																		  entity_template_arg + to_string(entity_it->id));

	entity_it->c_spacecraft_ptr = &*Component::Spacecraft::Colony.emplace(&*entity_it, entity_template.crew); // Example: 50 crew members
	entity_it->c_spacecraft_ptr->under_construction = true;
	entity_it->c_spacecraft_ptr->hull_points = 1.0f;

	if (entity_template.Turrets.empty() == false)
	{
		entity_it->c_weapons_ptr = &*Component::Weapons::Colony.emplace(&*entity_it);

		for (TurretTemplate turret : entity_template.Turrets)
		{
			float damage = 10.0f;
			switch (turret.weapon_name_arg)
			{
				case Weapon_Name::Ravager_Cannon:
				{
					damage = 25.0f;
				}
				break;

				case Weapon_Name::Stinger_Missile:
				{
					damage = 30.0f;
				}
				break;

				case Weapon_Name::Winter_King_Beam:
				{
					damage = 400.0f;
				}
			}

			entity_it->c_weapons_ptr->Turrets.emplace_back(Turret{turret.weapon_type_arg, turret.weapon_name_arg, damage,
												   &*entity_it, turret.position_offset_local_arg, name_texture_atlas_subrect_map["faustus"], Vector2d{50.0, 50.0}});
		}
	}

	if (entity_template.jump_drive)
	{
		entity_it->c_jump_drive_ptr = &*Component::Jump_Drive::Colony.emplace(&*entity_it,
																			  0.0f, entity_template.jump_drive->charge_rate,
																			  entity_template.jump_drive->min_range,
																			  entity_template.jump_drive->min_range,
																			  entity_template.jump_drive->intersystem);
	}

	if (entity_template.ji_field)
	{
		entity_it->c_jump_inhibition_field_ptr = &*Component::Jump_Inhibition_Field::Colony.emplace(&*entity_it,
																									entity_template.ji_field->radius, true);

	}

	if (entity_template.constructor)
	{
		entity_it->c_constructor_ptr = &*Component::Constructor::Colony.emplace(&*entity_it);
		entity_it->c_constructor_ptr->construction_speed_multiplier = entity_template.constructor->construction_speed_multiplier;
	}

}

void Component::Constructor::Step()
{
	if (entity_under_construction_ptr != nullptr)
	{
		if (entity_under_construction_ptr->c_spacecraft_ptr->hull_points < entity_under_construction_ptr->c_spacecraft_ptr->max_hull_points)
		{
			// Construction in progress
			entity_under_construction_ptr->c_spacecraft_ptr->hull_points += construction_speed_multiplier;
		}
		else // Construction finished
		{
			entity_under_construction_ptr->c_spacecraft_ptr->hull_points = entity_under_construction_ptr->c_spacecraft_ptr->max_hull_points;
			entity_under_construction_ptr->c_spacecraft_ptr->under_construction = false;
			entity_under_construction_ptr = nullptr;
		}
	}
}

bool Entity::Step() // add new component here
{
	bool to_be_erased = false;

	if (c_drawable_ptr != nullptr)
	{
		c_drawable_ptr->Draw();
	}

	if (c_dynamic_ptr != nullptr)
	{
		c_dynamic_ptr->Step();
	}

	if (c_engines_ptr != nullptr)
	{
		c_engines_ptr->Step();
	}

	if (c_star_system_ptr != nullptr)
	{
		c_star_system_ptr->Draw();
		c_star_system_ptr->Process_Particles();
		c_star_system_ptr->Process_Effects();
		c_star_system_ptr->Process_Ordnance();
	}

	if (c_spacecraft_ptr != nullptr)
	{
		to_be_erased = c_spacecraft_ptr->Step();
		c_spacecraft_ptr->Process_orders();
	}

	if (c_weapons_ptr != nullptr)
	{
		c_weapons_ptr->Step();
	}

	if (c_jump_drive_ptr != nullptr)
	{
		c_jump_drive_ptr->Step();
	}

	if (c_jump_inhibition_field_ptr != nullptr)
	{
		c_jump_inhibition_field_ptr->Draw();
	}

	if (debug and c_collidable_ptr != nullptr)
	{
		if (holds_alternative<vector<Vector2d>>(c_collidable_ptr->radius_or_polygon)) //polygon
		{
			vector<Vector2d> polygon = get<vector<Vector2d>>(c_collidable_ptr->radius_or_polygon);

			vector<Vector2d> worldspace_polygon;
			worldspace_polygon.reserve(polygon.size());

			for (auto& point : polygon)
			{
				worldspace_polygon.emplace_back(SpriteToWorldCoords(point, c_drawable_ptr->sprite));
			}

			Linelist{Colors::White, Draw_Layer::GUI_Lower, Blend_Mode::Opaque, linestrip_to_linelist(worldspace_polygon)}.Enqueue();
		}
		else //radius
		{
			double radius = get<double>(c_collidable_ptr->radius_or_polygon);
			vector<Vector2d> vertices = GetCircleVertices(c_drawable_ptr->sprite.getPosition(), radius, 32);
			Linelist{Colors::White, Draw_Layer::GUI_Lower, Blend_Mode::Opaque, vertices}.Enqueue();
		}
	}

	if (c_constructor_ptr != nullptr)
	{
		c_constructor_ptr->Step();
	}

	return to_be_erased;
}

void Entity::Destroy_Components() // add new component here
{
	if (c_drawable_ptr != nullptr)
	{
		c_drawable_ptr->Destroy();
	}

	if (c_selectable_ptr != nullptr)
	{
		c_selectable_ptr->Destroy();
	}

	if (c_spacecraft_ptr != nullptr)
	{
		c_spacecraft_ptr->Destroy();
	}

	if (c_jump_inhibition_field_ptr != nullptr)
	{
		c_jump_inhibition_field_ptr->Destroy();
	}

}

void Entity::Erase_Components() // add new component here
{
	if (c_drawable_ptr != nullptr)
	{
		Component::Drawable::Colony.erase(Component::Drawable::Colony.get_iterator(c_drawable_ptr));
	}

	if (c_collidable_ptr != nullptr)
	{
		Component::Collidable::Colony.erase(Component::Collidable::Colony.get_iterator(c_collidable_ptr));
	}

	if (c_dynamic_ptr != nullptr)
	{
		Component::Dynamic::Colony.erase(Component::Dynamic::Colony.get_iterator(c_dynamic_ptr));
	}

	if (c_engines_ptr != nullptr)
	{
		Component::Engines::Colony.erase(Component::Engines::Colony.get_iterator(c_engines_ptr));
	}

	if (c_selectable_ptr != nullptr)
	{
		Component::Selectable::Colony.erase(Component::Selectable::Colony.get_iterator(c_selectable_ptr));
	}

	if (c_star_system_ptr != nullptr)
	{
		Component::Star_System::Colony.erase(Component::Star_System::Colony.get_iterator(c_star_system_ptr));
	}

	if (c_jump_node_ptr != nullptr)
	{
		Component::Jump_Node::Colony.erase(Component::Jump_Node::Colony.get_iterator(c_jump_node_ptr));
	}

	if (c_star_ptr != nullptr)
	{
		Component::Star::Colony.erase(Component::Star::Colony.get_iterator(c_star_ptr));
	}

	if (c_planet_ptr != nullptr)
	{
		Component::Planet::Colony.erase(Component::Planet::Colony.get_iterator(c_planet_ptr));
	}

	if (c_spacecraft_ptr != nullptr)
	{
		Component::Spacecraft::Colony.erase(Component::Spacecraft::Colony.get_iterator(c_spacecraft_ptr));
	}

	if (c_weapons_ptr != nullptr)
	{
		Component::Weapons::Colony.erase(Component::Weapons::Colony.get_iterator(c_weapons_ptr));
	}

	if (c_jump_drive_ptr != nullptr)
	{
		Component::Jump_Drive::Colony.erase(Component::Jump_Drive::Colony.get_iterator(c_jump_drive_ptr));
	}

	if (c_jump_inhibition_field_ptr != nullptr)
	{
		Component::Jump_Inhibition_Field::Colony.erase(Component::Jump_Inhibition_Field::Colony.get_iterator(c_jump_inhibition_field_ptr));
	}

	if (c_constructor_ptr != nullptr)
	{
		Component::Constructor::Colony.erase(Component::Constructor::Colony.get_iterator(c_constructor_ptr));
	}

	// WARNING: this function leaves dangling pointers to erased components in this entity. The entity MUST be erased now.
}

void Process_ECS()
{
	auto it = Entity::Colony.begin();
	while (it != Entity::Colony.end())
	{
		if (it->Step()) //returns true - to be erased
		{
			it->Destroy_Components();
			it->Erase_Components();
			Entity::Valid_ids.erase(it->id);
			it = Entity::Colony.erase(it);
		}
		else
		{
			++it;
		}
	}
}