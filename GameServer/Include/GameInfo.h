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
	GP_GAMEIN,	// InGame�� ó�� ����
	GP_LEVELSTART,

	GP_USERAPPEAR,// ���� ���� ���
	GP_USERDISAPPEAR,
	GP_OTHERUSERDISAPPEAR,

	GP_SYNC,
	// Ŭ�󿡼� �޴� �뵵�� ���� �̻��
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
	NT_SENTRY, // ���
	NT_TOTEM, // ����
	NT_CREATURE, // ��ȯ��
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

// ��ų ����
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

enum class ESkillProperty // �Ӽ�
{
	Physical, // ����
	Magical, // ���� Psychic�� ����
	Fire, // ȭ��
	Frozen, // �ñ�
	Lightning, // ����
	Posion, // �� Plagued ����
	Divine, // �ż�
};

enum class ESkillOptionType
{
	Damage, // ��������ġ
	PercentDamage, // �߰�������
	AugmentPropt, // �Ӽ��ο�

	ATKUp,
	DEFUp,
	ATKDown,
	DEFDown,

	Heals,
	PercentHeals,
	EnhancesHP, //  �Ͻ������� Maxü�� ����
	EnhancesMP,//
	HPDown, //
	HPRecovery,//ü�� ȸ���ӵ�
	MPRecovery,//���� ȸ���ӵ�

	IncreaseMoveSpeed,
	IncreaseATKSpeed,
	SlowDown, // �̼ӵ�ȭ
	Cripple, // ���ӵ�ȭ

	DamageReturned, //������ �ݻ�
	DamageImmunity, //���ظ鿪
	PercentLifeSteal,//����(�ּҼ�ġ ���ϰ�) �� ���ظ� �� ���ݷ��� % ���� �� ü���� %���� �����ؾ���...

	//DOT ��������
	Bleed,//����
	Poison,//�ߵ�
	Burn,//ȭ��
	Shock,//����
	Freeze,//����

	Stun,//����
	BlindEnemy, //�� �Ǹ� : ���� ��� ������ ��ȿ
	Weaken, // ���ֿ�(�ɷ�ġ ���� ����)
	Summon, //��ȯ�� ��ȯ / �ͷ����� ��ġ���� ���� 
	UnSummon, //��ȯ�� ����

	Sneak, //���� (stealth, DarkSide) ��׷ΰ� �Ȳ�����.
	Taunt, //����
	Kinematic, // �����̵�ȿ�� ������, ��Ȧ���� ����
	Transform,//����
	Polymorph, //����
	Resurrection // ��Ȱ
};


// Once, Duration, Interval ���� 50, 5, 1 �����Ǿ��ٸ�
// 5�ʵ��� �� 1�ʸ��� 50�� �������� �ش�.
enum class ESkillActionType
{
	Once,
	Multi, 
	Duration,	// ���ӽð�
	Interval	// �ð�����
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
	int		SerialNumber;	// ������ ������ ��� ������ ��ȣ, ����� ��� ���� ��ȣ
	int		OptionData;	
};

// ����Ʈ ������ �����ϱ� ���� �����ͺ��̽��� ����� �ű⿡ ����Ʈ ������ �̸� �����ϴ���
// �ƴϸ� ���Ϸ� ����Ʈ ��� ������ ����� ������ ���۵� �� ����Ʈ ������ �ҷ��´�.
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
