#pragma once

#include "mysql.h"
#include "TableInfo.h"

#pragma comment(lib, "libmysql.lib")

class CDatabaseManager
{
private:
	CDatabaseManager();
	~CDatabaseManager();

private:
	static CDatabaseManager* m_pInst;

public:
	static CDatabaseManager* GetInst();
	static void DestroyInst();

private:
	MYSQL* m_pConnector;
	MYSQL	m_Conn;

public:
	bool Init(const char* pHost, const char* pID, const char* pPassword, const char* pDBName, unsigned int iPort);
	bool Login(int* pUserNumber, const char* pID, const char* pPassword);
	bool SignUp(const char* Name, const char* Email, const char* pID, const char* pPassword);
	bool GetPlayerPos(int CharacterNumber, float& PosX, float& PosY, float& PosZ, float& ScaleX, float& ScaleY, float& ScaleZ, float& RotX, float& RotY, float& RotZ);
	bool GetCharacterTable(CharacterTable& table, int iUserNumber);
	bool GetCharacterInfoTable(CharacterInfoTable& table, int iCharacterNumber);
	bool SaveCharacterTable(CharacterInfoTable& table, int UserNumber);
	bool SaveCharacterInfo(CharacterInfoTable& table, int UserNumber, int Count);

	bool UpdateCharacterInfo(CharacterInfoTable table);
	bool DeleteCharacter(int iCharacterNumber, int iUserNumber);
	bool GetPlayerInventoryList(int iCharacterNumber, int iUserNumber);
	bool UpdateTransform(int UserNumber, int CharacterNumber, float PosX, float PosY, float PosZ, float ScaleX, float ScaleY, float ScaleZ,
		float RotX, float RotY, float RotZ);
	bool CreateCharacterTransform(int UserNumber, int CharacterNumber);

	bool CreateCharacterSkill(int CharacterNumber);
	bool SaveCharacterSkill(int CharacterNumber, int Skill1_1, int Skill1_2, int Skill1_3, int Skill2_1, int Skill2_2, int Skill2_3, int Skill2_4, int Skill3_1, int Skill3_2, int Skill3_3);
	bool GetCharacterSkillInfo(int CharacterNumber);

	bool SaveCharacterQuest();
	bool GetCharacterQuest(int& CharacterNumber, int& OptionData);
};

