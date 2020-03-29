//--------------------------------------------------------------------------------------
//
//                     "DirectX 11 ile Bilgisayara Karþý Tank Oyunu"
//                             Last Update : 07.06.2017
//
//--------------------------------------------------------------------------------------

#include <windows.h>
#include <cmath>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"
#include <DXGI.h>
#include <dinput.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include "ShortestPath.h"
#define V 34
#define INF 999
using namespace std;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mworldInvTranspose;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 vEyePos;
	XMFLOAT4 vLightPos[2];
	XMFLOAT4 vMeshColor;
	XMFLOAT4 vLightColor[2];
}cBuffer;

struct intersection
{
	float     t = 0.0F;
	bool isWall = false;
}intersected;

//--------------------------------------------------------------------------------------
// Matrix Assignment
//--------------------------------------------------------------------------------------
int adjMatrixt[V][V] =
{
	//   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17  18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33
	{   0,  60, INF, INF, INF, INF, INF, INF,  40, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //0
	{  60,   0, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //1
	{ INF, INF,   0,  70, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //2
	{ INF, INF,  70,   0, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //3
	{ INF,  30, INF, INF,   0,  50, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //4
	{ INF, INF,  30, INF,  50,   0,  40, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //5
	{ INF, INF, INF, INF, INF,  40,   0,  30, INF, INF, INF, INF,  50, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //6
	{ INF, INF, INF,  30, INF, INF,  30,   0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  90, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //7
	{  40, INF, INF, INF, INF, INF, INF, INF,   0,  30, INF, INF, INF,  50, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //8
	{ INF, INF, INF, INF, INF, INF, INF, INF,  30,   0, INF, INF, INF, INF,  50, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //9
	{ INF, INF, INF, INF,  30, INF, INF, INF, INF, INF,   0,  30, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //10
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30,   0,  60, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //11
	{ INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,  60,   0, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //12
	{ INF, INF, INF, INF, INF, INF, INF, INF,  50, INF, INF, INF, INF,   0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  60, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //13
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF,  50, INF, INF, INF, INF,   0,  30, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //14
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF,  30,   0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //15
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,   0,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //16
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30,   0,  30, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //17
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,  30,   0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //18
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,   0,  20, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //19
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  20,   0,  50, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF, INF, INF }, //20
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  50,   0,  30, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF, INF }, //21
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30,   0,  50, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF }, //22
	{ INF, INF, INF, INF, INF, INF, INF,  90, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  50,   0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF }, //23
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  60, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,   0,  50, INF, INF, INF, INF,  30, INF, INF, INF }, //24
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF,  50,   0, INF, INF, INF, INF, INF,  30, INF, INF }, //25
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,   0,  20, INF, INF, INF, INF,  30, INF }, //26
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,  20,   0, INF, INF, INF, INF, INF, INF }, //27
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF,   0,  50, INF, INF,  30, INF }, //28
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  50,   0, INF, INF, INF,  30 }, //29
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF, INF,   0,  50, INF, INF }, //30
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, INF, INF,  50,   0, INF, INF }, //31
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 30, INF, INF, INF, INF, INF,   0,  100 }, //32
	{ INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF,  30, INF, INF, 100,   0 }  //33
};

XMFLOAT3 coord[V] = {
	XMFLOAT3( 95, 0, -95),		//0
	XMFLOAT3( 95, 0, -35 ),		//1
	XMFLOAT3( 95, 0, 25 ),		//2
	XMFLOAT3( 95, 0, 95 ),		//3	
	XMFLOAT3( 65, 0, -35 ),		//4
	XMFLOAT3( 65, 0, 25 ),		//5
	XMFLOAT3( 65, 0, 65 ),		//6
	XMFLOAT3( 65, 0, 95 ),		//7
	XMFLOAT3( 55, 0, -95 ),		//8
	XMFLOAT3( 55, 0, -65 ),		//9
	XMFLOAT3( 35, 0, -35 ),		//10
	XMFLOAT3( 35, 0, -5 ),		//11
	XMFLOAT3( 35, 0, 55 ),		//12
	XMFLOAT3( 5, 0, -95 ),		//13
	XMFLOAT3( 5, 0, -65 ),		//14
	XMFLOAT3( 5, 0, -35 ),		//15
	XMFLOAT3( 5, 0, -5 ),		//16
	XMFLOAT3( 5, 0, 25 ),		//17
	XMFLOAT3( 5, 0, 55 ),		//18
	XMFLOAT3( -25, 0, -65 ),		//19
	XMFLOAT3( -25, 0, -45 ),		//20
	XMFLOAT3( -25, 0, 5 ),		//21
	XMFLOAT3( -25, 0, 35 ),		//22
	XMFLOAT3( -25, 0, 85 ),		//23
	XMFLOAT3( -55, 0, -95 ),		//24
	XMFLOAT3( -55, 0, -45 ),		//25
	XMFLOAT3( -55, 0, -15 ),		//26
	XMFLOAT3( -55, 0, 5 ),		//27
	XMFLOAT3( -55, 0, 35 ),		//28
	XMFLOAT3(-55, 0, 85 ),		//29
	XMFLOAT3(-85, 0, -95 ),		//30
	XMFLOAT3(-85, 0, -45 ),		//31
	XMFLOAT3(-85, 0, -15 ),		//32
	XMFLOAT3(-85, 0, 85 ),		//33
};

//--------------------------------------------------------------------------------------
// Definition Dijstra Functions
//--------------------------------------------------------------------------------------
void Dijkstra::read(int *adjMatrixt){

	for (int i = 0; i<V; i++){
		for (int j = 0; j<V; j++){
			adjMatrix[i][j] = *adjMatrixt;
			*adjMatrixt++;
		}
	}
}

void Dijkstra::sourceDesDef(int src, int des){
	source = src;
	destination = des;
}

void Dijkstra::initialize(){

	for (int i = 0; i<V; i++) {

		mark[i] = false;

		predecessor[i] = -1;

		distance[i] = INF;
	}

	distance[source] = 0;

}

int Dijkstra::getClosestUnmarkedNode(){

	int minDistance = INF;

	int closestUnmarkedNode;

	for (int i = 0; i<V; i++) {

		if ((!mark[i]) && (minDistance >= distance[i])) {

			minDistance = distance[i];

			closestUnmarkedNode = i;

		}

	}

	return closestUnmarkedNode;

}

void Dijkstra::calculateDistance()
{

	initialize();

	int minDistance = INF;

	int closestUnmarkedNode;

	int count = 0;

	while (count < V) {

		closestUnmarkedNode = getClosestUnmarkedNode();

		mark[closestUnmarkedNode] = true;

		for (int i = 0; i<V; i++) {

			if ((!mark[i]) && (adjMatrix[closestUnmarkedNode][i]>0)) {

				if (distance[i] > distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i]) {

					distance[i] = distance[closestUnmarkedNode] + adjMatrix[closestUnmarkedNode][i];

					predecessor[i] = closestUnmarkedNode;

				}

			}

		}

		count++;

	}
	printPath(destination);
}

