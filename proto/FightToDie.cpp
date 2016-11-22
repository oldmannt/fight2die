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

//640x480 16λ����ģʽ
#define WINDOW_WIDTH	800   // size of window
#define WINDOW_HEIGHT	600
#define WINDOW_BPP		16
#define WINDOWED_APP	1

#define SURFACE_MEMORY   DDSCAPS_SYSTEMMEMORY	 //�ڴ���Դ�DDSCAPS_VIDEOMEMORY
const int WORLD_WIDTH = 1600;	// ��ͼ��С
const int WORLD_HEIGHT = 1200;

const int FRAME_RATE = 17;	// ??����

const int MAX_STARS = 500;	// ���Ǹ���
#define STAR_PANEL0		0	// 3��ƽ��
#define STAR_PANEL1		1
#define STAR_PANEL2		2
const USHORT STAR_COLOR0 =  ((255 & 31) + ((255 & 63) << 5) + ((255 & 31) << 11));
const USHORT STAR_COLOR1 =  ((180 & 31) + ((180 & 63) << 5) + ((180 & 31) << 11));
const USHORT STAR_COLOR2 =  ((100 & 31) + ((100& 63) << 5) + ((100 & 31) << 11));

#define GAME_STATE_STORY	0	// ��Ϸ״̬
#define GAME_STATE_MENU		1
#define GAME_STATE_LOAD		2
#define GAME_STATE_RESET	3
#define GAME_STATE_RUNNING	4
#define GAME_STATE_PAUSED	5
#define GAME_STATE_EXIT		6
#define GAME_STATE_LOST		7
#define GAME_STATE_WIN		8

/////////////////////////////////////////    ����   ////////////////////////////////////////

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

/////////////////////////////////////////�л������////////////////////////////////////////

#define INDEXI_HEALTH				0	//�л���player,BOB�����varsI�����±�
#define INDEXI_DAMAGE				1
#define INDEXI_PLASMA_RANGE			2
#define INDEXI_SCORE				3
#define INDEXI_FIGHT_TYPE			4

#define FIGHT_TYPE_PLAYER			0	// ս������
#define FIGHT_TYPE_WRAITHB			1
#define FIGHT_TYPE_WRAITHR			2
#define FIGHT_TYPE_BEE				3
#define FIGHT_TYPE_GUNSHIP			4

#define INDEXI_MOVESPEED			5 // player��BOB�����varsI�����±�
#define INDEXI_FIRESPEED			6
#define INDEXI_BOMBS				7
#define INDEXI_BOMB_RANGE			8	// ��ը��Χ
#define INDEXI_BOMB_DAMAGE			9
#define INDEXI_BOMB_PUSH			10	//ը����ըǿ���ڵл�������
#define INDEXI_COUNTER_MOVEUP		11	//�ƶ����ټ���
#define INDEXI_COUNTER_FIREUP		12	//�������ټ���
#define INDEXI_COUNTER_INVINCIBLE	13	// �޵�ʱ����� ����0ʱ�޵�
#define INDEXI_KILLS				14  // ɱ����
			

// ���״̬��־
#define PLAYER_STATE_DEAD      		0		// �������
#define PLAYER_STATE_ALIVE      	1		// ��һ���
#define PLAYER_STATE_MOVEUP			2		// �ƶ�����
#define PLAYER_STATE_INVINCIBLE		4		// �޵�״̬
#define PLAYER_STATE_FIREUP			8		// ��������
#define PLAYER_STATE_HAND			16		// �ѻ�õ��ߡ��֡�


// player����
const int PLAYER_MAX_HEALTH = 200;					// ��ҳ�ʼ����
const int PLAYER_MOVESPEED = 180/(1000/FRAME_RATE);			// ����ÿ֡
//const int PLAYER_MOVEUP = 6;
const int PLAYER_BOMBS = 50;				// ��ҳ�ʼը����
const int BOMB_RANGE = 256;					// ը����ը��Χ �����α߳�
const int PLAYER_DAMAGE = 45;				// ��ҳ�ʼ������
const int BOMB_DAMAGE = 60;					// ը��ɱ����
const int BOMB_PUSH = 500;					// ը����ըǿ���ڵл�������
const int PLAYER_FIRESPEED = 1000/FRAME_RATE/5;		//  ÿ��5�� ��λ��֡
//const int PLAYER_FIREUP = 3;
const float PLAYER_MASS = 20;				// ����
const float PLAYER_PLASMA_SPEED = 960/(1000/FRAME_RATE);		//ÿ������
const float PLAYER_BOMB_SPEED = 12;
const int PLAYER_PLASMA_RANGE = (WINDOW_WIDTH*5/4)/PLAYER_PLASMA_SPEED;	// ��̼���

// �л�BOB
#define INDEXI_DIR					5
#define INDEXI_DIRPRE				6
#define INDEXI_DIRGUNSHIP			7
#define INDEXI_ATTACK_RANGE			8
#define INDEXI_DYING				9	// ����ʱ��
#define INDEXI_MAXHEALTH			10


#define INDEXF_WORLDX				0 // ����BOB��varsF�����±�
#define INDEXF_WORLDY				1

#define INDEXF_MASS					2 // player���л�BOB
#define INDEXF_PLASMA_SPEED			3 // �ӵ��ٶ�

#define INDEXF_BOMB_SPEED			4 // ���BOB

#define INDEXF_MAX_MOVESPEED		4 // �л�BOB
#define INDEXF_MOVESPEED			5 // �ƶ��ٶ�
#define INDEXF_ACC					6 // ���ٶ�
#define INDEXF_ACCY					7 // GUNSHIP Y������ٶ�

// �л�״̬
#define ENEMY_STATE_DEAD	0
#define ENEMY_STATE_ALIVE	1
#define ENEMY_STATE_DYING	2

// �л�����
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
															// �ӵ��ٶ�
const float WRAITHB_PLASMA_SPEED = 400/(1000/FRAME_RATE);	// ����ÿ֡ һ��400������
const float WRAITHR_PLASMA_SPEED = 400/(1000/FRAME_RATE);
const float BEE_PLASMA_SPEED = 400/(1000/FRAME_RATE);
const float GUNSHIP_PLASMA_SPEED = 450/(1000/FRAME_RATE);

const int ATTACK_RANGE_WRAITHB = 78 + 15; // ���� �����ж��ľ���
const int ATTACK_RANGE_WRAITHR = 78 + 15;	// 78= 40 + 38
const int ATTACK_RANGE_BEE	   = 67 + 15;   // 67 = 29 + 38
const int ATTACK_RANGE_GUNSHIP = 117 + WINDOW_HEIGHT/2;

const int RANGE_WRAITHB = ATTACK_RANGE_WRAITHB/WRAITHB_PLASMA_SPEED;	// ��λ��֡ ���
const int RANGE_WRAITHR = ATTACK_RANGE_WRAITHR/WRAITHR_PLASMA_SPEED;
const int RANGE_BEE		= ATTACK_RANGE_BEE/BEE_PLASMA_SPEED;
const int RANGE_GUNSHIP = ATTACK_RANGE_GUNSHIP/GUNSHIP_PLASMA_SPEED;

const int FIRECOUNT_WRAITHB = 1000/FRAME_RATE/2;	// ����Ƶ�� ����֡����һ�� ��λ֡
const int FIRECOUNT_WRAITHR = 1000/FRAME_RATE/2;	// һ�빥��2��
const int FIRECOUNT_BEE		= 1000/FRAME_RATE/2;
const int FIRECOUNT_GUNSHIP = (1000/FRAME_RATE)/2.5;

const int SCORE_WRAITHB = 10;	// ��ֵ
const int SCORE_WRAITHR = 20;
const int SCORE_BEE		= 25;
const int SCORE_GUNSHIP = 100;

const float MOVESPEED_WRAITHB = 150/(1000/FRAME_RATE);	// ����ƶ����� ����ÿ��
const float MOVESPEED_WRAITHR = 150/(1000/FRAME_RATE);
const float MOVESPEED_BEE	  = 180/(1000/FRAME_RATE);
const float MOVESPEED_GUNSHIPX = 300/(1000/FRAME_RATE);	// X�����������
const float MOVESPEED_GUNSHIPY = 50.0/(1000.0/FRAME_RATE);  // Y�����������

const float ACC_WRAITHB = 0.1f;		// ���ٶ� ����ÿ֡ƽ��
const float ACC_WRAITHR = 0.1f;
const float ACC_BEE		= 0.2f;
const float ACC_GUNSHIP = 0.1f;
const float ACC_GUNSHIPY = 0.01f;

const float MASS_WRAITHB = 10;	//����
const float MASS_WRAITHR = 10;
const float MASS_BEE	 = 5;
const float MASS_GUNSHIP = 100;

#define DIR_NNNN		0	// ����
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

#define DIR_BEE_EEEE		0	// BEE�ķ���
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

#define DIR_RIGHT		0		// gunship����
#define DIR_LEFT		1		// gunship����

const int MIN_DIST = WINDOW_WIDTH*5/4;		// �л�����ʱ����ҵ��������Զ����
const int MAX_DIST = WINDOW_WIDTH/4;

/*const int TRACK_WRAITHB = 1000/FRAME_RATE/2;	// ����ʱ�����жϵ�Ƶ�� 5��25��7:54ȡ��
const int TRACK_WRAITHR = 1000/FRAME_RATE/2;
const int TRACK_BEE		= 1000/FRAME_RATE/3;
const int TRACK_GUNSHIP = 1000/FRAME_RATE/2;*/

const int DYING_COUNTER = 1000/FRAME_RATE;	// 1��Ĵ���

/////////////////////////////////////////��ͼ���////////////////////////////////////////
// ��ҳ������RECTʱ�ƶ�SCREEN
const RECT SCREEN_MOVE = {WINDOW_WIDTH/4, WINDOW_HEIGHT/4,
						  WINDOW_WIDTH*3/4, WINDOW_HEIGHT*3/4};
// SCREEN���ܳ��������Χ
const RECT SCREEN_RANGE = { 0, 0,WORLD_WIDTH-WINDOW_WIDTH, WORLD_HEIGHT-WINDOW_HEIGHT};

/////////////////////////////////////////�ӵ�///////////////////////////////////////////

// �ӵ�state	����״̬
#define PlASMA_STATE_OFF	0	// δ����
#define PLASMA_STATE_ON		1

//anim_state	���涯�����кţ�����Ϊ�ӵ����ͱ�־
#define PLASMA_ANIM_PLAYER		0	// ��ҵ��ӵ�
#define PLASMA_ANIM_WRAITH		1	// �л�A
#define PLASMA_ANIM_GUNSHIP		2	// �л�D

//#define PLASMA_ANIM_BOMB		5	// ը��

//varsI����
#define INDEXI_PLASMA_DAMAGE		0	// �ӵ����˺�

const int MAX_PLASMA = 80;	// ȫ���ӵ�����

/////////////////////////////////////////ը��///////////////////////////////////////////

#define BOMB_INDEXI_DAMAGE			0
#define INDEXI_RANGE			1	// ը���ķ�Χ��������
#define INDEXI_PUSH				2
//#define INDEXI_FIRE_X			3
//#define INDEXI_FIRE_Y			4	// ը��Ҫ�ɵ��ĵ�

const int MAX_BOMB = 1;

/////////////////////////////////////////��ը////////////////////////////////////////

// ״̬
#define BURST_STATE_OFF			0
#define BURST_STATE_ON			1

// varsI����
#define INDEXI_BURST_WIDTH			0
#define INDEXI_BURST_HEIGHT			1

const int MAX_BURSTS = 30;

/////////////////////////////////////////����////////////////////////////////////////

const int MAX_PARTICLES = 128;

// ����״̬
#define PARTICLE_STATE_DEAD               0
#define PARTICLE_STATE_ALIVE              1

// ��������
#define PARTICLE_TYPE_FLICKER             0
#define PARTICLE_TYPE_FADE                1 

// ������ɫ
#define PARTICLE_COLOR_RED				0 
#define PARTICLE_COLOR_GREEN			1 
#define PARTICLE_COLOR_BLUE				2 
#define PARTICLE_COLOR_WHITE			3 

/////////////////////////////////////////����////////////////////////////////////////

