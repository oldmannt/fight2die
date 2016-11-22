// INCLUDES ///////////////////////////////////////////////

#define INITGUID

#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <fcntl.h>

#include <ddraw.h>  // directX includes
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <dinput.h>


#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"



// MACROS ///////////////////////////////////////////////

#define DOT_PRODUCT(ux,uy,vx,vy) ((ux)*(vx) + (uy)*(vy))

// DEFINES ////////////////////////////////////////////////

//640x480 16位窗口模式
#define WINDOW_WIDTH	800   // size of window
#define WINDOW_HEIGHT	600
#define WINDOW_BPP		16
#define WINDOWED_APP	1

#define SURFACE_MEMORY   DDSCAPS_SYSTEMMEMORY	 //内存或显存DDSCAPS_VIDEOMEMORY
const int WORLD_WIDTH = 1600;	// 地图大小
const int WORLD_HEIGHT = 1200;

const int FRAME_RATE = 17;	// ??秒数

const int MAX_STARS = 500;	// 星星个数
#define STAR_PANEL0		0	// 3个平面
#define STAR_PANEL1		1
#define STAR_PANEL2		2
const USHORT STAR_COLOR0 =  ((255 & 31) + ((255 & 63) << 5) + ((255 & 31) << 11));
const USHORT STAR_COLOR1 =  ((180 & 31) + ((180 & 63) << 5) + ((180 & 31) << 11));
const USHORT STAR_COLOR2 =  ((100 & 31) + ((100& 63) << 5) + ((100 & 31) << 11));

#define GAME_STATE_STORY	0	// 游戏状态
#define GAME_STATE_MENU		1
#define GAME_STATE_LOAD		2
#define GAME_STATE_RESET	3
#define GAME_STATE_RUNNING	4
#define GAME_STATE_PAUSED	5
#define GAME_STATE_EXIT		6
#define GAME_STATE_LOST		7
#define GAME_STATE_WIN		8

/////////////////////////////////////////    声音   ////////////////////////////////////////

const int MAX_SOUND_FIRE = 16;
const int MAX_SOUND_EXPL = 16;
int g_nSoundMain = -1,
	g_nSoundBomb = -1,
	g_nSoundPlayerMove = -1,
	g_nSoundPlayerDie  = -1,
	g_nSoundBombOut	   = -1,
	g_nSoundBegin	   = -1;

int g_nSoundFire[MAX_SOUND_FIRE] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int g_nSoundExpl[MAX_SOUND_EXPL] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

/////////////////////////////////////////敌机和玩家////////////////////////////////////////

#define INDEXI_HEALTH				0	//敌机，player,BOB对象的varsI数组下标
#define INDEXI_DAMAGE				1
#define INDEXI_PLASMA_RANGE			2
#define INDEXI_SCORE				3
#define INDEXI_FIGHT_TYPE			4

#define FIGHT_TYPE_PLAYER			0	// 战机类型
#define FIGHT_TYPE_WRAITHB			1
#define FIGHT_TYPE_WRAITHR			2
#define FIGHT_TYPE_BEE				3
#define FIGHT_TYPE_GUNSHIP			4

#define INDEXI_MOVESPEED			5 // player，BOB对象的varsI数组下标
#define INDEXI_FIRESPEED			6
#define INDEXI_BOMBS				7
#define INDEXI_BOMB_RANGE			8	// 爆炸范围
#define INDEXI_BOMB_DAMAGE			9
#define INDEXI_BOMB_PUSH			10	//炸弹爆炸强加在敌机的速率
#define INDEXI_COUNTER_MOVEUP		11	//移动加速计数
#define INDEXI_COUNTER_FIREUP		12	//攻击加速计数
#define INDEXI_COUNTER_INVINCIBLE	13	// 无敌时间计数 大于0时无敌
#define INDEXI_KILLS				14  // 杀敌数
			

// 玩家状态标志
#define PLAYER_STATE_DEAD      		0		// 玩家死了
#define PLAYER_STATE_ALIVE      	1		// 玩家活着
#define PLAYER_STATE_MOVEUP			2		// 移动加速
#define PLAYER_STATE_INVINCIBLE		4		// 无敌状态
#define PLAYER_STATE_FIREUP			8		// 攻击加速
#define PLAYER_STATE_HAND			16		// 已获得道具“手”


// player常量
const int PLAYER_MAX_HEALTH = 200;					// 玩家初始生命
const int PLAYER_MOVESPEED = 180/(1000/FRAME_RATE);			// 象素每帧
//const int PLAYER_MOVEUP = 6;
const int PLAYER_BOMBS = 50;				// 玩家初始炸弹数
const int BOMB_RANGE = 256;					// 炸弹爆炸范围 正方形边长
const int PLAYER_DAMAGE = 45;				// 玩家初始攻击力
const int BOMB_DAMAGE = 60;					// 炸弹杀伤力
const int BOMB_PUSH = 500;					// 炸弹爆炸强加在敌机的速率
const int PLAYER_FIRESPEED = 1000/FRAME_RATE/5;		//  每秒5次 单位是帧
//const int PLAYER_FIREUP = 3;
const float PLAYER_MASS = 20;				// 质量
const float PLAYER_PLASMA_SPEED = 960/(1000/FRAME_RATE);		//每秒象素
const float PLAYER_BOMB_SPEED = 12;
const int PLAYER_PLASMA_RANGE = (WINDOW_WIDTH*5/4)/PLAYER_PLASMA_SPEED;	// 射程计数

// 敌机BOB
#define INDEXI_DIR					5
#define INDEXI_DIRPRE				6
#define INDEXI_DIRGUNSHIP			7
#define INDEXI_ATTACK_RANGE			8
#define INDEXI_DYING				9	// 垂死时间
#define INDEXI_MAXHEALTH			10


#define INDEXF_WORLDX				0 // 所有BOB的varsF数组下标
#define INDEXF_WORLDY				1

#define INDEXF_MASS					2 // player，敌机BOB
#define INDEXF_PLASMA_SPEED			3 // 子弹速度

#define INDEXF_BOMB_SPEED			4 // 玩家BOB

#define INDEXF_MAX_MOVESPEED		4 // 敌机BOB
#define INDEXF_MOVESPEED			5 // 移动速度
#define INDEXF_ACC					6 // 加速度
#define INDEXF_ACCY					7 // GUNSHIP Y方向加速度

// 敌机状态
#define ENEMY_STATE_DEAD	0
#define ENEMY_STATE_ALIVE	1
#define ENEMY_STATE_DYING	2

// 敌机常量
const int MAX_WRAITH = 200;
const int MAX_BEE	= 100;
const int MAX_GUNSHIP = 50;

const int HEALTH_WRAITHB = 50;
const int HEALTH_WRAITHR = 100;
const int HEALTH_BEE	 = 100;
const int HEALTH_GUNSHIP = 1000;

const int DAMAGE_WRAITHB = 2;
const int DAMAGE_WRAITHR = 4;
const int DAMAGE_BEE	 = 2;
const int DAMAGE_GUNSHIP = 5;
															// 子弹速度
const float WRAITHB_PLASMA_SPEED = 400/(1000/FRAME_RATE);	// 象素每帧 一秒400个象素
const float WRAITHR_PLASMA_SPEED = 400/(1000/FRAME_RATE);
const float BEE_PLASMA_SPEED = 400/(1000/FRAME_RATE);
const float GUNSHIP_PLASMA_SPEED = 450/(1000/FRAME_RATE);

const int ATTACK_RANGE_WRAITHB = 78 + 15; // 象素 攻击判定的距离
const int ATTACK_RANGE_WRAITHR = 78 + 15;	// 78= 40 + 38
const int ATTACK_RANGE_BEE	   = 67 + 15;   // 67 = 29 + 38
const int ATTACK_RANGE_GUNSHIP = 117 + WINDOW_HEIGHT/2;

const int RANGE_WRAITHB = ATTACK_RANGE_WRAITHB/WRAITHB_PLASMA_SPEED;	// 单位是帧 射程
const int RANGE_WRAITHR = ATTACK_RANGE_WRAITHR/WRAITHR_PLASMA_SPEED;
const int RANGE_BEE		= ATTACK_RANGE_BEE/BEE_PLASMA_SPEED;
const int RANGE_GUNSHIP = ATTACK_RANGE_GUNSHIP/GUNSHIP_PLASMA_SPEED;

const int FIRECOUNT_WRAITHB = 1000/FRAME_RATE/2;	// 攻击频率 多少帧攻击一次 单位帧
const int FIRECOUNT_WRAITHR = 1000/FRAME_RATE/2;	// 一秒攻击2次
const int FIRECOUNT_BEE		= 1000/FRAME_RATE/2;
const int FIRECOUNT_GUNSHIP = (1000/FRAME_RATE)/2.5;

const int SCORE_WRAITHB = 10;	// 分值
const int SCORE_WRAITHR = 20;
const int SCORE_BEE		= 25;
const int SCORE_GUNSHIP = 100;

const float MOVESPEED_WRAITHB = 150/(1000/FRAME_RATE);	// 最大移动速率 象素每秒
const float MOVESPEED_WRAITHR = 150/(1000/FRAME_RATE);
const float MOVESPEED_BEE	  = 180/(1000/FRAME_RATE);
const float MOVESPEED_GUNSHIPX = 300/(1000/FRAME_RATE);	// X方向最大速率
const float MOVESPEED_GUNSHIPY = 50.0/(1000.0/FRAME_RATE);  // Y方向最大速率

const float ACC_WRAITHB = 0.1f;		// 加速度 象素每帧平方
const float ACC_WRAITHR = 0.1f;
const float ACC_BEE		= 0.2f;
const float ACC_GUNSHIP = 0.1f;
const float ACC_GUNSHIPY = 0.01f;

const float MASS_WRAITHB = 10;	//质量
const float MASS_WRAITHR = 10;
const float MASS_BEE	 = 5;
const float MASS_GUNSHIP = 100;

#define DIR_NNNN		0	// 方向
#define DIR_NNNE		1
#define DIR_NNEE		2
#define DIR_NEEE		3
#define DIR_EEEE		4
#define DIR_EEES		5
#define DIR_EESS		6
#define DIR_ESSS		7
#define DIR_SSSS		8
#define DIR_SSSW		9
#define DIR_SSWW		10
#define DIR_SWWW		11
#define DIR_WWWW		12
#define DIR_WWWN		13
#define DIR_WWNN		14
#define DIR_WNNN		15

#define DIR_BEE_EEEE		0	// BEE的方向
#define DIR_BEE_EEES		1
#define DIR_BEE_EESS		2
#define DIR_BEE_ESSS		3
#define DIR_BEE_SSSS		4
#define DIR_BEE_SSSW		5
#define DIR_BEE_SSWW		6
#define DIR_BEE_SWWW		7
#define DIR_BEE_WWWW		8
#define DIR_BEE_WWWN		9
#define DIR_BEE_WWNN		10
#define DIR_BEE_WNNN		11
#define DIR_BEE_NNNN		12
#define DIR_BEE_NNNE		13
#define DIR_BEE_NNEE		14
#define DIR_BEE_NEEE		15

#define DIR_RIGHT		0		// gunship向右
#define DIR_LEFT		1		// gunship向左

const int MIN_DIST = WINDOW_WIDTH*5/4;		// 敌机出现时离玩家的最近和最远距离
const int MAX_DIST = WINDOW_WIDTH/4;

/*const int TRACK_WRAITHB = 1000/FRAME_RATE/2;	// 跟踪时方向判断的频率 5月25日7:54取消
const int TRACK_WRAITHR = 1000/FRAME_RATE/2;
const int TRACK_BEE		= 1000/FRAME_RATE/3;
const int TRACK_GUNSHIP = 1000/FRAME_RATE/2;*/

const int DYING_COUNTER = 1000/FRAME_RATE;	// 1秒的垂死

/////////////////////////////////////////地图相关////////////////////////////////////////
// 玩家超出这个RECT时移动SCREEN
const RECT SCREEN_MOVE = {WINDOW_WIDTH/4, WINDOW_HEIGHT/4,
						  WINDOW_WIDTH*3/4, WINDOW_HEIGHT*3/4};
// SCREEN不能超出这个范围
const RECT SCREEN_RANGE = { 0, 0,WORLD_WIDTH-WINDOW_WIDTH, WORLD_HEIGHT-WINDOW_HEIGHT};

/////////////////////////////////////////子弹///////////////////////////////////////////

// 子弹state	保存状态
#define PlASMA_STATE_OFF	0	// 未激活
#define PLASMA_STATE_ON		1

//anim_state	保存动画序列号，并作为子弹类型标志
#define PLASMA_ANIM_PLAYER		0	// 玩家的子弹
#define PLASMA_ANIM_WRAITH		1	// 敌机A
#define PLASMA_ANIM_GUNSHIP		2	// 敌机D

//#define PLASMA_ANIM_BOMB		5	// 炸弹

//varsI数组
#define INDEXI_PLASMA_DAMAGE		0	// 子弹的伤害

const int MAX_PLASMA = 80;	// 全版子弹个数

/////////////////////////////////////////炸弹///////////////////////////////////////////

#define BOMB_INDEXI_DAMAGE			0
#define INDEXI_RANGE			1	// 炸弹的范围，正方形
#define INDEXI_PUSH				2
//#define INDEXI_FIRE_X			3
//#define INDEXI_FIRE_Y			4	// 炸弹要飞到的点

const int MAX_BOMB = 1;

/////////////////////////////////////////爆炸////////////////////////////////////////

// 状态
#define BURST_STATE_OFF			0
#define BURST_STATE_ON			1

// varsI数组
#define INDEXI_BURST_WIDTH			0
#define INDEXI_BURST_HEIGHT			1

const int MAX_BURSTS = 30;

/////////////////////////////////////////粒子////////////////////////////////////////

const int MAX_PARTICLES = 128;

// 粒子状态
#define PARTICLE_STATE_DEAD               0
#define PARTICLE_STATE_ALIVE              1

// 粒子类型
#define PARTICLE_TYPE_FLICKER             0
#define PARTICLE_TYPE_FADE                1 

// 粒子颜色
#define PARTICLE_COLOR_RED				0 
#define PARTICLE_COLOR_GREEN			1 
#define PARTICLE_COLOR_BLUE				2 
#define PARTICLE_COLOR_WHITE			3 

/////////////////////////////////////////道具////////////////////////////////////////

// 道具类型
#define POWERUP_TYPE_HEALTH       		0		// 回血
#define POWERUP_TYPE_MOVE           	1		// 移动加速
#define POWERUP_TYPE_BOMB				2		// 炸弹
#define POWERUP_TYPE_FIRE   			3		// 攻击加速
#define POWERUP_TYPE_INVINCIBLE			4		// 无敌
#define POWERUP_TYPE_HAND				5		// 手

#define POWERUP_TIME					300		// 道掉出后的存活时间
#define POWERUP_STATE_DEAD      0		// 未激活
#define POWERUP_STATE_ALIVE     1		// 已激活

#define INDEXI_POWERUP_TYPE      0		// varsI数组下标，道具类型

#define HEALTH_START 			0		// 掉出的概率设置
#define HEALTH_END				9
#define MOVE_START				20
#define MOVE_END				29
#define BOMB_START				40
#define BOMB_END				41
#define FIRE_START				50
#define FIRE_END				59
#define INVINCIBLE_START		70
#define INVINCIBLE_END			79
#define HAND_START				90
#define HAND_END				90

const int POWERUP_HEALTH = 10;
const int POWERUP_MOVE   = 320/(1000/FRAME_RATE);		// 一秒320
const int POWERUP_MOVE_TIME = 20*1000/FRAME_RATE;		// 移动加速持续20秒
const int POWERUP_FIREUP_TIME = 20*1000/FRAME_RATE;		// 攻击加速持续20秒
const int POWERUP_FIREUP = 1000/FRAME_RATE/10;		// 加速的攻击间隔秒20次
const int POWERUP_INVINCIBLE_TIME = 20*1000/FRAME_RATE;	// 无敌持续20秒
const int MAX_POWERUPS = 16;					// 道具个数上限
const int POWERUP_LIVETIME = 5*1000/FRAME_RATE;		// 5秒
const int POWERUP_HAND_GET = 1000/FRAME_RATE; // 1秒

/////////////////////////////////文本////////////////////////

#define TEXT_GREEN		0
#define TEXT_RED		1

const int MAX_CHAR = 96;

/////////////////////////////////小地图//////////////////////

const int MAP_SCALE = 10;	// 缩小倍数
const int MAP_LEFT = 320;	// 定位四个边框
const int MAP_TOP = 0;
const int MAP_RIGHT = 454;
const int MAP_BOTTOM = 94;

/////////////////////////////////碰撞///////////////////////

const float g_fE = 0.8;		// 碰撞恢复系数 1为完全弹碰撞

const int NUM_COLLISION = 1 + MAX_WRAITH + MAX_BEE+ MAX_GUNSHIP;

///////////////////////////////敌机出现的批数///////////////

const int MAX_ARM = 8;
int g_nWraith = MAX_WRAITH;	// 当前可用各类敌机数
int g_nBee = MAX_BEE;
int g_nGunship = MAX_GUNSHIP;

////////////////////////////////刷情///////////////////////

const float fTextSpeed = 1.5;

// TYPES //////////////////////////////////////////////////

// 星空的数据结构
typedef struct STAR_TYP
{
	int x,y;					 // 星星的位置
	USHORT usColor;               // 星星的颜色
	int nPlane;					 // 星星所在的平面
} STAR;

// 粒子
typedef struct PARTICLE_TYP
{
	int nState;           // 粒子状态
	int nType;            // 效果类型
	int x,y;             // 地图坐标
	int xv,yv;           // 速度
	USHORT usCurColor;				// 颜色
	int nRed;
	int nGreen;
	int nBlue;
	int nCounter;         // 计数器
	int nMaxCount;       // 计数器最大值
} PARTICLE, *PARTICLE_PTR;

// 一个对象一批出现的敌机
typedef struct ARM_TYP
{
	UINT unFrame;	// 帧序号
	int nWraithB;	// 敌机A数
	int nWraithR;	// 敌机B数
	int nBee;		// 敌机C数
	int nGunShip;	// 敌机D数
}ARM,*ARM_PTR;


// 全局变量 ///////////////////////////////////////////////
HWND main_window_handle           = NULL; // save the window handle
HINSTANCE main_instance           = NULL; // save the instance

