cbuffer VS_constant_buffer
{
	float4x4 ViewProjection;
};

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

VS_OUTPUT main(int texture_ID : TEXID, int lighting_mode : LIGHTING, float2 position : POSITION, float4 color : COLOR, float2 uv_diffuse : UVDIFF, float2 uv_glow : UVGLOW)
{
	VS_OUTPUT output;

	output.texture_ID = texture_ID;
	output.lighting_mode = lighting_mode;
	output.position = position;

	output.color = color;

	output.uv_diffuse = uv_diffuse;
	output.uv_glow = uv_glow;

	output.SS_position = mul(float4(position.x, position.y, 0.0, 1.0), transpose(ViewProjection));

	return output;
}