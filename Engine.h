#pragma once

#define NOMINMAX

//constants
constexpr int MAX_LIGHTS = 1024; // light limit, also change in PixelShader.hlsl;
constexpr int TEXTURE_SLOTS = 32;
constexpr int MAX_QUAD_INSTANCES = 1048576;
constexpr double AU = 149597870700.0; // in meters;
constexpr double NS_IN_SEC = 1000000000.0;

//std
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <optional>
#include <variant>

//boost
#include <boost\container\deque.hpp> // must be boost::deque, std::deque does not allow incomplete types which are required for deque<Order> Order::Suborders

//directx
#include <d3d11.h>
#include <dxgi.h>
#include <dinput.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <tchar.h>

#include <D:\Programming\CPP\VS_Solutions\DirectXTK-main\Inc\WICTextureLoader.h>
#include <D:\Programming\CPP\VS_Solutions\FW1FontWrapper-1.1\Source\FW1FontWrapper.h>
#include <D:\Programming\CPP\Sources\rapidjson-master\include\rapidjson\document.h>

// Rect
#include "Rect.h"

// imgui
#include "imgui-master\imgui.h"
#include "imgui-master\imgui_impl_win32.h"
#include "imgui-master\imgui_impl_dx11.h"

// Link libraries (alternative to Properties->Linker->Input->Additional dependencies)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef BUILD_CONFIG_DEBUG
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\FW1FontWrapper-1.1\\Bin\\Debug\\FW1FontWrapper_Debug.lib")
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\DirectXTK-main\\Bin\\Desktop_2022\\x64\\Debug\\DirectXTK.lib")
#elif BUILD_CONFIG_PROFILE
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\FW1FontWrapper-1.1\\Bin\\Profile\\FW1FontWrapper_Profile.lib")
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\DirectXTK-main\\Bin\\Desktop_2022\\x64\\Profile\\DirectXTK.lib")
#elif BUILD_CONFIG_RELEASE
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\FW1FontWrapper-1.1\\Bin\\Release\\FW1FontWrapper_Release.lib")
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\DirectXTK-main\\Bin\\Desktop_2022\\x64\\Release\\DirectXTK.lib")
#endif

using std::vector;
using std::array;
using std::string;
using std::wstring;
using std::min;
using std::to_string;
using std::ifstream;
using std::unordered_map;
using std::max;
using std::remove_if;
using std::unordered_set;
using std::get;
using std::optional;
using std::clamp;
using std::variant;
using std::holds_alternative;

using boost::container::deque;

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;
using DirectX::XMMATRIX;
using DirectX::XMVectorGetByIndex;
using DirectX::CreateWICTextureFromFile;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int number_of_active_lights = 0;

// DirectX Object Declarations //

ID3D11Device* D3D11_Device;
ID3D11DeviceContext* D3D11_Device_Context;
IDXGISwapChain* SwapChain;
ID3D11Texture2D* BackBuffer;
ID3D11RenderTargetView* BackBuffer_RTV;
ID3D11DepthStencilState* pDepthdisabledStencilState;
ID3D11InputLayout* D3D11_Vertex_Layout;
ID3D11VertexShader* D3D11_Vertex_Shader;
ID3D11PixelShader* D3D11_Pixel_Shader;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11Buffer* D3D11_PS_Constant_Buffer;
ID3D11Buffer* D3D11_VS_Constant_Buffer;
ID3D11BlendState* Blend_Add;
ID3D11BlendState* Blend_Alpha;
ID3D11RasterizerState* NonecullMode;
ID3D11SamplerState* TextureSamplerState;
ID3D11Buffer* VertexBuffer;
ID3D11Buffer* IndexBuffer;

LPDIRECTINPUT8 DirectInput;

IFW1Factory* pFW1Factory;
IFW1FontWrapper* pFontWrapper;

// *** Enums *** //

enum class Blend_Mode
{
	Opaque, Alpha, Additive
};

enum class Topology_Type
{
	Trianglelist, Linelist
};

enum class Shader_Lighting_Mode
{
	normal_lighting, no_lighting
};

enum class Active_Sprite //used in Drawable component
{
	Normal, Icon, None
};

enum class Order_Status
{
	Unopened, In_Progress, Finished
};

enum class Draw_Layer
{
	Background_and_Universe = 0,
	Effects_Lower = 1,
	Spacecrafts = 2,
	Effects_Higher = 3,
	GUI_Lower = 4,
	GUI_Higher = 5
};


// *** Classes declarations *** //

class Vertex;
class Light_HLSL;
class VS_Constant_Buffer;
class PS_Constant_Buffer;
class Camera;
class Mouse;
class Linelist;
class Sprite;
class Icon;
class Particle;
class Color;
class Entity_Ref;

class Texture_Atlas;
class Texture_Animated;

class Texture_Atlas_Subrect;
class Animation;

vector<Texture_Atlas> Texture_Atlases;
vector<Texture_Animated> Animated_Textures;

vector<Texture_Atlas_Subrect> Texture_Atlas_Subrects;
vector<Animation> Animations;

unordered_map<string, Texture_Atlas_Subrect*> name_texture_atlas_subrect_map;
unordered_map<string, Animation*> name_animation_map;

int texture_id_counter = 0;

// *** Functions declarations *** //

bool InitializeWindow(HINSTANCE const hInstance, const int ShowWnd, const int width, const int height, const bool windowed);
bool InitDirectInput(HINSTANCE const hInstance);
bool InitializeDirect3d11App();
void Initialize_game();
void Step();
void Render();
void CleanUp();
LRESULT CALLBACK WndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam);
wstring string_to_wstring(const string& input);
string wstring_to_string(const wstring& input);
void UpdateBuffer(ID3D11Buffer* const Buffer, const void* source, const size_t size);
void Process_Input();
Vector2d WorldToSpriteCoords(const Vector2d& point, Sprite& sprite);
Vector2d SpriteToWorldCoords(const Vector2d& point, Sprite& sprite);
Vector2f WorldToScreenCoords(Vector2d point);
Vector2d ScreenToWorldCoords(Vector2f point);
vector<Vector2d> GetCircleVertices(Vector2d position_arg, double radius_arg, int resolution_arg);
void spawn_systems();
void Parse_data();
void spawn_celestial_bodies();
void DrawGUI();
void draw_deferred_strings();

// *** Variables declarations *** //

Stopwatch Stopwatch_FPS_limit;
Stopwatch Second;

const LPCTSTR WndClassName = L"DirectX Game";
HWND hwnd = nullptr;
HRESULT hresult;
MSG msg;

Vector2i screen_resolution;
bool windowed;
bool running = true;
bool paused = false;
bool debug = true;
double camera_scrolling_speed;
double camera_smoothing_factor; //bigger = more camera inertia
const vector<Vector2d> collision_square{Vector2d(0.5, 0.5), Vector2d(0.5, -0.5), Vector2d(-0.5, -0.5), Vector2d(-0.5, 0.5)};

float average_fps;
float fps_cap;	// settings.json  --  positive: limited fps,  0.0f: no fps limit,  negative: vsync
nanoseconds average_frame_time;
boost::circular_buffer<nanoseconds> past_frametimes{60};
nanoseconds target_frame_time;

int num_of_systems;
Vector2d galaxy_size; //determined dynamically based on num_of_systems
const double system_radius{AU};
const double jump_node_radius{1500.0};
const double icon_culling_view_size{AU * 8};

DoubleRect selection_rectangle;

const double minimum_light_intensity = 0.05; // lights weaker than this get culled if outside the camera rectangle

vector<string> magnitudes; //Star magnitudes and constellation names
vector<string> constellations;

// Class representing a color. Channels: Red/Green/Blue/Alpha, represented as floats from 0.0f to 1.0f
class Color
{
public:
	Color() : r(1.0f), g(0.0f), b(0.0f), a(1.0f)
	{}

	// Constructor from individual r, g, b, a values (float values 0.0f - 1.0f)
	Color(float red, float green, float blue, float alpha = 1.0f)
		: r(red), g(green), b(blue), a(alpha)
	{
		validate();
	}

	// Constructor from 0-255 integer values (converts to 0.0f - 1.0f)
	Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha = 255)
	{
		// Clamp integers to 0-255 and convert to 0.0f-1.0f floats
		r = clamp(red, 0u, 255u) / 255.0f;
		g = clamp(green, 0u, 255u) / 255.0f;
		b = clamp(blue, 0u, 255u) / 255.0f;
		a = clamp(alpha, 0u, 255u) / 255.0f;
	}

	// Getters
	inline float getR() const { return r; }
	inline float getG() const { return g; }
	inline float getB() const { return b; }
	inline float getA() const { return a; }

	inline XMFLOAT4 getXMFLOAT4()
	{
		return XMFLOAT4(r, g, b, a);
	}

	inline XMFLOAT3 getXMFLOAT3()
	{
		return XMFLOAT3(r, g, b);
	}

	// Setters (with validation)
	// Clamp float values to the range [0.0f, 1.0f]
	inline void setR(float red) { r = clamp(red, 0.0f, 1.0f); }
	inline void setG(float green) { g = clamp(green, 0.0f, 1.0f); }
	inline void setB(float blue) { b = clamp(blue, 0.0f, 1.0f); }
	inline void setA(float alpha) { a = clamp(alpha, 0.0f, 1.0f); }

	inline void print_rgba() const
	{
		print(runtime("R:{}, G:{}, B:{}, A:{}" + NEWL), r, g, b, a);
	}

private:
	float r;
	float g;
	float b;
	float a;

	inline void validate() const
	{
		if (r < 0.0f or r > 1.0f or g < 0.0f or g > 1.0f or b < 0.0f or b > 1.0f or a < 0.0f or a > 1.0f)
		{
			throw std::out_of_range("Color values must be between 0.0 and 1.0.");
		}
	}
};