POINT	g_ptScreen	= {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};	// 显示窗口在地图中的位置

int g_nMouseX,g_nMouseY;				// 光标位置
int g_nGameState = GAME_STATE_STORY;	// 游戏状态

float g_fCosTab16[16];// 三角函数表
float g_fSinTab16[16];
float g_fCosTab64[64];
float g_fSinTab64[64];
float g_fTanTab4[4];

// 标识敌机出的数量和时间的结构
ARM g_Arm[MAX_ARM]= 
{
	{1*1000/FRAME_RATE,20,0,0,0},
	{10*1000/FRAME_RATE,20,20,0,0},
	{30*1000/FRAME_RATE,0,30,0,0},
	{50*1000/FRAME_RATE,10,10,10,10},
	{70*1000/FRAME_RATE,60,0,0,0},
	{80*1000/FRAME_RATE,0,60,0,0},
	{90*1000/FRAME_RATE,0,0,80,0},
	{100*1000/FRAME_RATE,0,0,0,20}
};		

BOB g_bobPlayer;		// 玩家
BOB g_bobAim;			// 准星
STAR g_Star[MAX_STARS];	// 星空
BOB g_bobPlasma[MAX_PLASMA];	// 子弹
BOB g_bobBomb[MAX_BOMB];				// 炸弹
BOB g_bobBrusts[MAX_BURSTS];		//爆炸
PARTICLE g_Particle[MAX_PARTICLES]; // 粒子
BOB g_bobPowerup[MAX_POWERUPS];		// 道具
LPDIRECTDRAWSURFACE7 g_lpddsGreenText[MAX_CHAR];		// 红字和绿字
LPDIRECTDRAWSURFACE7 g_lpddsRedText[MAX_CHAR];	

BOB g_bobWraith[MAX_WRAITH];		// 敌机A，B
BOB g_bobBee[MAX_BEE];				// 敌机C
BOB g_bobGunship[MAX_GUNSHIP];			// 敌机D

BOB g_bobMap;	// 地图

// 参与碰撞的BOB MAX_WRAITH + MAX_BEE + MAX_GUNSHIP
BOB_PTR g_pbobCollision[NUM_COLLISION];		

BITMAP_IMAGE g_imgBegingBack;	// 保存背景图片
BOB g_bobStory;		// 字幕滚动


// 函数 //////////////////////////////////////////////////

void GameInit(void);		// 游戏初始化
void GameMain(void);		// 游戏主循环
void GameShutdown(void);		// 释放游戏资源

void CreateTables(void);	// 创建查找表

void LoadSoundMusic(void);	// 读取声音

inline void WorldReset(void);	//// 重置地图

// 画矩形 血条
void DrawBlood(int nLeft, int nTop,int nHight, int nCur, int nMax);
void TextInit(void);				// 初始化文字
void TextDraw(int x, int y, int width, int height, int color, TCHAR* szText); // 输出文字
void TextRelease(void);

void PlayerInit(void);	//初始化player
void PlyerReset(void);	// 重置player数据
void PlayerDraw(void);	// 绘制玩家
void PlayerMove(void);	// 移动玩家
void PlayerRefresh(void);	// 玩家状态更新
inline void PlayerRelease(void);	// 释放玩家
void PlayerInfo(void);		// 显示玩家信息

inline void AimInit(void);		// 初始化准星
inline void AimDraw(void);		// 画准星
inline void AimRelease(void);	// 释放准星

void StartInitReset(void);	// 初始化星星
void StarMove(float dx, float dy);	// 移动星星
void StarDraw(void);	// 画星星

void PlasmaInit(void);		// 初始化子弹
void PlasmaRelease(void);	// 释放子弹
void PlasmaMove(void);		// 移动子弹
void PlasmaDraw(void);		// 画子弹
void PlasmaFire(int x, int y, float fVx, float fVy, BOB_PTR pbobSource);		// 发射子弹
void PlasmaReset(void);

void BombInit(void);	// 初始化炸弹
void BombReset(void);	// 重置炸弹
void BombRelease(void);	// 释放炸弹
void BombFire(int x, int y, float fVx, float fVy, int nCount,BOB_PTR pbobSource);	// 发射炸弹
void BombMove(void);	// 移动炸弹
void BombDraw(void);	// 画炸弹

void BurstInit(void);		// 初始化爆炸
void BurstReset(void);		// 重置爆炸
int  BurstStart(int x, int y, int width, int height, float xv,float yv);	//开始爆炸
void BurstMove(void);		// 移动爆炸
void BurstDraw(void);		// 画爆炸
void BurstRelease(void);	// 释放爆炸

void ParticleInitReset(void);	// 初始化粒子
void ParticleStart(int nType, int nColor, int nTime, int x, int y, int vx, int vy); // 激活一个粒子
void ParticleStartExplosion(int nType, int nColor, int nTime, int x, int y, int vx, int vy, int nParticles); // 爆炸粒子
void ParticleMove(void);	// 移动粒子
void ParticleDraw(void);	// 画粒子

void PowerupInit(void);		// 初始化道具
void PowerupReset(void);	// 重置道具
void PowerupStart(float x,float y);	// 掉出道具
void PowerupDraw(void);		// 画道具
void PowerupRelease(void);	// 释放道具
void PowerupRefresh(void);	// 更新道具
void PowerupBeGeted(BOB_PTR bobBeGeted);		// 道具被拿

int LoadSurface(LPDIRECTDRAWSURFACE7 lpdds, // 目标表面
				BITMAP_FILE_PTR bitmap, // 源位图
				int cx,int cy,   // cell or absolute pos. to scan image from
				int width, int height,int mode) ;
// 将一个表面BLT到另一个表面上
int BltSuerface(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // 在目标表面的位置
				int nSrcWidth, int nSrcHeight); // 源大小
// 将一个表面BLT到另一个表面上 并缩放
int BltSuerfaceScaled(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
					  int nDestX, int nDestY,  // 在目标表面的位置
					  int nSrcWidth, int nSrcHeight, // 源大小
					  int nDestWidth, int nDestHeight); // 目标大小

void WraithInit(void);		// 初始化Wraith
void WraithRelease(void);	// 释放
void WraithReset(void);		// 重置
void EnemyStart(int nNum, int nType);	// 出现敌机
void WraithMove(void);		// 移动
void WraithDraw(void);		// 绘制

void MapInit(void);		// 初始化地图
void MapDraw(void);		// 画地图
inline void MapRelease(void);	// 释放地图

void BeeInit(void);		// 初始化Bee
void BeeRelease(void);	// 释放
void BeeReset(void);		// 重置
void BeeMove(void);		// 移动
void BeeDraw(void);		// 绘制

void GunshipInit(void);		// 初始化Gunship
void GunshipRelease(void);	// 译放
void GunshipReset(void);	// 重置
void GunshipMove(void);		// 移动
void GunshipDraw(void);		// 绘制

void StoryInit(void);	// 初始化剧情显示
void StoryBegin(void);	// 开场
void StoryRelease(void);	

// 计算 平方和的根
float FastDistance(float x, float y)
{
	// 误差3.5%

	return sqrt(x*x + y*y);

	/*x = abs(x);
	y = abs(y);

	float mn = MIN(x,y);


	return(x+y-(mn/2)-(mn/4)+(mn/16));*/
} // end FastDistance



// 生成碰撞参与对象表
void CollisionInit(void);
void Collision(void);	// 碰撞处理
LRESULT CALLBACK WindowProc(HWND hwnd, 
							UINT msg, 
							WPARAM wparam, 
							LPARAM lparam)
{
	// what is the message 
	switch(msg)
	{	
	case WM_SYSCOMMAND:	// 屏蔽系统菜单

		break;

	case WM_CREATE: 

		break;
	case WM_PAINT:

		break;

	case WM_DESTROY: 

		PostQuitMessage(0);

		break;

	default:
		return (DefWindowProc(hwnd, msg, wparam, lparam));

	} // end switch

	return 0;
} // end WinProc


int WINAPI WinMain(	HINSTANCE hinstance,
				   HINSTANCE hprevinstance,
				   LPSTR lpcmdline,
				   int ncmdshow)
{
	WNDCLASSEX winclass; 
	HWND	   hwnd;	 
	MSG		   msg;		 
	TCHAR szAppName[] = "FightToDie";

	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
								CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= szAppName;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	// 保存实例句柄到全局变量
	main_instance = hinstance;

	if (!RegisterClassEx(&winclass))
		return(0);

	if (!(hwnd = CreateWindowEx(NULL,                  
		szAppName,     
		szAppName, 
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION, 
		0,0,	  
		WINDOW_WIDTH,WINDOW_HEIGHT,  // 窗口大小
		NULL,	  
		NULL,	  
		hinstance,
		NULL)))	
		return(0);

	// 保存窗口句柄到全局
	main_window_handle = hwnd;

	RECT window_rect = {0,0,WINDOW_WIDTH-1,WINDOW_HEIGHT-1};

	AdjustWindowRectEx(&window_rect,
		GetWindowStyle(hwnd),
		GetMenu(hwnd) != NULL,
		GetWindowExStyle(hwnd));

	window_client_x0 = -window_rect.left;
	window_client_y0 = -window_rect.top;
	MoveWindow(hwnd,
		100, // x position
		50, // y position
		window_rect.right - window_rect.left, // width
		window_rect.bottom - window_rect.top, // height
		TRUE);

	ShowWindow(hwnd, SW_SHOW);
	
	// 游戏初始化
	GameInit();

	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // end if

		// 游戏主循环
		GameMain();

	} // end while

	// 释放游戏资源
	GameShutdown();

	return(msg.wParam);

} // end WinMain

// 游戏初始化
void GameInit(void)
{
	// 产生随机数种子
	srand(Start_Clock());

	// 创建查找表
	CreateTables();

	// 初始化DirectDraw
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// DirectSound初始化
	DSound_Init();

	// DirectInput初始化
	DInput_Init();

	// 键盘初始化
	DInput_Init_Keyboard();

	// 初始化鼠标
	DInput_Init_Mouse();

	// 读取声音
	LoadSoundMusic();

	// 星空初始化
	StartInitReset();

	// 子弹初始化
	PlasmaInit();

	// 炸弹初始化
	BombInit();

	// 爆炸初始化
	BurstInit();

	// 粒子系统初始化
	ParticleInitReset();

	// 道具初始化
	PowerupInit();

	// 玩家初始化
	PlayerInit();

	// 敌机初始化
	WraithInit();
	BeeInit();
	GunshipInit();

	// 初始化文字
	TextInit();

	// 初始化准星
	AimInit();

	// 初始化
	MapInit();

	// 碰撞初始化
	CollisionInit();

	// 刷情初始化
	StoryInit();

}// end GameInit

// 游戏资源释放
void GameShutdown(void)
{
	// 刷情
	StoryRelease();
	// 地图
	MapRelease();

	//准星
	AimRelease();

	// ASCII文字
	TextRelease();

	// 敌机
	GunshipRelease();
	BeeRelease();
	WraithRelease();

	// 玩家
	PlayerRelease();

	// 道具
	PowerupRelease();

	//爆炸
	BurstRelease();

	// 炸弹
	BombRelease();

	// 子弹
	PlasmaRelease();

	// release Mouse
	DInput_Release_Mouse();

	// release Keyboard
	DInput_Release_Keyboard();

	// shut down directinput
	DInput_Shutdown();

	// unload sounds
	DSound_Delete_All_Sounds();

	// shutdown directsound
	DSound_Shutdown();

	// shutdown directdraw
	DDraw_Shutdown();

}// end GameShutdown