void Dijkstra::printPath(int des){
	
	if (predecessor[des] != -1, des != source) {

		if (des != destination) path.push(des);

		printPath(predecessor[des]);

	}

	//while (path.size() != 0)
	//{
	//	cout << path.front() << " ";
	//	path.pop();
	//}
}

vector<intersection> intersections;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = NULL;
ID3D11DeviceContext*                g_pImmediateContext = NULL;
IDXGISwapChain*                     g_pSwapChain = NULL;
ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11PixelShader*                  g_pPixelShader_Textured = NULL;
ID3D11PixelShader*                  g_pPixelShader_Solid = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;
ID3D11Buffer*                       g_pConstantBuffer = NULL;
ID3D11ShaderResourceView*           g_pTextureGround = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;

ID3D11RasterizerState*				g_pRasterizerStateSolid = NULL;

XMMATRIX                            g_World_Tank;
XMMATRIX                            g_World_Enemy;
XMMATRIX                            g_World_Ball;
XMMATRIX                            g_World_Ball_Enemy;
XMMATRIX                            g_World_RedDot;

ID3D11ShaderResourceView*           g_pTextureTank_Body = NULL;
ID3D11ShaderResourceView*           g_pTextureTank_Palette = NULL;
ID3D11ShaderResourceView*           g_pTextureTank_Turret = NULL;
ID3D11ShaderResourceView*           g_pTexture_Wall = NULL;

ID3D11Buffer*                       g_pVertexBuffer_Tank_Body = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Tank_Palette = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Tank_Turret = NULL;

ID3D11Buffer*                       g_pVertexBuffer_Walls = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Ball = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Ground = NULL;
ID3D11Buffer*                       g_pVertexBuffer_RedDot = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Light = NULL;

int vertexCountTank_Body;
int vertexCountTank_Palette;
int vertexCountTank_Turret;
int vertexCountWalls;
int vertexCountBall;
int vertexCountGround;
int vertexCountRedDot;
int vertexCountLight;

SimpleVertex* verticesTank_Body;
SimpleVertex* verticesTank_Palette;
SimpleVertex* verticesTank_Turret;
SimpleVertex* verticesWalls;
SimpleVertex* verticesBall;
SimpleVertex* verticesGround;
SimpleVertex* verticesRedDot;
SimpleVertex* verticesLight;

// Enemy_Tank
ID3D11Buffer*                       g_pVertexBuffer_Enemy_Tank_Body = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Enemy_Tank_Palette = NULL;
ID3D11Buffer*                       g_pVertexBuffer_Enemy_Tank_Turret = NULL;

XMVECTOR Ro;
XMVECTOR Rd;

XMFLOAT3 Tanker_Translation = XMFLOAT3( 95, 0, -95);		// Bizim kontrol ettiðimiz tankýn baþlangýç konumu
XMMATRIX Tanker_Trans;
XMVECTOR Translation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

XMFLOAT3 EnemyTanker_Translation = XMFLOAT3(-85, 0, 85);		// Düþman tanký baþlangýç konumu
XMMATRIX EnemyTanker_Trans;


float reddotDistance = 0.0;
float t_Distance = FLT_MAX;
bool renderTank = true;
bool renderTankBall = false;
bool renderEnemy = true;
bool renderEnemyBall = true;
bool PlayFireOnce = true;
bool PlayHitOnce = true;
bool PlayEnemyFireOnce = false;
bool FireEnemyBall = false;

//-------------------------------------------------------------------------------------
// DIRECT INPUT
//-------------------------------------------------------------------------------------
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rot = 0.01f;
double countsPerSecond = 0.0;
__int64 CounterStart = 0;
int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;

bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);
void UpdateCamera();
void StartTimer();
double GetTime();
double GetFrameTime();

//-------------------------------------------------------------------------------------
// FIRST PERSON CAMERA
//-------------------------------------------------------------------------------------

XMVECTOR Eye;
XMVECTOR At;
XMVECTOR Up;

XMVECTOR DefaultDown		= XMVectorSet(0.0f, 0.0f,-1.0f, 0.0f);
XMVECTOR DefaultLeft		= XMVectorSet(-1.0f,0.0f, 0.0f, 0.0f);
XMVECTOR DefaultForward		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight		= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward			= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight			= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camLeft			= XMVectorSet(-1.0f,0.0f, 0.0f, 0.0f);

XMVECTOR RedDotPosition		= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR ballDirection		= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR BarrelHeight		= XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR EnemyBarrelDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

float ShootingAngle = 0.0;
float BarrelRotationAngel = 0.0;
float BarrelRotationAngelDerece = 0.0;

XMMATRIX camRotationMatrix;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render(double time);

float IntersectTriangle(XMVECTOR Ro, XMVECTOR Rd, SimpleVertex* verticesModel, int vertexCount, XMMATRIX& g_World);
SimpleVertex* Obj_Loader(char* filename, int* verticesCount);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	if (FAILED(InitWindow(hInstance, nCmdShow)))		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

	if (!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			frameCount++;
			if (GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}

			frameTime = GetFrameTime();

			DetectInput(frameTime);

			Render(frameTime);
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1920, 1080 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"DirectX 11 ile Bilgisayara Karþý Tank Oyunu", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd) return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL) OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();
	return S_OK;
}