namespace Colors
{
	const Color AliceBlue = {0.941176534f, 0.972549081f, 1.000000000f};
	const Color AntiqueWhite = {0.980392218f, 0.921568692f, 0.843137324f};
	const Color Aqua = {0.000000000f, 1.000000000f, 1.000000000f};
	const Color Aquamarine = {0.498039246f, 1.000000000f, 0.831372619f};
	const Color Azure = {0.941176534f, 1.000000000f, 1.000000000f};
	const Color Beige = {0.960784376f, 0.960784376f, 0.862745166f};
	const Color Bisque = {1.000000000f, 0.894117713f, 0.768627524f};
	const Color Black = {0.000000000f, 0.000000000f, 0.000000000f};
	const Color BlanchedAlmond = {1.000000000f, 0.921568692f, 0.803921640f};
	const Color Blue = {0.000000000f, 0.000000000f, 1.000000000f};
	const Color BlueViolet = {0.541176498f, 0.168627456f, 0.886274576f};
	const Color Brown = {0.647058845f, 0.164705887f, 0.164705887f};
	const Color BurlyWood = {0.870588303f, 0.721568644f, 0.529411793f};
	const Color CadetBlue = {0.372549027f, 0.619607866f, 0.627451003f};
	const Color Chartreuse = {0.498039246f, 1.000000000f, 0.000000000f};
	const Color Chocolate = {0.823529482f, 0.411764741f, 0.117647067f};
	const Color Coral = {1.000000000f, 0.498039246f, 0.313725501f};
	const Color CornflowerBlue = {0.392156899f, 0.584313750f, 0.929411829f};
	const Color Cornsilk = {1.000000000f, 0.972549081f, 0.862745166f};
	const Color Crimson = {0.862745166f, 0.078431375f, 0.235294133f};
	const Color Cyan = {0.000000000f, 1.000000000f, 1.000000000f};
	const Color DarkBlue = {0.000000000f, 0.000000000f, 0.545098066f};
	const Color DarkCyan = {0.000000000f, 0.545098066f, 0.545098066f};
	const Color DarkGoldenrod = {0.721568644f, 0.525490224f, 0.043137256f};
	const Color DarkGray = {0.662745118f, 0.662745118f, 0.662745118f};
	const Color DarkGreen = {0.000000000f, 0.392156899f, 0.000000000f};
	const Color DarkKhaki = {0.741176486f, 0.717647076f, 0.419607878f};
	const Color DarkMagenta = {0.545098066f, 0.000000000f, 0.545098066f};
	const Color DarkOliveGreen = {0.333333343f, 0.419607878f, 0.184313729f};
	const Color DarkOrange = {1.000000000f, 0.549019635f, 0.000000000f};
	const Color DarkOrchid = {0.600000024f, 0.196078449f, 0.800000072f};
	const Color DarkRed = {0.545098066f, 0.000000000f, 0.000000000f};
	const Color DarkSalmon = {0.913725555f, 0.588235319f, 0.478431404f};
	const Color DarkSeaGreen = {0.560784340f, 0.737254918f, 0.545098066f};
	const Color DarkSlateBlue = {0.282352954f, 0.239215702f, 0.545098066f};
	const Color DarkSlateGray = {0.184313729f, 0.309803933f, 0.309803933f};
	const Color DarkTurquoise = {0.000000000f, 0.807843208f, 0.819607913f};
	const Color DarkViolet = {0.580392182f, 0.000000000f, 0.827451050f};
	const Color DeepPink = {1.000000000f, 0.078431375f, 0.576470613f};
	const Color DeepSkyBlue = {0.000000000f, 0.749019623f, 1.000000000f};
	const Color DimGray = {0.411764741f, 0.411764741f, 0.411764741f};
	const Color DodgerBlue = {0.117647067f, 0.564705908f, 1.000000000f};
	const Color Firebrick = {0.698039234f, 0.133333340f, 0.133333340f};
	const Color FloralWhite = {1.000000000f, 0.980392218f, 0.941176534f};
	const Color ForestGreen = {0.133333340f, 0.545098066f, 0.133333340f};
	const Color Fuchsia = {1.000000000f, 0.000000000f, 1.000000000f};
	const Color Gainsboro = {0.862745166f, 0.862745166f, 0.862745166f};
	const Color GhostWhite = {0.972549081f, 0.972549081f, 1.000000000f};
	const Color Gold = {1.000000000f, 0.843137324f, 0.000000000f};
	const Color Goldenrod = {0.854902029f, 0.647058845f, 0.125490203f};
	const Color Gray = {0.501960814f, 0.501960814f, 0.501960814f};
	const Color Green = {0.000000000f, 0.501960814f, 0.000000000f};
	const Color GreenYellow = {0.678431392f, 1.000000000f, 0.184313729f};
	const Color Honeydew = {0.941176534f, 1.000000000f, 0.941176534f};
	const Color HotPink = {1.000000000f, 0.411764741f, 0.705882370f};
	const Color IndianRed = {0.803921640f, 0.360784322f, 0.360784322f};
	const Color Indigo = {0.294117659f, 0.000000000f, 0.509803951f};
	const Color Ivory = {1.000000000f, 1.000000000f, 0.941176534f};
	const Color Khaki = {0.941176534f, 0.901960850f, 0.549019635f};
	const Color Lavender = {0.901960850f, 0.901960850f, 0.980392218f};
	const Color LavenderBlush = {1.000000000f, 0.941176534f, 0.960784376f};
	const Color LawnGreen = {0.486274540f, 0.988235354f, 0.000000000f};
	const Color LemonChiffon = {1.000000000f, 0.980392218f, 0.803921640f};
	const Color LightBlue = {0.678431392f, 0.847058892f, 0.901960850f};
	const Color LightCoral = {0.941176534f, 0.501960814f, 0.501960814f};
	const Color LightCyan = {0.878431439f, 1.000000000f, 1.000000000f};
	const Color LightGoldenrodYellow = {0.980392218f, 0.980392218f, 0.823529482f};
	const Color LightGreen = {0.564705908f, 0.933333397f, 0.564705908f};
	const Color LightGray = {0.827451050f, 0.827451050f, 0.827451050f};
	const Color LightPink = {1.000000000f, 0.713725507f, 0.756862819f};
	const Color LightSalmon = {1.000000000f, 0.627451003f, 0.478431404f};
	const Color LightSeaGreen = {0.125490203f, 0.698039234f, 0.666666687f};
	const Color LightSkyBlue = {0.529411793f, 0.807843208f, 0.980392218f};
	const Color LightSlateGray = {0.466666698f, 0.533333361f, 0.600000024f};
	const Color LightSteelBlue = {0.690196097f, 0.768627524f, 0.870588303f};
	const Color LightYellow = {1.000000000f, 1.000000000f, 0.878431439f};
	const Color Lime = {0.000000000f, 1.000000000f, 0.000000000f};
	const Color LimeGreen = {0.196078449f, 0.803921640f, 0.196078449f};
	const Color Linen = {0.980392218f, 0.941176534f, 0.901960850f};
	const Color Magenta = {1.000000000f, 0.000000000f, 1.000000000f};
	const Color Maroon = {0.501960814f, 0.000000000f, 0.000000000f};
	const Color MediumAquamarine = {0.400000036f, 0.803921640f, 0.666666687f};
	const Color MediumBlue = {0.000000000f, 0.000000000f, 0.803921640f};
	const Color MediumOrchid = {0.729411781f, 0.333333343f, 0.827451050f};
	const Color MediumPurple = {0.576470613f, 0.439215720f, 0.858823597f};
	const Color MediumSeaGreen = {0.235294133f, 0.701960802f, 0.443137288f};
	const Color MediumSlateBlue = {0.482352972f, 0.407843173f, 0.933333397f};
	const Color MediumSpringGreen = {0.000000000f, 0.980392218f, 0.603921592f};
	const Color MediumTurquoise = {0.282352954f, 0.819607913f, 0.800000072f};
	const Color MediumVioletRed = {0.780392230f, 0.082352944f, 0.521568656f};
	const Color MidnightBlue = {0.098039225f, 0.098039225f, 0.439215720f};
	const Color MintCream = {0.960784376f, 1.000000000f, 0.980392218f};
	const Color MistyRose = {1.000000000f, 0.894117713f, 0.882353008f};
	const Color Moccasin = {1.000000000f, 0.894117713f, 0.709803939f};
	const Color NavajoWhite = {1.000000000f, 0.870588303f, 0.678431392f};
	const Color Navy = {0.000000000f, 0.000000000f, 0.501960814f};
	const Color OldLace = {0.992156923f, 0.960784376f, 0.901960850f};
	const Color Olive = {0.501960814f, 0.501960814f, 0.000000000f};
	const Color OliveDrab = {0.419607878f, 0.556862772f, 0.137254909f};
	const Color Orange = {1.000000000f, 0.647058845f, 0.000000000f};
	const Color OrangeRed = {1.000000000f, 0.270588249f, 0.000000000f};
	const Color Orchid = {0.854902029f, 0.439215720f, 0.839215755f};
	const Color PaleGoldenrod = {0.933333397f, 0.909803987f, 0.666666687f};
	const Color PaleGreen = {0.596078455f, 0.984313786f, 0.596078455f};
	const Color PaleTurquoise = {0.686274529f, 0.933333397f, 0.933333397f};
	const Color PaleVioletRed = {0.858823597f, 0.439215720f, 0.576470613f};
	const Color PapayaWhip = {1.000000000f, 0.937254965f, 0.835294187f};
	const Color PeachPuff = {1.000000000f, 0.854902029f, 0.725490212f};
	const Color Peru = {0.803921640f, 0.521568656f, 0.247058839f};
	const Color Pink = {1.000000000f, 0.752941251f, 0.796078503f};
	const Color Plum = {0.866666734f, 0.627451003f, 0.866666734f};
	const Color PowderBlue = {0.690196097f, 0.878431439f, 0.901960850f};
	const Color Purple = {0.501960814f, 0.000000000f, 0.501960814f};
	const Color Red = {1.000000000f, 0.000000000f, 0.000000000f};
	const Color RosyBrown = {0.737254918f, 0.560784340f, 0.560784340f};
	const Color RoyalBlue = {0.254901975f, 0.411764741f, 0.882353008f};
	const Color SaddleBrown = {0.545098066f, 0.270588249f, 0.074509807f};
	const Color Salmon = {0.980392218f, 0.501960814f, 0.447058856f};
	const Color SandyBrown = {0.956862807f, 0.643137276f, 0.376470625f};
	const Color SeaGreen = {0.180392161f, 0.545098066f, 0.341176480f};
	const Color SeaShell = {1.000000000f, 0.960784376f, 0.933333397f};
	const Color Sienna = {0.627451003f, 0.321568638f, 0.176470593f};
	const Color Silver = {0.752941251f, 0.752941251f, 0.752941251f};
	const Color SkyBlue = {0.529411793f, 0.807843208f, 0.921568692f};
	const Color SlateBlue = {0.415686309f, 0.352941185f, 0.803921640f};
	const Color SlateGray = {0.439215720f, 0.501960814f, 0.564705908f};
	const Color Snow = {1.000000000f, 0.980392218f, 0.980392218f};
	const Color SpringGreen = {0.000000000f, 1.000000000f, 0.498039246f};
	const Color SteelBlue = {0.274509817f, 0.509803951f, 0.705882370f};
	const Color Tan = {0.823529482f, 0.705882370f, 0.549019635f};
	const Color Teal = {0.000000000f, 0.501960814f, 0.501960814f};
	const Color Thistle = {0.847058892f, 0.749019623f, 0.847058892f};
	const Color Tomato = {1.000000000f, 0.388235331f, 0.278431386f};
	const Color Turquoise = {0.250980407f, 0.878431439f, 0.815686345f};
	const Color Violet = {0.933333397f, 0.509803951f, 0.933333397f};
	const Color Wheat = {0.960784376f, 0.870588303f, 0.701960802f};
	const Color White = {1.000000000f, 1.000000000f, 1.000000000f};
	const Color WhiteSmoke = {0.960784376f, 0.960784376f, 0.960784376f};
	const Color Yellow = {1.000000000f, 1.000000000f, 0.000000000f};
	const Color YellowGreen = {0.603921592f, 0.803921640f, 0.196078449f};
};

const Color system_circle_color{1.0f, 1.0f, 1.0f, 1.0f};

class Texture_Atlas_Subrect
{
public:
	Texture_Atlas* texture_atlas;
	Vector2f topleft; // topleft and size are RELATIVE coordinates, from 0.0f to 1.0f!
	Vector2f size;

	Texture_Atlas_Subrect(Texture_Atlas* atlas, Vector2f topleft_arg, Vector2f size_arg) :
		texture_atlas(atlas),
		topleft(topleft_arg),
		size(size_arg)
	{}
};

class Animation
{
public:
	Texture_Animated* texture_animated;
	float current_frame = 0.0f;
	bool looping;
	bool to_be_deleted = false;

	Animation(Texture_Animated* texture, bool looping_arg);

	void advance();
};

class Transformable
{
public:
	Transformable(Vector2d position_arg, double orientation_arg, Vector2d scaling_arg) :
		position(position_arg),
		rotation(orientation_arg),
		scaling(scaling_arg),
		Transformation(GetTranslationMatrix(position_arg) * GetRotationMatrixY(orientation_arg) * GetScalingMatrix(scaling_arg))
	{
		InverseTransformation = GetInverse(Transformation);
	}

	Transformable()
	{}

	inline const Vector2d& getPosition() const
	{
		return position;
	}

	inline double getRotation() const
	{
		return rotation;
	}

	inline const Vector2d& getScaling() const
	{
		return scaling;
	}

	inline void setPosition(Vector2d pos)
	{
		position = pos;

		TransformationNeedsUpdate = true;
		InverseTransformationNeedsUpdate = true;
	}

	inline void setRotation(double rad)
	{
		rotation = normalize_angle_rad(rad);

		TransformationNeedsUpdate = true;
		InverseTransformationNeedsUpdate = true;
	}

