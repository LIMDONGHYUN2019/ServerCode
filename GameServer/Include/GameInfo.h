#pragma once

#include "Engine.h"
#include "Vector3.h"
#include "CriticalSection.h"

extern CCriticalSection	g_MainCrt;

#define	MAX_USER	100
#define	EMAIL_SIZE	128
#define	ID_SIZE		64
#define	PASS_SIZE	64
#define JOBNAME_SIZE 64
#define	DESC_SIZE	512
#define ID_CharacterMax 3
#define INVENTORY_MAX 30
#define GroupUser_Max 4

#define ITEMNAME_SIZE 32
#define ItemDrop_SIZE 3
#define GOLDDROP_SIZE 500

#define QUESTNAME_SIZE 64

#define	AUTO_SAVE_TIME	600.f

#define	LOGIN_PORT	6380

#define	GAMESERVER_IP	"127.0.0.1"
#define	GAMESERVER_PORT	6381

#define	ROOMSERVER_IP	"127.0.0.1"
#define	ROOMSERVER_PORT	6382

#define	CHATTINGSERVER_IP	"127.0.0.1"
#define	CHATTINGSERVER_PORT	6383

#define	AISERVER_IP	"127.0.0.1"
#define	AISERVER_PORT	6384

#define	MONITORINGSERVER_IP "127.0.0.1"
#define	MONITORINGSERVER_PORT	6385

enum GAME_PROTOCOL
{
	GP_CONNECT,
	GP_DISCONNECT,
	GP_CONNECT_FAIL,

	GP_CREATECHARACTER,
	GP_SELECT_CHARACTERINFO,
	GP_CREATE_CHARACTERINFO,
	GP_DELETE_CHARACTERINFO,

	GP_GAMESTART,
	GP_GAMEIN,	// InGame에 처음 들어갈때
	GP_LEVELSTART,

	GP_USERAPPEAR,// 유저 등장 헤더
	GP_USERDISAPPEAR,
	GP_OTHERUSERDISAPPEAR,

	GP_SYNC,
	// 클라에서 받는 용도로 현재 미사용
	GP_MOVESTART,
	GP_MOVE,
	GP_MOVEEND,
	GP_ROTATION,

	GP_OTHERUSERAPPEAR,
	GP_OTHERMOVESTART,
	GP_OTHERMOVE,
	GP_OTHERMOVEEND,
	GP_ATTACK,

	GP_CHARACTERLEVELUP,

	GP_PLAYERATTACKDAMAGE,
	GP_MONSTERCREATE,
	GP_MONSTERAI,
	GP_MONSTERDEATH,
	GP_MONSTERITEMDROP,

	GP_SkillList,
	GP_SkillUp,
	GP_SkillSave,

	GP_STORELIST,
	GP_INVENTORYLIST,
	GP_INVENTORYADD,
	GP_INVENTORYDELETE,
	GP_INVENTORYGOLD,

	GP_QUESTLIST,
	GP_QUESTCOMPLETE,

	GP_TRADEREQUEST,
	GP_TRADEREQUESTACCEPT,
	GP_TRADECANCEL,
	GP_TRADEADD,
	GP_TRADEDELETE,
	GP_TRADEREADY,
	GP_TRADESUCCESS,

	GP_GROUPCREATE,
	GP_GROUPREQUEST,
	GP_GROUPACCEPT,
	GP_GROUPCANCEL,
	GP_GROUPLEAVE,
	GP_GROUPKICK,
	GP_GROUPSUCCESS,

	GP_MAINPORTAL,
	GP_SEVAROGBOSSROOM,

	GP_FINDMONSTERPATH,

	GP_ALIVE
};

enum GameObjectProtocol
{
	MA_IDLE,

	MA_TRACESTART,
	MA_TRACE,
	MA_TRACEEND,

	MA_PATROLSTART,
	MA_PATROL,
	MA_PATROLEND,

	MA_DAMAGE,