bool InitDirectInput(HINSTANCE hInstance)
{
	HRESULT HR = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	HR = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);
	HR = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	HR = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	HR = DIKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	HR = DIMouse->SetDataFormat(&c_dfDIMouse);
	HR = DIMouse->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	return true;
}

bool canShoot(vector<intersection> intersections)
{
	float min_distance = FLT_MAX;
	int   min_indis = 0;

	for (int i = 0; i < intersections.size(); i++)
	if (intersections.at(i).t < min_distance)
	{
		min_indis = i;
		min_distance = intersections.at(i).t;
	}

	if (intersections.at(min_indis).isWall)
		return false;
	else
		return true;
}

float MinDistance(vector<intersection> intersections)
{
	float min_distance = FLT_MAX;
	int   min_indis = 0;
	
	for (int i = 0; i < intersections.size(); i++)
	if (intersections.at(i).t < min_distance)
	{
		min_indis = i;
		min_distance = intersections.at(i).t;
	}

	return min_distance;
}

void DetectInput(double time)
{
	
	DIMOUSESTATE mouseCurrState;
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	if (keyboardState[DIK_ESCAPE] & 0x80) PostMessage(g_hWnd, WM_DESTROY, 0, 0);

	float speed = 10.0F * time;

	//if (keyboardState[DIK_P] & 0x80)
	//{
	//	Translation = speed * DefaultLeft;
	//	XMFLOAT3 TranslationF3;
	//	XMStoreFloat3(&TranslationF3, Translation);
	//	g_World_Enemy._41 += TranslationF3.x;
	//	g_World_Enemy._42 += TranslationF3.y;
	//	g_World_Enemy._43 += TranslationF3.z;
	//}

	if (keyboardState[DIK_A] & 0x80)
	{
		Translation = speed * camLeft;
		XMFLOAT3 TranslationF3;
		XMStoreFloat3(&TranslationF3, Translation);
		Tanker_Translation.x += TranslationF3.x;
		Tanker_Translation.y += TranslationF3.y;
		Tanker_Translation.z += TranslationF3.z;
	}

	if (keyboardState[DIK_D] & 0x80)
	{
		Translation = speed * camRight;
		XMFLOAT3 TranslationF3;
		XMStoreFloat3(&TranslationF3, Translation);
		Tanker_Translation.x += TranslationF3.x;
		Tanker_Translation.y += TranslationF3.y;
		Tanker_Translation.z += TranslationF3.z;
	}

	if (keyboardState[DIK_W] & 0x80)
	{
		Ro = BarrelHeight;
		Rd = XMVector3Normalize(At - Eye);

		float t = FLT_MAX;
		bool move = true;
		t = IntersectTriangle(Ro, Rd, verticesWalls, vertexCountWalls, XMMatrixIdentity());

		if ((t<11.0F) && (t>0.0F)) move = false;
		if (move)
		{
			Translation = speed * camForward;
			XMFLOAT3 TranslationF3;
			XMStoreFloat3(&TranslationF3, Translation);
			Tanker_Translation.x += TranslationF3.x;
			Tanker_Translation.y += TranslationF3.y;
			Tanker_Translation.z += TranslationF3.z;
		}
	}

	if (keyboardState[DIK_S] & 0x80)
	{
		Ro = BarrelHeight;
		Rd = XMVector3Normalize(Eye - At);

		float t = FLT_MAX;
		bool move = true;
		t = IntersectTriangle(Ro, Rd, verticesWalls, vertexCountWalls, XMMatrixIdentity());

		if ((t<3.0F) && (t>0.0F)) move = false;
		if (move)
		{
			Translation = -speed * camForward;
			XMFLOAT3 TranslationF3;
			XMStoreFloat3(&TranslationF3, Translation);
			Tanker_Translation.x += TranslationF3.x;
			Tanker_Translation.y += TranslationF3.y;
			Tanker_Translation.z += TranslationF3.z;
		}
	}

	if (!(keyboardState[DIK_SPACE] & 0x80))					// SPACE is UP
	{
		PlayFireOnce = true;
	}

	if (keyboardState[DIK_SPACE] & 0x80)					// SPACE is PRESSED
	{
		if (PlayFireOnce) // Mouse sol buton basýlý iken PlaySound()'un 1 kez koþmasý için
		PlaySound(TEXT("fire.wav"), NULL, SND_FILENAME | SND_ASYNC);
		PlayFireOnce = false;

		renderTankBall = true;

		ballDirection = XMVector3Normalize(At - Eye);

		// Merminin Baþlangýç noktasýný setle
		XMVECTOR Namlu_Ucu	= 6 * XMVector3Normalize(At - Eye);
		XMFLOAT4 Namlu_Ucu_F4; XMStoreFloat4(&Namlu_Ucu_F4, Namlu_Ucu);
		g_World_Ball		= g_World_Tank;
		g_World_Ball._41	= g_World_Ball._41 + Namlu_Ucu_F4.x;
		g_World_Ball._42	= g_World_Ball._42 + 1.5 + Namlu_Ucu_F4.y;
		g_World_Ball._43	= g_World_Ball._43 + Namlu_Ucu_F4.z;
	}

	if (!(mouseCurrState.rgbButtons[0] & 0x80))				// Left Button is UP
	{
	
	}

	if (mouseCurrState.rgbButtons[0] & 0x80)				// Left Button is PRESSED
	{
		FireEnemyBall = false;
		ShootingAngle = -1;
		renderEnemy = true;

		renderTank = true;
		PlayHitOnce = true;
	}

	if (!(mouseCurrState.rgbButtons[1] & 0x80))				// Right Button is UP
	{
		g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1920 / (FLOAT)1080, 0.01f, 1000.0f);
		cBuffer.mProjection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
	}

	if (mouseCurrState.rgbButtons[1] & 0x80)				// Right Button is PRESSED (for Zoom)
	{
		g_Projection = XMMatrixPerspectiveFovLH(XM_PI/12, 1920 / (FLOAT)1080, 0.01f, 1000.0f);
		cBuffer.mProjection = XMMatrixTranspose(g_Projection);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
	}

	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseLastState.lX * 0.001f;
		camPitch += mouseCurrState.lY * 0.001f;
		mouseLastState = mouseCurrState;
	}

	UpdateCamera();

	return;
}

void UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	At = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	At = XMVector3Normalize(At);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camLeft = XMVector3TransformCoord(DefaultLeft,RotateYTempMatrix);
	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	Up = XMVector3TransformCoord(Up, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	Eye += moveLeftRight * camRight;
	Eye += moveBackForward * camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	At = Eye + At;

	g_View = XMMatrixLookAtLH(Eye, At, Up);

	BarrelHeight = Eye + XMVectorSet(0.0f, -1.5f, 0.0f, 0.0f);

	// Bu kýsým burda mý olmalý ? Spacete mi?
	// Enemy öldüðünde mermisi de ölüyor ölmemeli

	//
	// INTERSECTION TEST FOR TANK -> ENEMY SHOOT
	//
	Ro = BarrelHeight;
	Rd = XMVector3Normalize(At - Eye);
	
	t_Distance = IntersectTriangle(Ro, Rd, verticesWalls, vertexCountWalls, XMMatrixIdentity());
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = true;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Body, vertexCountTank_Body, g_World_Enemy);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Palette, vertexCountTank_Palette, g_World_Enemy);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Turret, vertexCountTank_Turret, g_World_Enemy);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesGround, vertexCountGround, g_World);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	if (intersections.size() > 0)
	{
		reddotDistance = MinDistance(intersections);
		RedDotPosition = Ro + reddotDistance * XMVector3Normalize(At - Eye);

		XMVECTOR iPoint = Ro + reddotDistance * Rd;
		XMVECTOR currentBallPosition;
		XMFLOAT4 cBallPos_F4;
		cBallPos_F4.x = g_World_Ball._41;
		cBallPos_F4.y = g_World_Ball._42;
		cBallPos_F4.z = g_World_Ball._43;
		currentBallPosition = XMLoadFloat4(&cBallPos_F4);

		if (renderTankBall && canShoot(intersections) && (XMVectorGetX(XMVector3Length(iPoint - currentBallPosition)) < 0.5))
		{
			renderEnemy = false;
			renderTankBall = false;											

			PlaySound(TEXT("hit.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}

		if (renderTankBall && !canShoot(intersections) && (XMVectorGetX(XMVector3Length(iPoint - currentBallPosition)) < 0.5))
		{
			renderTankBall = false;
		}
	}
	intersections.clear();
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	//
	// INTERSECTION TEST FOR ENEMY -> TANK SHOOT
	//
	Ro = XMVectorSet(g_World_Ball_Enemy._41, g_World_Ball_Enemy._42, g_World_Ball_Enemy._43, 0);
	Rd = EnemyBarrelDirection;

	t_Distance = IntersectTriangle(Ro, Rd, verticesWalls, vertexCountWalls, XMMatrixIdentity());
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = true;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Body, vertexCountTank_Body, g_World_Tank);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Palette, vertexCountTank_Palette, g_World_Tank);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesTank_Turret, vertexCountTank_Turret, g_World_Tank);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	t_Distance = IntersectTriangle(Ro, Rd, verticesGround, vertexCountGround, g_World);
	if (t_Distance > 0)
	{
		intersected.t = t_Distance;
		intersected.isWall = false;
		intersections.push_back(intersected);
	}
	if (intersections.size() > 0)
	{
		XMVECTOR iPoint = Ro + MinDistance(intersections) * Rd;
		XMVECTOR currentBallPosition;
		XMFLOAT4 cBallPos_F4;
		cBallPos_F4.x = g_World_Ball_Enemy._41;
		cBallPos_F4.y = g_World_Ball_Enemy._42;
		cBallPos_F4.z = g_World_Ball_Enemy._43;
		currentBallPosition = XMLoadFloat4(&cBallPos_F4);

		if (canShoot(intersections) && (XMVectorGetX(XMVector3Length(iPoint - currentBallPosition)) < 0.5))
		{
			renderTank = false;

			if (PlayHitOnce)
				PlaySound(TEXT("hit.wav"), NULL, SND_FILENAME | SND_ASYNC);
			PlayHitOnce = false;
		}

		if (!canShoot(intersections) && (XMVectorGetX(XMVector3Length(iPoint - currentBallPosition)) < 0.5))
		{
			renderEnemyBall = true;								//Kullandýðým tankýn mermisi vurunca yok edildi
		}
	}
	intersections.clear();
	//////////////////////////////////////////////////////////////////////////////////////////////////////

}

void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = double(frequencyCount.QuadPart);
	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}

void pathCalculate()                                         //Shortest Path Calculate and Push Stack
{
	for (int i = 0; i < V; i++)
	{
		if (fabs(coord[i].x - EnemyTanker_Translation.x) < 4.0F && fabs(coord[i].z - EnemyTanker_Translation.z) < 4.0F)
		{
			for (int j = 0; j < V; j++)
			{
				if (fabs(coord[j].x - Tanker_Translation.x) < 4.0F && fabs(coord[j].z - Tanker_Translation.z) < 4.0F)
				{
					G.sourceDesDef(j, i);
					while (!G.path.empty())
					{
						G.path.pop();
					}
					G.calculateDistance();
				}
			}
		}
	}
}