	inline void setScaling(Vector2d scale)
	{
		scaling = scale;

		TransformationNeedsUpdate = true;
		InverseTransformationNeedsUpdate = true;
	}

	const Matrix3x3d& getTransformation()
	{
		if (TransformationNeedsUpdate)
		{
			//recompute Transformation
			Transformation = GetTranslationMatrix(position) * GetRotationMatrixY(rotation) * GetScalingMatrix(scaling);
			TransformationNeedsUpdate = false;
		}

		return Transformation;
	}

	const Matrix3x3d& getInverseTransformation()
	{
		if (InverseTransformationNeedsUpdate)
		{
			//recompute InverseTransformation
			InverseTransformation = GetInverse(GetTranslationMatrix(position) * GetRotationMatrixY(rotation) * GetScalingMatrix(scaling));
			InverseTransformationNeedsUpdate = false;
		}

		return InverseTransformation;
	}

private:
	Vector2d position; //global world position
	double rotation;
	Vector2d scaling;

	Matrix3x3d Transformation; //translation, rotation, scaling
	bool TransformationNeedsUpdate = false;
	Matrix3x3d InverseTransformation;
	bool InverseTransformationNeedsUpdate = false;
};

// optimized for purely translatable sprites, rotation and scaling is set at construction and fixed, no inverse transformation, direct matrix editing for translation
class Translatable
{
public:
	Translatable(Vector2d position_arg, double orientation_arg, Vector2d scaling_arg) :
		position(position_arg),
		rotation(orientation_arg),
		scaling(scaling_arg),
		CombinedRotationScaling(GetRotationMatrixY(orientation_arg) * GetScalingMatrix(scaling_arg))
	{
		Transformation = GetTranslationMatrix(position_arg) * CombinedRotationScaling;
	}

	Translatable()
	{}

	inline const Vector2d& getPosition() const
	{
		return position;
	}

	inline double getRotation() const
	{
		return rotation;
	}

	inline const Vector2d& getScaling() const
	{
		return scaling;
	}

	inline void setPosition(Vector2d pos)
	{
		position = pos;
		Transformation.matrix[0][2] = pos.x;
		Transformation.matrix[1][2] = pos.y;
	}

	inline void translate(Vector2d velocity)
	{
		position += velocity;
		Transformation.matrix[0][2] += velocity.x;
		Transformation.matrix[1][2] += velocity.y;
	}

	const Matrix3x3d& getTransformation()
	{
		return Transformation;
	}

private:
	Vector2d position; //global world position
	double rotation;
	Vector2d scaling;

	Matrix3x3d Transformation; //translation, rotation, scaling
	Matrix3x3d CombinedRotationScaling;
};

class Sprite_Translatable_Only : public Translatable
{
public:
	Color color;
	Draw_Layer drawing_layer;
	variant<Texture_Atlas_Subrect*, Animation> graphics;
	Blend_Mode blending_mode;
	Shader_Lighting_Mode shader_lighting_mode;	// 0: normal lighting, 1: no lighting

	Sprite_Translatable_Only(Shader_Lighting_Mode shader_lighting_mode_arg, Color color_arg, Draw_Layer drawing_layer_arg, variant<Texture_Atlas_Subrect*, Animation> graphics_arg, Blend_Mode blending_mode_arg,
		   Vector2d position_arg, double orientation_arg, Vector2d scaling_arg) :
		Translatable(position_arg, orientation_arg, scaling_arg),
		shader_lighting_mode(shader_lighting_mode_arg),
		color(color_arg),
		drawing_layer(drawing_layer_arg),
		graphics(graphics_arg),
		blending_mode(blending_mode_arg)
	{}

	Sprite_Translatable_Only()
	{}

	void Enqueue();
};

class Camera final
{
private:
	double zoom = 1.0;

public:
	Vector2d position;

	Vector2d position_leading;
	double zoom_leading = 1.0;

	// Projection matrix, initialized as identity matrix
	XMMATRIX projection_matrix = DirectX::XMMatrixIdentity();

	DoubleRect relative_rectangle;
	DoubleRect world_rectangle;
	Vector2d scaling;

	bool zoom_changed = false;

	Camera()
	{}

	//refresh scaling and relative bounding box
	void refresh()
	{
		scaling = Vector2d(screen_resolution.x * zoom, screen_resolution.y * zoom);

		Vector2d cam_scale_half = scaling / 2.0;
		Vector2d cam_topleft = cam_scale_half.get_negated();

		relative_rectangle = DoubleRect{cam_topleft, scaling};
		world_rectangle = DoubleRect{cam_topleft + position, scaling};
	}

	inline double get_zoom() const
	{
		return zoom;
	}

	void set_zoom(double new_zoom)
	{
		zoom = new_zoom;
		zoom_changed = true;
		refresh();
		update_projection_matrix();
	}

	void Step();

	void update_projection_matrix()
	{
		// Update projection matrix - uses DirectX function to build an orthogonal projection matrix for a left-handed coordinate system.
		projection_matrix = DirectX::XMMatrixOrthographicLH(float(screen_resolution.x) * float(zoom), float(screen_resolution.y) * float(zoom), 0.0f, 1.0f);
	}
};
Camera camera;

class Sprite : public Transformable
{
public:
	Color color;
	Draw_Layer drawing_layer;
	variant<Texture_Atlas_Subrect*, Animation> graphics;
	Texture_Atlas_Subrect* glowmap_subrect;
	Blend_Mode blending_mode;
	Shader_Lighting_Mode shader_lighting_mode;	// 0 = normal lighting, 1 = no lighting

	Sprite(Shader_Lighting_Mode shader_lighting_mode_arg, Color color_arg, Draw_Layer drawing_layer_arg, variant<Texture_Atlas_Subrect*, Animation> graphics_arg, Blend_Mode blending_mode_arg,
		   Vector2d position_arg, double orientation_arg, Vector2d scaling_arg, Texture_Atlas_Subrect* glowmap_subrect_arg = nullptr) :
		Transformable(position_arg, orientation_arg, scaling_arg),
		shader_lighting_mode(shader_lighting_mode_arg),
		color(color_arg),
		drawing_layer(drawing_layer_arg),
		graphics(graphics_arg),
		glowmap_subrect(glowmap_subrect_arg),
		blending_mode(blending_mode_arg)
	{}

	Sprite()
	{}

	DoubleRect get_sprite_bounding_box(bool world_coords);

	void Enqueue();
};

Sprite background_sprite;

class Icon
{
public:
	Sprite sprite;
	Vector2d size_on_screen{double(screen_resolution.x) * 0.0125,
							double(screen_resolution.x) * 0.0125}; //size of the icon in pixels

	Icon(Color color_arg,
		 Draw_Layer drawing_layer_arg,
		 Blend_Mode blending_mode_arg,
		 Texture_Atlas_Subrect* texture_subrect_arg,
		 Vector2d size_on_screen_arg) :
		sprite(Shader_Lighting_Mode::no_lighting, color_arg, drawing_layer_arg, texture_subrect_arg, blending_mode_arg,
			   Vector2d{0.0,0.0}, 0, size_on_screen_arg * camera.get_zoom()),
		size_on_screen(size_on_screen_arg)
	{}
};

// A function to convert a line defined by vertices with a linestrip topology (a continuous line connecting all the points)
// to a line defined by vertices with a linelist topology (composed of line segments, two vertices for each line segment).
// Use to render a linestrip using the Linelist class.
vector<Vector2d> linestrip_to_linelist(vector<Vector2d>& linestrip_verts)
{
	vector<Vector2d> linelist_verts;
	linelist_verts.reserve((linestrip_verts.size() - 1) * 2);

	for (int i = 0; i < linestrip_verts.size() - 1; ++i)
	{
		linelist_verts.emplace_back(linestrip_verts[i]);
		linelist_verts.emplace_back(linestrip_verts[i + 1]);
	}

	return linelist_verts;
}

vector<Color> linestrip_colors_to_linelist(vector<Color>& linestrip_colors)
{
	vector<Color> linelist_colors;
	linelist_colors.reserve((linestrip_colors.size() - 1) * 2);

	for (int i = 0; i < linestrip_colors.size() - 1; ++i)
	{
		linelist_colors.emplace_back(linestrip_colors[i]);
		linelist_colors.emplace_back(linestrip_colors[i + 1]);
	}

	return linelist_colors;
}

class Linelist final
{
public:
	Draw_Layer drawing_layer;
	vector<Vector2d> vertices;
	Blend_Mode blending_mode;
	vector<Color> colors; // contains either a single color for all vertices, or multiple colors - one for each vertex

	Linelist(Color color_arg, Draw_Layer drawing_layer_arg, Blend_Mode blending_mode_arg) :
		drawing_layer(drawing_layer_arg), blending_mode(blending_mode_arg)
	{
		colors.emplace_back(color_arg);
	}

	Linelist(Color color_arg, Draw_Layer drawing_layer_arg, Blend_Mode blending_mode_arg, vector<Vector2d> vertices_arg) :
		drawing_layer(drawing_layer_arg), blending_mode(blending_mode_arg), vertices(vertices_arg)
	{
		colors.emplace_back(color_arg);
	}

	Linelist(Draw_Layer drawing_layer_arg, Blend_Mode blending_mode_arg, vector<Vector2d> vertices_arg, vector<Color> vertex_colors_arg) :
		drawing_layer(drawing_layer_arg), blending_mode(blending_mode_arg), vertices(vertices_arg), colors(vertex_colors_arg)
	{}

	void Enqueue();

};

Linelist jump_node_lines{Colors::Aquamarine, Draw_Layer::Background_and_Universe, Blend_Mode::Opaque};

enum class Light_Type
{
	point, tube, spotlight
};

class Light final
{
public:
	Color color;
	float intensity;
	float falloff_exp;

	Vector2d position1;
	Vector2d position2; //for tube light

	float direction; // for spotlight
	float cone_exp;

	Light_Type type;

	bool disabled;

	double culling_threshold_distance;

	Light()
	{}

	// point
	Light(bool disabled_arg,
		  Color color_arg, float intensity_arg, float falloff_exp_arg,
		  Vector2d position_arg) :
		disabled(disabled_arg),
		type(Light_Type::point),
		color(color_arg),
		intensity(intensity_arg),
		falloff_exp(falloff_exp_arg),
		position1(position_arg)
	{
		set_culling_threshold_distance();
	}

	// tube
	Light(bool disabled_arg,
		  Color color_arg, float intensity_arg, float falloff_exp_arg,
		  Vector2d position1_arg, Vector2d position2_arg) :
		disabled(disabled_arg),
		type(Light_Type::tube),
		color(color_arg),
		intensity(intensity_arg),
		falloff_exp(falloff_exp_arg),
		position1(position1_arg),
		position2(position2_arg)
	{
		set_culling_threshold_distance();
	}

	// spotlight
	Light(bool disabled_arg,
		  Color color_arg, float intensity_arg, float falloff_exp_arg,
		  Vector2d position_arg,
		  float direction_arg, float cone_exp_arg) :
		disabled(disabled_arg),
		type(Light_Type::spotlight),
		color(color_arg),
		intensity(intensity_arg),
		falloff_exp(falloff_exp_arg),
		position1(position_arg),
		direction(direction_arg),
		cone_exp(cone_exp_arg)
	{
		set_culling_threshold_distance();
	}

	inline void set_culling_threshold_distance();

	void Enqueue();
};

//color code for use in Deferred String color
unsigned long get_string_color(int a, int b, int g, int r)
{
	return ((a & 0xff) << 24) + ((b & 0xff) << 16) + ((g & 0xff) << 8)
		+ (r & 0xff);
}

class Particle
{
public:
	Sprite_Translatable_Only sprite;
	int lifetime;
	Vector2d velocity;

	Particle(Vector2d position_arg, Vector2d scaling_arg, Color color_arg, Draw_Layer drawing_layer_arg,
			 int lifetime_arg, Vector2d velocity_arg) :
		sprite(Shader_Lighting_Mode::no_lighting, color_arg, drawing_layer_arg, name_texture_atlas_subrect_map["particle"], Blend_Mode::Additive,
			   position_arg, 0.0, scaling_arg),
		lifetime(lifetime_arg),
		velocity(velocity_arg)
	{}