// ��������
#define POWERUP_TYPE_HEALTH       		0		// ��Ѫ
#define POWERUP_TYPE_MOVE           	1		// �ƶ�����
#define POWERUP_TYPE_BOMB				2		// ը��
#define POWERUP_TYPE_FIRE   			3		// ��������
#define POWERUP_TYPE_INVINCIBLE			4		// �޵�
#define POWERUP_TYPE_HAND				5		// ��

#define POWERUP_TIME					300		// ��������Ĵ��ʱ��
#define POWERUP_STATE_DEAD      0		// δ����
#define POWERUP_STATE_ALIVE     1		// �Ѽ���

#define INDEXI_POWERUP_TYPE      0		// varsI�����±꣬��������

#define HEALTH_START 			0		// �����ĸ�������
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
const int POWERUP_MOVE   = 320/(1000/FRAME_RATE);		// һ��320
const int POWERUP_MOVE_TIME = 20*1000/FRAME_RATE;		// �ƶ����ٳ���20��
const int POWERUP_FIREUP_TIME = 20*1000/FRAME_RATE;		// �������ٳ���20��
const int POWERUP_FIREUP = 1000/FRAME_RATE/10;		// ���ٵĹ��������20��
const int POWERUP_INVINCIBLE_TIME = 20*1000/FRAME_RATE;	// �޵г���20��
const int MAX_POWERUPS = 16;					// ���߸�������
const int POWERUP_LIVETIME = 5*1000/FRAME_RATE;		// 5��
const int POWERUP_HAND_GET = 1000/FRAME_RATE; // 1��

/////////////////////////////////�ı�////////////////////////

#define TEXT_GREEN		0
#define TEXT_RED		1

const int MAX_CHAR = 96;

/////////////////////////////////С��ͼ//////////////////////

const int MAP_SCALE = 10;	// ��С����
const int MAP_LEFT = 320;	// ��λ�ĸ��߿�
const int MAP_TOP = 0;
const int MAP_RIGHT = 454;
const int MAP_BOTTOM = 94;

/////////////////////////////////��ײ///////////////////////

const float g_fE = 0.8;		// ��ײ�ָ�ϵ�� 1Ϊ��ȫ����ײ

const int NUM_COLLISION = 1 + MAX_WRAITH + MAX_BEE+ MAX_GUNSHIP;

///////////////////////////////�л����ֵ�����///////////////

const int MAX_ARM = 8;
int g_nWraith = MAX_WRAITH;	// ��ǰ���ø���л���
int g_nBee = MAX_BEE;
int g_nGunship = MAX_GUNSHIP;

////////////////////////////////ˢ��///////////////////////

const float fTextSpeed = 1.5;

// TYPES //////////////////////////////////////////////////

// �ǿյ����ݽṹ
typedef struct STAR_TYP
{
	int x,y;					 // ���ǵ�λ��
	USHORT usColor;               // ���ǵ���ɫ
	int nPlane;					 // �������ڵ�ƽ��
} STAR;

// ����
typedef struct PARTICLE_TYP
{
	int nState;           // ����״̬
	int nType;            // Ч������
	int x,y;             // ��ͼ����
	int xv,yv;           // �ٶ�
	USHORT usCurColor;				// ��ɫ
	int nRed;
	int nGreen;
	int nBlue;
	int nCounter;         // ������
	int nMaxCount;       // ���������ֵ
} PARTICLE, *PARTICLE_PTR;

// һ������һ�����ֵĵл�
typedef struct ARM_TYP
{
	UINT unFrame;	// ֡���
	int nWraithB;	// �л�A��
	int nWraithR;	// �л�B��
	int nBee;		// �л�C��
	int nGunShip;	// �л�D��
}ARM,*ARM_PTR;


// ȫ�ֱ��� ///////////////////////////////////////////////
HWND main_window_handle           = NULL; // save the window handle
HINSTANCE main_instance           = NULL; // save the instance

POINT	g_ptScreen	= {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};	// ��ʾ�����ڵ�ͼ�е�λ��

int g_nMouseX,g_nMouseY;				// ���λ��
int g_nGameState = GAME_STATE_STORY;	// ��Ϸ״̬

float g_fCosTab16[16];// ���Ǻ�����
float g_fSinTab16[16];
float g_fCosTab64[64];
float g_fSinTab64[64];
float g_fTanTab4[4];

// ��ʶ�л�����������ʱ��Ľṹ
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

BOB g_bobPlayer;		// ���
BOB g_bobAim;			// ׼��
STAR g_Star[MAX_STARS];	// �ǿ�
BOB g_bobPlasma[MAX_PLASMA];	// �ӵ�
BOB g_bobBomb[MAX_BOMB];				// ը��
BOB g_bobBrusts[MAX_BURSTS];		//��ը
PARTICLE g_Particle[MAX_PARTICLES]; // ����
BOB g_bobPowerup[MAX_POWERUPS];		// ����
LPDIRECTDRAWSURFACE7 g_lpddsGreenText[MAX_CHAR];		// ���ֺ�����
LPDIRECTDRAWSURFACE7 g_lpddsRedText[MAX_CHAR];	

BOB g_bobWraith[MAX_WRAITH];		// �л�A��B
BOB g_bobBee[MAX_BEE];				// �л�C
BOB g_bobGunship[MAX_GUNSHIP];			// �л�D

BOB g_bobMap;	// ��ͼ

// ������ײ��BOB MAX_WRAITH + MAX_BEE + MAX_GUNSHIP
BOB_PTR g_pbobCollision[NUM_COLLISION];		

BITMAP_IMAGE g_imgBegingBack;	// ���汳��ͼƬ
BOB g_bobStory;		// ��Ļ����


// ���� //////////////////////////////////////////////////

void GameInit(void);		// ��Ϸ��ʼ��
void GameMain(void);		// ��Ϸ��ѭ��
void GameShutdown(void);		// �ͷ���Ϸ��Դ

void CreateTables(void);	// �������ұ�

void LoadSoundMusic(void);	// ��ȡ����

inline void WorldReset(void);	//// ���õ�ͼ

// ������ Ѫ��
void DrawBlood(int nLeft, int nTop,int nHight, int nCur, int nMax);
void TextInit(void);				// ��ʼ������
void TextDraw(int x, int y, int width, int height, int color, TCHAR* szText); // �������
void TextRelease(void);

void PlayerInit(void);	//��ʼ��player
void PlyerReset(void);	// ����player����
void PlayerDraw(void);	// �������
void PlayerMove(void);	// �ƶ����
void PlayerRefresh(void);	// ���״̬����
inline void PlayerRelease(void);	// �ͷ����
void PlayerInfo(void);		// ��ʾ�����Ϣ

inline void AimInit(void);		// ��ʼ��׼��
inline void AimDraw(void);		// ��׼��
inline void AimRelease(void);	// �ͷ�׼��

void StartInitReset(void);	// ��ʼ������
void StarMove(float dx, float dy);	// �ƶ�����
void StarDraw(void);	// ������

void PlasmaInit(void);		// ��ʼ���ӵ�
void PlasmaRelease(void);	// �ͷ��ӵ�
void PlasmaMove(void);		// �ƶ��ӵ�
void PlasmaDraw(void);		// ���ӵ�
void PlasmaFire(int x, int y, float fVx, float fVy, BOB_PTR pbobSource);		// �����ӵ�
void PlasmaReset(void);

void BombInit(void);	// ��ʼ��ը��
void BombReset(void);	// ����ը��
void BombRelease(void);	// �ͷ�ը��
void BombFire(int x, int y, float fVx, float fVy, int nCount,BOB_PTR pbobSource);	// ����ը��
void BombMove(void);	// �ƶ�ը��
void BombDraw(void);	// ��ը��

void BurstInit(void);		// ��ʼ����ը
void BurstReset(void);		// ���ñ�ը
int  BurstStart(int x, int y, int width, int height, float xv,float yv);	//��ʼ��ը
void BurstMove(void);		// �ƶ���ը
void BurstDraw(void);		// ����ը
void BurstRelease(void);	// �ͷű�ը

void ParticleInitReset(void);	// ��ʼ������
void ParticleStart(int nType, int nColor, int nTime, int x, int y, int vx, int vy); // ����һ������
void ParticleStartExplosion(int nType, int nColor, int nTime, int x, int y, int vx, int vy, int nParticles); // ��ը����
void ParticleMove(void);	// �ƶ�����
void ParticleDraw(void);	// ������

void PowerupInit(void);		// ��ʼ������
void PowerupReset(void);	// ���õ���
void PowerupStart(float x,float y);	// ��������
void PowerupDraw(void);		// ������
void PowerupRelease(void);	// �ͷŵ���
void PowerupRefresh(void);	// ���µ���
void PowerupBeGeted(BOB_PTR bobBeGeted);		// ���߱���

int LoadSurface(LPDIRECTDRAWSURFACE7 lpdds, // Ŀ�����
				BITMAP_FILE_PTR bitmap, // Դλͼ
				int cx,int cy,   // cell or absolute pos. to scan image from
				int width, int height,int mode) ;
// ��һ������BLT����һ��������
int BltSuerface(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // ��Ŀ������λ��
				int nSrcWidth, int nSrcHeight); // Դ��С
// ��һ������BLT����һ�������� ������
int BltSuerfaceScaled(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
					  int nDestX, int nDestY,  // ��Ŀ������λ��
					  int nSrcWidth, int nSrcHeight, // Դ��С
					  int nDestWidth, int nDestHeight); // Ŀ���С

void WraithInit(void);		// ��ʼ��Wraith
void WraithRelease(void);	// �ͷ�
void WraithReset(void);		// ����
void EnemyStart(int nNum, int nType);	// ���ֵл�
void WraithMove(void);		// �ƶ�
void WraithDraw(void);		// ����

void MapInit(void);		// ��ʼ����ͼ
void MapDraw(void);		// ����ͼ
inline void MapRelease(void);	// �ͷŵ�ͼ

void BeeInit(void);		// ��ʼ��Bee
void BeeRelease(void);	// �ͷ�
void BeeReset(void);		// ����
void BeeMove(void);		// �ƶ�
void BeeDraw(void);		// ����

void GunshipInit(void);		// ��ʼ��Gunship
void GunshipRelease(void);	// ���
void GunshipReset(void);	// ����
void GunshipMove(void);		// �ƶ�
void GunshipDraw(void);		// ����

void StoryInit(void);	// ��ʼ��������ʾ
void StoryBegin(void);	// ����
void StoryRelease(void);	

// ���� ƽ���͵ĸ�
float FastDistance(float x, float y)
{
	// ���3.5%

	return sqrt(x*x + y*y);

	/*x = abs(x);
	y = abs(y);

	float mn = MIN(x,y);


	return(x+y-(mn/2)-(mn/4)+(mn/16));*/
} // end FastDistance



// ������ײ��������
void CollisionInit(void);
void Collision(void);	// ��ײ����
LRESULT CALLBACK WindowProc(HWND hwnd, 
							UINT msg, 
							WPARAM wparam, 
							LPARAM lparam)
{
	// what is the message 
	switch(msg)
	{	
	case WM_SYSCOMMAND:	// ����ϵͳ�˵�

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

	// ����ʵ�������ȫ�ֱ���
	main_instance = hinstance;

	if (!RegisterClassEx(&winclass))
		return(0);

	if (!(hwnd = CreateWindowEx(NULL,                  
		szAppName,     
		szAppName, 
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION, 
		0,0,	  
		WINDOW_WIDTH,WINDOW_HEIGHT,  // ���ڴ�С
		NULL,	  
		NULL,	  
		hinstance,
		NULL)))	
		return(0);

	// ���洰�ھ����ȫ��
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
	
	// ��Ϸ��ʼ��
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

		// ��Ϸ��ѭ��
		GameMain();

	} // end while

	// �ͷ���Ϸ��Դ
	GameShutdown();

	return(msg.wParam);

} // end WinMain