// 游戏主循环
void GameMain(void)
{
	static bool s_bBomb = false,	//投弹标志
				s_bPause = false; // 暂停标志
	static UINT s_unFrame = 0;		// 当前帧序号
	static int s_nArm = 0;		// 下一批敌机批号

	// 帧同步
	Start_Clock();

	// 刷黑后显示缓存
	DDraw_Fill_Surface(lpddsback, 0);

	// 读取键盘
	DInput_Read_Keyboard();

	// 捕获鼠标
	lpdimouse->Acquire();

	// 读取鼠标
	DInput_Read_Mouse();


	// 游戏故事背介绍
	if (g_nGameState == GAME_STATE_STORY)
	{
		StoryBegin();
		//g_nGameState = GAME_STATE_MENU;
		// 开场音乐
		if (DSound_Status_Sound(g_nSoundBegin) != DSBSTATUS_PLAYING)
			DSound_Play(g_nSoundBegin);

		
		if (keyboard_state[DIK_SPACE])
		{
			g_nGameState = GAME_STATE_RESET;
			DSound_Stop_Sound(g_nSoundBegin);

		}

		// flip the surfaces
		DDraw_Flip();

		// 每秒60帧
		Wait_Clock(FRAME_RATE);

		return;
	}
	// 游戏开始菜单
	else if (g_nGameState == GAME_STATE_MENU)
	{
		
		g_nGameState = GAME_STATE_RESET;
		//g_nGameState = GAME_STATE_LOAD;
		//g_nGameState = GAME_STATE_EXIT;

	}
	// 读取游戏
	else if (g_nGameState == GAME_STATE_LOAD)
	{

		g_nGameState = GAME_STATE_RUNNING;
	}
	// 退出游戏
	else if (g_nGameState == GAME_STATE_EXIT)
	{
		PostMessage(main_window_handle, WM_QUIT,0,0);

		// stop all sounds
		//DSound_Stop_All_Sounds();
	}
	// 暂停游戏显示菜单
	else if (g_nGameState == GAME_STATE_PAUSED)
	{
		//g_nGameState = GAME_STATE_RUNNING;
		//g_nGameState = GAME_STATE_RESET;
		//g_nGameState = GAME_STATE_EXIT;

		if (keyboard_state[DIK_P] && s_bPause == false)	// 暂停
		{
			s_bPause = true;
			g_nGameState = GAME_STATE_RUNNING;
		}
		if (!keyboard_state[DIK_P])
			s_bPause = false;





	}
	// 重新开始
	else if (g_nGameState == GAME_STATE_RESET)
	{
		WorldReset();
		PlyerReset();
		StartInitReset();
		PlasmaReset();
		BombReset();
		BurstReset();
		ParticleInitReset();
		PowerupReset();
		WraithReset();
		BeeReset();
		GunshipReset();
		g_nGameState = GAME_STATE_RUNNING;
	}
	// 运行游戏
	else if (g_nGameState == GAME_STATE_RUNNING)
	{
		static enemy = 1;
		// 背景音乐
		if (DSound_Status_Sound(g_nSoundMain) != DSBSTATUS_PLAYING)
		{
			DSound_Play(g_nSoundMain);
		}

		// 帧序号大于下一批敌机的帧序号 且有足够的未激活敌机时，出现下一批敌机
		if (s_unFrame >= g_Arm[s_nArm].unFrame 
			&& g_nWraith >= g_Arm[s_nArm].nWraithB + g_Arm[s_nArm].nWraithR
			&& g_nBee >= g_Arm[s_nArm].nBee
			&& g_nGunship >= g_Arm[s_nArm].nGunShip
			&& s_nArm<MAX_ARM)
		{
			EnemyStart(g_Arm[s_nArm].nWraithB, FIGHT_TYPE_WRAITHB);
			EnemyStart(g_Arm[s_nArm].nWraithR, FIGHT_TYPE_WRAITHR);
			EnemyStart(g_Arm[s_nArm].nBee, FIGHT_TYPE_BEE);
			EnemyStart(g_Arm[s_nArm].nGunShip, FIGHT_TYPE_GUNSHIP);
			s_nArm++;
		}

		// 游戏胜利判定
		if (s_nArm == MAX_ARM && g_nWraith +10 >= MAX_WRAITH
					&& g_nBee == MAX_BEE && g_nGunship == MAX_GUNSHIP)
		{
			g_nGameState = GAME_STATE_WIN;
		}

		// 绘制星星
		StarDraw();
		
		if (keyboard_state[DIK_P] && s_bPause == false)	// 暂停
		{
			s_bPause = true;
			g_nGameState = GAME_STATE_PAUSED;
			return;
		}
		if (!keyboard_state[DIK_P])
			s_bPause = false;
		//移动准星
		g_bobAim.x+= mouse_state.lX;
		g_bobAim.y+= mouse_state.lY;

		// 处理准星越界
		if (g_bobAim.x<0)
			g_bobAim.x = 0;
		else if (g_bobAim.x>WINDOW_WIDTH)
			g_bobAim.x = WINDOW_WIDTH;
		if (g_bobAim.y<0)
			g_bobAim.y = 0;
		else if (g_bobAim.y>WINDOW_HEIGHT)
			g_bobAim.y = WINDOW_HEIGHT;

		PlayerRefresh();
		
		// 向上
		if (keyboard_state[DIK_W] || keyboard_state[DIK_UP])
		{
			g_bobPlayer.yv = -g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		// 向下
		else if (keyboard_state[DIK_S] || keyboard_state[DIK_DOWN])
		{
			g_bobPlayer.yv = g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		else
		{
			g_bobPlayer.yv = 0;
		}
		// 向左
		if (keyboard_state[DIK_A] || keyboard_state[DIK_LEFT])
		{
			g_bobPlayer.xv = -g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		// 向左
		else if (keyboard_state[DIK_D] || keyboard_state[DIK_RIGHT])
		{
			g_bobPlayer.xv = g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		else
		{
			g_bobPlayer.xv = 0;
		}
		// 射击
		if (mouse_state.rgbButtons[0])
		{
			//控制射击频率
			if (g_bobPlayer.counter_1 == 0)
			{
				float x = g_bobPlayer.x+g_bobPlayer.width/2;
				float y = g_bobPlayer.y+g_bobPlayer.height/2;

				float dx = (g_bobAim.x + g_bobAim.width/2) - x;
				float dy = (g_bobAim.y + g_bobAim.height/2) - y;
				float len = sqrt(dx*dx + dy*dy);

				float xv = dx/len * g_bobPlayer.varsF[INDEXF_PLASMA_SPEED];
				float yv = dy/len * g_bobPlayer.varsF[INDEXF_PLASMA_SPEED];

				PlasmaFire(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2,
							g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2,
							xv, yv, &g_bobPlayer);
				// 设置攻击频率计数
				g_bobPlayer.counter_1 = g_bobPlayer.max_count_1;
			}// end if
			
		}// end mouse if

		// 扔弹
		if (mouse_state.rgbButtons[1] && s_bBomb == false)
		{
			s_bBomb = true;

			// 玩家是否还有炸弹
			if (g_bobPlayer.varsI[INDEXI_BOMBS] > 0)
			{
				g_bobPlayer.varsI[INDEXI_BOMBS]--;

				//计算速度 
				float x = g_bobPlayer.x+g_bobPlayer.width/2;
				float y = g_bobPlayer.y+g_bobPlayer.height/2;

				float dx = (g_bobAim.x + g_bobAim.width/2) - x;
				float dy = (g_bobAim.y + g_bobAim.height/2) - y;
				float len = sqrt(dx*dx + dy*dy);

				float xv = dx/len * g_bobPlayer.varsF[INDEXF_BOMB_SPEED];
				float yv = dy/len * g_bobPlayer.varsF[INDEXF_BOMB_SPEED];

				int nCount = len/g_bobPlayer.varsF[INDEXF_BOMB_SPEED]+0.5;
				
				if (nCount == 0)
					nCount = 1;
				BombFire(x, y, xv, yv, nCount, &g_bobPlayer);
			}
			else // 炸弹用完
				DSound_Play(g_nSoundBombOut);

		}// end mouse if
		if (!mouse_state.rgbButtons[1])
		{
			s_bBomb = false;
		}
		// 移动player
		PlayerMove();

		// 更新道具
		PowerupRefresh();

		// 移动弹炸弹 放在敌机移动之前，防止炸毁的敌机乱飞
		BombMove();

		// 移动敌机AB
		WraithMove();

		// 移动BEE
		BeeMove();

		// 移动敌机D
		GunshipMove();

		// 移动粒子
		ParticleMove();

		// 移动子弹
		PlasmaMove();	

		// 碰撞处理
		Collision();

		// 帧计数
		s_unFrame++;
	}

	// 移动爆炸
	BurstMove();

	// 绘制player
	PlayerDraw();

	// 绘制星空
	//StarDraw();

	// 绘制道具
	PowerupDraw();

	// 绘制敌机D
	GunshipDraw();

	// 绘制敌机AB
	WraithDraw();

	// 绘制Bee
	BeeDraw();

	// 绘制粒子
	ParticleDraw();		

	// 绘制子弹
	PlasmaDraw();

	// 绘制炸弹
	BombDraw();

	// 画爆炸
	BurstDraw();

	// 画地图
	MapDraw();

	// 画player信息
	PlayerInfo();

	// 画准星
	AimDraw();

	// 游戏失败
	if (g_nGameState == GAME_STATE_LOST)
	{
		TextDraw(150,200,50,50,TEXT_RED,"GAME");
		TextDraw(450,200,50,50,TEXT_RED,"OVER");
	}
	// 游戏胜利
	if (g_nGameState == GAME_STATE_WIN)
	{
		TextDraw(150,200,50,50,TEXT_RED,"YOU");
		TextDraw(450,200,50,50,TEXT_RED,"WIN");
	}

	// flip the surfaces
	DDraw_Flip();

	if (keyboard_state[DIK_ESCAPE])	// 菜单
	{
		g_nGameState = GAME_STATE_EXIT;
		return;
	}

	// 每秒60帧
	Wait_Clock(FRAME_RATE);

}// end GameMain

// 创建查找表
void CreateTables(void)
{
	int i=0;
	float fang;
	for (i=0; i < 16; i++)
	{
		fang = PI*(i*22.5)/180;

		g_fCosTab16[i] = -cos(fang+PI/2);
		g_fSinTab16[i] = -sin(fang+PI/2);

	} // end for i

	for (i=0; i<64; i++)
	{
		fang = i*PI/32;

		g_fCosTab64[i] = -cos(fang + PI/2);
		g_fSinTab64[i] = -sin(fang + PI/2);
	}

	for (i=0; i<4; i++)
	{
		g_fTanTab4[i] = tan((1+2*i)*PI/16);
	}
}// end CreateTable

// 读取声音
void LoadSoundMusic(void)
{
	// 开场音乐
	if ((g_nSoundBegin = DSound_Load_WAV("SOUND/storyBegin.WAV"))==-1)
		return;

	// 主音乐
	if ((g_nSoundMain = DSound_Load_WAV("SOUND/starsng.WAV"))==-1)
		return;

	// 移动
	if ((g_nSoundPlayerMove = DSound_Load_WAV("SOUND/engines0.WAV"))==-1)
		return;

	// 扔炸弹
	if ((g_nSoundBomb = DSound_Load_WAV("SOUND/plasma_shot.wav"))==-1)
		return;

	// 炸弹用完
	if ((g_nSoundBombOut = DSound_Load_WAV("SOUND/empty.wav"))==-1)
		return;

	// 爆炸
	if ((g_nSoundExpl[0] = DSound_Load_WAV("SOUND/EXPL1.WAV"))==-1)
		return;

	if ((g_nSoundExpl[1] = DSound_Load_WAV("SOUND/EXPL2.WAV"))==-1)
		return;

	// 复制爆炸
	for (int index=2; index < MAX_SOUND_EXPL; index++)
	{
		// replicate sound
		g_nSoundExpl[index] = DSound_Replicate_Sound(g_nSoundExpl[rand()%2]);  

	} // end for index

	// 子弹声音
	if ((g_nSoundFire[0] = DSound_Load_WAV("SOUND/PULSE.WAV"))==-1)
		return;

	// 子弹 copies
	for (index=1; index < MAX_SOUND_FIRE; index++)
	{
		// replicate sound
		g_nSoundFire[index] = DSound_Replicate_Sound(g_nSoundFire[0]);  
	} // end for index

}// end LoadSoundMusic

// 重置地图
inline void WorldReset(void)
{
	g_ptScreen.x = WINDOW_WIDTH/2;
	g_ptScreen.y = WINDOW_HEIGHT/2;
}// end WorldRese

// 画血条
void DrawBlood(int nLeft, int nTop, int nHeight, int nCur, int nMax)
{
	DDBLTFX ddbltfx; // Blt的参数
	RECT rectDest;	 // 目标区域

	if (nCur < 0)
		nCur = 0;

	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RGB16Bit(0,255,0); // 绿
	rectDest.left = nLeft;
	rectDest.top = nTop;
	rectDest.right = nLeft+nCur-1;
	rectDest.bottom = nTop + nHeight -1;
	// 绿色血条
	lpddsback->Blt(&rectDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);

	if (nMax - nCur > 0)
	{
		ddbltfx.dwFillColor = RGB16Bit(255,0,0); // 红
		rectDest.left = nLeft+nCur-1;
		rectDest.top = nTop;
		rectDest.right = nLeft+nMax-1;
		rectDest.bottom = nTop + nHeight -1;
		// 红色血条
		lpddsback->Blt(&rectDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);
	}// end if

}// end DrawBlood


// 初始化文字
void TextInit(void)
{
	// 读取图片
	Load_Bitmap_File(&bitmap16bit, "images/ASCII.bmp");
	for (int i=0; i<MAX_CHAR; i++)
	{
		g_lpddsGreenText[i] = DDraw_Create_Surface(12,12,SURFACE_MEMORY,0);
		LoadSurface(g_lpddsGreenText[i], &bitmap16bit,i%16,i/16,12,12,BITMAP_EXTRACT_MODE_CELL);
		g_lpddsRedText[i] = DDraw_Create_Surface(12,12,SURFACE_MEMORY,0);
		LoadSurface(g_lpddsRedText[i], &bitmap16bit,i%16,6+i/16,12,12,BITMAP_EXTRACT_MODE_CELL);
	}
	Unload_Bitmap_File(&bitmap16bit);
	
}// end TextInit

// 释放文字
void TextRelease(void)
{
	for (int i=0; i<MAX_CHAR; i++)
	{
		if (g_lpddsGreenText[i] != NULL)
			g_lpddsGreenText[i]->Release();
		if (g_lpddsRedText[i] != NULL)
			g_lpddsRedText[i]->Release();
	}
}// end TextRelease

void TextDraw(int x, int y, int width, int height, int color, TCHAR* szText)
{
	int i = 0;
	LPDIRECTDRAWSURFACE7* lppddsColor;

	if (color == TEXT_GREEN)
		lppddsColor = g_lpddsGreenText;
	else 
		lppddsColor = g_lpddsRedText;

	while(*(szText+i)!=NULL)
	{
		if (width != 12 || height != 12)
			BltSuerfaceScaled(lppddsColor[*(szText+i)-32],lpddsback,x+i*width,y,12,12,width,height);
		else
			BltSuerface(lppddsColor[*(szText+i)-32],lpddsback,x+i*width,y,width,height);
		i++;
	}// end while

}// end TextDraw

//初始化player
void PlayerInit(void)
{
	int i;  // for

	// 读取图片
	Load_Bitmap_File(&bitmap16bit, "images/player.bmp");

	// 创建BOB
	Create_BOB(&g_bobPlayer,320,240,48,36,18,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		SURFACE_MEMORY,0,16);

	// 将图片读取到帧
	for (i = 0; i <= 17; i++)
		Load_Frame_BOB16(&g_bobPlayer,&bitmap16bit,i,i%8,i/8,BITMAP_EXTRACT_MODE_CELL);

	// 释放图片
	Unload_Bitmap_File(&bitmap16bit);

	int nPlayerAnim[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};	// player动画序列
	// 加载动画帧序列
	Load_Animation_BOB(&g_bobPlayer, 0, 16, nPlayerAnim);
	// 设置当前动画
	Set_Animation_BOB(&g_bobPlayer, 0);


	// 设置动画速率
	Set_Anim_Speed_BOB(&g_bobPlayer,3);
	
}// end PlayerInit

// plyaer数据重置
void PlyerReset(void)
{
	// 
	g_bobPlayer.varsI[INDEXI_HEALTH] = PLAYER_MAX_HEALTH;
	g_bobPlayer.varsI[INDEXI_DAMAGE] = PLAYER_DAMAGE;
	g_bobPlayer.varsI[INDEXI_PLASMA_RANGE] = PLAYER_PLASMA_RANGE;
	g_bobPlayer.varsI[INDEXI_SCORE] = 0;
	g_bobPlayer.varsI[INDEXI_MOVESPEED] = PLAYER_MOVESPEED;
	g_bobPlayer.varsI[INDEXI_FIRESPEED] = PLAYER_FIRESPEED;
	g_bobPlayer.varsI[INDEXI_BOMBS] = PLAYER_BOMBS;
	g_bobPlayer.varsI[INDEXI_BOMB_RANGE] = BOMB_RANGE;
	g_bobPlayer.varsI[INDEXI_BOMB_DAMAGE] = BOMB_DAMAGE;
	g_bobPlayer.varsI[INDEXI_BOMB_PUSH] = BOMB_PUSH;
	g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP] = 0;
	g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP] = 0;
	g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE] = 0;

	// 玩家攻击频率
	g_bobPlayer.counter_1 = 0;
	g_bobPlayer.max_count_1 = g_bobPlayer.varsI[INDEXI_FIRESPEED];

	g_bobPlayer.varsF[INDEXF_WORLDX] = (WORLD_WIDTH-g_bobPlayer.width)/2;
	g_bobPlayer.varsF[INDEXF_WORLDY] = (WORLD_HEIGHT-g_bobPlayer.height)/2;
	g_bobPlayer.varsF[INDEXF_MASS] = PLAYER_MASS;
	g_bobPlayer.varsF[INDEXF_PLASMA_SPEED] = PLAYER_PLASMA_SPEED;
	g_bobPlayer.varsF[INDEXF_BOMB_SPEED] = PLAYER_BOMB_SPEED;

	g_bobPlayer.x = (WINDOW_WIDTH-g_bobPlayer.width)/2;
	g_bobPlayer.y = (WINDOW_HEIGHT-g_bobPlayer.height)/2;

	g_bobPlayer.state = PLAYER_STATE_ALIVE;
}// end PlayerReset

// 绘制player
void PlayerDraw(void)
{
	if ((g_bobPlayer.state & PLAYER_STATE_ALIVE) == PLAYER_STATE_ALIVE)
	{
		Draw_BOB16(&g_bobPlayer, lpddsback);
		Animate_BOB(&g_bobPlayer);
		// 如果player处于无敌状态，画防护罩
		if ((g_bobPlayer.state&PLAYER_STATE_INVINCIBLE)==PLAYER_STATE_INVINCIBLE)
		{
			RECT dest,source;
			int i=rand()&3;
			// 目标矩形
			dest.left = g_bobPlayer.x;
			dest.top = g_bobPlayer.y;
			dest.right = g_bobPlayer.x+g_bobPlayer.width;
			dest.bottom =g_bobPlayer.y+g_bobPlayer.height;
			// 源矩形
			source.left = 0;
			source.top = 0;
			source.right = g_bobPlayer.width;
			source.bottom = g_bobPlayer.height;

			lpddsback->Blt(&dest, g_bobPlayer.images[16+i],
				&source,(DDBLT_WAIT | DDBLT_KEYSRC),
				NULL);
		}
	}
}// end PlayerDraw

// 移动玩家
void PlayerMove(void)
{
	int dx=0, 
		  dy=0; //用于保存窗口移动量
	//移动player地图坐标
	g_bobPlayer.varsF[INDEXF_WORLDX] += g_bobPlayer.xv;
	g_bobPlayer.varsF[INDEXF_WORLDY] += g_bobPlayer.yv;
	// 得到视口坐标
	g_bobPlayer.x = g_bobPlayer.varsF[INDEXF_WORLDX]-g_ptScreen.x;
	g_bobPlayer.y = g_bobPlayer.varsF[INDEXF_WORLDY]-g_ptScreen.y;


	// player越界控制
	if (g_bobPlayer.varsF[INDEXF_WORLDX] > WORLD_WIDTH-g_bobPlayer.width)
	{
		g_bobPlayer.varsF[INDEXF_WORLDX] = WORLD_WIDTH-g_bobPlayer.width;
		g_bobPlayer.x = WINDOW_WIDTH-g_bobPlayer.width;
	}
	else if (g_bobPlayer.varsF[INDEXF_WORLDX] < 0)
	{
		g_bobPlayer.varsF[INDEXF_WORLDX] = 0;
		g_bobPlayer.x = 0;
	}
	if (g_bobPlayer.varsF[INDEXF_WORLDY] > WORLD_HEIGHT-g_bobPlayer.height)
	{
		g_bobPlayer.varsF[INDEXF_WORLDY] = WORLD_HEIGHT-g_bobPlayer.height;
		g_bobPlayer.y = WINDOW_HEIGHT-g_bobPlayer.height;
	}
	else if (g_bobPlayer.varsF[INDEXF_WORLDY] < 0)
	{
		g_bobPlayer.varsF[INDEXF_WORLDY] = 0;
		g_bobPlayer.y = 0;
	}

	// 移动显示窗口
	dx = 0;
	dy = 0;
	if (g_bobPlayer.x<SCREEN_MOVE.left) 
	{
		dx = g_bobPlayer.x - SCREEN_MOVE.left;
		g_ptScreen.x += dx;
	}
	else if(g_bobPlayer.x>SCREEN_MOVE.right)
	{
		dx = g_bobPlayer.x - SCREEN_MOVE.right;
		g_ptScreen.x += dx;
	}

	if (g_bobPlayer.y<SCREEN_MOVE.top)
	{
		dy = g_bobPlayer.y - SCREEN_MOVE.top;
		g_ptScreen.y += dy;
	}
	else if (g_bobPlayer.y>SCREEN_MOVE.bottom)
	{
		dy = g_bobPlayer.y -SCREEN_MOVE.bottom;
		g_ptScreen.y += dy;
	}

	
	// 显示窗口越界控制
	if (g_ptScreen.x < SCREEN_RANGE.left)
	{
		g_ptScreen.x = SCREEN_RANGE.left;
		dx = 0;
	}
	else if (g_ptScreen.x > SCREEN_RANGE.right)
	{
		g_ptScreen.x = SCREEN_RANGE.right;
		dx = 0;
	}
	if (g_ptScreen.y < SCREEN_RANGE.top)
	{
		g_ptScreen.y = SCREEN_RANGE.top;
		dy = 0;
	}
	else if (g_ptScreen.y > SCREEN_RANGE.bottom)
	{
		g_ptScreen.y = SCREEN_RANGE.bottom;
		dy = 0;
	}

	// 移动星星
	if (dx!=0 || dy!=0)
		StarMove(dx,dy);

	// 重新得到视口坐标
	g_bobPlayer.x = g_bobPlayer.varsF[INDEXF_WORLDX]-g_ptScreen.x;
	g_bobPlayer.y = g_bobPlayer.varsF[INDEXF_WORLDY]-g_ptScreen.y;

	//if (g_bobPlayer.xv != 0 || g_bobPlayer.yv != 0)
	//	if (DSound_Status_Sound(g_nSoundPlayerMove)==0)
	//		DSound_Play(g_nSoundPlayerMove);

}// end PlayerMove

// 玩家状态处理
void PlayerRefresh(void)
{
	// 还活着？
	if (g_bobPlayer.varsI[INDEXI_HEALTH]<=0)
	{
		// 死了
		g_bobPlayer.state = PLAYER_STATE_DEAD;
		g_nGameState = GAME_STATE_LOST;
		return;
		// 暂时不处理死亡
		//g_bobPlayer.varsI[INDEXI_HEALTH] = 0;
		

	}// end if health

	g_bobPlayer.max_count_1 = g_bobPlayer.varsI[INDEXI_FIRESPEED];
	g_bobPlayer.varsI[INDEXI_MOVESPEED] = PLAYER_MOVESPEED;

	// 移动加速状态
	if ((g_bobPlayer.state & PLAYER_STATE_MOVEUP) == PLAYER_STATE_MOVEUP)
	{
		g_bobPlayer.varsI[INDEXI_MOVESPEED] = POWERUP_MOVE;
		// 是否到期
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_MOVEUP;
	}// end moveup

	// 攻击加速状态
	if ((g_bobPlayer.state & PLAYER_STATE_FIREUP) == PLAYER_STATE_FIREUP)
	{
		g_bobPlayer.max_count_1 = POWERUP_FIREUP;
		// 是否到期
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_FIREUP;
	}// end fireup

	// 无敌状态
	if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) == PLAYER_STATE_INVINCIBLE)
	{
		// 是否到期
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_INVINCIBLE;
	}// end invincible

	// 更新射击频率计数 0 时为可射击
	if (g_bobPlayer.counter_1>0)
	{
		g_bobPlayer.counter_1--;
	}
}// end PlayerRefresh