	void Step()
	{
		sprite.translate(velocity);
		lifetime -= 1;
	}
};

class Effect
{
public:
	Sprite_Translatable_Only sprite;
	Vector2d velocity;
	Light light;

	Effect()
	{}

	Effect(Vector2d position_arg, double rotation_arg, Vector2d scaling_arg, Color color_arg, Animation* animation_arg, Blend_Mode blending_mode_arg, Draw_Layer draw_layer_arg,
		   Vector2d velocity_arg, bool light_disabled_arg = true, const float light_falloff_exponent_arg = 0.0f, const Color& light_diffuse_arg = Color{0.0f, 0.0f, 0.0f}, float light_intensity_arg = 0.0f) :
		sprite(Shader_Lighting_Mode::no_lighting, color_arg, draw_layer_arg, *animation_arg, blending_mode_arg,
			   position_arg, rotation_arg, scaling_arg),
		velocity(velocity_arg),
		light(light_disabled_arg, light_diffuse_arg, light_intensity_arg, light_falloff_exponent_arg, position_arg)
	{
		Animation& animation = get<Animation>(sprite.graphics);
		animation.looping = false;
	}

	void Step();
};

class Deferred_String
{
public:
	wstring str;
	float font_size;
	Vector2f screen_position;
	unsigned int color = 0xff66cd00;

	Deferred_String(wstring str_arg,
					float font_size_arg,
					Vector2f screen_position_arg,
					unsigned int color_arg) :
		str(str_arg),
		font_size(font_size_arg),
		screen_position(screen_position_arg),
		color(color_arg)
	{}
};

vector<Deferred_String> strings_to_draw;

void draw_string_deferred(string& str, float font_size, float screen_pos_x, float screen_pos_y, unsigned int color = 0xff66cd00)
{
	strings_to_draw.emplace_back(string_to_wstring(str), font_size, Vector2f(screen_pos_x, screen_pos_y), color);
}

class Vertex final
{
public:
	int32_t texture_ID;
	int32_t lighting_mode; // 0: normal lighting,  1: no lighting
	XMFLOAT2 position;

	XMFLOAT4 color;

	XMFLOAT2 uv_diffuse;
	XMFLOAT2 uv_glow;

	Vertex(const int32_t texture_ID_arg, const int32_t lighting_mode_arg, XMFLOAT2 position_arg,
		   Color color_arg,
		   XMFLOAT2 uv_diffuse_arg, XMFLOAT2 uv_glow_arg) :
		texture_ID(texture_ID_arg),
		lighting_mode(lighting_mode_arg),
		position(position_arg),

		color(color_arg.getXMFLOAT4()),

		uv_diffuse(uv_diffuse_arg),
		uv_glow(uv_glow_arg)
	{}
};

class Drawing_Layer
{
public:
	vector<Vertex> trianglelist_opaque;
	vector<Vertex> trianglelist_alpha;
	vector<Vertex> trianglelist_additive;

	vector<Vertex> linelist_additive;
	vector<Vertex> linelist_opaque;

	void Draw()
	{
			// Quads
		D3D11_Device_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// **** Opaque Quads **** //
		draw_vertices(trianglelist_opaque, Topology_Type::Trianglelist);


		// **** Alpha Quads **** //
		D3D11_Device_Context->OMSetBlendState(Blend_Alpha, nullptr, 0xffffffff); //Alpha blending

		draw_vertices(trianglelist_alpha, Topology_Type::Trianglelist);


		// **** Additive Quads **** //
		D3D11_Device_Context->OMSetBlendState(Blend_Add, nullptr, 0xffffffff); //Additive blending

		draw_vertices(trianglelist_additive, Topology_Type::Trianglelist);


			// Lines
		D3D11_Device_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// **** Additive Lines **** //
		draw_vertices(linelist_additive, Topology_Type::Linelist);

		// **** Opaque Lines **** //
		D3D11_Device_Context->OMSetBlendState(nullptr, nullptr, 0xffffffff); //Opaque blending

		draw_vertices(linelist_opaque, Topology_Type::Linelist);
	}


private:
	void draw_vertices(vector<Vertex>& vertices, Topology_Type topology)
	{
		if (not vertices.empty())
		{
			const UINT offset_zero = 0;
			const UINT stride_vertex = sizeof(Vertex);
			size_t vertex_count = vertices.size();

			UpdateBuffer(VertexBuffer, &vertices[0], stride_vertex * vertex_count);	// update VertexBuffer data
			D3D11_Device_Context->IASetVertexBuffers(0, 1, &(VertexBuffer), &stride_vertex, &offset_zero);	// set VertexBuffer

			if (topology == Topology_Type::Trianglelist)
			{
				D3D11_Device_Context->DrawIndexed((vertex_count / 4) * 6, 0, 0);	// draw quads (indexed trianglelist)
			}
			else //Topology_Type == Linelist
			{
				D3D11_Device_Context->Draw(vertices.size(), 0);	// draw lines (Linelist)
			}

			vertices.clear();
		}
	}
};

vector<Drawing_Layer> Drawing_Layers(6); //from back to front (Painter's algorithm)

class Texture_Atlas final
{
public:
	const int id;
	string name;
	Vector2i dimensions;

	ID3D11ShaderResourceView* resource_view = nullptr;

	Texture_Atlas(const std::string& name_arg, LPCWSTR path_arg, Vector2i dimensions_arg) :
		id(texture_id_counter),
		name(name_arg),
		dimensions(dimensions_arg)
	{
		++texture_id_counter;

		// Load texture from file using DirectX::CreateWICTextureFromFile
		HRESULT hresult = DirectX::CreateWICTextureFromFile(D3D11_Device, D3D11_Device_Context, path_arg, nullptr, &resource_view);
		if (FAILED(hresult))
		{
			// Handle the error
			print(runtime("ERROR: Texture loading failed. (HRESULT:{}) path:{}" + NEWL), int(hresult), wstring_to_string(path_arg));
		}
	}

	~Texture_Atlas()
	{
		if (resource_view != nullptr)
		{
			resource_view->Release();
			resource_view = nullptr;
		}
	}
};

class Texture_Animated final
{
public:
	const int id;
	string name;
	Vector2i dimensions;

	float animation_speed;
	const Vector2i frame_dimensions;
	vector<FloatRect> frames;

	ID3D11ShaderResourceView* resource_view = nullptr;

	Texture_Animated(const std::string& name_arg, LPCWSTR path_arg, Vector2i dimensions_arg, const int last_frame_index_arg, const float animation_speed_arg,
					 Vector2i frame_dimensions_arg) :
		id(texture_id_counter),
		name(name_arg),
		dimensions(dimensions_arg),
		animation_speed(animation_speed_arg),
		frame_dimensions(frame_dimensions_arg)
	{
		++texture_id_counter;

		// Load texture from file using DirectX::CreateWICTextureFromFile
		HRESULT hresult = DirectX::CreateWICTextureFromFile(D3D11_Device, D3D11_Device_Context, path_arg, nullptr, &resource_view);
		if (FAILED(hresult))
		{
			// Handle the error
			print(runtime("ERROR: Texture loading failed. (HRESULT:{}) path:{}" + NEWL), int(hresult), wstring_to_string(path_arg));
		}

		// compute animation frames
		const int number_of_frames = last_frame_index_arg + 1;
		frames.reserve(number_of_frames);

		for (int i = 0; i < number_of_frames; i++)
		{
			FloatRect frame_subrect = compute_frame_subrect(i);
			frames.emplace_back(frame_subrect);
		}
	}

	FloatRect compute_frame_subrect(int frame_index) const
	{
		// frame_subrect is in RELATIVE coordinates from 0.0f to 1.0f!

		const double hor_step_size = double(frame_dimensions.x) / double(dimensions.x);
		const double ver_step_size = double(frame_dimensions.y) / double(dimensions.y);
		const double num_ver_steps = double(dimensions.x) / double(frame_dimensions.x);

		const int frame_row = floor(double(frame_index) / num_ver_steps);
		const int frame_column = floor(double(frame_index) - (double(frame_row) * num_ver_steps));

		FloatRect frame_subrect;
		frame_subrect.topleft.x = hor_step_size * frame_column;
		frame_subrect.topleft.y = ver_step_size * frame_row;
		frame_subrect.size.x = hor_step_size;
		frame_subrect.size.y = ver_step_size;

		return frame_subrect;
	}

	~Texture_Animated()
	{
		if (resource_view != nullptr)
		{
			resource_view->Release();
			resource_view = nullptr;
		}
	}
};

class Light_HLSL final //members must be aligned to float4, otherwise padded
{
public:
	XMFLOAT3 color;
	float falloff_exp;

	XMFLOAT2 position1;
	XMFLOAT2 position2;

	float direction;
	float cone_exp;
	float type;
	float padding;

	Light_HLSL()
	{}
};

class PS_Constant_Buffer final
{
public:
	array<Light_HLSL, MAX_LIGHTS> Lights;
	DirectX::XMINT4 number_of_active_lights; //only the first int is used, the rest is padding
};
PS_Constant_Buffer PS_constant_buffer;

enum class Key_Event
{
	None, Pressed, Released
};

enum class Mouse_Button
{
	Left, Right, Wheel
};

class Key final
{
public:
	const int code;
	bool pressed_down = false;
	nanoseconds hold_time;
	Key_Event key_event = Key_Event::None;

	Key(const int code_arg, vector<Key*>& key_ptrs_arg) :
		code(code_arg)
	{
		key_ptrs_arg.emplace_back(this);
	}
};

class Mouse final
{
public:
	Sprite sprite;

	Vector2f screen_position_in_pixels; //screen center at (0, 0)
	Vector2f screen_position_for_imgui; //topleft screen corner at (0, 0)
	Vector2f screen_position_normalized; //from -1 to 1
	Vector2d world_position;
	Vector2d scaling;
	const float sensitivity = 1.0f;

	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE current_state;

	vector<Key*> Key_ptrs;

	Key Left_Button = Key(0, Key_ptrs);
	Key Right_Button = Key(1, Key_ptrs);
	Key Wheel = Key(2, Key_ptrs);

	float wheel_turns = 0;

	Mouse()
	{}

	void Refresh_Key_State();
	void Refresh_Movement();
};

Mouse mouse;

class Keyboard final
{
public:
	IDirectInputDevice8* DIKeyboard;
	BYTE current_state[256];

	vector<Key*> Key_ptrs;

	Key Key_ESCAPE = Key(DIK_ESCAPE, Key_ptrs);
	Key Key_BACKSPACE = Key(DIK_BACKSPACE, Key_ptrs);
	Key Key_LCONTROL = Key(DIK_LCONTROL, Key_ptrs);
	Key Key_LSHIFT = Key(DIK_LSHIFT, Key_ptrs);
	Key Key_TAB = Key(DIK_TAB, Key_ptrs);
	Key Key_ADD = Key(DIK_ADD, Key_ptrs);
	Key Key_SUBTRACT = Key(DIK_SUBTRACT, Key_ptrs);
	Key Key_PGDN = Key(DIK_PGDN, Key_ptrs);
	Key Key_RIGHT = Key(DIK_RIGHT, Key_ptrs);
	Key Key_LEFT = Key(DIK_LEFT, Key_ptrs);	
	Key Key_UP = Key(DIK_UP, Key_ptrs);	
	Key Key_DOWN = Key(DIK_DOWN, Key_ptrs);
	Key Key_SPACE = Key(DIK_SPACE, Key_ptrs);
	Key Key_RETURN = Key(DIK_RETURN, Key_ptrs);
	Key Key_LALT = Key(DIK_LMENU, Key_ptrs);