float Angle()
{
	float angleTemp = 0;
	if (fabs(coord[G.source].x - g_World_Enemy._41) < 4.0F){
		if (g_World_Enemy._43 - coord[G.source].z < 0.0F)
		{
			angleTemp = 0;
		}
		else if (g_World_Enemy._43 - coord[G.source].z > 0.0F)
		{
			angleTemp = XM_PI;
		}
	}
	else if (fabs(coord[G.source].z - g_World_Enemy._43) < 4.0F){
		if (g_World_Enemy._41 - coord[G.source].x < 0.0F)
		{
			angleTemp = XM_PI / 2;
		}
		else if (g_World_Enemy._41 - coord[G.source].x > 0.0F)
		{
			angleTemp = -(XM_PI / 2);
		}
	}
	return angleTemp;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;
	
	G.read(&adjMatrixt[0][0]);					//Komþuluk Matrisi Atamasý(Fonksiyon Bir Kez Çaðrýldýðý Düþünülerek Buraya Yazýldý)

	//pathCalculate();

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"DX11_Tank_Oyunu.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"DX11_Tank_Oyunu.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))	return hr;

	// Compile the pixel shader Textured
	ID3DBlob* pPStextureBlob = NULL;
	hr = CompileShaderFromFile(L"DX11_Tank_Oyunu.fx", "PS_Textured", "ps_4_0", &pPStextureBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPStextureBlob->GetBufferPointer(), pPStextureBlob->GetBufferSize(), NULL, &g_pPixelShader_Textured);
	pPStextureBlob->Release();
	if (FAILED(hr))	return hr;


	// Compile the pixel shader Solid
	ID3DBlob* pPSsolidBlob = NULL;
	hr = CompileShaderFromFile(L"DX11_Tank_Oyunu.fx", "PS_Solid", "ps_4_0", &pPSsolidBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSsolidBlob->GetBufferPointer(), pPSsolidBlob->GetBufferSize(), NULL, &g_pPixelShader_Solid);
	pPSsolidBlob->Release();
	if (FAILED(hr))	return hr;

	// Read Light.obj and fill g_pVertexBuffer_Light vertex buffer
	verticesLight = Obj_Loader("Media/Light.obj", &vertexCountLight);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountLight;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesLight;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Light);
	if (FAILED(hr))     return hr;

	// Read Tank_T90_Body.obj and fill g_pVertexBuffer_Tank_Body vertex buffer
	verticesTank_Body = Obj_Loader("Media/Tank_T90_Body.obj", &vertexCountTank_Body);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Body;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Body;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Tank_Body);
	if (FAILED(hr))     return hr;

	// Read Tank_T90_Palette.obj and fill g_pVertexBuffer_Tank_Palette vertex buffer
	verticesTank_Palette = Obj_Loader("Media/Tank_T90_Palette.obj", &vertexCountTank_Palette);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Palette;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Palette;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Tank_Palette);
	if (FAILED(hr))     return hr;

	// Read Turret.obj and fill g_pVertexBuffer_Turret vertex buffer
	verticesTank_Turret = Obj_Loader("Media/Tank_T90_Turret.obj", &vertexCountTank_Turret);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Turret;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Turret;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Tank_Turret);
	if (FAILED(hr))     return hr;


	// VERTEX BUFFERS FOR ENEMY TANK
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Body;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Body;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Enemy_Tank_Body);
	if (FAILED(hr))     return hr;

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Palette;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Palette;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Enemy_Tank_Palette);
	if (FAILED(hr))     return hr;

	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountTank_Turret;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesTank_Turret;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Enemy_Tank_Turret);
	if (FAILED(hr))     return hr;


	// VERTEX BUFFER FOR BALL
	// Read Ball.obj and fill g_pVertexBuffer_Ball vertex buffer
	verticesBall = Obj_Loader("Media/Ball.obj", &vertexCountBall);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountBall;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesBall;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Ball);
	if (FAILED(hr))     return hr;


	// Read Walls.obj and fill g_pVertexBuffer_Walls vertex buffer
	verticesWalls = Obj_Loader("Media/Walls.obj", &vertexCountWalls);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountWalls;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesWalls;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Walls);
	if (FAILED(hr))     return hr;

	// Read Ground.obj and fill g_pVertexBuffer_Ground vertex buffer
	verticesGround = Obj_Loader("Media/Ground.obj", &vertexCountGround);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountGround;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesGround;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_Ground);
	if (FAILED(hr))     return hr;

	// Read RedDot.obj and fill g_pVertexBuffer_RedDot vertex buffer
	verticesRedDot = Obj_Loader("Media/RedDot.obj", &vertexCountRedDot);
	bd.ByteWidth = sizeof(SimpleVertex)* vertexCountRedDot;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = verticesRedDot;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_RedDot);
	if (FAILED(hr))     return hr;

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Create the constant buffers
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))	return hr;

	bd.ByteWidth = sizeof(ConstantBuffer);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))	return hr;

	bd.ByteWidth = sizeof(ConstantBuffer);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr))  return hr;


	// Load the Texture Plane
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Media/Ground.jpg", NULL, NULL, &g_pTextureGround, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture Tank_T90_Body.png
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Media/Tank_T90_Body.png", NULL, NULL, &g_pTextureTank_Body, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture Tank_T90_Palette.png
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Media/Tank_T90_Palette.png", NULL, NULL, &g_pTextureTank_Palette, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture Tank_T90_Turret.png
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Media/Tank_T90_Turret.png", NULL, NULL, &g_pTextureTank_Turret, NULL);
	if (FAILED(hr))	return hr;

	// Load the Texture Brick.jpg
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Media/Brick.jpg", NULL, NULL, &g_pTexture_Wall, NULL);
	if (FAILED(hr))	return hr;


	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))	return hr;

	// Rasterizer state object
	//D3D11_RASTERIZER_DESC RasterDesc;
	//ZeroMemory(&RasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	//RasterDesc.FillMode = D3D11_FILL_SOLID;
	//RasterDesc.CullMode = D3D11_CULL_BACK;
	////RasterDesc.FrontCounterClockwise = true;
	//RasterDesc.DepthClipEnable = TRUE;
	//g_pd3dDevice->CreateRasterizerState(&RasterDesc, &g_pRasterizerStateSolid);

	// Initialize the world matrices
	g_World = XMMatrixIdentity();
	g_World_Tank = XMMatrixIdentity();
	g_World_Enemy = XMMatrixIdentity();
	g_World_RedDot = XMMatrixIdentity();
	g_World_Ball = XMMatrixIdentity();
	g_World_Ball_Enemy = XMMatrixIdentity();

	// Düþman tankýn baþlangýç konumu
	/*g_World_Enemy._41 = -85;								//Kapatýldý.
	g_World_Enemy._42 =   0;
	g_World_Enemy._43 =  85;*/

	// Initialize the view matrix
	Eye = XMVectorSet(0.0f, 30.0f, -60.0f, 0.0f);
	At  = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	Up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f);

	cBuffer.mProjection = XMMatrixTranspose(g_Projection);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render(double time)
{
	// Update our time
	rot += 1.0f * time;
	if (rot > 6.28f) rot = 0.0f;

	// Clear the back buffer
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Clear the depth buffer to 1.0 (max depth)
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	cBuffer.mView = XMMatrixTranspose(g_View);
	XMStoreFloat4(&cBuffer.vEyePos, Eye);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);

	XMFLOAT4 vLightPos[2] =
	{
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	};

	XMFLOAT4 vLightColors[2] =
	{
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)
	};

	cBuffer.vLightColor[0] = vLightColors[0];
	cBuffer.vLightColor[1] = vLightColors[1];
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Light, &stride, &offset);

	XMMATRIX mRotate;
	XMMATRIX mTranslate1, mTranslate2, mTranslate3;

	mTranslate1 = XMMatrixTranslation(-25.0f, 0.0f, 0.0f);
	mTranslate2 = XMMatrixTranslation(-40.0f, 40.0f, 40.0f);
	mRotate = XMMatrixRotationY(rot);

	XMVECTOR vLightPos0 = XMLoadFloat4(&vLightPos[0]);
	vLightPos0 = XMVector3Transform(vLightPos0, mTranslate1);
	vLightPos0 = XMVector3Transform(vLightPos0, mRotate);
	vLightPos0 = XMVector3Transform(vLightPos0, mTranslate2);
	XMStoreFloat4(&vLightPos[0], vLightPos0);

	cBuffer.mWorld = XMMatrixTranspose(mTranslate1 * mRotate * mTranslate2);
	cBuffer.vLightPos[0] = vLightPos[0];
	cBuffer.vMeshColor = vLightColors[0];
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);

	// Render White Light
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader(g_pPixelShader_Solid, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->Draw(36, 0);

	mTranslate1 = XMMatrixTranslation(25.0f, 0.0f, 0.0f);
	mTranslate3 = XMMatrixTranslation(40.0f, 40.0f, -40.0f);

	XMVECTOR vLightPos1 = XMLoadFloat4(&vLightPos[1]);
	vLightPos1 = XMVector3Transform(vLightPos1, mTranslate1);
	vLightPos1 = XMVector3Transform(vLightPos1, mRotate);
	vLightPos1 = XMVector3Transform(vLightPos1, mTranslate3);
	XMStoreFloat4(&vLightPos[1], vLightPos1);

	cBuffer.mWorld = XMMatrixTranspose(mTranslate1 * mRotate * mTranslate3);
	cBuffer.vLightPos[1] = vLightPos[1];
	cBuffer.vMeshColor = vLightColors[1];
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);

	// Render Yellow Light
	g_pImmediateContext->Draw(36, 0);

	// Render RedDot (Niþangah)
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_RedDot, &stride, &offset);
	XMFLOAT4 CHairPosition;
	XMStoreFloat4(&CHairPosition, RedDotPosition);
	XMMATRIX mScale = XMMatrixScaling(1 + reddotDistance * 0.05, 1 + reddotDistance * 0.05, 1 + reddotDistance * 0.05);
	XMMATRIX mTranslate = XMMatrixTranslation(CHairPosition.x, CHairPosition.y, CHairPosition.z);
	g_World_RedDot = mScale * mTranslate;
	cBuffer.mWorld = XMMatrixTranspose(g_World_RedDot);
	cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader(g_pPixelShader_Solid, NULL, 0);
	g_pImmediateContext->Draw(vertexCountRedDot, 0);


	g_World = XMMatrixIdentity();
	cBuffer.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);


	// Render Ground
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Ground, &stride, &offset);
	cBuffer.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureGround);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
	g_pImmediateContext->Draw(vertexCountGround, 0);


	// Render Walls
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Walls, &stride, &offset);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexture_Wall);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
	g_pImmediateContext->Draw(vertexCountWalls, 0);

	// Tank Position
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	Tanker_Trans = XMMatrixTranslation(Tanker_Translation.x, Tanker_Translation.y, Tanker_Translation.z); ///Bura Deðiþtirildi y+50
	g_World_Tank = camRotationMatrix * Tanker_Trans;
	cBuffer.mWorld = XMMatrixTranspose(g_World_Tank);

	// Eye Position
	XMVECTOR goBack = 4 * (Eye - At);
	XMFLOAT4 goBack_Float4;
	XMStoreFloat4(&goBack_Float4, goBack);
	Eye = XMVectorSet(Tanker_Translation.x + goBack_Float4.x, Tanker_Translation.y + goBack_Float4.y+3, Tanker_Translation.z + goBack_Float4.z, 1); ///Bura Deðiþtirildi y+53
	XMFLOAT4 Eye_Float4;
	XMStoreFloat4(&Eye_Float4, Eye);
	cBuffer.vEyePos = Eye_Float4;


	// Render Our Tank
	if (renderTank)
	{
		cBuffer.vMeshColor = XMFLOAT4(0.7f, 0.0f, 0.0f, 1.0f);

		// Render Tank : Body
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Tank_Body, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Body);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Body, 0);

		// Render Tank : Palette
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Tank_Palette, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Palette);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Palette, 0);

		// Render Tank  : Turret
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Tank_Turret, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Turret);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Turret, 0);
	}


	// Render Our Ball
	if (renderTankBall)
	{
		g_World_Ball = XMMatrixRotationZ(0.1) * g_World_Ball; // mermiyi döndür

		XMFLOAT4 ballDirection_Float4;
		XMStoreFloat4(&ballDirection_Float4, ballDirection);
		g_World_Ball._41 += 0.3 * ballDirection_Float4.x;
		g_World_Ball._42 += 0.3 * ballDirection_Float4.y;
		g_World_Ball._43 += 0.3 * ballDirection_Float4.z;
		cBuffer.mWorld = XMMatrixTranspose(g_World_Ball);
		cBuffer.vMeshColor = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Ball, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Solid, NULL, 0);
		g_pImmediateContext->Draw(vertexCountBall, 0);
	}


	// Render Enemy
	if (renderEnemy)
	{
		pathCalculate();

		float angle;

		float speed = 20.0F * time;

		if (!G.path.empty())
		{
			if (fabs(coord[G.path.front()].x - g_World_Enemy._41) < 4.0F)
			{
				if (EnemyTanker_Translation.z - coord[G.path.front()].z < -4.0F)
				{

					Translation = speed / 2 * DefaultForward;
					XMFLOAT3 TranslationF3;
					XMStoreFloat3(&TranslationF3, Translation);
					EnemyTanker_Translation.x += TranslationF3.x;
					EnemyTanker_Translation.y += TranslationF3.y;
					EnemyTanker_Translation.z += TranslationF3.z;

					angle = 0.0F;
					
				}
				else if (EnemyTanker_Translation.z - coord[G.path.front()].z > 4.0F)
				{

					Translation = speed / 2 * DefaultDown;
					XMFLOAT3 TranslationF3;
					XMStoreFloat3(&TranslationF3, Translation);
					EnemyTanker_Translation.x += TranslationF3.x;
					EnemyTanker_Translation.y += TranslationF3.y;
					EnemyTanker_Translation.z += TranslationF3.z;
					
					angle = XM_PI;
					
				}
				else
				{
					G.path.pop();
					pathCalculate();
				}
			}
			else if (fabs(coord[G.path.front()].z - g_World_Enemy._43) < 4.0F)
			{
				if (EnemyTanker_Translation.x - coord[G.path.front()].x < -4.0F)
				{

					Translation = speed / 2 * DefaultRight;
					XMFLOAT3 TranslationF3;
					XMStoreFloat3(&TranslationF3, Translation);
					EnemyTanker_Translation.x += TranslationF3.x;
					EnemyTanker_Translation.y += TranslationF3.y;
					EnemyTanker_Translation.z += TranslationF3.z;

					angle = XM_PI / 2;
					
				}
				else if (EnemyTanker_Translation.x - coord[G.path.front()].x > 4.0F)
				{

					Translation = speed / 2 * DefaultLeft;
					XMFLOAT3 TranslationF3;
					XMStoreFloat3(&TranslationF3, Translation);
					EnemyTanker_Translation.x += TranslationF3.x;
					EnemyTanker_Translation.y += TranslationF3.y;
					EnemyTanker_Translation.z += TranslationF3.z;

					angle = -(XM_PI / 2);
					
				}
				else
				{
					G.path.pop();
					pathCalculate();
				}
			}
			else
			{
				G.path.pop();				
				pathCalculate();
			}
		}
		else
		{
			angle = Angle();
		}

		g_World_Enemy = XMMatrixRotationY(angle) * XMMatrixTranslation(EnemyTanker_Translation.x, EnemyTanker_Translation.y, EnemyTanker_Translation.z);

		cBuffer.mWorld = XMMatrixTranspose(g_World_Enemy);
		cBuffer.vMeshColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

		// Enemy : Body
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Enemy_Tank_Body, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Body);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Body, 0);

		// Enemy : Palette
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Enemy_Tank_Palette, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Palette);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Palette, 0);

		// Enemy  : Turret
		g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Enemy_Tank_Turret, &stride, &offset);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
		g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
		g_pImmediateContext->PSSetShader(g_pPixelShader_Textured, NULL, 0);
		g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
		g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureTank_Turret);
		g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
		g_pImmediateContext->Draw(vertexCountTank_Turret, 0);

	}

	//
	// Render Enemy Ball
	//
	XMVECTOR Tank_Position		= XMVectorSet(g_World_Tank._41, g_World_Tank._42, g_World_Tank._43, 0);
	XMVECTOR Enemy_Position		= XMVectorSet(g_World_Enemy._41, g_World_Enemy._42, g_World_Enemy._43, 0);
	XMVECTOR ShootingDirection	= XMVector3Normalize(Tank_Position - Enemy_Position);
	EnemyBarrelDirection		= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	EnemyBarrelDirection		= XMVector3Normalize(XMVector3TransformNormal(EnemyBarrelDirection, g_World_Enemy)); // XMVector3Transform() veya XMVector3TransformCoord() ile çalýþmýyor???
	ShootingAngle				= XMVectorGetX(XMVector3Dot(ShootingDirection, EnemyBarrelDirection));

	if (ShootingAngle < 0.9999)
	{
		BarrelRotationAngel		= 0.03;
		g_World_Enemy			= XMMatrixRotationY(BarrelRotationAngel) * g_World_Enemy;
		EnemyBarrelDirection	= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		EnemyBarrelDirection	= XMVector3Normalize(XMVector3TransformNormal(EnemyBarrelDirection, g_World_Enemy)); // XMVector3Transform() veya XMVector3TransformCoord() ile çalýþmýyor???
		ShootingAngle			= XMVectorGetX(XMVector3Dot(ShootingDirection, EnemyBarrelDirection));

		PlayEnemyFireOnce		= true;
		renderEnemyBall			= true;
	}
	else //if (ShootingAngle > 0.9999)
	{
		BarrelRotationAngel = 0.0;

		if (PlayEnemyFireOnce)
		{
			PlaySound(TEXT("fire.wav"), NULL, SND_FILENAME | SND_ASYNC);
			PlayEnemyFireOnce = false;

			// Merminin Baþlangýç noktasýný setle
			XMVECTOR Namlu_Ucu = 6 * EnemyBarrelDirection;
			XMFLOAT4 Namlu_Ucu_F4; XMStoreFloat4(&Namlu_Ucu_F4, Namlu_Ucu);
			g_World_Ball_Enemy = g_World_Enemy;
			g_World_Ball_Enemy._41 = g_World_Ball_Enemy._41 + Namlu_Ucu_F4.x;
			g_World_Ball_Enemy._42 = g_World_Ball_Enemy._42 + 1.5 + Namlu_Ucu_F4.y;
			g_World_Ball_Enemy._43 = g_World_Ball_Enemy._43 + Namlu_Ucu_F4.z;
		}

		if (renderEnemyBall)
		{
			g_World_Ball_Enemy = XMMatrixRotationZ(0.1) * g_World_Ball_Enemy; // mermiyi döndür

			XMFLOAT4 EnemyBarrelDirection_Float4;
			XMStoreFloat4(&EnemyBarrelDirection_Float4, EnemyBarrelDirection);
			g_World_Ball_Enemy._41 += 0.3 * EnemyBarrelDirection_Float4.x;
			g_World_Ball_Enemy._42 += 0.3 * EnemyBarrelDirection_Float4.y;
			g_World_Ball_Enemy._43 += 0.3 * EnemyBarrelDirection_Float4.z;
			cBuffer.mWorld = XMMatrixTranspose(g_World_Ball_Enemy);
			cBuffer.vMeshColor = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
			g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_Ball, &stride, &offset);
			g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cBuffer, 0, 0);
			g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
			g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pConstantBuffer);
			g_pImmediateContext->PSSetShader(g_pPixelShader_Solid, NULL, 0);
			g_pImmediateContext->Draw(vertexCountBall, 0);
		}

		XMVECTOR Ball_Initial_Position = XMVectorSet(g_World_Enemy._41, g_World_Enemy._42, g_World_Enemy._43, 0);
		XMVECTOR Ball_Current_Position = XMVectorSet(g_World_Ball_Enemy._41, g_World_Ball_Enemy._42, g_World_Ball_Enemy._43, 0);
		float ballTravelDistance = XMVectorGetX(XMVector3Length(Ball_Current_Position - Ball_Initial_Position));
		float shootingDistance = XMVectorGetX(XMVector3Length(Tank_Position - Enemy_Position));
		if (ballTravelDistance > shootingDistance) ShootingAngle = 0.0;
	}
	////////////////////////////////////////////////////////////////// Render Enemy Ball
	
	// Print something
	//std::wostringstream outs;
	//outs << " Print something = (" ;
	//SetWindowText(g_hWnd, outs.str().c_str());

	// Present our back buffer to our front buffer
	g_pSwapChain->Present(0, 0);

}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pVertexBuffer_RedDot) g_pVertexBuffer_RedDot->Release();
	if (g_pVertexBuffer_Light) g_pVertexBuffer_Light->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pPixelShader) g_pPixelShader_Solid->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

