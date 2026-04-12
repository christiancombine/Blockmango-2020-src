#pragma once

namespace LORD
{
	/*
	��Ч���ù���Ķ���
	*/
	enum SoundType
	{
		ST_Invalid = -1,

		//��Ч��UI����
		ST_ClickSure,						// 0 ���ȷ����ť
		ST_ClickCancel,						// 1 ���ȡ����ť
		ST_ErrorOperation,					// 2 �������
		ST_InvalidOperation,				// 3 ��Ч����
		ST_OpenPanel,						// 4 ��һ�����
		ST_ClosePanel,						// 5 �ر�һ�����
		ST_OpenSubPanel,					// 6 �򿪶�������ͼ����
		ST_CloseSubPanel,					// 7 �رն�������ͼ����
		ST_PopUpTips,						// 8 ������ʾ��
		ST_GetMail,							// 9 �ʼ�
		ST_PrivateChat,						// 10 ˽��
		ST_NormalCountDown,					// 11 ����ʱ����ͨ
		ST_UrgentCountDown,					// 12 ����ʱ������
		ST_GetItem,							// 13 �����Ʒ
		ST_GetCoin,							// 14 ��ý��
		ST_GetDiamond,						// 15 �����ʯ

		//��Ч��ս����
		ST_MalePlayerBeHit,					// 16 ������ܻ�
		ST_FemalePlayerBeHit,				// 17 Ů����ܻ�
		ST_PlayerBeHit3,					// 18 ����ܻ�3
		ST_PullTheBow,						// 19 ����
		ST_Shoot,							// 20 ����
		ST_LaunchRocket,					// 21 ������
		ST_ShootArrowSp,					// 22 ���������
		ST_MonsterWound,					// 23 ��ʬ����
		ST_MonsterDead,						// 24 ��ʬ����
		ST_PlaceGrassBlock,					// 25 ���òݷ���
		ST_BreakGrassBlock,					// 26 �ƻ��ݷ���
		ST_PlaceDirtBlock,					// 27 �����෽��
		ST_BreakDirtBlock,					// 28 �ƻ��෽��
		ST_PlaceWoodBlock,					// 29 ����ľ����
		ST_BreakWoodBlock,					// 30 �ƻ�ľ����
		ST_PlaceSoftBlock,					// 31 �������ʷ���
		ST_BreakSoftBlock,					// 32 �ƻ����ʷ���
		ST_PlaceSandBlock,					// 33 ����ɳ�ӷ���
		ST_BreakSandBlock,					// 34 �ƻ�ɳ�ӷ���
		ST_PlaceSnowBlock,					// 35 ����ѩ����	
		ST_BreakSnowBlock,					// 36 �ƻ�ѩ����
		ST_PlaceStoneBlock,					// 37 ����ʯ����
		ST_BreakStoneBlock,					// 38 �ƻ�ʯ����
		ST_PlaceIronBlock,					// 39 ����������
		ST_BreakIronBlock,					// 40 �ƻ�������
		ST_PlaceGoldBlock,					// 41 ���ý𷽿�
		ST_BreakGoldBlock,					// 42 �ƻ��𷽿�
		ST_PlaceDiamondBlock,				// 43 ������ʯ����
		ST_BreakDiamondBlock,				// 44 �ƻ���ʯ����
		ST_StepOnCloth,						// 45 ���߲��ʵ���
		ST_StepOnGrass,						// 46 ���߲ݵ�
		ST_StepOnSand,						// 47 ����ɳ��
		ST_StepOnGravel,					// 48 ������ʯ��
		ST_StepOnLadder,					// 49 ��������
		ST_StepOnStone,						// 50 ����ʯ�ذ�
		ST_StepOnWood,						// 51 ����ľ�ذ�
		ST_Fuse,							// 52 ����
		ST_BrokenGlass,						// 53 �鲣��
		ST_WoodButton,						// 54 ľ��ť
		ST_OpenWoodDoor,					// 55 ��ľ��
		ST_CloseWoodDoor,					// 56 ��ľ��
		ST_OpenIronDoor,					// 57 ������
		ST_CloseIronDoor,					// 58 ������
		ST_Portal,							// 59 ������ portal.portal
		ST_Fizz,							// 60 ����˻˻��
		ST_Explosion,						// 61 ��ը random.explode
		ST_Stealth,							// 62 ����״̬
		ST_SpeedUp,							// 63 ����״̬
		ST_Jump,							// 64 ��Ծ״̬
		ST_NightVison,						// 65 ҹ��״̬