	Key Key_A = Key(DIK_A, Key_ptrs);
	Key Key_B = Key(DIK_B, Key_ptrs);
	Key Key_C = Key(DIK_C, Key_ptrs);
	Key Key_D = Key(DIK_D, Key_ptrs);
	Key Key_E = Key(DIK_E, Key_ptrs);
	Key Key_F = Key(DIK_F, Key_ptrs);
	Key Key_G = Key(DIK_G, Key_ptrs);
	Key Key_H = Key(DIK_H, Key_ptrs);
	Key Key_I = Key(DIK_I, Key_ptrs);
	Key Key_J = Key(DIK_J, Key_ptrs);
	Key Key_K = Key(DIK_K, Key_ptrs);
	Key Key_L = Key(DIK_L, Key_ptrs);
	Key Key_M = Key(DIK_M, Key_ptrs);
	Key Key_N = Key(DIK_N, Key_ptrs);
	Key Key_O = Key(DIK_O, Key_ptrs);
	Key Key_P = Key(DIK_P, Key_ptrs);
	Key Key_Q = Key(DIK_Q, Key_ptrs);
	Key Key_R = Key(DIK_R, Key_ptrs);
	Key Key_S = Key(DIK_S, Key_ptrs);
	Key Key_T = Key(DIK_T, Key_ptrs);
	Key Key_U = Key(DIK_U, Key_ptrs);
	Key Key_V = Key(DIK_V, Key_ptrs);
	Key Key_W = Key(DIK_W, Key_ptrs);
	Key Key_X = Key(DIK_X, Key_ptrs);
	Key Key_Y = Key(DIK_Y, Key_ptrs);
	Key Key_Z = Key(DIK_Z, Key_ptrs);

	Key Key_0 = Key(DIK_NUMPAD0, Key_ptrs);
	Key Key_1 = Key(DIK_NUMPAD1, Key_ptrs);
	Key Key_2 = Key(DIK_NUMPAD2, Key_ptrs);
	Key Key_3 = Key(DIK_NUMPAD3, Key_ptrs);
	Key Key_4 = Key(DIK_NUMPAD4, Key_ptrs);
	Key Key_5 = Key(DIK_NUMPAD5, Key_ptrs);
	Key Key_6 = Key(DIK_NUMPAD6, Key_ptrs);
	Key Key_7 = Key(DIK_NUMPAD7, Key_ptrs);
	Key Key_8 = Key(DIK_NUMPAD8, Key_ptrs);
	Key Key_9 = Key(DIK_NUMPAD9, Key_ptrs);

	void Refresh_Key_State();

	Keyboard()
	{}
};

Keyboard keyboard;

// Functions and class method definitions //

void Print_DX_Matrix(const XMMATRIX& m, const int decimals)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			print("{:.{}}", XMVectorGetByIndex(m.r[i], j), decimals);
			print("  ");
		}
		print(NEWL);
	}
}

// string to wstring
wstring string_to_wstring(const string& input)
{
	return wstring(input.begin(), input.end());
}

// wstring to string
string wstring_to_string(const wstring& input)
{
	return string(input.begin(), input.end());
}

template <typename object_type>
object_type* choose(vector<object_type>& choices)
{
	return &choices[get_random_int_in_range(0, choices.size() - 1)];
}

// get the vertices of a linelist representing a circle
vector<Vector2d> GetCircleVertices(Vector2d position_arg, double radius_arg, int resolution_arg)
{
	vector<Vector2d> verts;
	verts.reserve(resolution_arg * 2);

	for (int i = 0; i < resolution_arg; ++i)
	{
		double angle = (TAU / resolution_arg) * i;

		verts.emplace_back((Vector2d(sin(angle), cos(angle)) * radius_arg) + position_arg);

		if (i != 0)
		{
			verts.emplace_back((Vector2d(sin(angle), cos(angle)) * radius_arg) + position_arg);
		}

		if (i == resolution_arg - 1)
		{
			verts.emplace_back((Vector2d(sin(0.0), cos(0.0)) * radius_arg) + position_arg);
		}
	}

	return verts;
}

// converts collision polygon from texture coordinates (positive integers, topleft corner is (0, 0) )
// to sprite coordinates (signed doubles from -0.5 to 0.5, center of the sprite is (0.0, 0.0) )
vector<Vector2d> CollisionPolygonToSpriteCoords(const vector<Vector2d>& polygon_arg, Vector2d texture_size_arg)
{
	vector<Vector2d> Result;

	Result.reserve(polygon_arg.size());

	for (const Vector2d& coord : polygon_arg)
	{
		Result.emplace_back((coord.x / texture_size_arg.x) - 0.5,
							(coord.y / texture_size_arg.y) - 0.5);
	}

	return Result;
}

// sprite coords are from -0.5 to 0.5
Vector2d WorldToSpriteCoords(const Vector2d& world_point, Sprite& sprite)
{
	return TransformVector(world_point, sprite.getInverseTransformation());
}

// sprite coords are from -0.5 to 0.5
Vector2d SpriteToWorldCoords(const Vector2d& sprite_point, Sprite& sprite)
{
	return TransformVector(sprite_point, sprite.getTransformation());
}

// screen coords are from -1.0 to 1.0
Vector2f WorldToScreenCoords(Vector2d world_point)
{
	world_point -= camera.position;
	return Vector2f(world_point.x / (camera.get_zoom() * double(screen_resolution.x) * 0.5),
					world_point.y / (camera.get_zoom() * double(screen_resolution.y) * 0.5));
}

// screen coords are from -1.0 to 1.0
Vector2d ScreenToWorldCoords(Vector2f screen_point)
{
	return (Vector2d(screen_point.x * float(screen_resolution.x),
					 screen_point.y * float(screen_resolution.y)) * camera.get_zoom() * 0.5) + camera.position;
}

// returns pixel coords, screen center at (0, 0)
Vector2f WorldToScreenPixel(Vector2d world_point)
{
	return (world_point - camera.position) / camera.get_zoom();
}

//screen_px_point argument in pixels, screen center at (0, 0)
Vector2d ScreenPixelToWorld(Vector2f screen_px_point)
{
	return (Vector2d{screen_px_point} *camera.get_zoom()) + camera.position;
}

// from pixel coords with origin at screen center to pixel coords with origin at screen topleft corner (as used in ImGui)
Vector2f ScreenPixelToImguiCoords(Vector2f pixel)
{
	Vector2f screen_position_for_imgui;
	screen_position_for_imgui.x = pixel.x + float(screen_resolution.x) / 2.0f;
	screen_position_for_imgui.y = -pixel.y + float(screen_resolution.y) / 2.0f;
	return screen_position_for_imgui;
}

void UpdateBuffer(ID3D11Buffer* const Buffer, const void* source, const size_t size)	// uploads data to DX11 buffer (used to upload vertices to vertex buffer before drawing)
{
	D3D11_MAPPED_SUBRESOURCE ms = {};
	D3D11_Device_Context->Map(Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, source, size);
	D3D11_Device_Context->Unmap(Buffer, NULL);
}

void Animation::advance()
{
	current_frame += texture_animated->animation_speed; // advance animation

	int last_frame_index = texture_animated->frames.size() - 1;

	if (current_frame > last_frame_index) // if animation is at the end
	{
		if (looping) // if looping, rewind
		{
			current_frame = 0.0f;
		}
		else // not looping, to be deleted
		{
			to_be_deleted = true;
		}
	}

}

Animation::Animation(Texture_Animated* texture, bool looping_arg) :
	texture_animated(texture),
	looping(looping_arg)
{
	current_frame -= texture->animation_speed; // we run advance() before rendering the first frame, so the initial current_frame is set to "frame -1"
}

inline array<Vector2d, 4> get_transformed_cam_relative_verts(const Matrix3x3d& sprite_transform, const Vector2d& cam_position)
{
	double val_00_plus = sprite_transform.matrix[0][0] * 0.5;
	double val_00_minus = sprite_transform.matrix[0][0] * (-0.5);

	double val_01_plus = sprite_transform.matrix[0][1] * 0.5;
	double val_01_minus = sprite_transform.matrix[0][1] * (-0.5);

	double val_10_plus = sprite_transform.matrix[1][0] * 0.5;
	double val_10_minus = sprite_transform.matrix[1][0] * (-0.5);

	double val_11_plus = sprite_transform.matrix[1][1] * 0.5;
	double val_11_minus = sprite_transform.matrix[1][1] * (-0.5);

	double val_02 = sprite_transform.matrix[0][2];
	double val_12 = sprite_transform.matrix[1][2];

	return {
	Vector2d(
			(val_00_minus + val_01_minus + val_02) - cam_position.x,
			(val_10_minus + val_11_minus + val_12) - cam_position.y
			),

	Vector2d(
			(val_00_plus + val_01_minus + val_02) - cam_position.x,
			(val_10_plus + val_11_minus + val_12) - cam_position.y
			),

	Vector2d(
			(val_00_minus + val_01_plus + val_02) - cam_position.x,
			(val_10_minus + val_11_plus + val_12) - cam_position.y
			),

	Vector2d(
			(val_00_plus + val_01_plus + val_02) - cam_position.x,
			(val_10_plus + val_11_plus + val_12) - cam_position.y
			)
	};
}

inline array<Vector2d, 4> get_transformed_world_verts(const Matrix3x3d& sprite_transform)
{
	double val_00_plus = sprite_transform.matrix[0][0] * 0.5;
	double val_00_minus = sprite_transform.matrix[0][0] * (-0.5);

	double val_01_plus = sprite_transform.matrix[0][1] * 0.5;
	double val_01_minus = sprite_transform.matrix[0][1] * (-0.5);

	double val_10_plus = sprite_transform.matrix[1][0] * 0.5;
	double val_10_minus = sprite_transform.matrix[1][0] * (-0.5);

	double val_11_plus = sprite_transform.matrix[1][1] * 0.5;
	double val_11_minus = sprite_transform.matrix[1][1] * (-0.5);

	double val_02 = sprite_transform.matrix[0][2];
	double val_12 = sprite_transform.matrix[1][2];

	return {
	Vector2d(
			val_00_minus + val_01_minus + val_02,
			val_10_minus + val_11_minus + val_12
			),

	Vector2d(
			val_00_plus + val_01_minus + val_02,
			val_10_plus + val_11_minus + val_12
			),

	Vector2d(
			val_00_minus + val_01_plus + val_02,
			val_10_minus + val_11_plus + val_12
			),

	Vector2d(
			val_00_plus + val_01_plus + val_02,
			val_10_plus + val_11_plus + val_12
			),
	};
}

void Sprite::Enqueue()
{
	const Matrix3x3d& transform_matrix = getTransformation();

	array<Vector2d, 4> verts = get_transformed_cam_relative_verts(transform_matrix, camera.position);

	DoubleRect sprite_bb = get_bounding_box(verts); //sprite bounding box

	if (sprite_bb.size.x / camera.relative_rectangle.size.x > 1.0 / double(screen_resolution.x) and
		camera.relative_rectangle.intersects_rect(sprite_bb))
	{
		Vector2f subrect_topleft;
		Vector2f subrect_size;
		int texture_id;

		switch (graphics.index()) // returns the index of the type contained in the graphics variant: 0: Texture_Atlas_Subrect, 1: Animation
		{
			case 0: // static Texture_Atlas_Subrect
			{
				Texture_Atlas_Subrect* texture_atlas_subrect = get<Texture_Atlas_Subrect*>(graphics);

				subrect_topleft = texture_atlas_subrect->topleft;
				subrect_size = texture_atlas_subrect->size;

				texture_id = texture_atlas_subrect->texture_atlas->id;
			}
			break;

			case 1: // Animation
			{
				Animation& animation = get<Animation>(graphics);

				subrect_topleft = animation.texture_animated->frames[int(animation.current_frame)].topleft;
				subrect_size = animation.texture_animated->frames[int(animation.current_frame)].size;

				texture_id = animation.texture_animated->id;
			}
			break;
		}

		float min_x = subrect_topleft.x;
		float min_y = subrect_topleft.y;
		float max_x = subrect_topleft.x + subrect_size.x;
		float max_y = subrect_topleft.y + subrect_size.y;

		float glow_min_x = -1.0f;
		float glow_min_y = -1.0f;
		float glow_max_x = -1.0f;
		float glow_max_y = -1.0f;

		if (glowmap_subrect)
		{
			glow_min_x = glowmap_subrect->topleft.x;
			glow_min_y = glowmap_subrect->topleft.y;
			glow_max_x = glowmap_subrect->topleft.x + glowmap_subrect->size.x;
			glow_max_y = glowmap_subrect->topleft.y + glowmap_subrect->size.y;
		}

		vector<Vertex>* vertex_vect = nullptr;
		switch (blending_mode)
		{
			case Blend_Mode::Opaque:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_opaque;
				break;

			case Blend_Mode::Alpha:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_alpha;
				break;

			case Blend_Mode::Additive:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_additive;
				break;
		}

		// emplace quad vertices into vector of vertices to draw
		// texture_ID, lighting_mode, position, color, texture UV diffuse, texture UV glow
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[0].x, verts[0].y), color, XMFLOAT2(min_x, min_y), XMFLOAT2(glow_min_x, glow_min_y));	// 0
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[1].x, verts[1].y), color, XMFLOAT2(max_x, min_y), XMFLOAT2(glow_max_x, glow_min_y));	// 1
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[2].x, verts[2].y), color, XMFLOAT2(min_x, max_y), XMFLOAT2(glow_min_x, glow_max_y));	// 2
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[3].x, verts[3].y), color, XMFLOAT2(max_x, max_y), XMFLOAT2(glow_max_x, glow_max_y));	// 3
	}
}