// 显示玩家信息
void PlayerInfo(void)
{
	// 找一个可用的 g_bobPowerup
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			break;
	}
	// 血条图标
	Set_Pos_BOB(&g_bobPowerup[i],16,16);
	g_bobPowerup[i].curr_frame = POWERUP_TYPE_HEALTH;
	Draw_Scaled_BOB16(&g_bobPowerup[i], 16, 16, lpddsback);
	// 炸弹图标
	Set_Pos_BOB(&g_bobPowerup[i],16,569);
	g_bobPowerup[i].curr_frame = POWERUP_TYPE_BOMB;
	Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);

	int nTimeLen = 100;
	int nPower = 0;
	// 移动加速
	if ((g_bobPlayer.state & PLAYER_STATE_MOVEUP) == PLAYER_STATE_MOVEUP)
	{
		Set_Pos_BOB(&g_bobPowerup[i],664,491);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_MOVE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP]*nTimeLen/POWERUP_MOVE_TIME;
		DrawBlood(690,501,4,nPower,nTimeLen);
	}
	// 攻击加速
	if ((g_bobPlayer.state & PLAYER_STATE_FIREUP) == PLAYER_STATE_FIREUP)
	{
		Set_Pos_BOB(&g_bobPowerup[i],664,517);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_FIRE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP]*nTimeLen/POWERUP_FIREUP_TIME;
		DrawBlood(690,527,4,nPower,nTimeLen);
	}
	// 无敌
	if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) == PLAYER_STATE_INVINCIBLE)
	{
		Set_Pos_BOB(&g_bobPowerup[i], 664,543);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_INVINCIBLE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE]*nTimeLen/POWERUP_INVINCIBLE_TIME;
		DrawBlood(690,553,4,nPower,nTimeLen);
	}
	// 手
	if ((g_bobPlayer.state & PLAYER_STATE_HAND) == PLAYER_STATE_HAND)
	{
		Set_Pos_BOB(&g_bobPowerup[i], 688+72,569);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_HAND;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
	}

	// 血条
	DrawBlood(35,20,8,g_bobPlayer.varsI[INDEXI_HEALTH],PLAYER_MAX_HEALTH);

	TCHAR szBuffer[8] = "0000000";

	// 输出炸弹个数
	if (g_bobPlayer.varsI[INDEXI_BOMBS]>99)
		wsprintf(szBuffer,"%02d",99);
	else
		wsprintf(szBuffer,"%02d",g_bobPlayer.varsI[INDEXI_BOMBS]);
	TextDraw(40,575,12,12,TEXT_GREEN,szBuffer);	
	
	// 输出得分
	wsprintf(szBuffer,"%07d",g_bobPlayer.varsI[INDEXI_SCORE]);
	TextDraw(700, 16,12,12, TEXT_GREEN,szBuffer); 

	// 显示敌机生命值
	if (keyboard_state[DIK_LMENU])
	{
		for (i=1; i<NUM_COLLISION; i++)
		{
			if (g_pbobCollision[i]->state == 0)
				continue;
			int curLife = g_pbobCollision[i]->varsI[INDEXI_HEALTH]*g_pbobCollision[i]->width/g_pbobCollision[i]->varsI[INDEXI_MAXHEALTH];
			DrawBlood(g_pbobCollision[i]->x,g_pbobCollision[i]->y,4,
						curLife,	// 当前生命值
						g_pbobCollision[i]->width);	// 最大生命值
		}// end for i

	}
	
}// end PlayerInfo

// 释放玩家
inline void PlayerRelease(void)
{
	Destroy_BOB(&g_bobPlayer);
}// end PlayerRelease

// 初始化准星
inline void AimInit(void)
{
	// 读取图片
	Load_Bitmap_File(&bitmap16bit, "images/aim.bmp");

	// 创建BOB
	Create_BOB(&g_bobAim,WINDOW_WIDTH>>1,WINDOW_HEIGHT>>1,32,32,1,
		BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME,
		SURFACE_MEMORY,0,16);

	// 将图片读取到帧
	Load_Frame_BOB16(&g_bobAim,&bitmap16bit,0,0,0,BITMAP_EXTRACT_MODE_ABS);

	// 释放图片
	Unload_Bitmap_File(&bitmap16bit);
}// end AimInit

// 画准星
inline void AimDraw(void)
{
	Draw_BOB16(&g_bobAim, lpddsback);
}// end AimDraw

// 释放准星
inline void AimRelease(void)
{
	Destroy_BOB(&g_bobAim);
}// end AimRelease

// 初始化星星
void StartInitReset(void)
{
	int i=0;
	for (i=0; i<MAX_STARS; i++)
	{
		// 随机分布到3个平面
		switch(rand()&3) 
		{
		case STAR_PANEL0:
			g_Star[i].nPlane = STAR_PANEL0;
			g_Star[i].usColor = STAR_COLOR0;
			break;

		case STAR_PANEL1:
			g_Star[i].nPlane = STAR_PANEL1;
			g_Star[i].usColor = STAR_COLOR1;
			break;

		case STAR_PANEL2:
			g_Star[i].nPlane = STAR_PANEL2;
			g_Star[i].usColor = STAR_COLOR2;
			break;
		default:
			break;
		}// end switch

		g_Star[i].x = rand()%WINDOW_WIDTH;
		g_Star[i].y = rand()%WINDOW_HEIGHT;
	}//end for i
}// end StarInit

// 移动星星 在玩家移动之后调用
void StarMove(float dx, float dy)
{
	int i,
		nPanel0dx=0,
		nPanel0dy=0,
		nPanel1dx=0,
		nPanel1dy=0,
		nPanel2dx=0,
		nPanel2dy=0;
	// 各个平面星星的速度
	if (g_bobPlayer.x != 0 && g_bobPlayer.x != WINDOW_WIDTH - g_bobPlayer.width)
	{
		nPanel0dx = -dx - 0.5;
		nPanel1dx = -(dx)/2 - 0.5;
		nPanel2dx = -(dx)/3 - 0.5;
	}

	if (g_bobPlayer.y != 0 && g_bobPlayer.y != WINDOW_HEIGHT - g_bobPlayer.height)
	{
		nPanel0dy = -dy - 0.5;
		nPanel1dy = -(dy)/2 - 0.5;
		nPanel2dy = -(dy)/3 - 0.5;
	}
	
	for (i=0; i<MAX_STARS; i++)
	{
		switch (g_Star[i].nPlane)
		{
		case STAR_PANEL0:
			g_Star[i].x += nPanel0dx;
			g_Star[i].y += nPanel0dy;
			break;

		case STAR_PANEL1:
			g_Star[i].x += nPanel1dx;
			g_Star[i].y += nPanel1dy;
			break;

		case STAR_PANEL2:
			g_Star[i].x += nPanel2dx;
			g_Star[i].y += nPanel2dy;
			break;

		default:
			break;
		}// end switch


		// 越界处理
		if (g_Star[i].x > WINDOW_WIDTH)
			g_Star[i].x -= WINDOW_WIDTH;
		else if (g_Star[i].x < 0)
			g_Star[i].x += WINDOW_WIDTH;
		if (g_Star[i].y > WINDOW_HEIGHT)
			g_Star[i].y -= WINDOW_HEIGHT;
		else if (g_Star[i].y < 0)
			g_Star[i].y += WINDOW_HEIGHT;

	}//end for

}// end starMove

// 画星星
void StarDraw(void)
{
	// 锁定后缓存
	DDraw_Lock_Back_Surface();

	for (int i=0; i<MAX_STARS; i++)
	{
		// 在小地图内则不画
		if (g_Star[i].x > MAP_LEFT && g_Star[i].x < MAP_RIGHT+g_bobMap.width
			&& g_Star[i].y > MAP_TOP && g_Star[i].y < MAP_BOTTOM+g_bobMap.height)
			continue;
		Draw_Pixel16(g_Star[i].x, g_Star[i].y, g_Star[i].usColor, back_buffer,back_lpitch);
	}

	// 解锁
	DDraw_Unlock_Back_Surface();
}// end StarDraw

// 初始化子弹
void PlasmaInit(void)
{
	// 该函数初始化子弹

	// the plasma animations
	static int	/*PlasmaAnimPlayer[] = {0,1,2,3,4,5,6,7},*/
				PlasmaAnimPlayer[] = {24,25,26,27,28,29,30,31},
				PlasmaAnimWraith[] = {16,17,18,19,20,21,22,23},
				PlasmaAnimEnemy[]  = {8,9,10,11,12,13,14,15};

	// 读取子弹图片 
	Load_Bitmap_File(&bitmap16bit, "images/energyfull.bmp");

	// 创建子弹BOB对象
	Create_BOB(&g_bobPlasma[0],0,0,8,8,48,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		SURFACE_MEMORY,0,16);

	// 读取图片到帧
	for (int frame=0; frame < 48; frame++)
		Load_Frame_BOB16(&g_bobPlasma[0],&bitmap16bit,frame,frame%8,frame/8,BITMAP_EXTRACT_MODE_CELL);  

	// 设置帧速率
	Set_Anim_Speed_BOB(&g_bobPlasma[0],1);

	// 读取动画
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_PLAYER, 8, PlasmaAnimPlayer);
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_WRAITH, 8, PlasmaAnimEnemy);
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_GUNSHIP, 8, PlasmaAnimEnemy);

	// 未激活
	g_bobPlasma[0].state = PlASMA_STATE_OFF;

	// 复制MAX_PLASM-1个
	for (int i=1; i < MAX_PLASMA; i++)
	{
		Clone_BOB(&g_bobPlasma[0], &g_bobPlasma[i]);
	} // end for pulse

	// 释放位图缓存
	Unload_Bitmap_File(&bitmap16bit);
}// end PlasmaInit

// 释放子弹
void PlasmaRelease(void)
{
	for (int index=0; index < MAX_PLASMA; index++)
		Destroy_BOB(&g_bobPlasma[index]);
}// end PlasmaRelease

// 发射子弹
void PlasmaFire(int x, int y, float fVx, float fVy, BOB_PTR pbobSource)
{
	// 找一个可用的子弹BOB  x,y 源中心坐标
	for (int i=0; i < MAX_PLASMA; i++)
	{
		// OFF为可用
		if (g_bobPlasma[i].state == PlASMA_STATE_OFF)
		{
			// start this one up, note the use of world coords
			g_bobPlasma[i].varsF[INDEXF_WORLDX] = x-g_bobPlasma[i].width/2;
			g_bobPlasma[i].varsF[INDEXF_WORLDY] = y-g_bobPlasma[i].height/2;

			g_bobPlasma[i].xv = fVx;
			g_bobPlasma[i].yv = fVy;

			g_bobPlasma[i].varsI[INDEXI_PLASMA_DAMAGE] = pbobSource->varsI[INDEXI_DAMAGE];
			
			switch (pbobSource->varsI[INDEXI_FIGHT_TYPE])
			{
			case FIGHT_TYPE_PLAYER:
				Set_Animation_BOB(&g_bobPlasma[i],PLASMA_ANIM_PLAYER);	// 动画
				g_bobPlasma[i].max_count_1 = pbobSource->varsI[INDEXI_PLASMA_RANGE];	// 射程计数
				break;

			case FIGHT_TYPE_WRAITHB:
			case FIGHT_TYPE_WRAITHR:
			case FIGHT_TYPE_BEE:
				Set_Animation_BOB(&g_bobPlasma[i],PLASMA_ANIM_WRAITH);
				g_bobPlasma[i].max_count_1 = pbobSource->varsI[INDEXI_PLASMA_RANGE];
				break;

			case FIGHT_TYPE_GUNSHIP:
				Set_Animation_BOB(&g_bobPlasma[i],PLASMA_ANIM_GUNSHIP);
				g_bobPlasma[i].max_count_1 = pbobSource->varsI[INDEXI_PLASMA_RANGE];
				break;

			default:
				break;
			}// end switch

			g_bobPlasma[i].curr_frame = 0;
			g_bobPlasma[i].state =  PLASMA_STATE_ON;
			g_bobPlasma[i].counter_1 = 0; // used to track distance

			g_bobPlasma[i].anim_state = pbobSource->varsI[INDEXI_FIGHT_TYPE];


			// 声音
			for (int i=0; i < MAX_SOUND_FIRE; i++)
			{
				// test if this sound is playing
				if (DSound_Status_Sound(g_nSoundFire[i])==0)
				{
					DSound_Play(g_nSoundFire[i]);
					DSound_Set_Volume(g_nSoundFire[i], 50);
					break;
				} // end if

			} // end for i

			break;

		} // end if

	} // end for
}// end PlasmaFire

// 重置子弹
void PlasmaReset(void)
{
	for (int i = 0; i < MAX_PLASMA; i++)
	{
		g_bobPlasma[i].state = PlASMA_STATE_OFF;
	} // end for i
}// end PlasmaReset

// 移动子弹
void PlasmaMove(void)
{
	//状态判断和设置，命中检测，命中处理，爆炸处理
	int i=0,j=0;
	for (i=0; i<MAX_PLASMA; i++)
	{
		if (g_bobPlasma[i].state == PlASMA_STATE_OFF)
			continue;

		g_bobPlasma[i].varsF[INDEXF_WORLDX] += g_bobPlasma[i].xv;
		g_bobPlasma[i].varsF[INDEXF_WORLDY] += g_bobPlasma[i].yv;

		// 出界或 到期，即超出射程
		if (g_bobPlasma[i].varsF[INDEXF_WORLDX] > WORLD_WIDTH
			|| g_bobPlasma[i].varsF[INDEXF_WORLDX] < 0
			|| g_bobPlasma[i].varsF[INDEXF_WORLDY] > WORLD_HEIGHT
			|| g_bobPlasma[i].varsF[INDEXF_WORLDY] < 0
			|| g_bobPlasma[i].counter_1 > g_bobPlasma[i].max_count_1)
		{
			g_bobPlasma[i].state = PlASMA_STATE_OFF;
		}// end if

		for (j=1; j < NUM_COLLISION; j++)
		{
			// 如果不是player的子弹退出
			if (g_bobPlasma[i].anim_state != PLASMA_ANIM_PLAYER)
				break;

			// 如果敌机未激活跳过
			if (g_pbobCollision[j]->state != 1)
				continue;

			// test for collision 
			if (Collision_Test(g_pbobCollision[j]->varsF[INDEXF_WORLDX],g_pbobCollision[j]->varsF[INDEXF_WORLDY],
				g_pbobCollision[j]->width, g_pbobCollision[j]->height,
				g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
				g_bobPlasma[i].width, g_bobPlasma[i].height))
			{
				g_bobPlasma[i].state = PlASMA_STATE_OFF;

				// 减少敌机生命
				g_pbobCollision[j]->varsI[INDEXI_HEALTH] -= g_bobPlasma[i].varsI[INDEXI_PLASMA_DAMAGE];

				BurstStart(g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
					g_bobPlasma[i].width<<2, g_bobPlasma[i].height<<1,
					g_pbobCollision[j]->xv, g_pbobCollision[j]->yv);
				break; 
			}// end if collison
		}// end j for

	// 是否打中player
	// 如是player的弹 跳过
	if (g_bobPlasma[i].anim_state == PLASMA_ANIM_PLAYER)
		continue;
	if (Collision_Test(g_bobPlayer.varsF[INDEXF_WORLDX],g_bobPlayer.varsF[INDEXF_WORLDY],
		g_bobPlayer.width, g_bobPlayer.height,
		g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
		g_bobPlasma[i].width, g_bobPlasma[i].height))
	{
		g_bobPlasma[i].state = PlASMA_STATE_OFF;
		//爆炸
		BurstStart(g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
			g_bobPlasma[i].width<<2, g_bobPlasma[i].height<<1,
			g_bobPlayer.xv, g_bobPlayer.yv);
		// 不是无敌就去血
		if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) != PLAYER_STATE_INVINCIBLE)
			g_bobPlayer.varsI[INDEXI_HEALTH] -= g_bobPlasma[i].varsI[INDEXI_PLASMA_DAMAGE];

	}// end if collision player

		// 射程计数++
		g_bobPlasma[i].counter_1++;
	}//end for
}// end PlasmaMove