// ��Ϸ��ʼ��
void GameInit(void)
{
	// �������������
	srand(Start_Clock());

	// �������ұ�
	CreateTables();

	// ��ʼ��DirectDraw
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// DirectSound��ʼ��
	DSound_Init();

	// DirectInput��ʼ��
	DInput_Init();

	// ���̳�ʼ��
	DInput_Init_Keyboard();

	// ��ʼ�����
	DInput_Init_Mouse();

	// ��ȡ����
	LoadSoundMusic();

	// �ǿճ�ʼ��
	StartInitReset();

	// �ӵ���ʼ��
	PlasmaInit();

	// ը����ʼ��
	BombInit();

	// ��ը��ʼ��
	BurstInit();

	// ����ϵͳ��ʼ��
	ParticleInitReset();

	// ���߳�ʼ��
	PowerupInit();

	// ��ҳ�ʼ��
	PlayerInit();

	// �л���ʼ��
	WraithInit();
	BeeInit();
	GunshipInit();

	// ��ʼ������
	TextInit();

	// ��ʼ��׼��
	AimInit();

	// ��ʼ��
	MapInit();

	// ��ײ��ʼ��
	CollisionInit();

	// ˢ���ʼ��
	StoryInit();

}// end GameInit

// ��Ϸ��Դ�ͷ�
void GameShutdown(void)
{
	// ˢ��
	StoryRelease();
	// ��ͼ
	MapRelease();

	//׼��
	AimRelease();

	// ASCII����
	TextRelease();

	// �л�
	GunshipRelease();
	BeeRelease();
	WraithRelease();

	// ���
	PlayerRelease();

	// ����
	PowerupRelease();

	//��ը
	BurstRelease();

	// ը��
	BombRelease();

	// �ӵ�
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

// ��Ϸ��ѭ��
void GameMain(void)
{
	static bool s_bBomb = false,	//Ͷ����־
				s_bPause = false; // ��ͣ��־
	static UINT s_unFrame = 0;		// ��ǰ֡���
	static int s_nArm = 0;		// ��һ���л�����

	// ֡ͬ��
	Start_Clock();

	// ˢ�ں���ʾ����
	DDraw_Fill_Surface(lpddsback, 0);

	// ��ȡ����
	DInput_Read_Keyboard();

	// �������
	lpdimouse->Acquire();

	// ��ȡ���
	DInput_Read_Mouse();


	// ��Ϸ���±�����
	if (g_nGameState == GAME_STATE_STORY)
	{
		StoryBegin();
		//g_nGameState = GAME_STATE_MENU;
		// ��������
		if (DSound_Status_Sound(g_nSoundBegin) != DSBSTATUS_PLAYING)
			DSound_Play(g_nSoundBegin);

		
		if (keyboard_state[DIK_SPACE])
		{
			g_nGameState = GAME_STATE_RESET;
			DSound_Stop_Sound(g_nSoundBegin);

		}

		// flip the surfaces
		DDraw_Flip();

		// ÿ��60֡
		Wait_Clock(FRAME_RATE);

		return;
	}
	// ��Ϸ��ʼ�˵�
	else if (g_nGameState == GAME_STATE_MENU)
	{
		
		g_nGameState = GAME_STATE_RESET;
		//g_nGameState = GAME_STATE_LOAD;
		//g_nGameState = GAME_STATE_EXIT;

	}
	// ��ȡ��Ϸ
	else if (g_nGameState == GAME_STATE_LOAD)
	{

		g_nGameState = GAME_STATE_RUNNING;
	}
	// �˳���Ϸ
	else if (g_nGameState == GAME_STATE_EXIT)
	{
		PostMessage(main_window_handle, WM_QUIT,0,0);

		// stop all sounds
		//DSound_Stop_All_Sounds();
	}
	// ��ͣ��Ϸ��ʾ�˵�
	else if (g_nGameState == GAME_STATE_PAUSED)
	{
		//g_nGameState = GAME_STATE_RUNNING;
		//g_nGameState = GAME_STATE_RESET;
		//g_nGameState = GAME_STATE_EXIT;

		if (keyboard_state[DIK_P] && s_bPause == false)	// ��ͣ
		{
			s_bPause = true;
			g_nGameState = GAME_STATE_RUNNING;
		}
		if (!keyboard_state[DIK_P])
			s_bPause = false;





	}
	// ���¿�ʼ
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
	// ������Ϸ
	else if (g_nGameState == GAME_STATE_RUNNING)
	{
		static enemy = 1;
		// ��������
		if (DSound_Status_Sound(g_nSoundMain) != DSBSTATUS_PLAYING)
		{
			DSound_Play(g_nSoundMain);
		}

		// ֡��Ŵ�����һ���л���֡��� �����㹻��δ����л�ʱ��������һ���л�
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

		// ��Ϸʤ���ж�
		if (s_nArm == MAX_ARM && g_nWraith +10 >= MAX_WRAITH
					&& g_nBee == MAX_BEE && g_nGunship == MAX_GUNSHIP)
		{
			g_nGameState = GAME_STATE_WIN;
		}

		// ��������
		StarDraw();
		
		if (keyboard_state[DIK_P] && s_bPause == false)	// ��ͣ
		{
			s_bPause = true;
			g_nGameState = GAME_STATE_PAUSED;
			return;
		}
		if (!keyboard_state[DIK_P])
			s_bPause = false;
		//�ƶ�׼��
		g_bobAim.x+= mouse_state.lX;
		g_bobAim.y+= mouse_state.lY;

		// ����׼��Խ��
		if (g_bobAim.x<0)
			g_bobAim.x = 0;
		else if (g_bobAim.x>WINDOW_WIDTH)
			g_bobAim.x = WINDOW_WIDTH;
		if (g_bobAim.y<0)
			g_bobAim.y = 0;
		else if (g_bobAim.y>WINDOW_HEIGHT)
			g_bobAim.y = WINDOW_HEIGHT;

		PlayerRefresh();
		
		// ����
		if (keyboard_state[DIK_W] || keyboard_state[DIK_UP])
		{
			g_bobPlayer.yv = -g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		// ����
		else if (keyboard_state[DIK_S] || keyboard_state[DIK_DOWN])
		{
			g_bobPlayer.yv = g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		else
		{
			g_bobPlayer.yv = 0;
		}
		// ����
		if (keyboard_state[DIK_A] || keyboard_state[DIK_LEFT])
		{
			g_bobPlayer.xv = -g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		// ����
		else if (keyboard_state[DIK_D] || keyboard_state[DIK_RIGHT])
		{
			g_bobPlayer.xv = g_bobPlayer.varsI[INDEXI_MOVESPEED];
		}
		else
		{
			g_bobPlayer.xv = 0;
		}
		// ���
		if (mouse_state.rgbButtons[0])
		{
			//�������Ƶ��
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
				// ���ù���Ƶ�ʼ���
				g_bobPlayer.counter_1 = g_bobPlayer.max_count_1;
			}// end if
			
		}// end mouse if

		// �ӵ�
		if (mouse_state.rgbButtons[1] && s_bBomb == false)
		{
			s_bBomb = true;

			// ����Ƿ���ը��
			if (g_bobPlayer.varsI[INDEXI_BOMBS] > 0)
			{
				g_bobPlayer.varsI[INDEXI_BOMBS]--;

				//�����ٶ� 
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
			else // ը������
				DSound_Play(g_nSoundBombOut);

		}// end mouse if
		if (!mouse_state.rgbButtons[1])
		{
			s_bBomb = false;
		}
		// �ƶ�player
		PlayerMove();

		// ���µ���
		PowerupRefresh();

		// �ƶ���ը�� ���ڵл��ƶ�֮ǰ����ֹը�ٵĵл��ҷ�
		BombMove();

		// �ƶ��л�AB
		WraithMove();

		// �ƶ�BEE
		BeeMove();

		// �ƶ��л�D
		GunshipMove();

		// �ƶ�����
		ParticleMove();

		// �ƶ��ӵ�
		PlasmaMove();	

		// ��ײ����
		Collision();

		// ֡����
		s_unFrame++;
	}

	// �ƶ���ը
	BurstMove();

	// ����player
	PlayerDraw();

	// �����ǿ�
	//StarDraw();

	// ���Ƶ���
	PowerupDraw();

	// ���Ƶл�D
	GunshipDraw();

	// ���Ƶл�AB
	WraithDraw();

	// ����Bee
	BeeDraw();

	// ��������
	ParticleDraw();		

	// �����ӵ�
	PlasmaDraw();

	// ����ը��
	BombDraw();

	// ����ը
	BurstDraw();

	// ����ͼ
	MapDraw();

	// ��player��Ϣ
	PlayerInfo();

	// ��׼��
	AimDraw();

	// ��Ϸʧ��
	if (g_nGameState == GAME_STATE_LOST)
	{
		TextDraw(150,200,50,50,TEXT_RED,"GAME");
		TextDraw(450,200,50,50,TEXT_RED,"OVER");
	}
	// ��Ϸʤ��
	if (g_nGameState == GAME_STATE_WIN)
	{
		TextDraw(150,200,50,50,TEXT_RED,"YOU");
		TextDraw(450,200,50,50,TEXT_RED,"WIN");
	}

	// flip the surfaces
	DDraw_Flip();

	if (keyboard_state[DIK_ESCAPE])	// �˵�
	{
		g_nGameState = GAME_STATE_EXIT;
		return;
	}

	// ÿ��60֡
	Wait_Clock(FRAME_RATE);

}// end GameMain

// �������ұ�
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

// ��ȡ����
void LoadSoundMusic(void)
{
	// ��������
	if ((g_nSoundBegin = DSound_Load_WAV("SOUND/storyBegin.WAV"))==-1)
		return;

	// ������
	if ((g_nSoundMain = DSound_Load_WAV("SOUND/starsng.WAV"))==-1)
		return;

	// �ƶ�
	if ((g_nSoundPlayerMove = DSound_Load_WAV("SOUND/engines0.WAV"))==-1)
		return;

	// ��ը��
	if ((g_nSoundBomb = DSound_Load_WAV("SOUND/plasma_shot.wav"))==-1)
		return;

	// ը������
	if ((g_nSoundBombOut = DSound_Load_WAV("SOUND/empty.wav"))==-1)
		return;

	// ��ը
	if ((g_nSoundExpl[0] = DSound_Load_WAV("SOUND/EXPL1.WAV"))==-1)
		return;

	if ((g_nSoundExpl[1] = DSound_Load_WAV("SOUND/EXPL2.WAV"))==-1)
		return;

	// ���Ʊ�ը
	for (int index=2; index < MAX_SOUND_EXPL; index++)
	{
		// replicate sound
		g_nSoundExpl[index] = DSound_Replicate_Sound(g_nSoundExpl[rand()%2]);  

	} // end for index

	// �ӵ�����
	if ((g_nSoundFire[0] = DSound_Load_WAV("SOUND/PULSE.WAV"))==-1)
		return;

	// �ӵ� copies
	for (index=1; index < MAX_SOUND_FIRE; index++)
	{
		// replicate sound
		g_nSoundFire[index] = DSound_Replicate_Sound(g_nSoundFire[0]);  
	} // end for index

}// end LoadSoundMusic

// ���õ�ͼ
inline void WorldReset(void)
{
	g_ptScreen.x = WINDOW_WIDTH/2;
	g_ptScreen.y = WINDOW_HEIGHT/2;
}// end WorldRese

// ��Ѫ��
void DrawBlood(int nLeft, int nTop, int nHeight, int nCur, int nMax)
{
	DDBLTFX ddbltfx; // Blt�Ĳ���
	RECT rectDest;	 // Ŀ������

	if (nCur < 0)
		nCur = 0;

	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RGB16Bit(0,255,0); // ��
	rectDest.left = nLeft;
	rectDest.top = nTop;
	rectDest.right = nLeft+nCur-1;
	rectDest.bottom = nTop + nHeight -1;
	// ��ɫѪ��
	lpddsback->Blt(&rectDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);

	if (nMax - nCur > 0)
	{
		ddbltfx.dwFillColor = RGB16Bit(255,0,0); // ��
		rectDest.left = nLeft+nCur-1;
		rectDest.top = nTop;
		rectDest.right = nLeft+nMax-1;
		rectDest.bottom = nTop + nHeight -1;
		// ��ɫѪ��
		lpddsback->Blt(&rectDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);
	}// end if

}// end DrawBlood


// ��ʼ������
void TextInit(void)
{
	// ��ȡͼƬ
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

// �ͷ�����
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

//��ʼ��player
void PlayerInit(void)
{
	int i;  // for

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit, "images/player.bmp");

	// ����BOB
	Create_BOB(&g_bobPlayer,320,240,48,36,18,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		SURFACE_MEMORY,0,16);

	// ��ͼƬ��ȡ��֡
	for (i = 0; i <= 17; i++)
		Load_Frame_BOB16(&g_bobPlayer,&bitmap16bit,i,i%8,i/8,BITMAP_EXTRACT_MODE_CELL);

	// �ͷ�ͼƬ
	Unload_Bitmap_File(&bitmap16bit);

	int nPlayerAnim[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};	// player��������
	// ���ض���֡����
	Load_Animation_BOB(&g_bobPlayer, 0, 16, nPlayerAnim);
	// ���õ�ǰ����
	Set_Animation_BOB(&g_bobPlayer, 0);


	// ���ö�������
	Set_Anim_Speed_BOB(&g_bobPlayer,3);
	
}// end PlayerInit

// plyaer��������
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

	// ��ҹ���Ƶ��
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

// ����player
void PlayerDraw(void)
{
	if ((g_bobPlayer.state & PLAYER_STATE_ALIVE) == PLAYER_STATE_ALIVE)
	{
		Draw_BOB16(&g_bobPlayer, lpddsback);
		Animate_BOB(&g_bobPlayer);
		// ���player�����޵�״̬����������
		if ((g_bobPlayer.state&PLAYER_STATE_INVINCIBLE)==PLAYER_STATE_INVINCIBLE)
		{
			RECT dest,source;
			int i=rand()&3;
			// Ŀ�����
			dest.left = g_bobPlayer.x;
			dest.top = g_bobPlayer.y;
			dest.right = g_bobPlayer.x+g_bobPlayer.width;
			dest.bottom =g_bobPlayer.y+g_bobPlayer.height;
			// Դ����
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

// �ƶ����
void PlayerMove(void)
{
	int dx=0, 
		  dy=0; //���ڱ��洰���ƶ���
	//�ƶ�player��ͼ����
	g_bobPlayer.varsF[INDEXF_WORLDX] += g_bobPlayer.xv;
	g_bobPlayer.varsF[INDEXF_WORLDY] += g_bobPlayer.yv;
	// �õ��ӿ�����
	g_bobPlayer.x = g_bobPlayer.varsF[INDEXF_WORLDX]-g_ptScreen.x;
	g_bobPlayer.y = g_bobPlayer.varsF[INDEXF_WORLDY]-g_ptScreen.y;


	// playerԽ�����
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

	// �ƶ���ʾ����
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

	
	// ��ʾ����Խ�����
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

	// �ƶ�����
	if (dx!=0 || dy!=0)
		StarMove(dx,dy);

	// ���µõ��ӿ�����
	g_bobPlayer.x = g_bobPlayer.varsF[INDEXF_WORLDX]-g_ptScreen.x;
	g_bobPlayer.y = g_bobPlayer.varsF[INDEXF_WORLDY]-g_ptScreen.y;

	//if (g_bobPlayer.xv != 0 || g_bobPlayer.yv != 0)
	//	if (DSound_Status_Sound(g_nSoundPlayerMove)==0)
	//		DSound_Play(g_nSoundPlayerMove);

}// end PlayerMove

// ���״̬����
void PlayerRefresh(void)
{
	// �����ţ�
	if (g_bobPlayer.varsI[INDEXI_HEALTH]<=0)
	{
		// ����
		g_bobPlayer.state = PLAYER_STATE_DEAD;
		g_nGameState = GAME_STATE_LOST;
		return;
		// ��ʱ����������
		//g_bobPlayer.varsI[INDEXI_HEALTH] = 0;
		

	}// end if health

	g_bobPlayer.max_count_1 = g_bobPlayer.varsI[INDEXI_FIRESPEED];
	g_bobPlayer.varsI[INDEXI_MOVESPEED] = PLAYER_MOVESPEED;

	// �ƶ�����״̬
	if ((g_bobPlayer.state & PLAYER_STATE_MOVEUP) == PLAYER_STATE_MOVEUP)
	{
		g_bobPlayer.varsI[INDEXI_MOVESPEED] = POWERUP_MOVE;
		// �Ƿ���
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_MOVEUP;
	}// end moveup

	// ��������״̬
	if ((g_bobPlayer.state & PLAYER_STATE_FIREUP) == PLAYER_STATE_FIREUP)
	{
		g_bobPlayer.max_count_1 = POWERUP_FIREUP;
		// �Ƿ���
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_FIREUP;
	}// end fireup

	// �޵�״̬
	if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) == PLAYER_STATE_INVINCIBLE)
	{
		// �Ƿ���
		if (--g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE] <= 0)
			g_bobPlayer.state &= ~PLAYER_STATE_INVINCIBLE;
	}// end invincible

	// �������Ƶ�ʼ��� 0 ʱΪ�����
	if (g_bobPlayer.counter_1>0)
	{
		g_bobPlayer.counter_1--;
	}
}// end PlayerRefresh



// ��ʾ�����Ϣ
void PlayerInfo(void)
{
	// ��һ�����õ� g_bobPowerup
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			break;
	}
	// Ѫ��ͼ��
	Set_Pos_BOB(&g_bobPowerup[i],16,16);
	g_bobPowerup[i].curr_frame = POWERUP_TYPE_HEALTH;
	Draw_Scaled_BOB16(&g_bobPowerup[i], 16, 16, lpddsback);
	// ը��ͼ��
	Set_Pos_BOB(&g_bobPowerup[i],16,569);
	g_bobPowerup[i].curr_frame = POWERUP_TYPE_BOMB;
	Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);

	int nTimeLen = 100;
	int nPower = 0;
	// �ƶ�����
	if ((g_bobPlayer.state & PLAYER_STATE_MOVEUP) == PLAYER_STATE_MOVEUP)
	{
		Set_Pos_BOB(&g_bobPowerup[i],664,491);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_MOVE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP]*nTimeLen/POWERUP_MOVE_TIME;
		DrawBlood(690,501,4,nPower,nTimeLen);
	}
	// ��������
	if ((g_bobPlayer.state & PLAYER_STATE_FIREUP) == PLAYER_STATE_FIREUP)
	{
		Set_Pos_BOB(&g_bobPowerup[i],664,517);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_FIRE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_FIREUP]*nTimeLen/POWERUP_FIREUP_TIME;
		DrawBlood(690,527,4,nPower,nTimeLen);
	}
	// �޵�
	if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) == PLAYER_STATE_INVINCIBLE)
	{
		Set_Pos_BOB(&g_bobPowerup[i], 664,543);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_INVINCIBLE;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
		nPower = g_bobPlayer.varsI[INDEXI_COUNTER_INVINCIBLE]*nTimeLen/POWERUP_INVINCIBLE_TIME;
		DrawBlood(690,553,4,nPower,nTimeLen);
	}
	// ��
	if ((g_bobPlayer.state & PLAYER_STATE_HAND) == PLAYER_STATE_HAND)
	{
		Set_Pos_BOB(&g_bobPowerup[i], 688+72,569);
		g_bobPowerup[i].curr_frame = POWERUP_TYPE_HAND;
		Draw_Scaled_BOB16(&g_bobPowerup[i], 24, 24, lpddsback);
	}

	// Ѫ��
	DrawBlood(35,20,8,g_bobPlayer.varsI[INDEXI_HEALTH],PLAYER_MAX_HEALTH);

	TCHAR szBuffer[8] = "0000000";

	// ���ը������
	if (g_bobPlayer.varsI[INDEXI_BOMBS]>99)
		wsprintf(szBuffer,"%02d",99);
	else
		wsprintf(szBuffer,"%02d",g_bobPlayer.varsI[INDEXI_BOMBS]);
	TextDraw(40,575,12,12,TEXT_GREEN,szBuffer);	
	
	// ����÷�
	wsprintf(szBuffer,"%07d",g_bobPlayer.varsI[INDEXI_SCORE]);
	TextDraw(700, 16,12,12, TEXT_GREEN,szBuffer); 

	// ��ʾ�л�����ֵ
	if (keyboard_state[DIK_LMENU])
	{
		for (i=1; i<NUM_COLLISION; i++)
		{
			if (g_pbobCollision[i]->state == 0)
				continue;
			int curLife = g_pbobCollision[i]->varsI[INDEXI_HEALTH]*g_pbobCollision[i]->width/g_pbobCollision[i]->varsI[INDEXI_MAXHEALTH];
			DrawBlood(g_pbobCollision[i]->x,g_pbobCollision[i]->y,4,
						curLife,	// ��ǰ����ֵ
						g_pbobCollision[i]->width);	// �������ֵ
		}// end for i

	}
	
}// end PlayerInfo