void Sprite_Translatable_Only::Enqueue()
{
	const Matrix3x3d& transform_matrix = getTransformation();

	array<Vector2d, 4> verts = get_transformed_cam_relative_verts(transform_matrix, camera.position);

	DoubleRect sprite_bb = get_bounding_box(verts); //sprite bounding box

	if (sprite_bb.size.x / camera.relative_rectangle.size.x > 1.0 / double(screen_resolution.x) and
		camera.relative_rectangle.intersects_rect(sprite_bb))
	{
		Vector2f subrect_topleft;
		Vector2f subrect_size;
		int texture_id;

		switch (graphics.index()) // returns the index of the type contained in the graphics variant: 0: Texture_Atlas_Subrect, 1: Animation
		{
			case 0: // static Texture_Atlas_Subrect
			{
				Texture_Atlas_Subrect* texture_atlas_subrect = get<Texture_Atlas_Subrect*>(graphics);

				subrect_topleft = texture_atlas_subrect->topleft;
				subrect_size = texture_atlas_subrect->size;

				texture_id = texture_atlas_subrect->texture_atlas->id;
			}
			break;

			case 1: // Animation
			{
				Animation& animation = get<Animation>(graphics);

				subrect_topleft = animation.texture_animated->frames[int(animation.current_frame)].topleft;
				subrect_size = animation.texture_animated->frames[int(animation.current_frame)].size;

				texture_id = animation.texture_animated->id;
			}
			break;
		}

		float min_x = subrect_topleft.x;
		float min_y = subrect_topleft.y;
		float max_x = subrect_topleft.x + subrect_size.x;
		float max_y = subrect_topleft.y + subrect_size.y;

		vector<Vertex>* vertex_vect = nullptr;
		switch (blending_mode)
		{
			case Blend_Mode::Opaque:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_opaque;
				break;

			case Blend_Mode::Alpha:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_alpha;
				break;

			case Blend_Mode::Additive:
				vertex_vect = &Drawing_Layers[int(drawing_layer)].trianglelist_additive;
				break;
		}

		// emplace quad vertices into vector of vertices to draw
		// texture_ID, lighting_mode, position, color, texture UV diffuse, texture UV glow
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[0].x, verts[0].y), color, XMFLOAT2(min_x, min_y), XMFLOAT2(-1.0f, -1.0f));	// 0
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[1].x, verts[1].y), color, XMFLOAT2(max_x, min_y), XMFLOAT2(-1.0f, -1.0f));	// 1
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[2].x, verts[2].y), color, XMFLOAT2(min_x, max_y), XMFLOAT2(-1.0f, -1.0f));	// 2
		vertex_vect->emplace_back(texture_id, int32_t(shader_lighting_mode), XMFLOAT2(verts[3].x, verts[3].y), color, XMFLOAT2(max_x, max_y), XMFLOAT2(-1.0f, -1.0f));	// 3
	}
}

void Linelist::Enqueue()
{
	bool draw = false;
	int last_checked_vertex_offset = -1;
	while (last_checked_vertex_offset != vertices.size() - 1)
	{
		if (camera.world_rectangle.intersects_line_segment(vertices[last_checked_vertex_offset + 1],
														   vertices[last_checked_vertex_offset + 2]) == true)
		{
			draw = true;
			break;
		}
		else
		{
			last_checked_vertex_offset += 2;
		}
	}

	if (draw)
	{
		vector<Vertex>* vertex_vect = nullptr;
		switch (blending_mode)
		{
			case Blend_Mode::Additive:
			{
				vertex_vect = &Drawing_Layers[int(drawing_layer)].linelist_additive;
			}
			break;

			case Blend_Mode::Opaque:
			{
				vertex_vect = &Drawing_Layers[int(drawing_layer)].linelist_opaque;
			}
			break;
		}

		auto white_texel = name_texture_atlas_subrect_map["white"];
		float white_texel_x = white_texel->topleft.x + white_texel->size.x / 2;
		float white_texel_y = white_texel->topleft.y + white_texel->size.y / 2;

		if (colors.size() > 1) // multiple colors, one for each vertex
		{
			// compute camera relative positions and emplace vertices to draw
			for (int i = 0; i < vertices.size(); ++i)
			{
				vertex_vect->emplace_back(Texture_Atlases[0].id, int32_t(Shader_Lighting_Mode::no_lighting), XMFLOAT2(vertices[i].x - camera.position.x, vertices[i].y - camera.position.y),
										  colors[i], XMFLOAT2(white_texel_x, white_texel_y), XMFLOAT2(-1.0f, -1.0f));
			}
		}
		else // colors.size() == 1, all vertices should have the same single color
		{
			// compute camera relative positions and emplace vertices to draw
			for (Vector2d vertex : vertices)
			{	
				vertex_vect->emplace_back(Texture_Atlases[0].id, int32_t(Shader_Lighting_Mode::no_lighting), XMFLOAT2(vertex.x - camera.position.x, vertex.y - camera.position.y),
										  colors[0], XMFLOAT2(white_texel_x, white_texel_y), XMFLOAT2(-1.0f, -1.0f));
			}
		}
	}
}

inline void Light::set_culling_threshold_distance()
{
	float intensity_r = color.getR() * intensity;
	float intensity_g = color.getG() * intensity;
	float intensity_b = color.getB() * intensity;

	float grayscale_intensity = (intensity_r + intensity_g + intensity_b) / 3.0f;
	culling_threshold_distance = pow(grayscale_intensity / minimum_light_intensity, 1.0 / falloff_exp);
}

void Light::Enqueue()
{
	const double camera_diagonal_half = Vector2d(camera.world_rectangle.size.x/2.0, camera.world_rectangle.size.y/2.0).get_length();

	double distance;
	if (type == Light_Type::tube)
	{
		distance = point_to_line_segment_distance(position1, position2, camera.position);
	}
	else // point, spotlight
	{
		distance = point_distance(position1, camera.position);
	}

	if (disabled == false and
		distance < (culling_threshold_distance + camera_diagonal_half) and
		camera.get_zoom() < 100000000.0)
	{
		if (number_of_active_lights < MAX_LIGHTS)
		{
			PS_constant_buffer.Lights[number_of_active_lights].position1 = XMFLOAT2(position1.x - camera.position.x, position1.y - camera.position.y);
			PS_constant_buffer.Lights[number_of_active_lights].position2 = XMFLOAT2(position2.x - camera.position.x, position2.y - camera.position.y);
			PS_constant_buffer.Lights[number_of_active_lights].direction = direction;
			PS_constant_buffer.Lights[number_of_active_lights].cone_exp = cone_exp;
			PS_constant_buffer.Lights[number_of_active_lights].color = XMFLOAT3(color.getR() * intensity, color.getG() * intensity, color.getB() * intensity);
			PS_constant_buffer.Lights[number_of_active_lights].falloff_exp = falloff_exp;
			PS_constant_buffer.Lights[number_of_active_lights].type = float(type);

			++number_of_active_lights;
		}
		//else
		//{
		//	print("WARNING: too many lights! Some wont be rendered. Light limit:{}" + NEWL, MAX_LIGHTS);
		//}
	}
}

void Effect::Step()
{
	sprite.translate(velocity);

	if (sprite.graphics.index() == 1) // graphics variant contains Animation
	{
		Animation& animation = get<Animation>(sprite.graphics);
		animation.advance();
	}
}