// 画子弹
void PlasmaDraw(void)
{
	for (int i=0; i<MAX_PLASMA; i++)
	{
		// test if plasma pulse is in flight
		if (g_bobPlasma[i].state == PLASMA_STATE_ON)
		{
			// transform to screen coords
			g_bobPlasma[i].x = g_bobPlasma[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobPlasma[i].y = g_bobPlasma[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			if (g_bobPlasma[i].x > 0 && g_bobPlasma[i].x < WINDOW_WIDTH
				&& g_bobPlasma[i].y > 0 && g_bobPlasma[i].y < WINDOW_HEIGHT)
			{
				// 子弹贴到后缓存
				Draw_BOB(&g_bobPlasma[i],lpddsback);

				// 更新当前帧
				Animate_BOB(&g_bobPlasma[i]);
			}
		} // end if

	} // end for i
}// end PlasmaDraw

// 初始化炸弹
void BombInit(void)
{
	// 该函数初始化炸弹弹
		

	// 读取炸弹图片 
	Load_Bitmap_File(&bitmap16bit, "images/bomb2.bmp");

	// 创建子弹BOB对象
	Create_BOB(&g_bobBomb[0],0,0,32,32,8,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// 读取图片到帧
	for (int frame=0; frame < 8; frame++)
		Load_Frame_BOB16(&g_bobBomb[0],&bitmap16bit,frame,frame%8,frame/8,BITMAP_EXTRACT_MODE_CELL);  

	// 设置帧速率
	Set_Anim_Speed_BOB(&g_bobBomb[0],12);

	// 未激活
	g_bobBomb[0].state = PlASMA_STATE_OFF;

	// 释放位图缓存
	Unload_Bitmap_File(&bitmap16bit);

	int nBomb[16] = {0,1,2,3,4,5,6,7};	// 动画序列
	// 加载动画帧序列
	Load_Animation_BOB(&g_bobBomb[0], 0, 16, nBomb);
	// 设置当前动画
	Set_Animation_BOB(&g_bobBomb[0], 0);

	// 复制MAX_PLASM-1个
	for (int i=1; i < MAX_BOMB; i++)
	{
		Clone_BOB(&g_bobBomb[0], &g_bobBomb[i]);
	} // end for pulse

}// end BombInit

void BombReset(void)	// 重置炸弹
{
	for (int i = 0; i < MAX_BOMB; i++)
	{
		g_bobBomb[i].state = PlASMA_STATE_OFF;
	} // end for i
}// end BombRese

void BombRelease(void)	// 释放炸弹
{
	for (int index=0; index < MAX_BOMB; index++)
		Destroy_BOB(&g_bobBomb[index]);
}// end BombRelease

void BombFire(int x, int y, float fVx, float fVy, int nCount,BOB_PTR pbobSource)	// 发射炸弹
{
	// 找一个可用的炸弹BOB
	for (int i=0; i < MAX_BOMB; i++)
	{
		// OFF为可用
		if (g_bobBomb[i].state == PlASMA_STATE_OFF)
		{
			// 坐标变换为地图坐标
			g_bobBomb[i].varsF[INDEXF_WORLDX] = x-g_bobBomb[i].width/2+g_ptScreen.x;
			g_bobBomb[i].varsF[INDEXF_WORLDY] = y-g_bobBomb[i].height/2+g_ptScreen.y;

			g_bobBomb[i].xv = fVx;
			g_bobBomb[i].yv = fVy;

			// 设置炸弹伤害，爆炸范围，爆炸点
			g_bobBomb[i].varsI[INDEXI_DAMAGE] = pbobSource->varsI[INDEXI_BOMB_DAMAGE];
			g_bobBomb[i].varsI[INDEXI_RANGE]  = pbobSource->varsI[INDEXI_BOMB_RANGE];
			g_bobBomb[i].varsI[INDEXI_PUSH]   = pbobSource->varsI[INDEXI_BOMB_PUSH];
			g_bobBomb[i].max_count_1 = nCount;

			g_bobBomb[i].curr_frame = 0;
			g_bobBomb[i].state =  PLASMA_STATE_ON;

			// 声音
			DSound_Play(g_nSoundBomb);
			DSound_Set_Volume(g_nSoundBomb, 90);

		} // end if

	} // end for
}// end BombFire

void BombMove(void)	// 移动炸弹
{
	int i,j;

	for (i=0; i<MAX_BOMB; i++)
	{
		if (g_bobBomb[i].state == PlASMA_STATE_OFF)
			continue;

		g_bobBomb[i].varsF[INDEXF_WORLDX] += g_bobBomb[i].xv;
		g_bobBomb[i].varsF[INDEXF_WORLDY] += g_bobBomb[i].yv;

		// 飞到目标?
		if (++g_bobBomb[i].counter_1 == g_bobBomb[i].max_count_1)
		{
			g_bobBomb[i].state = PlASMA_STATE_OFF;
			g_bobBomb[i].counter_1 = 0;
			// 引爆
			float fBombX = g_bobBomb[i].varsF[INDEXF_WORLDX]+g_bobBomb[i].width/2;
			float fBombY = g_bobBomb[i].varsF[INDEXF_WORLDY]+g_bobBomb[i].height/2;
			int nBrust =BurstStart(fBombX,fBombY,
					g_bobPlayer.varsI[INDEXI_BOMB_RANGE],g_bobPlayer.varsI[INDEXI_BOMB_RANGE],0,0);
			
			// 处理被炸到的敌机
			for (j=1; j<NUM_COLLISION; j++)
			{
				if (g_pbobCollision[j]->state == 0)
					continue;

				// 判断和player的距离,如在范围内，攻击
				int dx = (g_pbobCollision[j]->varsF[INDEXF_WORLDX]+g_pbobCollision[j]->width/2)-fBombX;
				int dy = (g_pbobCollision[j]->varsF[INDEXF_WORLDY]+g_pbobCollision[j]->height/2)-fBombY;

				int length = Fast_Distance_2D(dx,dy);	// 距离

				
				if (length <= g_bobPlayer.varsI[INDEXI_BOMB_RANGE]/2)
				{
					// 造成伤害
					g_pbobCollision[j]->varsI[INDEXI_HEALTH]-=g_bobPlayer.varsI[INDEXI_BOMB_DAMAGE];
				

					// 防止除数为0
					if (length<1)
						length = 1;

					float fPush = g_bobPlayer.varsI[INDEXI_BOMB_PUSH]/length;

					g_pbobCollision[j]->xv += fPush*dx/length;
					g_pbobCollision[j]->yv += fPush*dy/length;


				}// end if
			}// end for j

		}
		else if (g_bobBomb[i].varsF[INDEXF_WORLDX] > WORLD_WIDTH
			|| g_bobBomb[i].varsF[INDEXF_WORLDX] < 0
			|| g_bobBomb[i].varsF[INDEXF_WORLDY] > WORLD_HEIGHT
			|| g_bobBomb[i].varsF[INDEXF_WORLDY] < 0)
		{
			g_bobBomb[i].state = PlASMA_STATE_OFF;
			g_bobBomb[i].counter_1 = 0;
		}// end if
	}//end for
}// end BombMove

void BombDraw(void)	// 画炸弹
{
	for (int i=0; i<MAX_BOMB; i++)
	{
		// test if plasma pulse is in flight
		if (g_bobBomb[i].state == PLASMA_STATE_ON)
		{
			// 坐标变换
			g_bobBomb[i].x = g_bobBomb[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobBomb[i].y = g_bobBomb[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			if (g_bobBomb[i].x > 0 && g_bobBomb[i].x < WINDOW_WIDTH
				&& g_bobBomb[i].y > 0 && g_bobBomb[i].y < WINDOW_HEIGHT)
			{
				// 炸弹弹贴到后缓存
				Draw_BOB(&g_bobBomb[i],lpddsback);
				Animate_BOB(&g_bobBomb[i]);
			}
		} // end if

	} // end for i
}// end BombDraw

// 初始化爆炸
void BurstInit(void)
{
	// 初始化爆炸 

	// 读取图片
	Load_Bitmap_File(&bitmap16bit, "images/explBig.BMP");

	// 创建爆炸bob
	Create_BOB(&g_bobBrusts[0],0,0,256,219,14,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// 加载图片到帧
	for (int frame=0; frame < 14; frame++)
		Load_Frame_BOB16(&g_bobBrusts[0],&bitmap16bit,frame,frame%4,frame/4,BITMAP_EXTRACT_MODE_CELL);  

	// 动画速率
	Set_Anim_Speed_BOB(&g_bobBrusts[0],1);

	// 爆炸的大小，用于放缩
	g_bobBrusts[0].varsI[INDEXI_BURST_WIDTH] = g_bobBrusts[0].width;
	g_bobBrusts[0].varsI[INDEXI_BURST_HEIGHT] = g_bobBrusts[0].height;

	// 未激活
	g_bobBrusts[0].state = BURST_STATE_OFF;

	for (int burst=1; burst<MAX_BURSTS; burst++)
	{
		memcpy(&g_bobBrusts[burst], &g_bobBrusts[0], sizeof(BOB));
	} // end for burst

	// 释放图片
	Unload_Bitmap_File(&bitmap16bit);

}// end BrustsInit

// 释放爆炸
void BurstRelease(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
		Destroy_BOB(&g_bobBrusts[index]);
}// end BrustRelease

// 重置爆炸
void BurstReset(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
		g_bobBrusts[index].state = BURST_STATE_OFF;
}// end BurstReset

// 开始爆炸
int BurstStart(int x, int y, int width, int height, float xv,float yv)
{
	// 找一个可用的爆炸 x,y是爆炸的中心位置 返回值是被激活的爆炸的ID
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// is this burst available?
		if (g_bobBrusts[index].state == BURST_STATE_OFF)
		{
			// 初始化动画
			Set_Anim_Speed_BOB(&g_bobBrusts[index],1);
			g_bobBrusts[index].curr_frame = 0;

			g_bobBrusts[index].xv = xv;
			g_bobBrusts[index].yv = yv;

			// 大小用于缩放
			g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH] = width;
			g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT] = height;

			g_bobBrusts[index].varsF[INDEXF_WORLDX] = x-width/2;
			g_bobBrusts[index].varsF[INDEXF_WORLDY] = y-height/2;

			// 坐标变换
			g_bobBrusts[index].x = g_bobBrusts[index].varsF[INDEXF_WORLDX]-g_ptScreen.x;
			g_bobBrusts[index].y = g_bobBrusts[index].varsF[INDEXF_WORLDY]-g_ptScreen.y;

			// 激活
			g_bobBrusts[index].state = BURST_STATE_ON;

			ParticleStartExplosion(PARTICLE_TYPE_FLICKER,0,40+rand()%80,
							x,y,xv,yv,g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH]/6);

			// 爆炸声音
			for (int sound_index=0; sound_index < MAX_SOUND_EXPL; sound_index++)
			{
				// 该ID是否在使用
				if (DSound_Status_Sound(g_nSoundExpl[sound_index])==0)
				{
					DSound_Play(g_nSoundExpl[sound_index]);
					DSound_Set_Volume(g_nSoundExpl[sound_index], 50);
					break;
				} // end if

			} // end for sound_index
			return index;

		} // end if

	} // end for index
}// end BurstStart

// 移动爆炸
void BurstMove(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// 激活的才需移动
		if (g_bobBrusts[index].state == BURST_STATE_ON)
		{

			g_bobBrusts[index].varsF[INDEXF_WORLDX]+=g_bobBrusts[index].xv;
			g_bobBrusts[index].varsF[INDEXF_WORLDY]+=g_bobBrusts[index].yv;

			// test if burst is off screen or done with animation
			if (g_bobBrusts[index].curr_frame >= g_bobBrusts[index].num_frames-1) 
			{
				// kill burst and put back on available list
				g_bobBrusts[index].state = BURST_STATE_OFF;
			} // end if

		} // end if

	} // end for index
}// end BurstMove

// 画爆炸
void BurstDraw(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// test if bursts pulse is in flight
		if (g_bobBrusts[index].state == BURST_STATE_ON)
		{
			// 坐标变换
			g_bobBrusts[index].x = g_bobBrusts[index].varsF[INDEXF_WORLDX]-g_ptScreen.x;
			g_bobBrusts[index].y = g_bobBrusts[index].varsF[INDEXF_WORLDY]-g_ptScreen.y;

			// is scaling needed
			if (g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH]!=g_bobBrusts[index].width 
			|| g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT]!=g_bobBrusts[index].height)
			{
				//缩放
				Draw_Scaled_BOB(&g_bobBrusts[index],
					g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH],g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT],
					lpddsback);
			}
			else // 不缩放
				Draw_BOB(&g_bobBrusts[index],lpddsback);

			// 更新动画
			Animate_BOB(&g_bobBrusts[index]);

		} // end if

	} // end for index
}// BurstDraw

// 初始化粒子
void ParticleInitReset(void)
{
	for (int i=0; i<MAX_PARTICLES; i++)
	{
		ZeroMemory(&g_Particle[i], sizeof(PARTICLE));
		g_Particle[i].nState = PARTICLE_STATE_DEAD;
		g_Particle[i].nType = PARTICLE_TYPE_FADE;
	}//end for i
}// end ParticleInitRese

void ParticleStart(int nType, int nColor, int nTime, int x, int y, int vx, int vy)
{
	// 找一个未激活的粒子
	for (int index=0; index < MAX_PARTICLES; index++)
	{
		if (g_Particle[index].nState == PARTICLE_STATE_ALIVE)
			continue;  

		g_Particle[index].nState = PARTICLE_STATE_ALIVE;
		g_Particle[index].nType  = nType;
		g_Particle[index].x     = x;	// 地图坐标
		g_Particle[index].y     = y;
		g_Particle[index].xv    = vx;
		g_Particle[index].yv    = vy;
		g_Particle[index].nCounter     = 0;
		g_Particle[index].nMaxCount   = nTime;     

		// 设置颜色范围
		switch(nColor)
		{
		case PARTICLE_COLOR_RED:
			{
				g_Particle[index].nGreen = 0;
				g_Particle[index].nBlue = 0;
				if (nType == PARTICLE_TYPE_FLICKER)
					g_Particle[index].nRed = rand()%256;
				else
					g_Particle[index].nRed  = 255;
			} break;

		case PARTICLE_COLOR_GREEN:
			{
				g_Particle[index].nRed = 0;
				g_Particle[index].nBlue = 0;
				if (nType == PARTICLE_TYPE_FLICKER)
					g_Particle[index].nGreen = rand()%256;
				else
					g_Particle[index].nGreen  = 255;
	
			} break;

		case PARTICLE_COLOR_BLUE:
			{
				g_Particle[index].nGreen = 0;
				g_Particle[index].nRed = 0;
				if (nType == PARTICLE_TYPE_FLICKER)
					g_Particle[index].nBlue = rand()%256;
				else
					g_Particle[index].nBlue  = 31;
			} break;

		case PARTICLE_COLOR_WHITE:
			{
				if (nType == PARTICLE_TYPE_FLICKER)
				{
					int nWhite = rand()%256;
					g_Particle[index].nGreen = nWhite;
					g_Particle[index].nRed = nWhite;
					g_Particle[index].nBlue = nWhite;
				}
				else
				{
					g_Particle[index].nGreen = 255;
					g_Particle[index].nRed = 255;
					g_Particle[index].nBlue = 255;
				}
			} break;
		default:
			break;

		} // end switch

		g_Particle[index].usCurColor = RGB16Bit(g_Particle[index].nRed,g_Particle[index].nGreen,g_Particle[index].nBlue);
		break; // exit for
	}// end for
}// end ParticleStart

// 开始爆炸粒子 x,y 必须是地图坐标
void ParticleStartExplosion(int nType, int nColor, int nTime, int x, int y, int vx, int vy, int nParticles)
{
	while(--nParticles >=0)
	{
		// 随机角度
		int ang = rand()%64;
		// 随机速率
		float vel = 2+rand()%4;

		ParticleStart(nType,RAND_RANGE(PARTICLE_COLOR_RED,PARTICLE_COLOR_WHITE),nTime,
			x+RAND_RANGE(-4,4),y+RAND_RANGE(-4,4), 
			vx+g_fCosTab64[ang]*vel, vy+g_fSinTab64[ang]*vel);        

	} // end while

} // end Start_Particle_Explosion

// 移动粒子
void ParticleMove(void)
{
	for (int index=0; index<MAX_PARTICLES; index++)
	{
		if (g_Particle[index].nState == PARTICLE_STATE_ALIVE)
		{
			// 移动
			g_Particle[index].x+=g_Particle[index].xv;
			g_Particle[index].y+=g_Particle[index].yv;

			// 颜色处理
			if (g_Particle[index].nType==PARTICLE_TYPE_FLICKER)
			{
				g_Particle[index].nRed = g_Particle[index].nRed ? rand()%255 : 0;
				g_Particle[index].nGreen = g_Particle[index].nGreen ? rand()%255 : 0;
				g_Particle[index].nBlue = g_Particle[index].nBlue ? rand()%255 : 0;

				g_Particle[index].usCurColor = 
							RGB16Bit(g_Particle[index].nRed,g_Particle[index].nGreen,g_Particle[index].nBlue);

				// now update counter
				if (++g_Particle[index].nCounter >= g_Particle[index].nMaxCount)
					g_Particle[index].nState = PARTICLE_STATE_DEAD;             

			} // end if
			else if (g_Particle[index].nType == PARTICLE_TYPE_FADE)
			{
				if (++g_Particle[index].nCounter >= g_Particle[index].nMaxCount)
				{
					// reset counter
					g_Particle[index].nCounter = 0;

					g_Particle[index].nRed -= 16;
					g_Particle[index].nGreen -= 16;
					g_Particle[index].nBlue -= 16;
					// update color
					if (g_Particle[index].nRed < 0
						&& g_Particle[index].nGreen < 0
						&& g_Particle[index].nBlue < 0)
					{
						g_Particle[index].nState = PARTICLE_STATE_DEAD;
						continue;
					} // end if
					else 
					{
						if (g_Particle[index].nRed < 0)
							g_Particle[index].nRed+=16;
						if (g_Particle[index].nGreen < 0)
							g_Particle[index].nGreen+=16;
						if (g_Particle[index].nBlue < 0)
							g_Particle[index].nBlue += 16;
					}// end else
					g_Particle[index].usCurColor = 
						RGB16Bit(g_Particle[index].nRed,g_Particle[index].nGreen,g_Particle[index].nBlue);

				} // end if

			} // end else

		} // end if 

	} // end for index
}// end ParticleMove

// 画粒子
void ParticleDraw(void)
{
	// lock back surface
	DDraw_Lock_Back_Surface();

	for (int index=0; index<MAX_PARTICLES; index++)
	{
		// test if particle is alive
		if (g_Particle[index].nState==PARTICLE_STATE_ALIVE)
		{
			// 坐标变换
			int x = g_Particle[index].x - g_ptScreen.x;
			int y = g_Particle[index].y - g_ptScreen.y;

			// test for clip
			if (x >= WINDOW_WIDTH || x < 0 || y >= WINDOW_HEIGHT || y < 0)
				continue;

			// draw the pixel
			Draw_Pixel16(x+1,y+1,g_Particle[index].usCurColor, back_buffer, back_lpitch);
			Draw_Pixel16(x,y,g_Particle[index].usCurColor, back_buffer, back_lpitch);
			Draw_Pixel16(x+1,y,g_Particle[index].usCurColor, back_buffer, back_lpitch);
			Draw_Pixel16(x,y+1,g_Particle[index].usCurColor, back_buffer, back_lpitch);

		} // end if

	} // end for index

	// unlock the secondary surface
	DDraw_Unlock_Back_Surface();
}// end ParticleDraw

// 初始化道具
void PowerupInit(void)
{
	int index;  // looping var

	// create the first bob
	Create_BOB(&g_bobPowerup[0],0,0,32,32,6,
		BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME,
		SURFACE_MEMORY,0,16);

	Load_Bitmap_File(&bitmap16bit, "Images/powerups.bmp");		

	for (index=0; index<6; index++)
		Load_Frame_BOB16(&g_bobPowerup[0],&bitmap16bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);  

	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);


	// set state to off
	g_bobPowerup[0].state = POWERUP_STATE_DEAD;

	g_bobPowerup[0].max_count_1 = POWERUP_LIVETIME;	// 设置生存时间
	g_bobPowerup[0].max_count_2 = POWERUP_HAND_GET;	// 用手拿的时间

	// make copies
	for (index=1; index < MAX_POWERUPS; index++)
	{
		Clone_BOB(&g_bobPowerup[0], &g_bobPowerup[index]);
	} // end for index
}//end PowerupInit

