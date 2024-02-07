#pragma once
#include<vector>

typedef struct _tagCharacterTable
{
	int	iTableNumber;
	int	iUserNumber;
	int	iCharacterCount;
	int	iCharacterNumber[3];
}CharacterTable, *PCharacterTable;

typedef struct _tagCharacterInfoTable
{
	int		iCharacterNumber;
	int		iCharacterTableNumber;
	int		iUserNumber;

	int		iMapLevel;
	char	strJob[64];
	char	strName[64];
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
	int		iExpMax;
	//unsigned __int64		iExpMax;
	int		iMoney;
	int		iSkillPoint;
}CharacterInfoTable, *PCharacterInfoTable;


 
//----------------------------------------------
//
//enum ItemType
//{
//	IT_WEAPON,
//	IT_ARMOR,
//	IT_POTION,
//	IT_END
//};
//
//enum ItemOptionType
//{
//	IOT_ATTACK,
//	IOT_ARMOR,
//	IOT_HP,
//	IOT_MP,
//	IOT_CRITICALRATIO,
//	IOT_CRITICALDAMAGE,
//	IOT_MOVESPEED,
//	IOT_END
//};
//
//struct _tagItemOption
//{
//	ItemOptionType	Type;
//	int		Option;
//};
//
//struct _tagItemInfo
//{
//	char	strName[32];
//	ItemType	Type;
//	int		Price;
//	int		Sell;
//	std::vector<_tagItemOption> vecOption;
//};