bool InitializeWindow(HINSTANCE const hInstance,
					  const int ShowWnd,
					  const int width, int height,
					  const bool windowed_arg)
{
	print(runtime("InitializeWindow(" + to_string(int(hInstance)) + " " + to_string(ShowWnd) + " " + to_string(width) + " " + to_string(height) + " " + to_string(windowed_arg) + ")" + NEWL));

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (not RegisterClassEx(&wc))
	{
		MessageBox(nullptr, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	RECT rect_str;
	rect_str.top = 50;
	rect_str.left = 50;
	rect_str.bottom = height + 50;
	rect_str.right = width + 50;

	AdjustWindowRectEx(&rect_str, WS_OVERLAPPEDWINDOW, false, NULL);

	int window_width = rect_str.right - rect_str.left;
	int window_height = rect_str.bottom - rect_str.top;

	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"Engine",
		WS_OVERLAPPEDWINDOW,
		rect_str.left, rect_str.top,
		window_width, window_height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (not hwnd)
	{
		MessageBox(nullptr, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	ZeroMemory(&msg, sizeof(MSG));

	return true;
}

bool InitDirectInput(HINSTANCE const hInstance)
{
	print(runtime(NEWL + "InitDirectInput()" + NEWL));
	hresult = DirectInput8Create(hInstance,
							DIRECTINPUT_VERSION,
							IID_IDirectInput8,
							(void**)&DirectInput,
							nullptr);

	hresult = DirectInput->CreateDevice(GUID_SysKeyboard, &keyboard.DIKeyboard, nullptr);
	hresult = keyboard.DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hresult = keyboard.DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hresult = DirectInput->CreateDevice(GUID_SysMouse, &mouse.DIMouse, nullptr);
	hresult = mouse.DIMouse->SetDataFormat(&c_dfDIMouse);
	hresult = mouse.DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

bool InitializeDirect3d11App()
{
	print(runtime(NEWL + "InitializeDirect3d11App()" + NEWL));

	// Describe our Display Mode
	DXGI_MODE_DESC display_mode_desc = {};
	display_mode_desc.Width = screen_resolution.x;
	display_mode_desc.Height = screen_resolution.y;
	display_mode_desc.RefreshRate.Numerator = 60;
	display_mode_desc.RefreshRate.Denominator = 1;
	//display_mode_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	display_mode_desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM; // HDR
	display_mode_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	display_mode_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Describe our Swap Chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = display_mode_desc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = windowed;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	#if BUILD_CONFIG_DEBUG
	UINT Flags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG;
	#else
	UINT Flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
	#endif

	// Create our Direct3D 11 Device and SwapChain	// D3D11_CREATE_DEVICE_DEBUG, D3D11_CREATE_DEVICE_BGRA_SUPPORT, D3D11_CREATE_DEVICE_SINGLETHREADED
	hresult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags,
									   nullptr, NULL, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &D3D11_Device, nullptr, &D3D11_Device_Context);

	// describe and create our backbuffer texture
	D3D11_TEXTURE2D_DESC BackBuffer_Texture2D_Desc = {};
	BackBuffer_Texture2D_Desc.Width = screen_resolution.x;
	BackBuffer_Texture2D_Desc.Height = screen_resolution.y;
	BackBuffer_Texture2D_Desc.MipLevels = 1;
	BackBuffer_Texture2D_Desc.ArraySize = 1;
	BackBuffer_Texture2D_Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	BackBuffer_Texture2D_Desc.SampleDesc.Count = 1;	//1
	BackBuffer_Texture2D_Desc.SampleDesc.Quality = 0;	//0
	BackBuffer_Texture2D_Desc.Usage = D3D11_USAGE_DEFAULT;
	BackBuffer_Texture2D_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	BackBuffer_Texture2D_Desc.CPUAccessFlags = 0;
	BackBuffer_Texture2D_Desc.MiscFlags = 0;
	D3D11_Device->CreateTexture2D(&BackBuffer_Texture2D_Desc, nullptr, &BackBuffer);

	// create backbuffer render target view
	hresult = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	hresult = D3D11_Device->CreateRenderTargetView(BackBuffer, nullptr, &BackBuffer_RTV);

	// create our blend states

	// blend add
	D3D11_RENDER_TARGET_BLEND_DESC additive_render_target_desc = {};
	additive_render_target_desc.BlendEnable = TRUE;
	additive_render_target_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	additive_render_target_desc.SrcBlend = D3D11_BLEND_ONE;
	additive_render_target_desc.DestBlend = D3D11_BLEND_ONE;
	additive_render_target_desc.BlendOp = D3D11_BLEND_OP_ADD;
	additive_render_target_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
	additive_render_target_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
	additive_render_target_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

	D3D11_BLEND_DESC AdditiveBlendDesc = {};
	//AdditiveblendDesc.AlphaToCoverageEnable = false;	// transparency artifacts if true
	AdditiveBlendDesc.RenderTarget[0] = additive_render_target_desc;
	D3D11_Device->CreateBlendState(&AdditiveBlendDesc, &Blend_Add);

	// blend alpha
	D3D11_RENDER_TARGET_BLEND_DESC alpha_render_target_desc = {};
	alpha_render_target_desc.BlendEnable = TRUE;
	alpha_render_target_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	alpha_render_target_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alpha_render_target_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alpha_render_target_desc.BlendOp = D3D11_BLEND_OP_ADD;
	alpha_render_target_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
	alpha_render_target_desc.DestBlendAlpha = D3D11_BLEND_ZERO;
	alpha_render_target_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

	D3D11_BLEND_DESC AlphaBlendDesc = {};
	//blendDesc.AlphaToCoverageEnable = false;	// transparency artifacts if true
	AlphaBlendDesc.RenderTarget[0] = alpha_render_target_desc;
	D3D11_Device->CreateBlendState(&AlphaBlendDesc, &Blend_Alpha);

	// Create and set the Viewport (subrectangle of the back buffer, we use full back buffer)
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screen_resolution.x;
	viewport.Height = screen_resolution.y;
	viewport.MinDepth = 0.0;
	viewport.MaxDepth = 1.0;
	D3D11_Device_Context->RSSetViewports(1, &viewport);

	// Describe and create the texture sampler state
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hresult = D3D11_Device->CreateSamplerState(&sampler_desc, &TextureSamplerState);
	D3D11_Device_Context->PSSetSamplers(0, 1, &TextureSamplerState);

	// describe and create our rasterizer states (counterclocwise culling, clockwise culling, no culling, wireframe)
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	rasterizer_desc.DepthClipEnable = FALSE;
	hresult = D3D11_Device->CreateRasterizerState(&rasterizer_desc, &NonecullMode);

	// Read shaders from shader file

	#ifdef BUILD_CONFIG_DEBUG
		D3DReadFileToBlob(L"Shaders\\Debug\\VertexShader_Debug.cso", &VS_Buffer);
		D3DReadFileToBlob(L"Shaders\\Debug\\PixelShader_Debug.cso", &PS_Buffer);
	#elif BUILD_CONFIG_PROFILE
		D3DReadFileToBlob(L"Shaders\\Profile\\VertexShader_Profile.cso", &VS_Buffer);
		D3DReadFileToBlob(L"Shaders\\Profile\\PixelShader_Profile.cso", &PS_Buffer);
	#elif BUILD_CONFIG_RELEASE
		D3DReadFileToBlob(L"Shaders\\Release\\VertexShader_Release.cso", &VS_Buffer);
		D3DReadFileToBlob(L"Shaders\\Release\\PixelShader_Release.cso", &PS_Buffer);
	#endif

	// Create the Shader Objects
	hresult = D3D11_Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), nullptr, &D3D11_Vertex_Shader);
	hresult = D3D11_Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), nullptr, &D3D11_Pixel_Shader);

	// Set Vertex and Pixel Shaders
	D3D11_Device_Context->VSSetShader(D3D11_Vertex_Shader, 0, 0);
	D3D11_Device_Context->PSSetShader(D3D11_Pixel_Shader, 0, 0);

	// describe and create vertex shader constant buffer
	D3D11_BUFFER_DESC vs_constant_buffer_desc = {};
	vs_constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vs_constant_buffer_desc.ByteWidth = sizeof(XMMATRIX); //we only pass XMMATRIX camera.projection_matrix to the VS Constant Buffer
	vs_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vs_constant_buffer_desc.CPUAccessFlags = 0;
	vs_constant_buffer_desc.MiscFlags = 0;
	hresult = D3D11_Device->CreateBuffer(&vs_constant_buffer_desc, nullptr, &D3D11_VS_Constant_Buffer);

	// describe and create pixel shader constant buffer
	D3D11_BUFFER_DESC ps_constant_buffer_desc = {};
	ps_constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	ps_constant_buffer_desc.ByteWidth = sizeof(PS_Constant_Buffer);
	ps_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_constant_buffer_desc.CPUAccessFlags = 0;
	ps_constant_buffer_desc.MiscFlags = 0;
	hresult = D3D11_Device->CreateBuffer(&ps_constant_buffer_desc, nullptr, &D3D11_PS_Constant_Buffer);																								// = 48 B

	// Describe, create and set the Input Layout (vertex format)
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"TEXID",		0, DXGI_FORMAT_R32_SINT,			0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0}, // 4 B
		{"LIGHTING",	0, DXGI_FORMAT_R32_SINT,			0, 4,	D3D11_INPUT_PER_VERTEX_DATA, 0}, // 4 B
		{"POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 8,	D3D11_INPUT_PER_VERTEX_DATA, 0}, // 8 B

		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0}, // 16 B

		{"UVDIFF",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0}, // 8 B
		{"UVGLOW",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 40,	D3D11_INPUT_PER_VERTEX_DATA, 0}  // 8 B
	};																								 // = 48 B		

	hresult = D3D11_Device->CreateInputLayout(layout, ARRAYSIZE(layout), VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &D3D11_Vertex_Layout);
	D3D11_Device_Context->IASetInputLayout(D3D11_Vertex_Layout);

	// describe and create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * MAX_QUAD_INSTANCES * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	hresult = D3D11_Device->CreateBuffer(&vertexBufferDesc, nullptr, &(VertexBuffer));

	// immutable index buffer for drawing quads: describe, create, initialize with data, and set 
	vector<DWORD> temp_indices;
	temp_indices.reserve(6);

	for (int i = 0; i < MAX_QUAD_INSTANCES; ++i)
	{
		temp_indices.emplace_back((i * 4));
		temp_indices.emplace_back((i * 4) + 1);
		temp_indices.emplace_back((i * 4) + 2);
		temp_indices.emplace_back((i * 4) + 2);
		temp_indices.emplace_back((i * 4) + 1);
		temp_indices.emplace_back((i * 4) + 3);
	}

	D3D11_SUBRESOURCE_DATA Index_Data = {};
	Index_Data.pSysMem = &temp_indices[0];

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * MAX_QUAD_INSTANCES * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	D3D11_Device->CreateBuffer(&indexBufferDesc, &Index_Data, &IndexBuffer);

	D3D11_Device_Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// create our depth stencil state (disabled)
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = FALSE;		//depth testing disabled
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// depth write disabled
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;	//stencil disabled
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;

	hresult = D3D11_Device->CreateDepthStencilState(&depth_stencil_desc, &pDepthdisabledStencilState);
	D3D11_Device_Context->OMSetDepthStencilState(pDepthdisabledStencilState, NULL);	// disable depth draw (dont need depth culling for 2D)

	// Set our Render Target
	D3D11_Device_Context->OMSetRenderTargets(1, &BackBuffer_RTV, NULL); //depth-stencil view is NULL
	D3D11_Device_Context->RSSetState(NonecullMode); //No Culling

	// FW1FontWrapper initialization
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(D3D11_Device, L"Arial", &pFontWrapper);

	return true;
}

string read_from_file(string path)
{
	string text;

	ifstream myfilein(path);
	if (myfilein.is_open())
	{
		print(runtime(path + " successfully opened." + NEWL));
		string line;
		while (getline(myfilein, line))
		{
			text.append(line);
			text.push_back('\n');
		}
		myfilein.close();
	}
	else { print(runtime("Unable to open " + path + "!!!" + NEWL)); }

	return text;
}

void Load_Texture_Atlas(string name, LPCWSTR texture_path, string json_path)
{
	string json = read_from_file(json_path);
	rapidjson::Document document;
	document.Parse(json.c_str());
	assert(document.IsObject());

	int texture_width = document["meta"]["size"]["w"].GetInt();
	int texture_height = document["meta"]["size"]["h"].GetInt();

	Texture_Atlases.emplace_back(name, texture_path, Vector2i{texture_width, texture_height});
	print(runtime("Loaded texture atlas: " + name + NEWL));

	// get texture subrects
	auto subrect_array = document["frames"].GetArray();
	for (auto& subrect_data : subrect_array)
	{
		string filename = subrect_data["filename"].GetString();

		Vector2i topleft_int = {subrect_data["frame"]["x"].GetInt(),
								subrect_data["frame"]["y"].GetInt()};
		Vector2i size_int = {subrect_data["frame"]["w"].GetInt(),
							 subrect_data["frame"]["h"].GetInt()};

		// convert to relative floating point coordinates from 0.0f to 1.0f
		Vector2f relative_topleft_float = {float(topleft_int.x) / float(texture_width),
										   float(topleft_int.y) / float(texture_height)};
		Vector2f relative_size_float = {float(size_int.x) / float(texture_width),
										float(size_int.y) / float(texture_height)};

		Texture_Atlas_Subrects.emplace_back(&Texture_Atlases.back(), relative_topleft_float, relative_size_float);
		name_texture_atlas_subrect_map[filename] = &Texture_Atlas_Subrects.back();
	}
}

void Load_Texture_Animated(string name, LPCWSTR texture_path,
						   Vector2i dimensions_arg, const int last_frame_index_arg, const float animation_speed_arg,
						   Vector2i frame_dimensions_arg, bool animated_arg)
{
	Animated_Textures.emplace_back(name, texture_path, dimensions_arg, last_frame_index_arg, animation_speed_arg, frame_dimensions_arg);
	print(runtime("Loaded animated texture: " + name + NEWL));

	Animations.emplace_back(&Animated_Textures.back(), animated_arg);
	name_animation_map[name] = &Animations.back();
}