	MA_ATTACK,
	MA_ATTACKEND,

	MA_SKILL1,
	MA_SKILL1END,

	MA_SKILL2,
	MA_SKILL2END
};

enum class GameObject_Type
{
	User,
	Monster,
	NPC
};

enum class AI_Type
{
	Idle,
	Trace,
	Attack,
	Death,
	Skill1,
	Skill2,
	Patrol
};

enum Monster_Type
{
	MT_Minion,
	MT_Golem,
	MT_Troll,
	MT_Boss,
	MT_FinalBoss,
	MT_Sevarog
};

enum NPC_Type
{
	NT_SENTRY, // 경비병
	NT_TOTEM, // 토템
	NT_CREATURE, // 소환수
	NT_PET
};

enum class ItemType
{
	IT_WEAPON,
	IT_ARMOR,
	IT_HELMET,
	IT_SHIELD,
	IT_POTION,
	IT_END
};

enum ItemOptionType
{
	IOT_ATTACK,
	IOT_ARMOR,
	IOT_HPMAX,
	IOT_MPMAX,
	IOT_HPRECOVERY,
	IOT_MPRECOVERY,
	IOT_CRITICALRATIO,
	IOT_CRITICALDAMAGE,
	IOT_MOVESPEED,
	IOT_END
};

enum class EItemGrade
{
	Normal,
	Rare,
	Epic,
	Unique,
	Legendary
};

enum class ELevel
{
	None,
	MainLevel,
	DesertLevel,
	SevarogBossRoom
};

typedef struct _tagMemberInfo
{
	char	strName[NAME_SIZE];
	char	strEmail[EMAIL_SIZE];
	char	strID[ID_SIZE];
	char	strPass[PASS_SIZE];
}MemberInfo, * PMemberInfo;

struct _tagCharacterInfo
{
	ELevel	MapLevel; //4
	char	strJob[JOBNAME_SIZE];
	char	strName[NAME_SIZE];
	int		iAttack;
	int		iArmor;
	float	fAttackSpeed;
	float	fMoveSpeed;
	float	fCritical;
	float	fCriticalDamage;
	int		iHP;
	int		iHPMax;
	int		iMP;
	int		iMPMax;
	int		iLevel;
	int		iExp;
	//unsigned __int64		iExpMax;
	int		iExpMax;
	int		iMoney;
	int		iSkillPoint;
};

struct _tagItemOption
{
	ItemOptionType	Type;
	int		Option;
};

struct _tagItemInfo
{
	char	strName[ITEMNAME_SIZE];
	EItemGrade Grade;
	ItemType	Type;
	int		Price;
	int		Sell;
	vector<_tagItemOption> vecOption;
};

enum MonitoringList
{
	LoginServer,
	GameServer,
	ChattingServer,
	MonsterAIServer
};

enum class AttackType
{
	None,
	NormalAttack,
	Skill1,
	Skill2,
};

struct PacketQueueData
{
	int	Protocol;
	int	Length;
	BYTE	Packet[MAX_BUFFER_LENGTH];
	class CUser* pSession;

	PacketQueueData()	:
		Protocol(-1),
		Length(0),
		Packet{},
		pSession(nullptr)
	{
	}
};

enum class ETargetDetectType
{
	None = 0,
	User = 0x1,
	NPC = 0x2
};

typedef int TargetDetectType;

enum class EPlayerAnimationType : unsigned char
{
	Idle,
	Run,
	Attack,
	Run_Attack,
	Death,
	Hit,
	Skill1,
	Skill2,
	Skill3
};

// 스킬 정보
enum class ESkillType
{
	Passive,
	Active
};

enum class ESkillJob
{
	GrayStone,
	TwinBlast,
	Serath,
	End
};

enum class ESkillProperty // 속성
{
	Physical, // 물리
	Magical, // 마법 Psychic도 포함
	Fire, // 화염
	Frozen, // 냉기
	Lightning, // 번개
	Posion, // 독 Plagued 역병
	Divine, // 신성
};