#pragma region Obj_Loader 

// The code that the function Obj_Loader includes is taken from the tutorial named
// "Tutorial 8: Loading Maya 2011 Models" at http://rastertek.com/tutdx11.html page. 
SimpleVertex* Obj_Loader(char* filename, int* verticesCount)
{
	ifstream fin;
	char input;

	// Initialize the counts.
	int vertexCount = 0;
	int textureCount = 0;
	int normalCount = 0;
	int faceCount = 0;

	// Open the file.
	fin.open(filename);

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input2;

	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	texcoords = new VertexType[textureCount];
	normals = new VertexType[normalCount];
	faces = new FaceType[faceCount];

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	*verticesCount = faceCount * 3;
	SimpleVertex* verticesModel = new SimpleVertex[*verticesCount];

	// Now loop through all the faces and output the three vertices for each face.
	int k = 0;
	for (int i = 0; i<faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		verticesModel[k].Pos.x = vertices[vIndex].x;
		verticesModel[k].Pos.y = vertices[vIndex].y;
		verticesModel[k].Pos.z = vertices[vIndex].z;

		verticesModel[k].Tex.x = texcoords[tIndex].x;
		verticesModel[k].Tex.y = texcoords[tIndex].y;

		verticesModel[k].Normal.x = normals[nIndex].x;
		verticesModel[k].Normal.y = normals[nIndex].y;
		verticesModel[k].Normal.z = normals[nIndex].z;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		k++;

		verticesModel[k].Pos.x = vertices[vIndex].x;
		verticesModel[k].Pos.y = vertices[vIndex].y;
		verticesModel[k].Pos.z = vertices[vIndex].z;

		verticesModel[k].Tex.x = texcoords[tIndex].x;
		verticesModel[k].Tex.y = texcoords[tIndex].y;

		verticesModel[k].Normal.x = normals[nIndex].x;
		verticesModel[k].Normal.y = normals[nIndex].y;
		verticesModel[k].Normal.z = normals[nIndex].z;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		k++;
		verticesModel[k].Pos.x = vertices[vIndex].x;
		verticesModel[k].Pos.y = vertices[vIndex].y;
		verticesModel[k].Pos.z = vertices[vIndex].z;

		verticesModel[k].Tex.x = texcoords[tIndex].x;
		verticesModel[k].Tex.y = texcoords[tIndex].y;

		verticesModel[k].Normal.x = normals[nIndex].x;
		verticesModel[k].Normal.y = normals[nIndex].y;
		verticesModel[k].Normal.z = normals[nIndex].z;

		k++;
	}

	// Release the four data structures.
	delete[] vertices;
	delete[] texcoords;
	delete[] normals;
	delete[] faces;

	return verticesModel;
}