void LoadResources()
{
	print(runtime("Loading textures..." + NEWL));

	Texture_Atlases.reserve(TEXTURE_SLOTS);		// Textures and Texture_Subrects vector must NOT move - reserve space for all textures in advance
	Texture_Atlas_Subrects.reserve(TEXTURE_SLOTS);

	Animated_Textures.reserve(TEXTURE_SLOTS);		// Textures and Texture_Subrects vector must NOT move - reserve space for all textures in advance
	Animations.reserve(TEXTURE_SLOTS);

	Load_Texture_Atlas("texture_atlas_1", L"Textures//texture_atlas_1.png", "Textures//texture_atlas_1.json"); // texture1 atlas

	Load_Texture_Animated("exp_1", L"Textures//exp_1.png", {4096, 3072}, 47, 1.0f, {512, 512}, false); // exp_1 (animated)

	Load_Texture_Animated("missile_trail_1", L"Textures//missile_trail_1.png", {1792, 1792}, 48, 1.0f, {256, 256}, false);

	Load_Texture_Animated("warpmap", L"Textures//warpmap.png", {8192, 4096}, 29, 1.0f, {1024, 1024}, true);

	Load_Texture_Animated("muzzle_flash", L"Textures//muzzle_flash.png", {1280, 256}, 4, 1.0f, {256, 256}, false);

	Load_Texture_Animated("redbeamglow", L"Textures//redbeamglow.png", {4096, 1536}, 23, 1.0f, {512, 512}, true);

	//set texture resources in pixel shader

	vector<ID3D11ShaderResourceView*> Texture_resources;
	Texture_resources.reserve(Texture_Atlases.size() + Animated_Textures.size());

	for (Texture_Atlas& texture_atlas : Texture_Atlases)
	{
		Texture_resources.emplace_back(texture_atlas.resource_view);
	}

	for (Texture_Animated& texture_animated : Animated_Textures)
	{
		Texture_resources.emplace_back(texture_animated.resource_view);
	}

	D3D11_Device_Context->PSSetShaderResources(0, Texture_resources.size(), &Texture_resources[0]);

	print(runtime(NEWL));
}

void CleanUp()	// release GPU resources
{
	SwapChain->SetFullscreenState(false, nullptr);
	PostMessage(hwnd, WM_DESTROY, 0, 0);

	//Release the COM Objects we created
	keyboard.DIKeyboard->Unacquire();
	mouse.DIMouse->Unacquire();
	BackBuffer_RTV->Release();
	D3D11_VS_Constant_Buffer->Release();
	D3D11_PS_Constant_Buffer->Release();
	D3D11_Vertex_Shader->Release();
	D3D11_Pixel_Shader->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	D3D11_Vertex_Layout->Release();
	Blend_Add->Release();
	Blend_Alpha->Release();
	//Blend_Overwrite->Release();
	NonecullMode->Release();
	BackBuffer->Release();
	TextureSamplerState->Release();
	DirectInput->Release();
	SwapChain->Release();
	D3D11_Device->Release();
	D3D11_Device_Context->Release();
	pFontWrapper->Release();
	pFW1Factory->Release();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Parse_data()
{
	print(runtime("Parsing settings.." + NEWL));

	string json = read_from_file("settings.json"); //read settings json

	//parse JSON and set engine parameters according to the settings.ini
	rapidjson::Document document;
	document.Parse(json.c_str());
	assert(document.IsObject());

	screen_resolution.x = document["screen_resolution.x"].GetInt();
	screen_resolution.y = document["screen_resolution.y"].GetInt();
	windowed = document["windowed"].GetBool();
	camera_scrolling_speed = document["camera_scrolling_speed"].GetDouble();
	fps_cap = document["fps_cap"].GetDouble();
	camera_smoothing_factor = document["camera_smoothing_factor"].GetDouble();
	num_of_systems = document["num_of_systems"].GetInt();

	print(runtime("Settings successfully parsed." + NEWL));

	target_frame_time = nanoseconds{int64_t(1000000000.0 / fps_cap)};

	//Stars
	print(runtime("Parsing stars..." + NEWL));

	string json2 = read_from_file("stars.json"); //read stars json

	//parse JSON string and fill magnitudes and constellations vectors
	rapidjson::Document document2;
	document2.Parse(json2.c_str());
	assert(document2.IsObject());
	assert(document2["magnitudes"].IsArray());
	assert(document2["constellations"].IsArray());

	magnitudes.reserve(document2["magnitudes"].Size());
	for (rapidjson::SizeType i = 0; i < document2["magnitudes"].Size(); ++i)
	{
		magnitudes.emplace_back(document2["magnitudes"][i].GetString());
	}

	constellations.reserve(document2["constellations"].Size());
	for (rapidjson::SizeType i = 0; i < document2["constellations"].Size(); ++i)
	{
		constellations.emplace_back(document2["constellations"][i].GetString());
	}

	print(runtime("Stars successfully parsed." + NEWL));
}

void draw_deferred_strings()
{
	for (Deferred_String& defstr : strings_to_draw)
	{
		pFontWrapper->DrawString(D3D11_Device_Context,
								 defstr.str.c_str(),
								 defstr.font_size, defstr.screen_position.x, defstr.screen_position.y, defstr.color, FW1_RESTORESTATE);
	}

	strings_to_draw.clear();
}

void Render()
{
	// Clearing the render target is commented out because it is unnecessary, since we overwrite it entirely during rendering
	//const array<float, 4> background_color = {0.0, 1.0, 0.0, 1.0};
	//D3D11_Device_Context->ClearRenderTargetView(BackBuffer_RTV, &background_color[0]);

	// Update and Set VS and PS Constant Buffers

	// view-projection matrix identical to the projection matrix, since view matrix is fixed to identity matrix in a 2D game,
	// so we pass the projection_matrix directly to VS Constant buffer
	D3D11_Device_Context->UpdateSubresource(D3D11_VS_Constant_Buffer, 0, nullptr, &camera.projection_matrix, 0, 0);
	D3D11_Device_Context->VSSetConstantBuffers(0, 1, &D3D11_VS_Constant_Buffer);

	D3D11_Device_Context->UpdateSubresource(D3D11_PS_Constant_Buffer, 0, nullptr, &PS_constant_buffer, 0, 0);
	D3D11_Device_Context->PSSetConstantBuffers(0, 1, &D3D11_PS_Constant_Buffer);

	number_of_active_lights = 0;

	for (Drawing_Layer& layer : Drawing_Layers)
	{
		layer.Draw();
	}

	draw_deferred_strings();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (fps_cap > 0.0) //if there is an fps cap
	{
		nanoseconds frame_time = Stopwatch_FPS_limit.get_elapsed_time();
		nanoseconds remaining_time = target_frame_time - frame_time;

		if (remaining_time > nanoseconds(1000000)) // 1 ms
		{
			std::this_thread::sleep_for(remaining_time*0.9);
		}

		do
		{
			frame_time = Stopwatch_FPS_limit.get_elapsed_time();
		}
		while
			(frame_time < target_frame_time);
	}

	past_frametimes.push_back(Stopwatch_FPS_limit.restart());

	if (Second.get_elapsed_time().count() > NS_IN_SEC / 2)
	{
		Second.restart();
		nanoseconds total_frame_time = std::accumulate(past_frametimes.begin(), past_frametimes.end(), nanoseconds(0));
		average_frame_time = total_frame_time / past_frametimes.size();
		average_fps = NS_IN_SEC / average_frame_time.count();
	}

	UINT Sync_interval = 0; // vsync off

	if (fps_cap < 0) // vsync on
	{
		Sync_interval = 1;
	}

	SwapChain->Present(Sync_interval, 0);	// present call, CPU waits for GPU here?
}

// process window events
LRESULT CALLBACK WndProc(const HWND hwnd_arg,
						 const UINT msg_arg,
						 const WPARAM wParam,
						 const LPARAM lParam)
{
	switch (msg_arg)
	{
		case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd_arg);
			running = false;
		}
		if (wParam == VK_SPACE)
		{
			paused = false;
		}
		return 0;
	}
	return DefWindowProc(hwnd_arg,
						 msg_arg,
						 wParam,
						 lParam);
}

void Camera::Step()
{
	const Vector2d location_difference = position_leading - position; //real camera position following leading camera position
	if (location_difference != Vector2d(0, 0))
	{
		position += location_difference / camera_smoothing_factor;
	}

	const double zoom_difference = zoom_leading - get_zoom(); //real camera zoom following leading camera zoom
	if (abs(zoom_difference) > 0.001)
	{
		set_zoom(get_zoom() + zoom_difference / camera_smoothing_factor);
	}
	else
	{
		set_zoom(zoom_leading);
		zoom_changed = false;
	}
}

void Mouse::Refresh_Key_State()
{
	// get up-to-date mouse input
	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &current_state);

	auto& imgui_io = ImGui::GetIO();

	for (Key* key_ptr : Key_ptrs)
	{
		bool currently_pressed_down = current_state.rgbButtons[key_ptr->code] & 0x80; // check current key status

		if (currently_pressed_down and key_ptr->pressed_down == false) // check for key pressed event
		{
			key_ptr->key_event = Key_Event::Pressed;
			key_ptr->hold_time = nanoseconds{0};
		}
		else if (not(currently_pressed_down) and key_ptr->pressed_down == true) // check for key released event
		{
			key_ptr->key_event = Key_Event::Released;
		}
		else // No event
		{
			key_ptr->key_event = Key_Event::None;
		}

		if (currently_pressed_down)
		{
			key_ptr->hold_time += average_frame_time;
			imgui_io.MouseDown[key_ptr->code] = true;
		}
		else // NOT pressed down
		{
			imgui_io.MouseDown[key_ptr->code] = false;
		}

		key_ptr->pressed_down = currently_pressed_down; // set current status
	}

	wheel_turns = -(float(current_state.lZ) / 120.0f); //lZ returns +-120 per one mouse wheel notch at default Windows setting. lZ returns negative if wheel turned towards the player, positive away from the player.
}

void Mouse::Refresh_Movement()
{
	// mouse movement on screen
	screen_position_in_pixels.x += current_state.lX * sensitivity;
	screen_position_in_pixels.y -= current_state.lY * sensitivity;

	// clamp mouse position to window area
	screen_position_in_pixels.x = max(float(screen_resolution.x) * -0.5f, min(screen_position_in_pixels.x, float(screen_resolution.x) * 0.5f));
	screen_position_in_pixels.y = max(float(screen_resolution.y) * -0.5f, min(screen_position_in_pixels.y, float(screen_resolution.y) * 0.5f));

	// mouse movement in world
	world_position = Vector2d(screen_position_in_pixels) * camera.get_zoom() + camera.position;
	screen_position_normalized = WorldToScreenCoords(world_position);

	// Get the ImGuiIO structure
	auto& io = ImGui::GetIO();

	// Calculate the mouse position in ImGui coordinates
	screen_position_for_imgui.x = screen_position_in_pixels.x + float(screen_resolution.x) / 2.0f;
	screen_position_for_imgui.y = -screen_position_in_pixels.y + float(screen_resolution.y) / 2.0f;

	// Use the new ImGui API for reporting mouse position
	io.AddMousePosEvent(screen_position_for_imgui.x, screen_position_for_imgui.y);
}

void Keyboard::Refresh_Key_State()
{
	// get up-to-date keyboard input
	keyboard.DIKeyboard->Acquire();
	keyboard.DIKeyboard->GetDeviceState(sizeof(keyboard.current_state), (LPVOID)&keyboard.current_state);

	for (Key* key_ptr : Key_ptrs)
	{
		bool currently_pressed_down = current_state[key_ptr->code] & 0x80; // check current key status

		if (currently_pressed_down and key_ptr->pressed_down == false) // check for key pressed event
		{
			key_ptr->key_event = Key_Event::Pressed;
			key_ptr->hold_time = nanoseconds{0};
		}
		else if (not(currently_pressed_down) and key_ptr->pressed_down == true) // check for key released event
		{
			key_ptr->key_event = Key_Event::Released;
		}
		else // No event
		{
			key_ptr->key_event = Key_Event::None;
		}

		if (currently_pressed_down)
		{
			key_ptr->hold_time += average_frame_time;
		}

		key_ptr->pressed_down = currently_pressed_down; // set current status
	}
}

DoubleRect Sprite::get_sprite_bounding_box(bool world_coords)
{
	const Matrix3x3d& matrix = getTransformation();

	if (world_coords == false)
	{
		return get_bounding_box(get_transformed_cam_relative_verts(matrix, camera.position)); //camera-relative sprite bounding box
	}
	else
	{
		return get_bounding_box(get_transformed_world_verts(matrix)); //sprite bounding box in world coords
	}
}