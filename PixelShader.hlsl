#define MAX_LIGHTS 1024

struct Light
{
	float3 diffuse;
	float falloff_exp;

	float2 position1;
	float2 position2;

	float direction;
	float cone_exp;
	float type;
	float padding;
};

cbuffer PS_constant_buffer
{
	Light Lights[MAX_LIGHTS];
	int4 number_of_active_lights; //only the first int is used, the rest is padding
};

Texture2D Texture0;
Texture2D Texture1;
Texture2D Texture2;
Texture2D Texture3;
Texture2D Texture4;
Texture2D Texture5;
Texture2D Texture6;
Texture2D Texture7;
Texture2D Texture8;
Texture2D Texture9;
Texture2D Texture10;
Texture2D Texture11;
Texture2D Texture12;
Texture2D Texture13;
Texture2D Texture14;
Texture2D Texture15;
Texture2D Texture16;
Texture2D Texture17;
Texture2D Texture18;
Texture2D Texture19;
Texture2D Texture20;
Texture2D Texture21;
Texture2D Texture22;
Texture2D Texture23;
Texture2D Texture24;
Texture2D Texture25;
Texture2D Texture26;
Texture2D Texture27;
Texture2D Texture28;
Texture2D Texture29;
Texture2D Texture30;
Texture2D Texture31; //TEXTURE_SLOTS

SamplerState ObjSamplerState;

float4 texture_sample(int id, float2 texcoord)
{
	[call] switch (id)
	{
		case 0:
		{
			return Texture0.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 1:
		{
			return Texture1.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 2:
		{
			return Texture2.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 3:
		{
			return Texture3.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 4:
		{
			return Texture4.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 5:
		{
			return Texture5.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 6:
		{
			return Texture6.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 7:
		{
			return Texture7.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 8:
		{
			return Texture8.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 9:
		{
			return Texture9.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 10:
		{
			return Texture10.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 11:
		{
			return Texture11.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 12:
		{
			return Texture12.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 13:
		{
			return Texture13.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 14:
		{
			return Texture14.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 15:
		{
			return Texture15.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 16:
		{
			return Texture16.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 17:
		{
			return Texture17.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 18:
		{
			return Texture18.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 19:
		{
			return Texture19.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 20:
		{
			return Texture20.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 21:
		{
			return Texture21.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 22:
		{
			return Texture22.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 23:
		{
			return Texture23.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 24:
		{
			return Texture24.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 25:
		{
			return Texture25.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 26:
		{
			return Texture26.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 27:
		{
			return Texture27.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 28:
		{
			return Texture28.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 29:
		{
			return Texture29.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 30:
		{
			return Texture30.Sample(ObjSamplerState, texcoord);
		}
		break;
		case 31:
		{
			return Texture31.Sample(ObjSamplerState, texcoord);
		}
		break;

		default:
		{
			return Texture0.Sample(ObjSamplerState, texcoord);
		}
		break;
	}
}

struct VS_OUTPUT
{
	int texture_ID : TEXID;
	int lighting_mode : LIGHTING;
	float2 position : POSITIONW;

	float4 color : COLOR;

	float2 uv_diffuse : UVDIFF;
	float2 uv_glow : UVGLOW;

	float4 SS_position : SV_POSITION;		// screen space position of the vertex: x, y, z-buffer, w-buffer
};

float dist_to_segment(float2 v, float2 w, float2 p)
{
	// Return minimum distance between line segment vw and point p

	float2 wv = w - v;
	float wvlength = length(wv);
	float length_sq = wvlength * wvlength;  // i.e. |w-v|^2 -  avoid a sqrt

	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.

	float t = clamp(dot(p - v, wv) / length_sq, 0.0, 1.0);

	float2 projection = v + t * wv;  // Projection falls on the segment
	return distance(p, projection);
}

//float vector_to_angle_rad(float2 V)
//{
//	float angle = atan2(V.y, V.x);
//	return angle;
//}

float2 angle_rad_to_vector(float angle)
{
	float2 V = {cos(angle), sin(angle)};
	return V;
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 texture_diffuse = texture_sample(input.texture_ID, input.uv_diffuse); // sample texture
	texture_diffuse *= input.color;	// diffuse vertex colors

	if (input.lighting_mode == 1)	// NO lighting (Background, HUD)
	{
		return texture_diffuse;
	}
	else //lighting
	{
		float4 final_pixel_color = {0.0, 0.0, 0.0, texture_diffuse.w};

		for (int i = 0; i < number_of_active_lights.x; ++i)		// go over Lights
		{
			switch (int(Lights[i].type))
			{
				case 0: // point light
				{
					float4 light_diffuse = {Lights[i].diffuse.x, Lights[i].diffuse.y, Lights[i].diffuse.z, 0.0};

					float2 LightToPixelVector = Lights[i].position1 - input.position;
					float attenuation_factor = pow(length(LightToPixelVector), Lights[i].falloff_exp);
					float intensity_factor = rcp(attenuation_factor);

					// diffuse light contribution
					float4 light_diffuse_to_pixel = min(light_diffuse * intensity_factor, 16.0);

					final_pixel_color += max(texture_diffuse * light_diffuse_to_pixel, 0.0);
				}
				break;

				case 1: // tube light
				{
					float4 light_diffuse = {Lights[i].diffuse.x, Lights[i].diffuse.y, Lights[i].diffuse.z, 0.0};

					float distance_to_tube = dist_to_segment(Lights[i].position1, Lights[i].position2, input.position);
					float attenuation_factor = pow(distance_to_tube, Lights[i].falloff_exp);
					float intensity_factor = rcp(attenuation_factor);

					// diffuse light contribution
					float4 light_diffuse_to_pixel = min(light_diffuse * intensity_factor, 16.0);

					final_pixel_color += max(texture_diffuse * light_diffuse_to_pixel, 0.0);
				}
				break;

				case 2: // spotlight
				{

					float2 LightToPixelVector = Lights[i].position1 - input.position;

					float2 light_dir = normalize(angle_rad_to_vector(Lights[i].direction));

					float2 ltp_norm = -normalize(LightToPixelVector);

					float dot_prod = dot(ltp_norm, light_dir);
					float dot_prod_pos_scaled = pow((dot_prod + 1.0) * 0.5, Lights[i].cone_exp);

					float4 light_diffuse = {Lights[i].diffuse.x, Lights[i].diffuse.y, Lights[i].diffuse.z, 0.0};
					float attenuation_factor = pow(length(LightToPixelVector), Lights[i].falloff_exp);
					float intensity_factor = rcp(attenuation_factor) * dot_prod_pos_scaled;

						// diffuse light contribution
					float4 light_diffuse_to_pixel = min(light_diffuse * intensity_factor, 16.0);

					final_pixel_color += max(texture_diffuse * light_diffuse_to_pixel, 0.0);
				}
				break;
			}

		}

		if (input.uv_glow.x >= 0) // glowmap
		{
			float4 glow = texture_sample(input.texture_ID, input.uv_glow);
			return final_pixel_color + float4(glow.x, glow.y, glow.z, 0.0f);
		}
		else // no glowmap
		{
			return final_pixel_color;
		}
	}
}