// 重置道具
void PowerupReset(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
		g_bobPowerup[i].state = POWERUP_STATE_DEAD;
}// end PowerupRese

// 释放道具
void PowerupRelease(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
		Destroy_BOB(&g_bobPowerup[i]);
}// end PowerupRelease

// 掉出道具
void PowerupStart(float x,float y)
{
	int nType = rand()%200; // 产生随机数，决定生成哪种类型道具或不生成

	if(nType >= HEALTH_START && nType <= HEALTH_END)
	{
		nType = POWERUP_TYPE_HEALTH;
	}
	else if(nType >= MOVE_START && nType <= MOVE_END)
	{
		nType = POWERUP_TYPE_MOVE;
	}
	else if(nType >= BOMB_START && nType <= BOMB_END)
	{
		nType = POWERUP_TYPE_BOMB;
	}
	else if(nType >= FIRE_START && nType <= FIRE_END)
	{
		nType = POWERUP_TYPE_FIRE;
	}
	else if(nType >= INVINCIBLE_START && nType <= INVINCIBLE_END)
	{
		nType = POWERUP_TYPE_INVINCIBLE;
	}
	else if(nType >= HAND_START && nType <= HAND_END)
	{
		nType = POWERUP_TYPE_HAND;
	}
	else	// 不产生
		return;

	for (int i=0; i<MAX_POWERUPS; i++)
	{
		if (g_bobPowerup[i].state == POWERUP_STATE_ALIVE)
			continue;

		g_bobPowerup[i].state = POWERUP_STATE_ALIVE;
		g_bobPowerup[i].varsI[INDEXI_POWERUP_TYPE] = nType;
	
		g_bobPowerup[i].curr_frame = nType;	// 设置要画的图片

		g_bobPowerup[i].varsF[INDEXF_WORLDX] = x - g_bobPowerup[i].width/2;
		g_bobPowerup[i].varsF[INDEXF_WORLDY] = y - g_bobPowerup[i].height/2;

		g_bobPowerup[i].counter_1 = 0;
		g_bobPowerup[i].counter_2 = 0;
		break;

	} // end for
	
}// end PowerupRelease

// 画道具
void PowerupDraw(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		// 跳过未激活的
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			continue;
		// 坐标变换
		g_bobPowerup[i].x = g_bobPowerup[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
		g_bobPowerup[i].y = g_bobPowerup[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

		Draw_BOB16(&g_bobPowerup[i], lpddsback);

	}// end for
}// end PowerupDraw

// 更新道具
void PowerupRefresh(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		// 跳过未激活的
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			continue;

		// 坐标标变换
		g_bobPowerup[i].x = g_bobPowerup[i].varsF[INDEXF_WORLDX]-g_ptScreen.x;
		g_bobPowerup[i].y = g_bobPowerup[i].varsF[INDEXF_WORLDY]-g_ptScreen.y;

		// 判断是否和玩家碰撞 或 被手拿
		if (Collision_BOBS(&g_bobPowerup[i], &g_bobPlayer))
		{
			PowerupBeGeted(&g_bobPowerup[i]);
		}// end if
		else if (Collision_BOBS(&g_bobPowerup[i], &g_bobAim)
			&& ((g_bobPlayer.state & PLAYER_STATE_HAND)==PLAYER_STATE_HAND))
		{
			if (++g_bobPowerup[i].counter_2 >=g_bobPowerup[i].max_count_2)
				PowerupBeGeted(&g_bobPowerup[i]);
		}
		else
		{
			g_bobPowerup[i].counter_2 = 0;
		}
 
		

		// 是否超过生存期
		if (++g_bobPowerup[i].counter_1 >= g_bobPowerup[i].max_count_1)
			g_bobPowerup[i].state = POWERUP_STATE_DEAD;
		

	}// end for
}// end PowerupRefresh

// 道具被拿
void PowerupBeGeted(BOB_PTR bobBeGeted)
{
	switch (bobBeGeted->varsI[INDEXI_POWERUP_TYPE])
	{
	case POWERUP_TYPE_HEALTH:	// 回血
		g_bobPlayer.varsI[INDEXI_HEALTH]+=POWERUP_HEALTH;
		if (g_bobPlayer.varsI[INDEXI_HEALTH]>PLAYER_MAX_HEALTH)
			g_bobPlayer.varsI[INDEXI_HEALTH] = PLAYER_MAX_HEALTH;
		break;

	case POWERUP_TYPE_MOVE:		// 移动加速
		g_bobPlayer.state |= PLAYER_STATE_MOVEUP;
		g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP] = POWERUP_MOVE_TIME;
		break;

	case POWERUP_TYPE_BOMB:		// 炸弹
		g_bobPlayer.varsI[INDEXI_BOMBS]++;
		break;

	case POWERUP_TYPE_FIRE:
		g_bobPlayer.state |= PLAYER_STATE_FIREUP;
		g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP] = POWERUP_FIREUP_TIME;
		break;

	case POWERUP_TYPE_INVINCIBLE:
		g_bobPlayer.state |= PLAYER_STATE_INVINCIBLE;
		g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE] = POWERUP_INVINCIBLE_TIME;
		break;

	case POWERUP_TYPE_HAND:
		g_bobPlayer.state |= PLAYER_STATE_HAND;
		break;

	default:
		break;
	}// end switch

	bobBeGeted->state = POWERUP_STATE_DEAD;

}// end PowerupBeGet

// 从位图结构读到表面
int LoadSurface(LPDIRECTDRAWSURFACE7 lpdds, // 目标表面
				BITMAP_FILE_PTR bitmap, // 源位图
				int cx,int cy,   // cell or absolute pos. to scan image from
				int width, int height,int mode)        
{
	// 从位图读取到表面

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	// 表面不可用
	if (!lpdds)
		return 0;

	USHORT *source_ptr,   // working pointers
		*dest_ptr;

	if (mode==BITMAP_EXTRACT_MODE_CELL)
	{
		// re-compute x,y
		cx = cx*(width+1) + 1;
		cy = cy*(height+1) + 1;
	} // end if



	// 源指针
	source_ptr = (USHORT*)bitmap->buffer + cy*bitmap->bitmapinfoheader.biWidth+cx;

	// 获得表面地址
	// set size of the structure
	ddsd.dwSize = sizeof(ddsd);

	// lock the display surface
	lpdds->Lock(NULL,
		&ddsd,
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);

	// assign a pointer to the memory surface for manipulation
	dest_ptr = (USHORT *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int index_y=0; index_y<height; index_y++)
	{
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr,width*2);

		// advance pointers
		dest_ptr   += (ddsd.lPitch>>1);
		source_ptr += bitmap->bitmapinfoheader.biWidth;
	} // end for index_y

	// unlock the surface 
	lpdds->Unlock(NULL);

	return 1;

} // end Load_Surface

// 将一个表面BLT到另一个表面上
int BltSuerface(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // 在目标表面的位置
				int nSrcWidth, int nSrcHeight) // 源大小
{
	// 将一个表面绘制到另一个表面上

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             

	// is this a valid LPDIRECTDRAWSURFACE7
	if (!src)
		return 1;

	// fill in the destination rect
	dest_rect.left   = nDestX;
	dest_rect.top    = nDestY;
	dest_rect.right  = nDestX+nSrcWidth;
	dest_rect.bottom = nDestY+nSrcHeight;

	// fill in the source rect
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = nSrcWidth;
	source_rect.bottom  = nSrcHeight;

	// blt to destination surface
	if (FAILED(dest->Blt(&dest_rect, src,
		&source_rect,(DDBLT_WAIT | DDBLT_KEYSRC),
		NULL)))
		return 0;

	return 1;
} // end BltSuerface

// 将一个表面BLT到另一个表面上 并缩放
int BltSuerfaceScaled(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // 在目标表面的位置
				int nSrcWidth, int nSrcHeight, // 源大小
				int nDestWidth, int nDestHeight) // 目标大小
{
	// 将一个表面绘制到另一个表面上

	RECT dest_rect,   // the destination rectangle
		source_rect; // the source rectangle                             

	// is this a valid LPDIRECTDRAWSURFACE7
	if (!src)
		return 1;

	// fill in the destination rect
	dest_rect.left   = nDestX;
	dest_rect.top    = nDestY;
	dest_rect.right  = nDestX+nDestWidth;
	dest_rect.bottom = nDestY+nDestWidth;

	// fill in the source rect
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = nSrcWidth;
	source_rect.bottom  = nSrcHeight;

	// blt to destination surface
	if (FAILED(dest->Blt(&dest_rect, src,
		&source_rect,(DDBLT_WAIT | DDBLT_KEYSRC),
		NULL)))
		return 0;

	return 1;
} // end BltSuerface


// 初始化Wraith
void WraithInit(void)
{
	int i=0;

	// 创建BOB
	Create_BOB(&g_bobWraith[0], 0, 0, 64, 64, 64,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// 读取图片
	Load_Bitmap_File(&bitmap16bit,"Images/wraithb.bmp");
	// 加载图片到帧 蓝色Wraith
	for (i=0; i<32; i++)
	{
		Load_Frame_BOB16(&g_bobWraith[0], &bitmap16bit, i, i%8, i/8, BITMAP_EXTRACT_MODE_CELL);
	}// end for
	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);

	// 读取图片
	Load_Bitmap_File(&bitmap16bit,"Images/wraithr.bmp");
	// 加载图片到帧 红色Wraith
	for (i=0; i<32; i++)
	{
		Load_Frame_BOB16(&g_bobWraith[0], &bitmap16bit, 32+i, i%8, i/8,BITMAP_EXTRACT_MODE_CELL);
	}// end for
	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);

	g_bobWraith[0].state = ENEMY_STATE_DEAD;

	for (i=1; i<MAX_WRAITH; i++)
	{
		Clone_BOB(&g_bobWraith[0], &g_bobWraith[i]);
	}

}// end WraithInit

// 释放wraith
void WraithRelease(void)
{
	for (int i=0; i<MAX_WRAITH; i++)
	{
		Destroy_BOB(&g_bobWraith[i]);
	}
}// end WraithRelease

// 移动Wraith
void WraithReset(void)
{
	for (int i=0; i<MAX_WRAITH; i++)
	{
		g_bobWraith[i].state = ENEMY_STATE_DEAD;
	}
}// end WraithReset

// 出现Wraith
void EnemyStart(int nNum, int nType)
{
	float fX, fY;
	int iang=0,i=0;
	if (nType == FIGHT_TYPE_WRAITHB )
	{
		for (i=0; i<MAX_WRAITH;i++)
		{
			if (g_bobWraith[i].state == ENEMY_STATE_ALIVE)
				continue;

			if (nNum-- <= 0)
				break;

			// 生成随机位置
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fSinTab64[iang];

			g_bobWraith[i].state = ENEMY_STATE_ALIVE;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobWraith[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobWraith[i].counter_1 = 0;	// 攻击频率计数
			g_bobWraith[i].max_count_1 = FIRECOUNT_WRAITHB;

			g_bobWraith[i].varsI[INDEXI_HEALTH] = HEALTH_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_DAMAGE] = DAMAGE_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_PLASMA_RANGE] = RANGE_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_SCORE] = SCORE_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = FIGHT_TYPE_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_DIR] = DIR_NNNN;
			g_bobWraith[i].varsI[INDEXI_DIRPRE] = DIR_NNNN;
			g_bobWraith[i].varsI[INDEXI_ATTACK_RANGE] = ATTACK_RANGE_WRAITHB;
			g_bobWraith[i].varsI[INDEXI_MAXHEALTH] = HEALTH_WRAITHB;

			g_bobWraith[i].varsF[INDEXF_WORLDX] = fX;
			g_bobWraith[i].varsF[INDEXF_WORLDY] = fY;
			g_bobWraith[i].varsF[INDEXF_MASS] = MASS_WRAITHB;
			g_bobWraith[i].varsF[INDEXF_PLASMA_SPEED] = WRAITHB_PLASMA_SPEED;
			g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED] = MOVESPEED_WRAITHB*MOVESPEED_WRAITHB;
			g_bobWraith[i].varsF[INDEXF_ACC] = ACC_WRAITHB;

			--g_nWraith;// 未激活wraith数-1
		}// end for i
		0;

	}// end if wraith
	else if (nType ==  FIGHT_TYPE_WRAITHR)
	{for (i=0; i<MAX_WRAITH;i++)
		{
			if (g_bobWraith[i].state == ENEMY_STATE_ALIVE)
				continue;

			if (nNum-- <= 0)
				break;

			// 生成随机位置
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fSinTab64[iang];

			g_bobWraith[i].state = ENEMY_STATE_ALIVE;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobWraith[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobWraith[i].counter_1 = 0;	//攻击频率计数
			g_bobWraith[i].max_count_1 = FIRECOUNT_WRAITHR;
			//g_bobWraith[i].counter_1 = 0;	// 跟踪方向判断频率
			//g_bobWraith[i].max_count_2 = TRACK_WRAITHB;

			g_bobWraith[i].varsI[INDEXI_HEALTH] = HEALTH_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_DAMAGE] = DAMAGE_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_PLASMA_RANGE] = RANGE_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_SCORE] = SCORE_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = FIGHT_TYPE_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_DIR] = DIR_NNNN;
			g_bobWraith[i].varsI[INDEXI_DIRPRE] = DIR_NNNN;
			g_bobWraith[i].varsI[INDEXI_ATTACK_RANGE] = ATTACK_RANGE_WRAITHR;
			g_bobWraith[i].varsI[INDEXI_MAXHEALTH] = HEALTH_WRAITHR;

			g_bobWraith[i].varsF[INDEXF_WORLDX] = fX;
			g_bobWraith[i].varsF[INDEXF_WORLDY] = fY;
			g_bobWraith[i].varsF[INDEXF_MASS] = MASS_WRAITHR;
			g_bobWraith[i].varsF[INDEXF_PLASMA_SPEED] = WRAITHR_PLASMA_SPEED;
			g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED] = MOVESPEED_WRAITHR*MOVESPEED_WRAITHR;
			g_bobWraith[i].varsF[INDEXF_ACC] = ACC_WRAITHR;

			--g_nWraith;// 未激活wraith数-1
		}// end for i

	}// end if wraith
	else if (nType == FIGHT_TYPE_BEE)
	{
		for (i=0; i<MAX_BEE; i++)
		{
			if (g_bobBee[i].state == ENEMY_STATE_ALIVE)
				continue;

			if (nNum-- <= 0)
				return;

			// 生成随机位置
			iang = rand()%64;
			fX = WORLD_WIDTH*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(WINDOW_HEIGHT,WINDOW_HEIGHT+WINDOW_HEIGHT>>1)*g_fSinTab64[iang];

			g_bobBee[i].state = ENEMY_STATE_ALIVE;
			g_bobBee[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobBee[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobBee[i].counter_1 = 0;	//攻击频率计数
			g_bobBee[i].max_count_1 = FIRECOUNT_BEE;

			g_bobBee[i].varsI[INDEXI_HEALTH] = HEALTH_BEE;
			g_bobBee[i].varsI[INDEXI_DAMAGE] = DAMAGE_BEE;
			g_bobBee[i].varsI[INDEXI_PLASMA_RANGE] = RANGE_BEE;
			g_bobBee[i].varsI[INDEXI_SCORE] = SCORE_BEE;
			g_bobBee[i].varsI[INDEXI_DIR] = DIR_BEE_EEEE;
			g_bobBee[i].varsI[INDEXI_DIRPRE] = DIR_BEE_EEEE;
			g_bobBee[i].varsI[INDEXI_ATTACK_RANGE] = ATTACK_RANGE_BEE;
			g_bobBee[i].varsI[INDEXI_MAXHEALTH] = HEALTH_BEE;

			g_bobBee[i].varsF[INDEXF_WORLDX] = fX;
			g_bobBee[i].varsF[INDEXF_WORLDY] = fY;
			g_bobBee[i].varsF[INDEXF_MASS] = MASS_BEE;
			g_bobBee[i].varsF[INDEXF_PLASMA_SPEED] = BEE_PLASMA_SPEED;
			g_bobBee[i].varsF[INDEXF_MAX_MOVESPEED] = MOVESPEED_BEE*MOVESPEED_BEE;
			g_bobBee[i].varsF[INDEXF_ACC] = ACC_BEE;

			--g_nBee;// 未激活wraith数-1

		}// end for bee

	}// end else bee
	else if (nType == FIGHT_TYPE_GUNSHIP)
	{
		for (i=0; i<MAX_GUNSHIP; i++)
		{
			if (g_bobGunship[i].state == ENEMY_STATE_ALIVE)
				continue;

			if (nNum-- <= 0)
				return;

			// 生成随机位置
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST);
			fY = g_bobPlayer.varsF[INDEXF_WORLDX]+RAND_RANGE(MIN_DIST,MAX_DIST);

			g_bobGunship[i].state = ENEMY_STATE_ALIVE;
			g_bobGunship[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobGunship[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobGunship[i].counter_1 = 0;	//攻击频率计数
			g_bobGunship[i].max_count_1 = FIRECOUNT_GUNSHIP;

			g_bobGunship[i].varsI[INDEXI_HEALTH] = HEALTH_GUNSHIP;
			g_bobGunship[i].varsI[INDEXI_DAMAGE] = DAMAGE_GUNSHIP;
			g_bobGunship[i].varsI[INDEXI_PLASMA_RANGE] = RANGE_GUNSHIP;
			g_bobGunship[i].varsI[INDEXI_SCORE] = SCORE_GUNSHIP;
			g_bobGunship[i].varsI[INDEXI_DIR] = DIR_NNNN;
			g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] = DIR_RIGHT;
			g_bobGunship[i].varsI[INDEXI_ATTACK_RANGE] = ATTACK_RANGE_GUNSHIP;
			g_bobGunship[i].varsI[INDEXI_MAXHEALTH] = HEALTH_GUNSHIP;

			g_bobGunship[i].varsF[INDEXF_WORLDX] = fX;
			g_bobGunship[i].varsF[INDEXF_WORLDY] = fY;
			g_bobGunship[i].varsF[INDEXF_MASS] = MASS_GUNSHIP;
			g_bobGunship[i].varsF[INDEXF_PLASMA_SPEED] = GUNSHIP_PLASMA_SPEED;
			g_bobGunship[i].varsF[INDEXF_MAX_MOVESPEED] = MOVESPEED_GUNSHIPX;
			g_bobGunship[i].varsF[INDEXF_MOVESPEED] = MOVESPEED_GUNSHIPY;
			g_bobGunship[i].varsF[INDEXF_ACC] = ACC_GUNSHIP;
			g_bobGunship[i].varsF[INDEXF_ACCY] = ACC_GUNSHIPY;

			--g_nGunship;// 未激活wraith数-1

		}// end for gunship

	}// end else gunship

}// end EnemyStart

void WraithMove(void)
{
	int i=0,nRand=0;
	for (i=0; i<MAX_WRAITH; i++)
	{
		if (g_bobWraith[i].state == ENEMY_STATE_DEAD)
			continue;

		// 加速
		//g_bobWraith[i].varsF[INDEXF_MOVESPEED]+= g_bobWraith[i].varsF[INDEXF_ACC];
		// 是否超速
		//if (g_bobWraith[i].varsF[INDEXF_MOVESPEED] > g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
		//	g_bobWraith[i].varsF[INDEXF_MOVESPEED] = g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED];

		if (g_bobWraith[i].state == ENEMY_STATE_ALIVE)
		{
			// 重新计数 方向判断频率
			//g_bobWraith[i].counter_2 = g_bobWraith[i].max_count_2;

			// 判断敌机死活
			if (g_bobWraith[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobWraith[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE]+=g_bobWraith[i].varsI[INDEXI_SCORE];
				continue;
			}

			//保存前一个方向
			g_bobWraith[i].varsI[INDEXI_DIRPRE] = g_bobWraith[i].varsI[INDEXI_DIR];

			float dydx,dx,dy;
			int tmpDir; // 目标方向

			dx = (g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 判断方向
			if( dx == 0 && dy > 0)				// 两点在直线上，且W在下面
				tmpDir = DIR_NNNN;
			else if( dx ==0 && dy < 0)			// 两点在直线上，且W在上面
				tmpDir = DIR_SSSS;
			else if( dx > 0 && dy == 0)			// 两点在直线上，且W在右面
				tmpDir = DIR_WWWW;
			else if( dx < 0 && dy == 0)			// 两点在直线上，且W在左面
				tmpDir = DIR_EEEE;
			else if( dx > 0 && dy > 0)			// 左上象限,根据dy,dx的比值确定角度范围,利用tan(x)的单调性
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_WWWN;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_WWNN;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_WNNN;
				else
					tmpDir = DIR_NNNN;
			}
			else if( dx >0 && dy < 0)			// 左下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_SWWW;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_SSWW;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_SSSW;
				else
					tmpDir = DIR_SSSS;
			}
			else if( dx < 0 && dy < 0)			// 右下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_EEES;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_EESS;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_ESSS;
				else
					tmpDir = DIR_SSSS;
			}
			else								// 右上象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_NEEE;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_NNEE;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_NNNE;
				else
					tmpDir = DIR_NNNN;
			}// end else


			g_bobWraith[i].varsI[INDEXI_DIR] = tmpDir;
			g_bobWraith[i].curr_frame = g_bobWraith[i].varsI[INDEXI_DIR];
			tmpDir -= g_bobWraith[i].varsI[INDEXI_DIRPRE];

			// 转向速率损失
			g_bobWraith[i].varsF[INDEXF_MOVESPEED] = g_fCosTab64[16+tmpDir*4]*g_bobWraith[i].varsF[INDEXF_MOVESPEED];

		
		
			// 更新速度
			//g_bobWraith[i].xv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			//g_bobWraith[i].yv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			g_bobWraith[i].xv += g_bobWraith[i].varsF[INDEXF_ACC]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			g_bobWraith[i].yv += g_bobWraith[i].varsF[INDEXF_ACC]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			// 是否超速
			if (g_bobWraith[i].xv*g_bobWraith[i].xv+g_bobWraith[i].yv*g_bobWraith[i].yv
							>g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobWraith[i].xv *= 0.8;
				g_bobWraith[i].yv *= 0.8;
			}

			// 粒子尾汽
			ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
						g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2+RAND_RANGE(-4,4),
						g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2+RAND_RANGE(-4,4), 
						(-int(g_bobWraith[i].xv)), (-int(g_bobWraith[i].yv)));

			// 判断和player的距离,如在范围内，攻击
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// 距离

			if (length <= g_bobWraith[i].varsI[INDEXI_ATTACK_RANGE]
				&& --g_bobWraith[i].counter_1 <= 0)
			{
				g_bobWraith[i].counter_1 = g_bobWraith[i].max_count_1;

				float xv = dx/length * g_bobWraith[i].varsF[INDEXF_PLASMA_SPEED];
				float yv = dy/length * g_bobWraith[i].varsF[INDEXF_PLASMA_SPEED];

				PlasmaFire(g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2,
							g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2,
							xv,yv, &g_bobWraith[i]);
			}// end if length

		}// end if alive
		else if (g_bobWraith[i].state == ENEMY_STATE_DYING)
		{

			// 是否超速
			if (g_bobWraith[i].xv*g_bobWraith[i].xv+g_bobWraith[i].yv*g_bobWraith[i].yv
							>g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobWraith[i].xv *= 0.8;
				g_bobWraith[i].yv *= 0.8;
			}

			// 随机大小的爆炸
			nRand = RAND_RANGE(16,64);

			if ((rand()%4)==1)
				BurstStart(g_bobWraith[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobWraith[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobWraith[i].xv,g_bobWraith[i].yv);  

			// 乱摆头
			nRand = RAND_RANGE(-1,1);
			g_bobWraith[i].varsI[INDEXI_DIR] = (g_bobWraith[i].varsI[INDEXI_DIR]+nRand+16)%16;
			g_bobWraith[i].curr_frame = g_bobWraith[i].varsI[INDEXI_DIR];

			// 垂死计数
			if (--g_bobWraith[i].varsI[INDEXI_DYING]<=0)
			{
				g_nWraith++;	// 未激活个数+1
				g_bobWraith[i].state = ENEMY_STATE_DEAD;

				// 掉道具
				PowerupStart(g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2,
					g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2);
			}

		}// end else dying

		// 速度加到世界坐标
		g_bobWraith[i].varsF[INDEXF_WORLDX] += g_bobWraith[i].xv;
		g_bobWraith[i].varsF[INDEXF_WORLDY] += g_bobWraith[i].yv;

		

	}// end for
}//end wraithMove