		//��Ч��������
		ST_Rain,							// 66 ����
		ST_Thunder,							// 67 ����
		ST_Lava,							// 68 ���� liquid.lava
		ST_Water,							// 69 ��ˮ liquid.water

		//��Ϸ��������ö�٣�
		ST_Click,							// 70 ��� random.click
		ST_OpenDoor,						// 71 ���� random.door_open
		ST_CloseDoor,						// 72 ���� random.door_close
		ST_BreakAnvil,						// 73 random.anvil_break
		ST_UseAnvil,						// 74 random.anvil_use
		ST_LandAnvil,						// 75 random.anvil_land
		ST_Glass,							// 76 random.glass
		ST_Swim,							// 77 liquid.swim	
		ST_ThornsHurt,						// 78 damage.thorns
		ST_LiquidSplash,					// 79 liquid.splash
		ST_BowHit,							// 80 random.bowhit
		ST_Pop,								// 81 random.pop
		ST_Break,							// 82 random.break
		ST_FallBig,							// 83 damage.fallbig
		ST_FallSmall,						// 84 damage.fallsmall
		ST_Orb,								// 85 random.orb
		ST_LevelUp,							// 86 random.levelup
		ST_Burp,							// 87 random.burp
		ST_IgniteFire,						// 88 fire.ignite
		ST_OpenChest,						// 89 random.chestopen
		ST_CloseChest,						// 90 random.chestclosed
		ST_PistonOut,						// 91 tile.piston.out
		ST_PistonIn,						// 92 tile.piston.in
		ST_LavaPop,							// 93 liquid.lavapop
		ST_Fire,							// 94 fire.fire
		ST_BreakGravelBlock,				// 95 
		ST_PlaceGravelBlock,				// 96
		ST_PlaceGlassBlock,					// 97
		ST_BreakGlassBlock,					// 98
		ST_StepOnGlass,						// 99
		ST_PlaceClothBlock,					// 100
		ST_BreakClothBlock,					// 101
		ST_StepOnSnow,						// 102
		ST_PlaceLadderBlock,				// 103
		ST_BreakLadderBlock,				// 104
		ST_PlaceAnvilBlock,					// 105
		ST_StepOnAnvil,						// 106
		ST_Drink,							// 107 ��ˮ random.drink
		ST_Eat,								// 108 �Զ��� random.eat
		ST_MobWitherSpawn,					// 109 mob.wither.spawn
		ST_MobEndDragon,					// 110 mob.enddragon.end
		ST_MobZombieUnfect,					// 111 mob.zombie.unfect
		ST_MobZombieInfect,					// 112 mob.zombie.infect
		ST_MobBatTakeoff,					// 113 mob.bat.takeoff
		ST_MobZombieWoodbreak,				// 114 mob.zombie.woodbreak
		ST_MobWitherShoot,					// 115 mob.wither.shoot
		ST_MobZombieMetal,					// 116 mob.zombie.metal
		ST_MobZombieWood,					// 117 mob.zombie.wood
		ST_MobGhastFireball,				// 118 mob.ghast.fireball
		ST_MobGhastCharge,					// 119 mob.ghast.charge

		ST_BuildWarGrade1 = 135,			// 135 
		ST_BuildWarGrade2,					// 136
		ST_BuildWarGrade3,					// 137
		ST_BuildWarGrade4,					// 138
		ST_BuildWarGrade5,					// 139
		ST_BuildWarGrade6,					// 140
		ST_BuildWarTelePos,					// 141
		ST_BuildWarTrans,					// 142
		ST_BuildWarFire,					// 143