enum class ESkillOptionType
{
	Damage, // 정해진수치
	PercentDamage, // 추가데미지
	AugmentPropt, // 속성부여

	ATKUp,
	DEFUp,
	ATKDown,
	DEFDown,

	Heals,
	PercentHeals,
	EnhancesHP, //  일시적으로 Max체력 증가
	EnhancesMP,//
	HPDown, //
	HPRecovery,//체력 회복속도
	MPRecovery,//마나 회복속도

	IncreaseMoveSpeed,
	IncreaseATKSpeed,
	SlowDown, // 이속둔화
	Cripple, // 공속둔화

	DamageReturned, //데미지 반사
	DamageImmunity, //피해면역
	PercentLifeSteal,//흡혈(최소수치 정하고) 이 피해를 내 공격력의 % 인지 적 체력의 %인지 결정해야함...

	//DOT 지속피해
	Bleed,//출혈
	Poison,//중독
	Burn,//화상
	Shock,//감전
	Freeze,//빙결

	Stun,//기절
	BlindEnemy, //적 실명 : 적의 모든 데미지 무효
	Weaken, // 저주용(능력치 절반 감소)
	Summon, //소환수 소환 / 터렛같은 설치물도 포함 
	UnSummon, //소환수 해제

	Sneak, //은신 (stealth, DarkSide) 어그로가 안끌린다.
	Taunt, //도발
	Kinematic, // 강제이동효과 끌어당김, 블랙홀같은 마법
	Transform,//변신
	Polymorph, //변이
	Resurrection // 부활
};


// Once, Duration, Interval 에서 50, 5, 1 지정되었다면
// 5초동안 매 1초마다 50의 데미지를 준다.
enum class ESkillActionType
{
	Once,
	Multi, 
	Duration,	// 지속시간
	Interval	// 시간간격
};

typedef struct _tagSkillOption
{
	ESkillOptionType	SkillOptionType;
	float				SkillOptionData;
}SkillOption, * PSkillOption;

typedef struct _tagSkillAction
{
	ESkillActionType	SkillActionType;
	float				SkillActionData;
}SkillAction, * PSkillAction;

typedef struct _tagSkillInfo
{
	ESkillType	eType;
	ESkillJob	eJob;
	ESkillProperty ePropt;
	int		iSkillNumber;
	int		iSkillMaxLevel;
	int		iRequiredLevel;
	float	fCooldown;
	char	strName[NAME_SIZE];
	char	strDesc[DESC_SIZE];
	vector<SkillOption>	vecOption;
	vector<SkillAction>	vecAction;
	vector<int>	vecNextSkillNumber;
	vector<int>	vecPrevSkillNumber;
}SkillInfo, * PSkillInfo;

enum class ESkillUpMessage
{
	Success,
	MaxLevelFail,
	SkillPointFail,
	RequiredLevelFail,
	LinkedSkillFail
};

enum class EQuestType : UINT8
{
	QT_COLLECTION,
	QT_HUNTER,
	QT_SPOT
};

struct _tagQuestOption
{
	EQuestType	Type;
	int		SerialNumber;	// 아이템 수집일 경우 아이템 번호, 사냥일 경우 몬스터 번호
	int		OptionData;	
};

// 퀘스트 정보를 저장하기 위한 데이터베이스를 만들고 거기에 퀘스트 정보를 미리 저장하던가
// 아니면 파일로 퀘스트 목록 파일을 만들고 서버가 시작될 때 퀘스트 정보를 불러온다.
struct _tagQuestInfo
{
	int		QuestNumber;
	char	strName[QUESTNAME_SIZE];
	char	strDesc[DESC_SIZE];
	_tagQuestOption	Option;
	int		QuestExp;

	_tagQuestInfo()	:
		strName{},
		QuestNumber(0),
		strDesc{},
		Option{},
		QuestExp(0)
	{
	}
};