// 绘制
void WraithDraw(void)
{
	int i,frame;
	for (i=0; i<MAX_WRAITH; i++)
	{
		if (g_bobWraith[i].state != ENEMY_STATE_DEAD)
		{
			g_bobWraith[i].x = g_bobWraith[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobWraith[i].y = g_bobWraith[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			// 切换同方向同类型尾部变绿的图片
			frame = ((g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE]-FIGHT_TYPE_WRAITHB)*2+ rand()%2)*16;
			g_bobWraith[i].curr_frame += frame;
			Draw_BOB16(&g_bobWraith[i], lpddsback);
			g_bobWraith[i].curr_frame -= frame;
		}//end if
	}// end for
}// end WraithDraw

// 初始化地图
void MapInit(void)
{
	// BMP图片
	Load_Bitmap_File(&bitmap16bit, "Images/hudart.bmp");

	Create_BOB(&g_bobMap,0,0,24,26,4,
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
				SURFACE_MEMORY,0,16);

	for (int i=0; i<4; i++)
		Load_Frame_BOB16(&g_bobMap, &bitmap16bit,i,i,0,BITMAP_EXTRACT_MODE_CELL);

	Unload_Bitmap_File(&bitmap16bit);

	Set_Pos_BOB(&g_bobMap, 320, 0);

}// end MapInit

// 释放地图
inline void MapRelease(void)
{
	Destroy_BOB(&g_bobMap);
}// end MapRelease

// 绘制地图
void MapDraw(void)
{
	int i=0;
	int nMapX, nMapY;
	USHORT usColor;
	// lock back surface
	DDraw_Lock_Back_Surface();

	// 画Wraith
	for (i=0; i<MAX_WRAITH; i++)
	{
		if (g_bobWraith[i].state == ENEMY_STATE_DEAD)
			continue;
		
		// 在地图外则不画
		if (g_bobWraith[i].varsF[INDEXF_WORLDX]<0 
			|| g_bobWraith[i].varsF[INDEXF_WORLDY]<0 
			|| g_bobWraith[i].varsF[INDEXF_WORLDX]>WORLD_WIDTH 
			|| g_bobWraith[i].varsF[INDEXF_WORLDY]>WORLD_HEIGHT)
			continue;

		nMapX = g_bobWraith[i].varsF[INDEXF_WORLDX]/10;
		nMapY = g_bobWraith[i].varsF[INDEXF_WORLDY]/10;

		
		if (g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] == FIGHT_TYPE_WRAITHB)
			usColor = RGB16Bit(0,0,255);
		else
			usColor = RGB16Bit(255,0,0);

		Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
		
	}// end for

	// 画BEE
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state == ENEMY_STATE_DEAD)
			continue;

		// 在地图外则不画
		if (g_bobBee[i].varsF[INDEXF_WORLDX]<0 
			|| g_bobBee[i].varsF[INDEXF_WORLDY]<0 
			|| g_bobBee[i].varsF[INDEXF_WORLDX]>WORLD_WIDTH 
			|| g_bobBee[i].varsF[INDEXF_WORLDY]>WORLD_HEIGHT)
			continue;

		nMapX = g_bobBee[i].varsF[INDEXF_WORLDX]/10;
		nMapY = g_bobBee[i].varsF[INDEXF_WORLDY]/10;

		usColor = RGB16Bit(255,255,0);

		Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);

	}// end for

	// 画Gunship
	for (i=0; i<MAX_GUNSHIP; i++)
	{
		if (g_bobGunship[i].state == ENEMY_STATE_DEAD)
			continue;

		// 在地图外则不画
		if (g_bobGunship[i].varsF[INDEXF_WORLDX]<0 
			|| g_bobGunship[i].varsF[INDEXF_WORLDY]<0 
			|| g_bobGunship[i].varsF[INDEXF_WORLDX]>WORLD_WIDTH 
			|| g_bobGunship[i].varsF[INDEXF_WORLDY]>WORLD_HEIGHT)
			continue;

		nMapX = g_bobGunship[i].varsF[INDEXF_WORLDX]/10;
		nMapY = g_bobGunship[i].varsF[INDEXF_WORLDY]/10;

		usColor = RGB16Bit(255,255,255);

		Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);

	}// end for

	// 画Player
	nMapX = g_bobPlayer.varsF[INDEXF_WORLDX]/10;
	nMapY = g_bobPlayer.varsF[INDEXF_WORLDY]/10;

	usColor = RGB16Bit(255,255,255);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	usColor = RGB16Bit(0,255,255);
	Draw_Pixel16(MAP_LEFT+nMapX+1, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY+1, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX-1, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY-1, usColor, back_buffer, back_lpitch);

	// 画显示窗口小框 高-2,左-2，右+2调整
	usColor = RGB16Bit(0,100,0);
	nMapX = MAP_LEFT+g_ptScreen.x/10;
	nMapY = MAP_TOP+g_ptScreen.y/10;
	HLine16(nMapX-2, nMapX+WINDOW_WIDTH/10-2, nMapY, usColor,back_buffer, back_lpitch);
	HLine16(nMapX-2, nMapX+WINDOW_WIDTH/10-2, nMapY+WINDOW_HEIGHT/10-2,usColor, back_buffer, back_lpitch);
	VLine16(nMapY, nMapY+WINDOW_HEIGHT/10-2, nMapX-2,usColor, back_buffer, back_lpitch);
	VLine16(nMapY, nMapY+WINDOW_HEIGHT/10-2, nMapX+WINDOW_WIDTH/10-2,usColor, back_buffer, back_lpitch);

	DDraw_Unlock_Back_Surface();

	// 画地图的四个角
	g_bobMap.curr_frame = 0;
	Set_Pos_BOB(&g_bobMap, MAP_LEFT-2, MAP_TOP);
	Draw_BOB16(&g_bobMap, lpddsback);

	g_bobMap.curr_frame = 1;
	Set_Pos_BOB(&g_bobMap, MAP_RIGHT, MAP_TOP);
	Draw_BOB16(&g_bobMap, lpddsback);

	g_bobMap.curr_frame = 2;
	Set_Pos_BOB(&g_bobMap, MAP_LEFT-2, MAP_BOTTOM);
	Draw_BOB16(&g_bobMap, lpddsback);

	g_bobMap.curr_frame = 3;
	Set_Pos_BOB(&g_bobMap, MAP_RIGHT, MAP_BOTTOM);
	Draw_BOB16(&g_bobMap, lpddsback);

}// end MapRelease

// 生成碰撞参与对象表
void CollisionInit(void)
{
	int i=0, j=0;


	// player
	g_pbobCollision[i++] = &g_bobPlayer;

	// Wraith
	for (j=0; j<MAX_WRAITH; j++)
	{
		g_pbobCollision[i++] = &g_bobWraith[j];
	}

	// BEE
	for (j=0; j<MAX_BEE; j++)
	{
		g_pbobCollision[i++] = &g_bobBee[j];
	}

	// Gunship
	for (j=0; j<MAX_GUNSHIP; j++)
	{
		g_pbobCollision[i++] = &g_bobGunship[j];
	}


}// end Collision

// 碰撞处理
void Collision(void)
{
	int i=0;
	float nabx = 0,
		  naby = 0,
		  length = 0,
		  tabx = 0,
		  taby = 0,
		  vait = 0,
		  vain = 0,
		  vbit = 0,
		  vbin = 0,
		  ma = 0,
		  mb = 0,
		  vafn = 0,
		  vaft = 0,
		  vbfn = 0,
		  vbft = 0,
		  xfa = 0,
		  yfa = 0,
		  xfb = 0,
		  yfb = 0;	// 碰撞处理要用到的变量， 使用时有说明
	int bob_a,bob_b;

	// 玩家和敌机的碰撞
	for (bob_a = 0; bob_a<NUM_COLLISION; bob_a++)
	{
		if (g_pbobCollision[bob_a]->state == 0)
			continue;
		for (bob_b = bob_a+1; bob_b < NUM_COLLISION; bob_b++)
		{
			if (g_pbobCollision[bob_b]->state == 0) 
				continue;

			// 碰撞判断
			if (Collision_Test(g_pbobCollision[bob_a]->varsF[INDEXF_WORLDX],g_pbobCollision[bob_a]->varsF[INDEXF_WORLDY],
								g_pbobCollision[bob_a]->width, g_pbobCollision[bob_a]->height,
								g_pbobCollision[bob_b]->varsF[INDEXF_WORLDX],g_pbobCollision[bob_b]->varsF[INDEXF_WORLDY],
								g_pbobCollision[bob_b]->width,g_pbobCollision[bob_b]->height))
			{
				// the g_pbobCollision have made contact, compute response
				// 计算向量n< nabx, naby>
				nabx = (g_pbobCollision[bob_b]->varsF[INDEXF_WORLDX]+ g_pbobCollision[bob_b]->width/2)
					- (g_pbobCollision[bob_a]->varsF[INDEXF_WORLDX]+ g_pbobCollision[bob_a]->width/2 );
				naby = (g_pbobCollision[bob_b]->varsF[INDEXF_WORLDY]+ g_pbobCollision[bob_b]->height/2)
					- (g_pbobCollision[bob_a]->varsF[INDEXF_WORLDY]+ g_pbobCollision[bob_b]->height/2 );
				length = sqrt(nabx*nabx + naby*naby);

				// 将n 规格化，即变成单位向量
				nabx/=length;
				naby/=length;

				// 计算与向量n垂直的向量t
				tabx =  -naby;
				taby =  nabx;

				// tangential is also normalized since it's just a rotated normal vector

				// step 2: compute all the initial velocities
				// notation ball: (a,b) initial: i, final: f, n: normal direction, t: tangential direction
				// 将碰撞两者的速度投影到n,t方向
				vait = DOT_PRODUCT(g_pbobCollision[bob_a]->xv, 
					g_pbobCollision[bob_a]->yv, 
					tabx, taby);

				vain = DOT_PRODUCT(g_pbobCollision[bob_a]->xv, 
					g_pbobCollision[bob_a]->yv, 
					nabx, naby);

				vbit = DOT_PRODUCT(g_pbobCollision[bob_b]->xv, 
					g_pbobCollision[bob_b]->yv, 
					tabx, taby);

				vbin = DOT_PRODUCT(g_pbobCollision[bob_b]->xv, 
					g_pbobCollision[bob_b]->yv, 
					nabx, naby);

				// 由动能守恒和动量守恒求出n方向上的速度分量

				ma = g_pbobCollision[bob_a]->varsF[INDEXF_MASS];
				mb = g_pbobCollision[bob_b]->varsF[INDEXF_MASS];

				vafn = (mb*vbin*(g_fE+1) + vain*(ma - g_fE*mb)) / (ma + mb);
				vbfn = (ma*vain*(g_fE+1) - vbin*(ma - g_fE*mb)) / (ma + mb);

				// now luckily the tangential components are the same before and after, so
				vaft = vait;
				vbft = vbit;

				// 将n,t坐标系下的速度转化到x,y坐标系

				xfa = vafn*nabx + vaft*tabx;
				yfa = vafn*naby + vaft*taby;

				xfb = vbfn*nabx + vbft*tabx;
				yfb = vbfn*naby + vbft*taby;

				// store results
				g_pbobCollision[bob_a]->xv = xfa;
				g_pbobCollision[bob_a]->yv = yfa;

				g_pbobCollision[bob_b]->xv = xfb;
				g_pbobCollision[bob_b]->yv = yfb;

				// update position
				g_pbobCollision[bob_a]->varsF[INDEXF_WORLDX]+=g_pbobCollision[bob_a]->xv;
				g_pbobCollision[bob_a]->varsF[INDEXF_WORLDY]+=g_pbobCollision[bob_a]->yv;

				g_pbobCollision[bob_b]->varsF[INDEXF_WORLDX]+=g_pbobCollision[bob_b]->xv;
				g_pbobCollision[bob_b]->varsF[INDEXF_WORLDY]+=g_pbobCollision[bob_b]->yv;


			} // end if

		} // end for bob_b
	}// end for bob_a

}// end Collision