		ST_Air_Dig = 311,					// 311 air_dig
		ST_Kill_Person = 312,				// 312 kill person
		ST_Cannon = 313,				// 313 use cannon
		ST_Convert = 314,				// 313 use Convert
		ST_Get_Item_1057_1066 = 315,				// 313 use cannon
		ST_Get_Item_1067_1084 = 316,				// 313 use cannon
		ST_Kill_Spider = 317,				// 313 use cannon
		ST_Task_Done = 318,				// 313 use cannon
		ST_Tree_Die = 319,				// 313 use cannon
		ST_Tree_Up_Level = 320,				// 313 use cannon
		ST_Buy_Dress = 321,					//321 buy dress
		ST_Open_Box = 322,					//322 open box
		ST_clear_manor = 323,
		ST_friend_invite = 324,
		ST_glide_wing = 325,
		ST_go_home = 326,
		ST_open_fly = 327,
		ST_place_template = 328,
		ST_rotate_click = 329,
		ST_rotate_done = 330,
		ST_sit_down = 331,
		ST_take_car = 332,
		ST_tiger_machine = 333,
		ST_GameCompleteAdd = 334,				//10041 ��Ϸ������������Ʒ��Ч
		ST_GameCompleteHide = 335,			//10042 ��Ϸ������������Ч��ʧ
		ST_PIXEL_DUAN_WEI = 336,				//10043 ���������λ����
		ST_race_ready = 377,
		ST_race_fail = 378,
		ST_race_win = 379,

		ST_open_inventory = 400,
		ST_close_inventory = 401,
		ST_select_item = 402,

		//10000�Ժ��ID������BGM��
		ST_BgmStart = 10000,		// 10000
		ST_BgmThemeSea = ST_BgmStart,	// 10000 ��������1
		ST_BgmThemeHome,					// 10001 ��������2
		ST_BgmThemeDead,					// 10002 ����
		ST_BgmThemeGameOver,				// 10003 ��Ϸ����
		ST_AircraftSound,					//10004 �ɻ���Ч
		ST_Loading,							//10005	��������������
		ST_ThemeSnowallBattle,				//10006	ѩ����������
		ST_ThemeSnowallBattleWaiting,		//10007	ѩ��ȴ���������
		ST_ThemeZombie,						//10008	��ʬ��ս��������
		ST_ThemeZombieWaiting,				//10009	��ʬ��ս�ȴ���������
		ST_ThemeSkywar,						//10010 �յ���������
		ST_ThemeSkywarWaiting,				//10011	�յ��ȴ���������
		ST_ThemeTNTrun,						//10012	TNT��������
		ST_ThemeSandDigger,				    //10013	��ɳ�ȴ���������
		ST_ThemeSandDiggerWaiting,		    //10014	��ɳ��������
		ST_ThemeJailBreak,					//10015	���˵ȴ���������
		ST_ThemeJailBreakWaiting,			//10016	������������
		ST_ThemeHungerGame,					//10017	������Ϸ�ȴ���������
		ST_ThemeHungerGameWaiting,			//10018	������Ϸ��������
		ST_ThemeHideAndSeek,				//10019	��èè�ȴ���������
		ST_ThemehideAndSeekWaiting,			//10020	��èè��������
		ST_ThemeClanwar,					//10021	����ս����������
		ST_ThemeMurdermystery,			    //10022	����ɱ����������
		ST_GameCompleteWin,					//10023 ��Ϸ������ʤ��
		ST_GameCompleteLose,				//10024	��Ϸ������ʧ��
		ST_TurnToZombie,                    //10025 �佩ʬ
		ST_Landing,							//10026 ����
		ST_Hit,								//10027 ������Ч
		ST_Crash,							//10028 ������Ч
		ST_ParkourBGM,						//10029 �����ܿ���������
		ST_BuildBattle,						//10030 ����ս��
		ST_ThrowPotWaiting,					//10031 ������ս�ȴ���������
		ST_Ranchers,						//10032
		ST_GameCompleteDraw,				//10033 ��Ϸ������ƽ��
		ST_Chase_Sneer,						//10034 ��èè������Ч
		ST_Bird_Simulator,					//10035 Bird_Simulator
		ST_PIXEL_GUN_HALL,					//10036 �����������BGM
		ST_PIXEL_GUN_GAME,					//10037 ���������ϷBGM
		ST_Bird_Simulator_Convert,			//10038
		ST_BED_WAR_HALL_BGM,				//10039 �𴲴���
		ST_BLOCK_CITY_BGM,				//10040 BlockCity
		ST_SkyBlockMain_BGM,				//10041 SkyBlockMain
		ST_SkyBlockMineCave_BGM,				//10042 SkyBlockMineCave
		ST_SkyBlockProduct_BGM,				//10043 SkyBlockProduct
		ST_WALKING_DEAD_BGM,                //10044 WalkingDeadBGM
		ST_SkyBlockMain_Christmas_BGM,				//10045 SkyBlockMainChristmas
		ST_SkyBlockMineCave_Christmas_BGM,				//10046 SkyBlockMineCaveChristmas

		ST_Total = 99999,
	};
}