#pragma endregion

float IntersectTriangle(XMVECTOR Ro, XMVECTOR Rd, SimpleVertex* verticesModel, int vertexCount, XMMATRIX& g_World)
{
	float min_t = FLT_MAX;
	float t = 0.0;

	XMVECTOR normal, R;
	float s, s1, s2, s3;
	XMVECTOR S, S1, S2, S3;

	for (int i = 0; i < vertexCount; i += 3)
	{

		XMVECTOR v0 = XMLoadFloat3(&verticesModel[i].Pos);
		XMVECTOR v1 = XMLoadFloat3(&verticesModel[i + 1].Pos);
		XMVECTOR v2 = XMLoadFloat3(&verticesModel[i + 2].Pos);

		v0 = XMVector3Transform(v0, g_World);
		v1 = XMVector3Transform(v1, g_World);
		v2 = XMVector3Transform(v2, g_World);

		normal = XMVector3Cross((v1 - v0), (v2 - v0));

		float D = -XMVectorGetX(XMVector3Dot(v0, normal));
		t = -(XMVectorGetX(XMVector3Dot(Ro, normal)) + D) / XMVectorGetX(XMVector3Dot(normal, Rd));

		if (t > 0)
		{
			R = Ro + t * Rd;

			S = XMVector3Cross((v1 - v0), (v2 - v0));
			S1 = XMVector3Cross((R - v0), (v2 - v0));
			S2 = XMVector3Cross((v1 - v0), (R - v0));
			S3 = XMVector3Cross((v1 - R), (v2 - R));

			s = XMVectorGetX(XMVector3Length(S));
			s1 = XMVectorGetX(XMVector3Length(S1));
			s2 = XMVectorGetX(XMVector3Length(S2));
			s3 = XMVectorGetX(XMVector3Length(S3));

			float fark = (float)::abs(s - (s1 + s2 + s3));
			float epsilon = 0.0001F;

			if ((fark <= epsilon) && (t < min_t)) min_t = t;
		}
	}

	return min_t;
}