// 初始化Bee
void BeeInit(void)
{
	int i=0;

	// 创建BOB
	Create_BOB(&g_bobBee[0], 0, 0, 22, 18, 32,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// 读取图片
	Load_Bitmap_File(&bitmap16bit,"Images/bee.bmp");
	// 加载图片到帧 蓝色Wraith
	for (i=0; i<32; i++)
	{
		Load_Frame_BOB16(&g_bobBee[0], &bitmap16bit, i, i%16, i/16, BITMAP_EXTRACT_MODE_CELL);
	}// end for
	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);

	g_bobBee[0].state = ENEMY_STATE_DEAD;

	for (i=1; i<MAX_BEE; i++)
	{
		Clone_BOB(&g_bobBee[0], &g_bobBee[i]);
	}
}// end BeeInit

// 释放Bee
void BeeRelease(void)
{
	for (int i=0; i<MAX_BEE; i++)
	{
		Destroy_BOB(&g_bobBee[i]);
	}
}// end BeeRelease

// 重置Bee
void BeeReset(void)
{
	for (int i=0; i<MAX_BEE; i++)
	{
		g_bobBee[i].state = ENEMY_STATE_DEAD;
	}
}// end WraithReset

//移动Bee
void BeeMove(void)
{
	int i=0,nRand=0;
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state == ENEMY_STATE_DEAD)
			continue;

		if (g_bobBee[i].state == ENEMY_STATE_ALIVE)
		{
			// 判断敌机死活
			if (g_bobBee[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobBee[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE]+=g_bobBee[i].varsI[INDEXI_SCORE];
				continue;
			}

			//保存前一个方向
			g_bobBee[i].varsI[INDEXI_DIRPRE] = g_bobBee[i].varsI[INDEXI_DIR];

			float dydx,dx,dy;
			int tmpDir; // 目标方向

			dx = (g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 判断方向
			if( dx == 0 && dy > 0)				// 两点在直线上，且W在下面
				tmpDir = DIR_BEE_NNNN;
			else if( dx ==0 && dy < 0)			// 两点在直线上，且W在上面
				tmpDir = DIR_BEE_SSSS;
			else if( dx > 0 && dy == 0)			// 两点在直线上，且W在右面
				tmpDir = DIR_BEE_WWWW;
			else if( dx < 0 && dy == 0)			// 两点在直线上，且W在左面
				tmpDir = DIR_BEE_EEEE;
			else if( dx > 0 && dy > 0)			// 左上象限,根据dy,dx的比值确定角度范围,利用tan(x)的单调性
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_BEE_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_BEE_WWWN;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_BEE_WWNN;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_BEE_WNNN;
				else
					tmpDir = DIR_BEE_NNNN;
			}
			else if( dx >0 && dy < 0)			// 左下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_BEE_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_BEE_SWWW;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_BEE_SSWW;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_BEE_SSSW;
				else
					tmpDir = DIR_BEE_SSSS;
			}
			else if( dx < 0 && dy < 0)			// 右下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_BEE_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_BEE_EEES;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_BEE_EESS;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_BEE_ESSS;
				else
					tmpDir = DIR_BEE_SSSS;
			}
			else								// 右上象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_BEE_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_BEE_NEEE;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_BEE_NNEE;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_BEE_NNNE;
				else
					tmpDir = DIR_BEE_NNNN;
			}// end else


			g_bobBee[i].varsI[INDEXI_DIR] = tmpDir;
			g_bobBee[i].curr_frame = g_bobBee[i].varsI[INDEXI_DIR];
			tmpDir -= g_bobBee[i].varsI[INDEXI_DIRPRE];

			// 转向速率损失
			g_bobBee[i].varsF[INDEXF_MOVESPEED] = g_fCosTab64[16+tmpDir*4]*g_bobBee[i].varsF[INDEXF_MOVESPEED];

			// 更新速度
			//g_bobWraith[i].xv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			//g_bobWraith[i].yv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			g_bobBee[i].xv += g_bobBee[i].varsF[INDEXF_ACC]*g_fCosTab64[((g_bobBee[i].varsI[INDEXI_DIR]+4)%16)*4];
			g_bobBee[i].yv += g_bobBee[i].varsF[INDEXF_ACC]*g_fSinTab64[((g_bobBee[i].varsI[INDEXI_DIR]+4)%16)*4];

			// 是否超速
			if (g_bobBee[i].xv*g_bobBee[i].xv+g_bobBee[i].yv*g_bobBee[i].yv
							>g_bobBee[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobBee[i].xv *= 0.8;
				g_bobBee[i].yv *= 0.8;
			}

			// 粒子尾汽
			ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED, 1,
				g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2+RAND_RANGE(-4,4),
				g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2+RAND_RANGE(-4,4), 
				(-int(g_bobBee[i].xv)), (-int(g_bobBee[i].yv)));

			// 判断和player的距离,如在范围内，攻击
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// 距离

			if (length <= g_bobBee[i].varsI[INDEXI_ATTACK_RANGE]
			&& --g_bobBee[i].counter_1 <= 0)
			{
				g_bobBee[i].counter_1 = g_bobBee[i].max_count_1;

				float xv = dx/length * g_bobBee[i].varsF[INDEXF_PLASMA_SPEED];
				float yv = dy/length * g_bobBee[i].varsF[INDEXF_PLASMA_SPEED];

				PlasmaFire(g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2,
					g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2,
					xv,yv, &g_bobBee[i]);
			}// end if length

		}// end if alive
		else if (g_bobBee[i].state == ENEMY_STATE_DYING)
		{

			// 是否超速
			if (g_bobBee[i].xv*g_bobBee[i].xv+g_bobBee[i].yv*g_bobBee[i].yv
							>g_bobBee[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobBee[i].xv *= 0.8;
				g_bobBee[i].yv *= 0.8;
			}

			// 随机大小的爆炸
			nRand = RAND_RANGE(16,64);

			if ((rand()%4)==1)
				BurstStart(g_bobBee[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobBee[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobBee[i].xv,g_bobBee[i].yv);  

			// 乱摆头
			nRand = RAND_RANGE(-1,1);
			g_bobBee[i].varsI[INDEXI_DIR] = (g_bobBee[i].varsI[INDEXI_DIR]+nRand+16)%16;
			g_bobBee[i].curr_frame = g_bobBee[i].varsI[INDEXI_DIR];

			// 垂死计数
			if (--g_bobBee[i].varsI[INDEXI_DYING]<=0)
			{
				g_nBee++;	// 未激活个数+1
				g_bobBee[i].state = ENEMY_STATE_DEAD;

				// 掉道具
				PowerupStart(g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2,
					g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2);
			}

		}// end else dying

		// 速度加到世界坐标
		g_bobBee[i].varsF[INDEXF_WORLDX] += g_bobBee[i].xv;
		g_bobBee[i].varsF[INDEXF_WORLDY] += g_bobBee[i].yv;



	}// end for
}//end BeeMove

// 绘制
void BeeDraw(void)
{
	int i,frame;
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state != ENEMY_STATE_DEAD)
		{
			g_bobBee[i].x = g_bobBee[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobBee[i].y = g_bobBee[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			// 切换同方向同类型尾部变红的图片
			frame = (rand()%2)*16;
			g_bobBee[i].curr_frame += frame;
			Draw_BOB16(&g_bobBee[i], lpddsback);
			g_bobBee[i].curr_frame -= frame;
		}//end if
	}// end for
}// end BeeDraw

// 初始化Gunship
void GunshipInit(void)
{
	int i=0;

	// 创建BOB
	Create_BOB(&g_bobGunship[0], 0, 0, 160, 70, 32,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// 读取图片
	Load_Bitmap_File(&bitmap16bit,"Images/gunship.bmp");
	// 加载图片到帧 
	for (i=0; i<32; i++)
	{
		Load_Frame_BOB16(&g_bobGunship[0], &bitmap16bit, i, i%4, i/4, BITMAP_EXTRACT_MODE_CELL);
	}// end for
	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);

	g_bobGunship[0].state = ENEMY_STATE_DEAD;

	for (i=1; i<MAX_GUNSHIP; i++)
	{
		Clone_BOB(&g_bobGunship[0], &g_bobGunship[i]);
	}
}// end GunshipInit

// 释放
void GunshipRelease(void)
{
	for (int i=0; i<MAX_GUNSHIP; i++)
	{
		Destroy_BOB(&g_bobGunship[i]);
	}
}// end GunshipRelease

// 重置
void GunshipReset(void)
{
	for (int i=0; i<MAX_GUNSHIP; i++)
	{
		g_bobGunship[i].state = ENEMY_STATE_DEAD;
	}
}// end GunshipReset

// 移动
void GunshipMove(void)
{
	int i=0,nRand=0;
	for (i=0; i<MAX_GUNSHIP; i++)
	{
		if (g_bobGunship[i].state == ENEMY_STATE_DEAD)
			continue;

		if (g_bobGunship[i].state == ENEMY_STATE_ALIVE)
		{
			// 判断敌机死活
			if (g_bobGunship[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobGunship[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE] += g_bobGunship[i].varsI[INDEXI_SCORE];
				continue;
			}

			float dydx,dx,dy;
			int tmpDir; // 目标方向

			dx = (g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 判断方向
			if( dx == 0 && dy > 0)				// 两点在直线上，且W在下面
				tmpDir = DIR_NNNN;
			else if( dx ==0 && dy < 0)			// 两点在直线上，且W在上面
				tmpDir = DIR_SSSS;
			else if( dx > 0 && dy == 0)			// 两点在直线上，且W在右面
				tmpDir = DIR_WWWW;
			else if( dx < 0 && dy == 0)			// 两点在直线上，且W在左面
				tmpDir = DIR_EEEE;
			else if( dx > 0 && dy > 0)			// 左上象限,根据dy,dx的比值确定角度范围,利用tan(x)的单调性
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_WWWN;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_WWNN;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_WNNN;
				else
					tmpDir = DIR_NNNN;
			}
			else if( dx >0 && dy < 0)			// 左下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_WWWW;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_SWWW;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_SSWW;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_SSSW;
				else
					tmpDir = DIR_SSSS;
			}
			else if( dx < 0 && dy < 0)			// 右下象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_EEES;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_EESS;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_ESSS;
				else
					tmpDir = DIR_SSSS;
			}
			else								// 右上象限
			{
				dydx = fabs(dy/dx); // 取绝对值
				if( dydx <=	g_fTanTab4[0])
					tmpDir = DIR_EEEE;
				else if( dydx > g_fTanTab4[0] && dydx <= g_fTanTab4[1])
					tmpDir = DIR_NEEE;
				else if( dydx > g_fTanTab4[1] && dydx <= g_fTanTab4[2])
					tmpDir = DIR_NNEE;
				else if( dydx > g_fTanTab4[2] && dydx <= g_fTanTab4[3])
					tmpDir = DIR_NNNE;
				else
					tmpDir = DIR_NNNN;
			}// end else


			g_bobGunship[i].varsI[INDEXI_DIR] = tmpDir;
			g_bobGunship[i].curr_frame = g_bobGunship[i].varsI[INDEXI_DIR];

			// 未超速则加速
			if (fabs(g_bobGunship[i].xv) > g_bobGunship[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobGunship[i].xv *= 0.8;
			}
			if (fabs(g_bobGunship[i].yv) > g_bobGunship[i].varsF[INDEXF_MOVESPEED])
			{
				g_bobGunship[i].yv *= 0.8;
			}
			g_bobGunship[i].xv += g_bobGunship[i].varsF[INDEXF_ACC];
			g_bobGunship[i].yv += g_bobGunship[i].varsF[INDEXF_ACCY];

			// 越界转向
			if (g_bobGunship[i].varsF[INDEXF_WORLDX] < 0)
			{
				// 转向右边
				g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] = DIR_RIGHT;
				g_bobGunship[i].varsF[INDEXF_ACC] = ACC_GUNSHIP;
			}				
			if (g_bobGunship[i].varsF[INDEXF_WORLDX] > WORLD_WIDTH)
			{
				// 转向左边
				g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] = DIR_LEFT;
				g_bobGunship[i].varsF[INDEXF_ACC] = -ACC_GUNSHIP;
			}
			
			if (dy < 0)
			{
				g_bobGunship[i].varsF[INDEXF_ACCY] = ACC_GUNSHIPY;
			}
			else
			{
				g_bobGunship[i].varsF[INDEXF_ACCY] = -ACC_GUNSHIPY;
			}

			// 粒子尾汽
			// 向右行驶
			if (g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] == DIR_RIGHT)
			{
				ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
					g_bobGunship[i].varsF[INDEXF_WORLDX]+18+RAND_RANGE(-4,4),
					g_bobGunship[i].varsF[INDEXF_WORLDY]+19+RAND_RANGE(-4,4), 
					(-int(g_bobGunship[i].xv)), (-int(g_bobGunship[i].yv)));
				ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
					g_bobGunship[i].varsF[INDEXF_WORLDX]+10+RAND_RANGE(-4,4),
					g_bobGunship[i].varsF[INDEXF_WORLDY]+37+RAND_RANGE(-4,4), 
					(-int(g_bobGunship[i].xv)), (-int(g_bobGunship[i].yv)));
			}
			else//向右
			{
				ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
					g_bobGunship[i].varsF[INDEXF_WORLDX]+144+RAND_RANGE(-4,4),
					g_bobGunship[i].varsF[INDEXF_WORLDY]+20+RAND_RANGE(-4,4), 
					(-int(g_bobGunship[i].xv)), (-int(g_bobGunship[i].yv)));
				ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
					g_bobGunship[i].varsF[INDEXF_WORLDX]+152+RAND_RANGE(-4,4),
					g_bobGunship[i].varsF[INDEXF_WORLDY]+37+RAND_RANGE(-4,4), 
					(-int(g_bobGunship[i].xv)), (-int(g_bobGunship[i].yv)));
			}

			// 判断和player的距离,如在范围内，攻击 , 如果太远则转向
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// 距离

			if (length <= g_bobGunship[i].varsI[INDEXI_ATTACK_RANGE]
			&& --g_bobGunship[i].counter_1 <= 0)
			{
				g_bobGunship[i].counter_1 = g_bobGunship[i].max_count_1;

				float xv = dx/length * g_bobGunship[i].varsF[INDEXF_PLASMA_SPEED];
				float yv = dy/length * g_bobGunship[i].varsF[INDEXF_PLASMA_SPEED];

				// 向右行驶
				if (g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] == DIR_RIGHT)
				{
					// 三门炮射击
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+82,	g_bobGunship[i].varsF[INDEXF_WORLDY]+11,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+140,	g_bobGunship[i].varsF[INDEXF_WORLDY]+17,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+143,	g_bobGunship[i].varsF[INDEXF_WORLDY]+26,
						xv,yv, &g_bobGunship[i]);
				}
				else // 向左
				{
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+21,	g_bobGunship[i].varsF[INDEXF_WORLDY]+15,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+16,	g_bobGunship[i].varsF[INDEXF_WORLDY]+25,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+78,	g_bobGunship[i].varsF[INDEXF_WORLDY]+12,
						xv,yv, &g_bobGunship[i]);
				}
				
			}// end if length

		}// end if alive
		else if (g_bobGunship[i].state == ENEMY_STATE_DYING)
		{
			// 随机大小的爆炸
			nRand = RAND_RANGE(32,120);

			if ((rand()%4)==1)
				BurstStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobGunship[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobGunship[i].xv,g_bobGunship[i].yv);  

			// 乱振动
			nRand = RAND_RANGE(-5,5);
			g_bobGunship[i].varsF[INDEXF_WORLDX] += nRand;
			g_bobGunship[i].varsF[INDEXF_WORLDY] += nRand;

			// 垂死计数
			if (--g_bobGunship[i].varsI[INDEXI_DYING]<=0)
			{
				g_nGunship++;	// 未激活个数+1
				g_bobGunship[i].state = ENEMY_STATE_DEAD;

				// 掉2个道具
				PowerupStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/4,
					g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);
				PowerupStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width*3/4,
					g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);
			}

		}// end else dying

		// 速度加到世界坐标
		g_bobGunship[i].varsF[INDEXF_WORLDX] += g_bobGunship[i].xv;
		g_bobGunship[i].varsF[INDEXF_WORLDY] += g_bobGunship[i].yv;

	}// end for
}// end GunshipMove

// 绘制
void GunshipDraw(void)
{
	int i,frame;
	for (i=0; i<MAX_GUNSHIP; i++)
	{
		if (g_bobGunship[i].state != ENEMY_STATE_DEAD)
		{
			g_bobGunship[i].x = g_bobGunship[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobGunship[i].y = g_bobGunship[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			frame = g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP]*16;
			g_bobGunship[i].curr_frame += frame;
			Draw_BOB16(&g_bobGunship[i], lpddsback);
			g_bobGunship[i].curr_frame -= frame;
		}//end if
	}// end for
}// end GunshipDraw

// 初始化剧情显示
void StoryInit(void)
{
	// 读取背景图片
	Load_Bitmap_File(&bitmap16bit, "images/begin.BMP");
	Create_Bitmap(&g_imgBegingBack,0,0,800,600,16);
	Load_Image_Bitmap16(&g_imgBegingBack, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// 读位图 
	Load_Bitmap_File(&bitmap16bit, "images/storyBegin.BMP");

	// 创建字幕BOB
	Create_BOB(&g_bobStory,100,600,600,1200,1,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// 读取位图到帧
	Load_Frame_BOB16(&g_bobStory,&bitmap16bit,0,0,0,BITMAP_EXTRACT_MODE_ABS);  
	Unload_Bitmap_File(&bitmap16bit);

}// end StoryInit

// 开场
void StoryBegin(void)
{
	DDraw_Lock_Back_Surface();

	// draw background
	Draw_Bitmap16(&g_imgBegingBack, back_buffer, back_lpitch,0);

	DDraw_Unlock_Back_Surface();

	g_bobStory.y = g_bobStory.y  - 1;

	Draw_BOB16(&g_bobStory, lpddsback);
}// end StroyBegin

// 释放
void StoryRelease(void)
{
	Destroy_Bitmap(&g_imgBegingBack);
}// StoryRelease