// �ͷ����
inline void PlayerRelease(void)
{
	Destroy_BOB(&g_bobPlayer);
}// end PlayerRelease

// ��ʼ��׼��
inline void AimInit(void)
{
	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit, "images/aim.bmp");

	// ����BOB
	Create_BOB(&g_bobAim,WINDOW_WIDTH>>1,WINDOW_HEIGHT>>1,32,32,1,
		BOB_ATTR_VISIBLE | BOB_ATTR_SINGLE_FRAME,
		SURFACE_MEMORY,0,16);

	// ��ͼƬ��ȡ��֡
	Load_Frame_BOB16(&g_bobAim,&bitmap16bit,0,0,0,BITMAP_EXTRACT_MODE_ABS);

	// �ͷ�ͼƬ
	Unload_Bitmap_File(&bitmap16bit);
}// end AimInit

// ��׼��
inline void AimDraw(void)
{
	Draw_BOB16(&g_bobAim, lpddsback);
}// end AimDraw

// �ͷ�׼��
inline void AimRelease(void)
{
	Destroy_BOB(&g_bobAim);
}// end AimRelease

// ��ʼ������
void StartInitReset(void)
{
	int i=0;
	for (i=0; i<MAX_STARS; i++)
	{
		// ����ֲ���3��ƽ��
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

// �ƶ����� ������ƶ�֮�����
void StarMove(float dx, float dy)
{
	int i,
		nPanel0dx=0,
		nPanel0dy=0,
		nPanel1dx=0,
		nPanel1dy=0,
		nPanel2dx=0,
		nPanel2dy=0;
	// ����ƽ�����ǵ��ٶ�
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


		// Խ�紦��
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

// ������
void StarDraw(void)
{
	// �����󻺴�
	DDraw_Lock_Back_Surface();

	for (int i=0; i<MAX_STARS; i++)
	{
		// ��С��ͼ���򲻻�
		if (g_Star[i].x > MAP_LEFT && g_Star[i].x < MAP_RIGHT+g_bobMap.width
			&& g_Star[i].y > MAP_TOP && g_Star[i].y < MAP_BOTTOM+g_bobMap.height)
			continue;
		Draw_Pixel16(g_Star[i].x, g_Star[i].y, g_Star[i].usColor, back_buffer,back_lpitch);
	}

	// ����
	DDraw_Unlock_Back_Surface();
}// end StarDraw

// ��ʼ���ӵ�
void PlasmaInit(void)
{
	// �ú�����ʼ���ӵ�

	// the plasma animations
	static int	/*PlasmaAnimPlayer[] = {0,1,2,3,4,5,6,7},*/
				PlasmaAnimPlayer[] = {24,25,26,27,28,29,30,31},
				PlasmaAnimWraith[] = {16,17,18,19,20,21,22,23},
				PlasmaAnimEnemy[]  = {8,9,10,11,12,13,14,15};

	// ��ȡ�ӵ�ͼƬ 
	Load_Bitmap_File(&bitmap16bit, "images/energyfull.bmp");

	// �����ӵ�BOB����
	Create_BOB(&g_bobPlasma[0],0,0,8,8,48,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		SURFACE_MEMORY,0,16);

	// ��ȡͼƬ��֡
	for (int frame=0; frame < 48; frame++)
		Load_Frame_BOB16(&g_bobPlasma[0],&bitmap16bit,frame,frame%8,frame/8,BITMAP_EXTRACT_MODE_CELL);  

	// ����֡����
	Set_Anim_Speed_BOB(&g_bobPlasma[0],1);

	// ��ȡ����
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_PLAYER, 8, PlasmaAnimPlayer);
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_WRAITH, 8, PlasmaAnimEnemy);
	Load_Animation_BOB(&g_bobPlasma[0], PLASMA_ANIM_GUNSHIP, 8, PlasmaAnimEnemy);

	// δ����
	g_bobPlasma[0].state = PlASMA_STATE_OFF;

	// ����MAX_PLASM-1��
	for (int i=1; i < MAX_PLASMA; i++)
	{
		Clone_BOB(&g_bobPlasma[0], &g_bobPlasma[i]);
	} // end for pulse

	// �ͷ�λͼ����
	Unload_Bitmap_File(&bitmap16bit);
}// end PlasmaInit

// �ͷ��ӵ�
void PlasmaRelease(void)
{
	for (int index=0; index < MAX_PLASMA; index++)
		Destroy_BOB(&g_bobPlasma[index]);
}// end PlasmaRelease

// �����ӵ�
void PlasmaFire(int x, int y, float fVx, float fVy, BOB_PTR pbobSource)
{
	// ��һ�����õ��ӵ�BOB  x,y Դ��������
	for (int i=0; i < MAX_PLASMA; i++)
	{
		// OFFΪ����
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
				Set_Animation_BOB(&g_bobPlasma[i],PLASMA_ANIM_PLAYER);	// ����
				g_bobPlasma[i].max_count_1 = pbobSource->varsI[INDEXI_PLASMA_RANGE];	// ��̼���
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


			// ����
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

// �����ӵ�
void PlasmaReset(void)
{
	for (int i = 0; i < MAX_PLASMA; i++)
	{
		g_bobPlasma[i].state = PlASMA_STATE_OFF;
	} // end for i
}// end PlasmaReset

// �ƶ��ӵ�
void PlasmaMove(void)
{
	//״̬�жϺ����ã����м�⣬���д�����ը����
	int i=0,j=0;
	for (i=0; i<MAX_PLASMA; i++)
	{
		if (g_bobPlasma[i].state == PlASMA_STATE_OFF)
			continue;

		g_bobPlasma[i].varsF[INDEXF_WORLDX] += g_bobPlasma[i].xv;
		g_bobPlasma[i].varsF[INDEXF_WORLDY] += g_bobPlasma[i].yv;

		// ����� ���ڣ����������
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
			// �������player���ӵ��˳�
			if (g_bobPlasma[i].anim_state != PLASMA_ANIM_PLAYER)
				break;

			// ����л�δ��������
			if (g_pbobCollision[j]->state != 1)
				continue;

			// test for collision 
			if (Collision_Test(g_pbobCollision[j]->varsF[INDEXF_WORLDX],g_pbobCollision[j]->varsF[INDEXF_WORLDY],
				g_pbobCollision[j]->width, g_pbobCollision[j]->height,
				g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
				g_bobPlasma[i].width, g_bobPlasma[i].height))
			{
				g_bobPlasma[i].state = PlASMA_STATE_OFF;

				// ���ٵл�����
				g_pbobCollision[j]->varsI[INDEXI_HEALTH] -= g_bobPlasma[i].varsI[INDEXI_PLASMA_DAMAGE];

				BurstStart(g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
					g_bobPlasma[i].width<<2, g_bobPlasma[i].height<<1,
					g_pbobCollision[j]->xv, g_pbobCollision[j]->yv);
				break; 
			}// end if collison
		}// end j for

	// �Ƿ����player
	// ����player�ĵ� ����
	if (g_bobPlasma[i].anim_state == PLASMA_ANIM_PLAYER)
		continue;
	if (Collision_Test(g_bobPlayer.varsF[INDEXF_WORLDX],g_bobPlayer.varsF[INDEXF_WORLDY],
		g_bobPlayer.width, g_bobPlayer.height,
		g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
		g_bobPlasma[i].width, g_bobPlasma[i].height))
	{
		g_bobPlasma[i].state = PlASMA_STATE_OFF;
		//��ը
		BurstStart(g_bobPlasma[i].varsF[INDEXF_WORLDX],g_bobPlasma[i].varsF[INDEXF_WORLDY],
			g_bobPlasma[i].width<<2, g_bobPlasma[i].height<<1,
			g_bobPlayer.xv, g_bobPlayer.yv);
		// �����޵о�ȥѪ
		if ((g_bobPlayer.state & PLAYER_STATE_INVINCIBLE) != PLAYER_STATE_INVINCIBLE)
			g_bobPlayer.varsI[INDEXI_HEALTH] -= g_bobPlasma[i].varsI[INDEXI_PLASMA_DAMAGE];

	}// end if collision player

		// ��̼���++
		g_bobPlasma[i].counter_1++;
	}//end for
}// end PlasmaMove

// ���ӵ�
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
				// �ӵ������󻺴�
				Draw_BOB(&g_bobPlasma[i],lpddsback);

				// ���µ�ǰ֡
				Animate_BOB(&g_bobPlasma[i]);
			}
		} // end if

	} // end for i
}// end PlasmaDraw

// ��ʼ��ը��
void BombInit(void)
{
	// �ú�����ʼ��ը����
		

	// ��ȡը��ͼƬ 
	Load_Bitmap_File(&bitmap16bit, "images/bomb2.bmp");

	// �����ӵ�BOB����
	Create_BOB(&g_bobBomb[0],0,0,32,32,8,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// ��ȡͼƬ��֡
	for (int frame=0; frame < 8; frame++)
		Load_Frame_BOB16(&g_bobBomb[0],&bitmap16bit,frame,frame%8,frame/8,BITMAP_EXTRACT_MODE_CELL);  

	// ����֡����
	Set_Anim_Speed_BOB(&g_bobBomb[0],12);

	// δ����
	g_bobBomb[0].state = PlASMA_STATE_OFF;

	// �ͷ�λͼ����
	Unload_Bitmap_File(&bitmap16bit);

	int nBomb[16] = {0,1,2,3,4,5,6,7};	// ��������
	// ���ض���֡����
	Load_Animation_BOB(&g_bobBomb[0], 0, 16, nBomb);
	// ���õ�ǰ����
	Set_Animation_BOB(&g_bobBomb[0], 0);

	// ����MAX_PLASM-1��
	for (int i=1; i < MAX_BOMB; i++)
	{
		Clone_BOB(&g_bobBomb[0], &g_bobBomb[i]);
	} // end for pulse

}// end BombInit

void BombReset(void)	// ����ը��
{
	for (int i = 0; i < MAX_BOMB; i++)
	{
		g_bobBomb[i].state = PlASMA_STATE_OFF;
	} // end for i
}// end BombRese

void BombRelease(void)	// �ͷ�ը��
{
	for (int index=0; index < MAX_BOMB; index++)
		Destroy_BOB(&g_bobBomb[index]);
}// end BombRelease

void BombFire(int x, int y, float fVx, float fVy, int nCount,BOB_PTR pbobSource)	// ����ը��
{
	// ��һ�����õ�ը��BOB
	for (int i=0; i < MAX_BOMB; i++)
	{
		// OFFΪ����
		if (g_bobBomb[i].state == PlASMA_STATE_OFF)
		{
			// ����任Ϊ��ͼ����
			g_bobBomb[i].varsF[INDEXF_WORLDX] = x-g_bobBomb[i].width/2+g_ptScreen.x;
			g_bobBomb[i].varsF[INDEXF_WORLDY] = y-g_bobBomb[i].height/2+g_ptScreen.y;

			g_bobBomb[i].xv = fVx;
			g_bobBomb[i].yv = fVy;

			// ����ը���˺�����ը��Χ����ը��
			g_bobBomb[i].varsI[INDEXI_DAMAGE] = pbobSource->varsI[INDEXI_BOMB_DAMAGE];
			g_bobBomb[i].varsI[INDEXI_RANGE]  = pbobSource->varsI[INDEXI_BOMB_RANGE];
			g_bobBomb[i].varsI[INDEXI_PUSH]   = pbobSource->varsI[INDEXI_BOMB_PUSH];
			g_bobBomb[i].max_count_1 = nCount;

			g_bobBomb[i].curr_frame = 0;
			g_bobBomb[i].state =  PLASMA_STATE_ON;

			// ����
			DSound_Play(g_nSoundBomb);
			DSound_Set_Volume(g_nSoundBomb, 90);

		} // end if

	} // end for
}// end BombFire

void BombMove(void)	// �ƶ�ը��
{
	int i,j;

	for (i=0; i<MAX_BOMB; i++)
	{
		if (g_bobBomb[i].state == PlASMA_STATE_OFF)
			continue;

		g_bobBomb[i].varsF[INDEXF_WORLDX] += g_bobBomb[i].xv;
		g_bobBomb[i].varsF[INDEXF_WORLDY] += g_bobBomb[i].yv;

		// �ɵ�Ŀ��?
		if (++g_bobBomb[i].counter_1 == g_bobBomb[i].max_count_1)
		{
			g_bobBomb[i].state = PlASMA_STATE_OFF;
			g_bobBomb[i].counter_1 = 0;
			// ����
			float fBombX = g_bobBomb[i].varsF[INDEXF_WORLDX]+g_bobBomb[i].width/2;
			float fBombY = g_bobBomb[i].varsF[INDEXF_WORLDY]+g_bobBomb[i].height/2;
			int nBrust =BurstStart(fBombX,fBombY,
					g_bobPlayer.varsI[INDEXI_BOMB_RANGE],g_bobPlayer.varsI[INDEXI_BOMB_RANGE],0,0);
			
			// ����ը���ĵл�
			for (j=1; j<NUM_COLLISION; j++)
			{
				if (g_pbobCollision[j]->state == 0)
					continue;

				// �жϺ�player�ľ���,���ڷ�Χ�ڣ�����
				int dx = (g_pbobCollision[j]->varsF[INDEXF_WORLDX]+g_pbobCollision[j]->width/2)-fBombX;
				int dy = (g_pbobCollision[j]->varsF[INDEXF_WORLDY]+g_pbobCollision[j]->height/2)-fBombY;

				int length = Fast_Distance_2D(dx,dy);	// ����

				
				if (length <= g_bobPlayer.varsI[INDEXI_BOMB_RANGE]/2)
				{
					// ����˺�
					g_pbobCollision[j]->varsI[INDEXI_HEALTH]-=g_bobPlayer.varsI[INDEXI_BOMB_DAMAGE];
				

					// ��ֹ����Ϊ0
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

void BombDraw(void)	// ��ը��
{
	for (int i=0; i<MAX_BOMB; i++)
	{
		// test if plasma pulse is in flight
		if (g_bobBomb[i].state == PLASMA_STATE_ON)
		{
			// ����任
			g_bobBomb[i].x = g_bobBomb[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobBomb[i].y = g_bobBomb[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			if (g_bobBomb[i].x > 0 && g_bobBomb[i].x < WINDOW_WIDTH
				&& g_bobBomb[i].y > 0 && g_bobBomb[i].y < WINDOW_HEIGHT)
			{
				// ը���������󻺴�
				Draw_BOB(&g_bobBomb[i],lpddsback);
				Animate_BOB(&g_bobBomb[i]);
			}
		} // end if

	} // end for i
}// end BombDraw

// ��ʼ����ը
void BurstInit(void)
{
	// ��ʼ����ը 

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit, "images/explBig.BMP");

	// ������ըbob
	Create_BOB(&g_bobBrusts[0],0,0,256,219,14,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// ����ͼƬ��֡
	for (int frame=0; frame < 14; frame++)
		Load_Frame_BOB16(&g_bobBrusts[0],&bitmap16bit,frame,frame%4,frame/4,BITMAP_EXTRACT_MODE_CELL);  

	// ��������
	Set_Anim_Speed_BOB(&g_bobBrusts[0],1);

	// ��ը�Ĵ�С�����ڷ���
	g_bobBrusts[0].varsI[INDEXI_BURST_WIDTH] = g_bobBrusts[0].width;
	g_bobBrusts[0].varsI[INDEXI_BURST_HEIGHT] = g_bobBrusts[0].height;

	// δ����
	g_bobBrusts[0].state = BURST_STATE_OFF;

	for (int burst=1; burst<MAX_BURSTS; burst++)
	{
		memcpy(&g_bobBrusts[burst], &g_bobBrusts[0], sizeof(BOB));
	} // end for burst

	// �ͷ�ͼƬ
	Unload_Bitmap_File(&bitmap16bit);

}// end BrustsInit

// �ͷű�ը
void BurstRelease(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
		Destroy_BOB(&g_bobBrusts[index]);
}// end BrustRelease

// ���ñ�ը
void BurstReset(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
		g_bobBrusts[index].state = BURST_STATE_OFF;
}// end BurstReset

// ��ʼ��ը
int BurstStart(int x, int y, int width, int height, float xv,float yv)
{
	// ��һ�����õı�ը x,y�Ǳ�ը������λ�� ����ֵ�Ǳ�����ı�ը��ID
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// is this burst available?
		if (g_bobBrusts[index].state == BURST_STATE_OFF)
		{
			// ��ʼ������
			Set_Anim_Speed_BOB(&g_bobBrusts[index],1);
			g_bobBrusts[index].curr_frame = 0;

			g_bobBrusts[index].xv = xv;
			g_bobBrusts[index].yv = yv;

			// ��С��������
			g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH] = width;
			g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT] = height;

			g_bobBrusts[index].varsF[INDEXF_WORLDX] = x-width/2;
			g_bobBrusts[index].varsF[INDEXF_WORLDY] = y-height/2;

			// ����任
			g_bobBrusts[index].x = g_bobBrusts[index].varsF[INDEXF_WORLDX]-g_ptScreen.x;
			g_bobBrusts[index].y = g_bobBrusts[index].varsF[INDEXF_WORLDY]-g_ptScreen.y;

			// ����
			g_bobBrusts[index].state = BURST_STATE_ON;

			ParticleStartExplosion(PARTICLE_TYPE_FLICKER,0,40+rand()%80,
							x,y,xv,yv,g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH]/6);

			// ��ը����
			for (int sound_index=0; sound_index < MAX_SOUND_EXPL; sound_index++)
			{
				// ��ID�Ƿ���ʹ��
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

// �ƶ���ը
void BurstMove(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// ����Ĳ����ƶ�
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

// ����ը
void BurstDraw(void)
{
	for (int index=0; index < MAX_BURSTS; index++)
	{
		// test if bursts pulse is in flight
		if (g_bobBrusts[index].state == BURST_STATE_ON)
		{
			// ����任
			g_bobBrusts[index].x = g_bobBrusts[index].varsF[INDEXF_WORLDX]-g_ptScreen.x;
			g_bobBrusts[index].y = g_bobBrusts[index].varsF[INDEXF_WORLDY]-g_ptScreen.y;

			// is scaling needed
			if (g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH]!=g_bobBrusts[index].width 
			|| g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT]!=g_bobBrusts[index].height)
			{
				//����
				Draw_Scaled_BOB(&g_bobBrusts[index],
					g_bobBrusts[index].varsI[INDEXI_BURST_WIDTH],g_bobBrusts[index].varsI[INDEXI_BURST_HEIGHT],
					lpddsback);
			}
			else // ������
				Draw_BOB(&g_bobBrusts[index],lpddsback);

			// ���¶���
			Animate_BOB(&g_bobBrusts[index]);

		} // end if

	} // end for index
}// BurstDraw

// ��ʼ������
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
	// ��һ��δ���������
	for (int index=0; index < MAX_PARTICLES; index++)
	{
		if (g_Particle[index].nState == PARTICLE_STATE_ALIVE)
			continue;  

		g_Particle[index].nState = PARTICLE_STATE_ALIVE;
		g_Particle[index].nType  = nType;
		g_Particle[index].x     = x;	// ��ͼ����
		g_Particle[index].y     = y;
		g_Particle[index].xv    = vx;
		g_Particle[index].yv    = vy;
		g_Particle[index].nCounter     = 0;
		g_Particle[index].nMaxCount   = nTime;     

		// ������ɫ��Χ
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

// ��ʼ��ը���� x,y �����ǵ�ͼ����
void ParticleStartExplosion(int nType, int nColor, int nTime, int x, int y, int vx, int vy, int nParticles)
{
	while(--nParticles >=0)
	{
		// ����Ƕ�
		int ang = rand()%64;
		// �������
		float vel = 2+rand()%4;

		ParticleStart(nType,RAND_RANGE(PARTICLE_COLOR_RED,PARTICLE_COLOR_WHITE),nTime,
			x+RAND_RANGE(-4,4),y+RAND_RANGE(-4,4), 
			vx+g_fCosTab64[ang]*vel, vy+g_fSinTab64[ang]*vel);        

	} // end while

} // end Start_Particle_Explosion

// �ƶ�����
void ParticleMove(void)
{
	for (int index=0; index<MAX_PARTICLES; index++)
	{
		if (g_Particle[index].nState == PARTICLE_STATE_ALIVE)
		{
			// �ƶ�
			g_Particle[index].x+=g_Particle[index].xv;
			g_Particle[index].y+=g_Particle[index].yv;

			// ��ɫ����
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

// ������
void ParticleDraw(void)
{
	// lock back surface
	DDraw_Lock_Back_Surface();

	for (int index=0; index<MAX_PARTICLES; index++)
	{
		// test if particle is alive
		if (g_Particle[index].nState==PARTICLE_STATE_ALIVE)
		{
			// ����任
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

// ��ʼ������
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

	g_bobPowerup[0].max_count_1 = POWERUP_LIVETIME;	// ��������ʱ��
	g_bobPowerup[0].max_count_2 = POWERUP_HAND_GET;	// �����õ�ʱ��

	// make copies
	for (index=1; index < MAX_POWERUPS; index++)
	{
		Clone_BOB(&g_bobPowerup[0], &g_bobPowerup[index]);
	} // end for index
}//end PowerupInit

// ���õ���
void PowerupReset(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
		g_bobPowerup[i].state = POWERUP_STATE_DEAD;
}// end PowerupRese

// �ͷŵ���
void PowerupRelease(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
		Destroy_BOB(&g_bobPowerup[i]);
}// end PowerupRelease

// ��������
void PowerupStart(float x,float y)
{
	int nType = rand()%200; // ��������������������������͵��߻�����

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
	else	// ������
		return;

	for (int i=0; i<MAX_POWERUPS; i++)
	{
		if (g_bobPowerup[i].state == POWERUP_STATE_ALIVE)
			continue;

		g_bobPowerup[i].state = POWERUP_STATE_ALIVE;
		g_bobPowerup[i].varsI[INDEXI_POWERUP_TYPE] = nType;
	
		g_bobPowerup[i].curr_frame = nType;	// ����Ҫ����ͼƬ

		g_bobPowerup[i].varsF[INDEXF_WORLDX] = x - g_bobPowerup[i].width/2;
		g_bobPowerup[i].varsF[INDEXF_WORLDY] = y - g_bobPowerup[i].height/2;

		g_bobPowerup[i].counter_1 = 0;
		g_bobPowerup[i].counter_2 = 0;
		break;

	} // end for
	
}// end PowerupRelease

// ������
void PowerupDraw(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		// ����δ�����
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			continue;
		// ����任
		g_bobPowerup[i].x = g_bobPowerup[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
		g_bobPowerup[i].y = g_bobPowerup[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

		Draw_BOB16(&g_bobPowerup[i], lpddsback);

	}// end for
}// end PowerupDraw

// ���µ���
void PowerupRefresh(void)
{
	for (int i=0; i<MAX_POWERUPS; i++)
	{
		// ����δ�����
		if (g_bobPowerup[i].state == POWERUP_STATE_DEAD)
			continue;

		// �����任
		g_bobPowerup[i].x = g_bobPowerup[i].varsF[INDEXF_WORLDX]-g_ptScreen.x;
		g_bobPowerup[i].y = g_bobPowerup[i].varsF[INDEXF_WORLDY]-g_ptScreen.y;

		// �ж��Ƿ�������ײ �� ������
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
 
		

		// �Ƿ񳬹�������
		if (++g_bobPowerup[i].counter_1 >= g_bobPowerup[i].max_count_1)
			g_bobPowerup[i].state = POWERUP_STATE_DEAD;
		

	}// end for
}// end PowerupRefresh

// ���߱���
void PowerupBeGeted(BOB_PTR bobBeGeted)
{
	switch (bobBeGeted->varsI[INDEXI_POWERUP_TYPE])
	{
	case POWERUP_TYPE_HEALTH:	// ��Ѫ
		g_bobPlayer.varsI[INDEXI_HEALTH]+=POWERUP_HEALTH;
		if (g_bobPlayer.varsI[INDEXI_HEALTH]>PLAYER_MAX_HEALTH)
			g_bobPlayer.varsI[INDEXI_HEALTH] = PLAYER_MAX_HEALTH;
		break;

	case POWERUP_TYPE_MOVE:		// �ƶ�����
		g_bobPlayer.state |= PLAYER_STATE_MOVEUP;
		g_bobPlayer.varsI[INDEXI_COUNTER_MOVEUP] = POWERUP_MOVE_TIME;
		break;

	case POWERUP_TYPE_BOMB:		// ը��
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

// ��λͼ�ṹ��������
int LoadSurface(LPDIRECTDRAWSURFACE7 lpdds, // Ŀ�����
				BITMAP_FILE_PTR bitmap, // Դλͼ
				int cx,int cy,   // cell or absolute pos. to scan image from
				int width, int height,int mode)        
{
	// ��λͼ��ȡ������

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	// ���治����
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



	// Դָ��
	source_ptr = (USHORT*)bitmap->buffer + cy*bitmap->bitmapinfoheader.biWidth+cx;

	// ��ñ����ַ
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

// ��һ������BLT����һ��������
int BltSuerface(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // ��Ŀ������λ��
				int nSrcWidth, int nSrcHeight) // Դ��С
{
	// ��һ��������Ƶ���һ��������

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

// ��һ������BLT����һ�������� ������
int BltSuerfaceScaled(LPDIRECTDRAWSURFACE7 src,LPDIRECTDRAWSURFACE7 dest, 
				int nDestX, int nDestY,  // ��Ŀ������λ��
				int nSrcWidth, int nSrcHeight, // Դ��С
				int nDestWidth, int nDestHeight) // Ŀ���С
{
	// ��һ��������Ƶ���һ��������

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


// ��ʼ��Wraith
void WraithInit(void)
{
	int i=0;

	// ����BOB
	Create_BOB(&g_bobWraith[0], 0, 0, 64, 64, 64,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit,"Images/wraithb.bmp");
	// ����ͼƬ��֡ ��ɫWraith
	for (i=0; i<32; i++)
	{
		Load_Frame_BOB16(&g_bobWraith[0], &bitmap16bit, i, i%8, i/8, BITMAP_EXTRACT_MODE_CELL);
	}// end for
	// unload data infile
	Unload_Bitmap_File(&bitmap16bit);

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit,"Images/wraithr.bmp");
	// ����ͼƬ��֡ ��ɫWraith
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

// �ͷ�wraith
void WraithRelease(void)
{
	for (int i=0; i<MAX_WRAITH; i++)
	{
		Destroy_BOB(&g_bobWraith[i]);
	}
}// end WraithRelease

// �ƶ�Wraith
void WraithReset(void)
{
	for (int i=0; i<MAX_WRAITH; i++)
	{
		g_bobWraith[i].state = ENEMY_STATE_DEAD;
	}
}// end WraithReset

// ����Wraith
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

			// �������λ��
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fSinTab64[iang];

			g_bobWraith[i].state = ENEMY_STATE_ALIVE;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobWraith[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobWraith[i].counter_1 = 0;	// ����Ƶ�ʼ���
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

			--g_nWraith;// δ����wraith��-1
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

			// �������λ��
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(MIN_DIST,MAX_DIST)*g_fSinTab64[iang];

			g_bobWraith[i].state = ENEMY_STATE_ALIVE;
			g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobWraith[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobWraith[i].counter_1 = 0;	//����Ƶ�ʼ���
			g_bobWraith[i].max_count_1 = FIRECOUNT_WRAITHR;
			//g_bobWraith[i].counter_1 = 0;	// ���ٷ����ж�Ƶ��
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

			--g_nWraith;// δ����wraith��-1
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

			// �������λ��
			iang = rand()%64;
			fX = WORLD_WIDTH*g_fCosTab64[iang];
			fY = g_bobPlayer.varsF[INDEXF_WORLDY]+ RAND_RANGE(WINDOW_HEIGHT,WINDOW_HEIGHT+WINDOW_HEIGHT>>1)*g_fSinTab64[iang];

			g_bobBee[i].state = ENEMY_STATE_ALIVE;
			g_bobBee[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobBee[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobBee[i].counter_1 = 0;	//����Ƶ�ʼ���
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

			--g_nBee;// δ����wraith��-1

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

			// �������λ��
			iang = rand()%64;
			fX = g_bobPlayer.varsF[INDEXF_WORLDX]+ RAND_RANGE(MIN_DIST,MAX_DIST);
			fY = g_bobPlayer.varsF[INDEXF_WORLDX]+RAND_RANGE(MIN_DIST,MAX_DIST);

			g_bobGunship[i].state = ENEMY_STATE_ALIVE;
			g_bobGunship[i].varsI[INDEXI_FIGHT_TYPE] = nType;
			g_bobGunship[i].varsI[INDEXI_DYING] = DYING_COUNTER;

			g_bobGunship[i].counter_1 = 0;	//����Ƶ�ʼ���
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

			--g_nGunship;// δ����wraith��-1

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

		// ����
		//g_bobWraith[i].varsF[INDEXF_MOVESPEED]+= g_bobWraith[i].varsF[INDEXF_ACC];
		// �Ƿ���
		//if (g_bobWraith[i].varsF[INDEXF_MOVESPEED] > g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
		//	g_bobWraith[i].varsF[INDEXF_MOVESPEED] = g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED];

		if (g_bobWraith[i].state == ENEMY_STATE_ALIVE)
		{
			// ���¼��� �����ж�Ƶ��
			//g_bobWraith[i].counter_2 = g_bobWraith[i].max_count_2;

			// �жϵл�����
			if (g_bobWraith[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobWraith[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE]+=g_bobWraith[i].varsI[INDEXI_SCORE];
				continue;
			}

			//����ǰһ������
			g_bobWraith[i].varsI[INDEXI_DIRPRE] = g_bobWraith[i].varsI[INDEXI_DIR];

			float dydx,dx,dy;
			int tmpDir; // Ŀ�귽��

			dx = (g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 �жϷ���
			if( dx == 0 && dy > 0)				// ������ֱ���ϣ���W������
				tmpDir = DIR_NNNN;
			else if( dx ==0 && dy < 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_SSSS;
			else if( dx > 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_WWWW;
			else if( dx < 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_EEEE;
			else if( dx > 0 && dy > 0)			// ��������,����dy,dx�ı�ֵȷ���Ƕȷ�Χ,����tan(x)�ĵ�����
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx >0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx < 0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else								// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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

			// ת��������ʧ
			g_bobWraith[i].varsF[INDEXF_MOVESPEED] = g_fCosTab64[16+tmpDir*4]*g_bobWraith[i].varsF[INDEXF_MOVESPEED];

		
		
			// �����ٶ�
			//g_bobWraith[i].xv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			//g_bobWraith[i].yv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			g_bobWraith[i].xv += g_bobWraith[i].varsF[INDEXF_ACC]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			g_bobWraith[i].yv += g_bobWraith[i].varsF[INDEXF_ACC]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			// �Ƿ���
			if (g_bobWraith[i].xv*g_bobWraith[i].xv+g_bobWraith[i].yv*g_bobWraith[i].yv
							>g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobWraith[i].xv *= 0.8;
				g_bobWraith[i].yv *= 0.8;
			}

			// ����β��
			ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 1,
						g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2+RAND_RANGE(-4,4),
						g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2+RAND_RANGE(-4,4), 
						(-int(g_bobWraith[i].xv)), (-int(g_bobWraith[i].yv)));

			// �жϺ�player�ľ���,���ڷ�Χ�ڣ�����
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// ����

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

			// �Ƿ���
			if (g_bobWraith[i].xv*g_bobWraith[i].xv+g_bobWraith[i].yv*g_bobWraith[i].yv
							>g_bobWraith[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobWraith[i].xv *= 0.8;
				g_bobWraith[i].yv *= 0.8;
			}

			// �����С�ı�ը
			nRand = RAND_RANGE(16,64);

			if ((rand()%4)==1)
				BurstStart(g_bobWraith[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobWraith[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobWraith[i].xv,g_bobWraith[i].yv);  

			// �Ұ�ͷ
			nRand = RAND_RANGE(-1,1);
			g_bobWraith[i].varsI[INDEXI_DIR] = (g_bobWraith[i].varsI[INDEXI_DIR]+nRand+16)%16;
			g_bobWraith[i].curr_frame = g_bobWraith[i].varsI[INDEXI_DIR];

			// ��������
			if (--g_bobWraith[i].varsI[INDEXI_DYING]<=0)
			{
				g_nWraith++;	// δ�������+1
				g_bobWraith[i].state = ENEMY_STATE_DEAD;

				// ������
				PowerupStart(g_bobWraith[i].varsF[INDEXF_WORLDX]+g_bobWraith[i].width/2,
					g_bobWraith[i].varsF[INDEXF_WORLDY]+g_bobWraith[i].height/2);
			}

		}// end else dying

		// �ٶȼӵ���������
		g_bobWraith[i].varsF[INDEXF_WORLDX] += g_bobWraith[i].xv;
		g_bobWraith[i].varsF[INDEXF_WORLDY] += g_bobWraith[i].yv;

		

	}// end for
}//end wraithMove

// ����
void WraithDraw(void)
{
	int i,frame;
	for (i=0; i<MAX_WRAITH; i++)
	{
		if (g_bobWraith[i].state != ENEMY_STATE_DEAD)
		{
			g_bobWraith[i].x = g_bobWraith[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobWraith[i].y = g_bobWraith[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			// �л�ͬ����ͬ����β�����̵�ͼƬ
			frame = ((g_bobWraith[i].varsI[INDEXI_FIGHT_TYPE]-FIGHT_TYPE_WRAITHB)*2+ rand()%2)*16;
			g_bobWraith[i].curr_frame += frame;
			Draw_BOB16(&g_bobWraith[i], lpddsback);
			g_bobWraith[i].curr_frame -= frame;
		}//end if
	}// end for
}// end WraithDraw

// ��ʼ����ͼ
void MapInit(void)
{
	// BMPͼƬ
	Load_Bitmap_File(&bitmap16bit, "Images/hudart.bmp");

	Create_BOB(&g_bobMap,0,0,24,26,4,
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
				SURFACE_MEMORY,0,16);

	for (int i=0; i<4; i++)
		Load_Frame_BOB16(&g_bobMap, &bitmap16bit,i,i,0,BITMAP_EXTRACT_MODE_CELL);

	Unload_Bitmap_File(&bitmap16bit);

	Set_Pos_BOB(&g_bobMap, 320, 0);

}// end MapInit

// �ͷŵ�ͼ
inline void MapRelease(void)
{
	Destroy_BOB(&g_bobMap);
}// end MapRelease

// ���Ƶ�ͼ
void MapDraw(void)
{
	int i=0;
	int nMapX, nMapY;
	USHORT usColor;
	// lock back surface
	DDraw_Lock_Back_Surface();

	// ��Wraith
	for (i=0; i<MAX_WRAITH; i++)
	{
		if (g_bobWraith[i].state == ENEMY_STATE_DEAD)
			continue;
		
		// �ڵ�ͼ���򲻻�
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

	// ��BEE
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state == ENEMY_STATE_DEAD)
			continue;

		// �ڵ�ͼ���򲻻�
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

	// ��Gunship
	for (i=0; i<MAX_GUNSHIP; i++)
	{
		if (g_bobGunship[i].state == ENEMY_STATE_DEAD)
			continue;

		// �ڵ�ͼ���򲻻�
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

	// ��Player
	nMapX = g_bobPlayer.varsF[INDEXF_WORLDX]/10;
	nMapY = g_bobPlayer.varsF[INDEXF_WORLDY]/10;

	usColor = RGB16Bit(255,255,255);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	usColor = RGB16Bit(0,255,255);
	Draw_Pixel16(MAP_LEFT+nMapX+1, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY+1, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX-1, MAP_TOP+nMapY, usColor, back_buffer, back_lpitch);
	Draw_Pixel16(MAP_LEFT+nMapX, MAP_TOP+nMapY-1, usColor, back_buffer, back_lpitch);

	// ����ʾ����С�� ��-2,��-2����+2����
	usColor = RGB16Bit(0,100,0);
	nMapX = MAP_LEFT+g_ptScreen.x/10;
	nMapY = MAP_TOP+g_ptScreen.y/10;
	HLine16(nMapX-2, nMapX+WINDOW_WIDTH/10-2, nMapY, usColor,back_buffer, back_lpitch);
	HLine16(nMapX-2, nMapX+WINDOW_WIDTH/10-2, nMapY+WINDOW_HEIGHT/10-2,usColor, back_buffer, back_lpitch);
	VLine16(nMapY, nMapY+WINDOW_HEIGHT/10-2, nMapX-2,usColor, back_buffer, back_lpitch);
	VLine16(nMapY, nMapY+WINDOW_HEIGHT/10-2, nMapX+WINDOW_WIDTH/10-2,usColor, back_buffer, back_lpitch);

	DDraw_Unlock_Back_Surface();

	// ����ͼ���ĸ���
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

// ������ײ��������
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

// ��ײ����
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
		  yfb = 0;	// ��ײ����Ҫ�õ��ı����� ʹ��ʱ��˵��
	int bob_a,bob_b;

	// ��Һ͵л�����ײ
	for (bob_a = 0; bob_a<NUM_COLLISION; bob_a++)
	{
		if (g_pbobCollision[bob_a]->state == 0)
			continue;
		for (bob_b = bob_a+1; bob_b < NUM_COLLISION; bob_b++)
		{
			if (g_pbobCollision[bob_b]->state == 0) 
				continue;

			// ��ײ�ж�
			if (Collision_Test(g_pbobCollision[bob_a]->varsF[INDEXF_WORLDX],g_pbobCollision[bob_a]->varsF[INDEXF_WORLDY],
								g_pbobCollision[bob_a]->width, g_pbobCollision[bob_a]->height,
								g_pbobCollision[bob_b]->varsF[INDEXF_WORLDX],g_pbobCollision[bob_b]->varsF[INDEXF_WORLDY],
								g_pbobCollision[bob_b]->width,g_pbobCollision[bob_b]->height))
			{
				// the g_pbobCollision have made contact, compute response
				// ��������n< nabx, naby>
				nabx = (g_pbobCollision[bob_b]->varsF[INDEXF_WORLDX]+ g_pbobCollision[bob_b]->width/2)
					- (g_pbobCollision[bob_a]->varsF[INDEXF_WORLDX]+ g_pbobCollision[bob_a]->width/2 );
				naby = (g_pbobCollision[bob_b]->varsF[INDEXF_WORLDY]+ g_pbobCollision[bob_b]->height/2)
					- (g_pbobCollision[bob_a]->varsF[INDEXF_WORLDY]+ g_pbobCollision[bob_b]->height/2 );
				length = sqrt(nabx*nabx + naby*naby);

				// ��n ��񻯣�����ɵ�λ����
				nabx/=length;
				naby/=length;

				// ����������n��ֱ������t
				tabx =  -naby;
				taby =  nabx;

				// tangential is also normalized since it's just a rotated normal vector

				// step 2: compute all the initial velocities
				// notation ball: (a,b) initial: i, final: f, n: normal direction, t: tangential direction
				// ����ײ���ߵ��ٶ�ͶӰ��n,t����
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

				// �ɶ����غ�Ͷ����غ����n�����ϵ��ٶȷ���

				ma = g_pbobCollision[bob_a]->varsF[INDEXF_MASS];
				mb = g_pbobCollision[bob_b]->varsF[INDEXF_MASS];

				vafn = (mb*vbin*(g_fE+1) + vain*(ma - g_fE*mb)) / (ma + mb);
				vbfn = (ma*vain*(g_fE+1) - vbin*(ma - g_fE*mb)) / (ma + mb);

				// now luckily the tangential components are the same before and after, so
				vaft = vait;
				vbft = vbit;

				// ��n,t����ϵ�µ��ٶ�ת����x,y����ϵ

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

// ��ʼ��Bee
void BeeInit(void)
{
	int i=0;

	// ����BOB
	Create_BOB(&g_bobBee[0], 0, 0, 22, 18, 32,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit,"Images/bee.bmp");
	// ����ͼƬ��֡ ��ɫWraith
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

// �ͷ�Bee
void BeeRelease(void)
{
	for (int i=0; i<MAX_BEE; i++)
	{
		Destroy_BOB(&g_bobBee[i]);
	}
}// end BeeRelease

// ����Bee
void BeeReset(void)
{
	for (int i=0; i<MAX_BEE; i++)
	{
		g_bobBee[i].state = ENEMY_STATE_DEAD;
	}
}// end WraithReset

//�ƶ�Bee
void BeeMove(void)
{
	int i=0,nRand=0;
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state == ENEMY_STATE_DEAD)
			continue;

		if (g_bobBee[i].state == ENEMY_STATE_ALIVE)
		{
			// �жϵл�����
			if (g_bobBee[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobBee[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE]+=g_bobBee[i].varsI[INDEXI_SCORE];
				continue;
			}

			//����ǰһ������
			g_bobBee[i].varsI[INDEXI_DIRPRE] = g_bobBee[i].varsI[INDEXI_DIR];

			float dydx,dx,dy;
			int tmpDir; // Ŀ�귽��

			dx = (g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 �жϷ���
			if( dx == 0 && dy > 0)				// ������ֱ���ϣ���W������
				tmpDir = DIR_BEE_NNNN;
			else if( dx ==0 && dy < 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_BEE_SSSS;
			else if( dx > 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_BEE_WWWW;
			else if( dx < 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_BEE_EEEE;
			else if( dx > 0 && dy > 0)			// ��������,����dy,dx�ı�ֵȷ���Ƕȷ�Χ,����tan(x)�ĵ�����
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx >0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx < 0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else								// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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

			// ת��������ʧ
			g_bobBee[i].varsF[INDEXF_MOVESPEED] = g_fCosTab64[16+tmpDir*4]*g_bobBee[i].varsF[INDEXF_MOVESPEED];

			// �����ٶ�
			//g_bobWraith[i].xv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fCosTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];
			//g_bobWraith[i].yv = g_bobWraith[i].varsF[INDEXF_MOVESPEED]*g_fSinTab64[g_bobWraith[i].varsI[INDEXI_DIR]*4];

			g_bobBee[i].xv += g_bobBee[i].varsF[INDEXF_ACC]*g_fCosTab64[((g_bobBee[i].varsI[INDEXI_DIR]+4)%16)*4];
			g_bobBee[i].yv += g_bobBee[i].varsF[INDEXF_ACC]*g_fSinTab64[((g_bobBee[i].varsI[INDEXI_DIR]+4)%16)*4];

			// �Ƿ���
			if (g_bobBee[i].xv*g_bobBee[i].xv+g_bobBee[i].yv*g_bobBee[i].yv
							>g_bobBee[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobBee[i].xv *= 0.8;
				g_bobBee[i].yv *= 0.8;
			}

			// ����β��
			ParticleStart(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED, 1,
				g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2+RAND_RANGE(-4,4),
				g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2+RAND_RANGE(-4,4), 
				(-int(g_bobBee[i].xv)), (-int(g_bobBee[i].yv)));

			// �жϺ�player�ľ���,���ڷ�Χ�ڣ�����
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// ����

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

			// �Ƿ���
			if (g_bobBee[i].xv*g_bobBee[i].xv+g_bobBee[i].yv*g_bobBee[i].yv
							>g_bobBee[i].varsF[INDEXF_MAX_MOVESPEED])
			{
				g_bobBee[i].xv *= 0.8;
				g_bobBee[i].yv *= 0.8;
			}

			// �����С�ı�ը
			nRand = RAND_RANGE(16,64);

			if ((rand()%4)==1)
				BurstStart(g_bobBee[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobBee[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobBee[i].xv,g_bobBee[i].yv);  

			// �Ұ�ͷ
			nRand = RAND_RANGE(-1,1);
			g_bobBee[i].varsI[INDEXI_DIR] = (g_bobBee[i].varsI[INDEXI_DIR]+nRand+16)%16;
			g_bobBee[i].curr_frame = g_bobBee[i].varsI[INDEXI_DIR];

			// ��������
			if (--g_bobBee[i].varsI[INDEXI_DYING]<=0)
			{
				g_nBee++;	// δ�������+1
				g_bobBee[i].state = ENEMY_STATE_DEAD;

				// ������
				PowerupStart(g_bobBee[i].varsF[INDEXF_WORLDX]+g_bobBee[i].width/2,
					g_bobBee[i].varsF[INDEXF_WORLDY]+g_bobBee[i].height/2);
			}

		}// end else dying

		// �ٶȼӵ���������
		g_bobBee[i].varsF[INDEXF_WORLDX] += g_bobBee[i].xv;
		g_bobBee[i].varsF[INDEXF_WORLDY] += g_bobBee[i].yv;



	}// end for
}//end BeeMove

// ����
void BeeDraw(void)
{
	int i,frame;
	for (i=0; i<MAX_BEE; i++)
	{
		if (g_bobBee[i].state != ENEMY_STATE_DEAD)
		{
			g_bobBee[i].x = g_bobBee[i].varsF[INDEXF_WORLDX] - g_ptScreen.x;
			g_bobBee[i].y = g_bobBee[i].varsF[INDEXF_WORLDY] - g_ptScreen.y;

			// �л�ͬ����ͬ����β������ͼƬ
			frame = (rand()%2)*16;
			g_bobBee[i].curr_frame += frame;
			Draw_BOB16(&g_bobBee[i], lpddsback);
			g_bobBee[i].curr_frame -= frame;
		}//end if
	}// end for
}// end BeeDraw

// ��ʼ��Gunship
void GunshipInit(void)
{
	int i=0;

	// ����BOB
	Create_BOB(&g_bobGunship[0], 0, 0, 160, 70, 32,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		SURFACE_MEMORY,0,16);

	// ��ȡͼƬ
	Load_Bitmap_File(&bitmap16bit,"Images/gunship.bmp");
	// ����ͼƬ��֡ 
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

// �ͷ�
void GunshipRelease(void)
{
	for (int i=0; i<MAX_GUNSHIP; i++)
	{
		Destroy_BOB(&g_bobGunship[i]);
	}
}// end GunshipRelease

// ����
void GunshipReset(void)
{
	for (int i=0; i<MAX_GUNSHIP; i++)
	{
		g_bobGunship[i].state = ENEMY_STATE_DEAD;
	}
}// end GunshipReset

// �ƶ�
void GunshipMove(void)
{
	int i=0,nRand=0;
	for (i=0; i<MAX_GUNSHIP; i++)
	{
		if (g_bobGunship[i].state == ENEMY_STATE_DEAD)
			continue;

		if (g_bobGunship[i].state == ENEMY_STATE_ALIVE)
		{
			// �жϵл�����
			if (g_bobGunship[i].varsI[INDEXI_HEALTH]<=0)
			{
				g_bobGunship[i].state = ENEMY_STATE_DYING;
				g_bobPlayer.varsI[INDEXI_SCORE] += g_bobGunship[i].varsI[INDEXI_SCORE];
				continue;
			}

			float dydx,dx,dy;
			int tmpDir; // Ŀ�귽��

			dx = (g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2+g_bobPlayer.xv) ;
			dy = (g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2)
				-(g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2+g_bobPlayer.yv);

			// 1 �жϷ���
			if( dx == 0 && dy > 0)				// ������ֱ���ϣ���W������
				tmpDir = DIR_NNNN;
			else if( dx ==0 && dy < 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_SSSS;
			else if( dx > 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_WWWW;
			else if( dx < 0 && dy == 0)			// ������ֱ���ϣ���W������
				tmpDir = DIR_EEEE;
			else if( dx > 0 && dy > 0)			// ��������,����dy,dx�ı�ֵȷ���Ƕȷ�Χ,����tan(x)�ĵ�����
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx >0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else if( dx < 0 && dy < 0)			// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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
			else								// ��������
			{
				dydx = fabs(dy/dx); // ȡ����ֵ
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

			// δ���������
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

			// Խ��ת��
			if (g_bobGunship[i].varsF[INDEXF_WORLDX] < 0)
			{
				// ת���ұ�
				g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] = DIR_RIGHT;
				g_bobGunship[i].varsF[INDEXF_ACC] = ACC_GUNSHIP;
			}				
			if (g_bobGunship[i].varsF[INDEXF_WORLDX] > WORLD_WIDTH)
			{
				// ת�����
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

			// ����β��
			// ������ʻ
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
			else//����
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

			// �жϺ�player�ľ���,���ڷ�Χ�ڣ����� , ���̫Զ��ת��
			dx = (g_bobPlayer.varsF[INDEXF_WORLDX]+g_bobPlayer.width/2)
				-(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/2);
			dy = (g_bobPlayer.varsF[INDEXF_WORLDY]+g_bobPlayer.height/2)
				-(g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);

			float length = sqrt(dx*dx+dy*dy);	// ����

			if (length <= g_bobGunship[i].varsI[INDEXI_ATTACK_RANGE]
			&& --g_bobGunship[i].counter_1 <= 0)
			{
				g_bobGunship[i].counter_1 = g_bobGunship[i].max_count_1;

				float xv = dx/length * g_bobGunship[i].varsF[INDEXF_PLASMA_SPEED];
				float yv = dy/length * g_bobGunship[i].varsF[INDEXF_PLASMA_SPEED];

				// ������ʻ
				if (g_bobGunship[i].varsI[INDEXI_DIRGUNSHIP] == DIR_RIGHT)
				{
					// ���������
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+82,	g_bobGunship[i].varsF[INDEXF_WORLDY]+11,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+140,	g_bobGunship[i].varsF[INDEXF_WORLDY]+17,
						xv,yv, &g_bobGunship[i]);
					PlasmaFire(g_bobGunship[i].varsF[INDEXF_WORLDX]+143,	g_bobGunship[i].varsF[INDEXF_WORLDY]+26,
						xv,yv, &g_bobGunship[i]);
				}
				else // ����
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
			// �����С�ı�ը
			nRand = RAND_RANGE(32,120);

			if ((rand()%4)==1)
				BurstStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+rand()%40, g_bobGunship[i].varsF[INDEXF_WORLDY]+rand()%8,
				nRand, (nRand >> 2) + (nRand >> 1),
				g_bobGunship[i].xv,g_bobGunship[i].yv);  

			// ����
			nRand = RAND_RANGE(-5,5);
			g_bobGunship[i].varsF[INDEXF_WORLDX] += nRand;
			g_bobGunship[i].varsF[INDEXF_WORLDY] += nRand;

			// ��������
			if (--g_bobGunship[i].varsI[INDEXI_DYING]<=0)
			{
				g_nGunship++;	// δ�������+1
				g_bobGunship[i].state = ENEMY_STATE_DEAD;

				// ��2������
				PowerupStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width/4,
					g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);
				PowerupStart(g_bobGunship[i].varsF[INDEXF_WORLDX]+g_bobGunship[i].width*3/4,
					g_bobGunship[i].varsF[INDEXF_WORLDY]+g_bobGunship[i].height/2);
			}

		}// end else dying

		// �ٶȼӵ���������
		g_bobGunship[i].varsF[INDEXF_WORLDX] += g_bobGunship[i].xv;
		g_bobGunship[i].varsF[INDEXF_WORLDY] += g_bobGunship[i].yv;

	}// end for
}// end GunshipMove

// ����
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

// ��ʼ��������ʾ
void StoryInit(void)
{
	// ��ȡ����ͼƬ
	Load_Bitmap_File(&bitmap16bit, "images/begin.BMP");
	Create_Bitmap(&g_imgBegingBack,0,0,800,600,16);
	Load_Image_Bitmap16(&g_imgBegingBack, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

	// ��λͼ 
	Load_Bitmap_File(&bitmap16bit, "images/storyBegin.BMP");

	// ������ĻBOB
	Create_BOB(&g_bobStory,100,600,600,1200,1,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// ��ȡλͼ��֡
	Load_Frame_BOB16(&g_bobStory,&bitmap16bit,0,0,0,BITMAP_EXTRACT_MODE_ABS);  
	Unload_Bitmap_File(&bitmap16bit);

}// end StoryInit

// ����
void StoryBegin(void)
{
	DDraw_Lock_Back_Surface();

	// draw background
	Draw_Bitmap16(&g_imgBegingBack, back_buffer, back_lpitch,0);

	DDraw_Unlock_Back_Surface();

	g_bobStory.y = g_bobStory.y  - 1;

	Draw_BOB16(&g_bobStory, lpddsback);
}// end StroyBegin

// �ͷ�
void StoryRelease(void)
{
	Destroy_Bitmap(&g_imgBegingBack);
}// StoryRelease







