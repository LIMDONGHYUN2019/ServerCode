
#include "SkillTree.h"
#include "Skill.h"
#include "../User.h"
#include "SkillManager.h"
#include "PathManager.h"
#include "../User.h"
#include "Stream.h"

CSkillTree::CSkillTree() :
	m_Owner(nullptr),
	m_iSkillPoint(0)
{
}

CSkillTree::CSkillTree(const CSkillTree& Tree)
{
	m_vecSkillList.clear();

	size_t	Size = Tree.m_vecSkillList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		CSkill* Skill = Tree.m_vecSkillList[i]->Clone();

		m_vecSkillList.push_back(Skill);
	}
}

CSkillTree::~CSkillTree()
{
}

void CSkillTree::AddSkillInfo(SkillInfo* Info)
{
	CSkill* Skill = new CSkill;

	Skill->Init(Info);

	m_vecSkillList.push_back(Skill);
}

ESkillUpMessage CSkillTree::SkillUp(int iSkillNumber, int& Index, int& Level)
{
	size_t	Size = m_vecSkillList.size();

	for (size_t i = 0; i < Size; ++i)
	{
		if (m_vecSkillList[i]->GetSkillInfo()->iSkillNumber == iSkillNumber)
		{
			Index = i;
			if (m_Owner->GetChracterInfo().iSkillPoint == 0)
			{
				return ESkillUpMessage::SkillPointFail;
			}
			else if (m_vecSkillList[i]->GetSkillInfo()->iRequiredLevel > m_Owner->GetChracterInfo().iLevel)
			{
				return ESkillUpMessage::RequiredLevelFail;
			}
			else if (m_vecSkillList[i]->GetSkillInfo()->iSkillMaxLevel == m_vecSkillList[i]->GetSkillLevel())
			{
				return ESkillUpMessage::MaxLevelFail;
			}
			else // 선행스킬이 있을떄
			{
				if (m_vecSkillList[i]->GetSkillInfo()->vecPrevSkillNumber.size() > 0)
				{
					bool bFlag = false;
					for (auto Number : m_vecSkillList[i]->GetSkillInfo()->vecPrevSkillNumber)
					{
						// 만약 두개일떄 하나마다 싹다 검사를 해야되는건가?
						for (int j = 0; j < Size; ++j)
						{
							if (j == i)
								continue;

							if (Number == m_vecSkillList[j]->GetSkillInfo()->iSkillNumber)
							{
								if (m_vecSkillList[j]->GetSkillLevel() > 0)
								{
									bFlag = true;
								}
							}
						}

						if (bFlag == false)
						{
							return ESkillUpMessage::LinkedSkillFail;
						}
						bFlag = false;
					}

				}

			}

			//else
			//{
			//	m_vecSkillList[i]->AddSkillLevel();
			//	Level = m_vecSkillList[i]->GetSkillLevel();
			//	Index = i;
			//	m_Owner->SubtractSkillPoint();
			//	return ESkillUpMessage ::Success;
			//}


		}
	}

	m_vecSkillList[Index]->AddSkillLevel();
	Level = m_vecSkillList[Index]->GetSkillLevel();
	m_Owner->SubtractSkillPoint();

	return ESkillUpMessage::Success;
}

void CSkillTree::SendSkillList(CUser* pSession)
{
	if (!m_vecSkillList.empty())
	{
		BYTE	Packet[MAX_BUFFER_LENGTH] = {};
		CStream	stream;
		stream.SetBuffer((char*)Packet);

		int	iCount = (int)m_vecSkillList.size();
		stream.Write<int>(&iCount, sizeof(int));

		for (int i = 0; i < iCount; ++i)
		{
			int iSkillLevel = m_vecSkillList[i]->GetSkillLevel();

			SkillInfo* pSkillInfo = m_vecSkillList[i]->GetSkillInfo();

			stream.Write(&iSkillLevel, sizeof(int));

			stream.Write(&pSkillInfo->eType, sizeof(int));
			stream.Write(&pSkillInfo->eJob, sizeof(int));
			stream.Write(&pSkillInfo->ePropt, sizeof(int));
			stream.Write(&pSkillInfo->iSkillNumber, sizeof(int));
			stream.Write(&pSkillInfo->iSkillMaxLevel, sizeof(int));
			stream.Write(&pSkillInfo->iRequiredLevel, sizeof(int));
			stream.Write(&pSkillInfo->fCooldown, sizeof(float));

			char	NameUTF8[256] = {};
			char	DescUTF8[256] = {};

			int	iLength = MultiByteToWideChar(CP_ACP, 0, pSkillInfo->strName, -1, 0, 0);

			if (iLength > 0)
			{
				TCHAR	strName[256] = {};
				MultiByteToWideChar(CP_ACP, 0, pSkillInfo->strName, -1, strName, iLength);
				int iUTF8Length = WideCharToMultiByte(CP_UTF8, 0, strName, iLength, 0, 0, 0, 0);

				if (iUTF8Length > 0)
				{
					WideCharToMultiByte(CP_UTF8, 0, strName, iLength, NameUTF8, iUTF8Length, 0, 0);
				}
			}

			// char -> tchar 바꾸기 위한 길이를 구한다.
			iLength = MultiByteToWideChar(CP_ACP, 0, pSkillInfo->strDesc, -1, 0, 0);

			if (iLength > 0)
			{
				TCHAR	strDesc[256] = {};

				// char -> tchar 로 변환
				MultiByteToWideChar(CP_ACP, 0, pSkillInfo->strDesc, -1, strDesc, iLength);

				// tchar -> utf8 로 변환하기 위한 길이를 구한다.
				int iUTF8Length = WideCharToMultiByte(CP_UTF8, 0, strDesc, iLength, 0, 0, 0, 0);

				if (iUTF8Length > 0)
				{
					// tchar -> utf8로 변환
					WideCharToMultiByte(CP_UTF8, 0, strDesc, iLength, DescUTF8, iUTF8Length, 0, 0);
				}
			}

			//cout << "●" << i + 1 << "● Number Skill ...." << endl;
			//cout << "Skill UTF8 Name - " << NameUTF8 << endl;
			//cout << "Skill UTF8 DESC - " << DescUTF8 << endl;

			int	TextLength = (int)strlen(NameUTF8);
			stream.Write(&TextLength, sizeof(int));
			stream.Write(NameUTF8, TextLength);

			TextLength = (int)strlen(DescUTF8);
			stream.Write(&TextLength, sizeof(int));
			stream.Write(DescUTF8, TextLength);

			int	iOptionCount = pSkillInfo->vecOption.size();
			stream.Write(&iOptionCount, sizeof(int));
			for (int j = 0; j < iOptionCount; ++j)
			{
				SkillOption tOption = pSkillInfo->vecOption[j];
				stream.Write(&tOption.SkillOptionType, sizeof(int));
				stream.Write(&tOption.SkillOptionData, sizeof(float));
			}

			int	iActionCount = pSkillInfo->vecAction.size();
			stream.Write(&iActionCount, sizeof(int));
			for (int j = 0; j < iActionCount; ++j)
			{
				SkillAction tAction = pSkillInfo->vecAction[j];
				stream.Write(&tAction.SkillActionType, sizeof(int));
				stream.Write(&tAction.SkillActionData, sizeof(float));
			}

			int iNextSkillNumberCount = pSkillInfo->vecNextSkillNumber.size();
			stream.Write(&iNextSkillNumberCount, sizeof(int));
			for (int j = 0; j < iNextSkillNumberCount; ++j)
			{
				int iNextSkillNumber = pSkillInfo->vecNextSkillNumber[j];
				stream.Write(&iNextSkillNumber, sizeof(int));
			}
			int iPrevSkillNumberCount = pSkillInfo->vecPrevSkillNumber.size();
			stream.Write(&iPrevSkillNumberCount, sizeof(int));
			for (int j = 0; j < iPrevSkillNumberCount; ++j)
			{
				int iPrevSkillNumber = pSkillInfo->vecPrevSkillNumber[j];
				stream.Write(&iPrevSkillNumber, sizeof(int));
			}
		}

		pSession->WritePacket(GP_SkillList, Packet, stream.GetLength());
	}
}

CSkillTree* CSkillTree::Clone()
{
	return new CSkillTree(*this);
}

void CSkillTree::Save()
{
	// 현재 내가 배운 스킬들을 ProtoType과 별도로 저장을 한다.
	if (!m_Owner)
		return;

	FILE* File = nullptr;
	char	strPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

	char	FullPath[MAX_PATH] = {};

	strcpy_s(FullPath, pPath);
	sprintf_s(FullPath, "%s%d.skl", pPath, m_Owner->GetCharacterNumber());

	fopen_s(&File, FullPath, "wb");

	if (File)
	{
		int	Count = (int)m_vecSkillList.size();

		fwrite(&Count, 4, 1, File);

		for (int i = 0; i < Count; ++i)
		{
			const SkillInfo* Info = m_vecSkillList[i]->GetSkillInfo();
			int	iSkillLevel = m_vecSkillList[i]->GetSkillLevel();

			fwrite(&Info->iSkillNumber, 4, 1, File);
			fwrite(&iSkillLevel, 4, 1, File);
		}

		fclose(File);
	}
}

void CSkillTree::Load()
{
	if (!m_Owner)
		return;

	FILE* File = nullptr;
	char	strPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);
	char	FullPath[MAX_PATH] = {};

	strcpy_s(FullPath, pPath);
	sprintf_s(FullPath, "%s%d.skl", pPath, m_Owner->GetCharacterNumber());

	fopen_s(&File, FullPath, "rb");

	if (File)
	{
		int	Count = 0;

		fread(&Count, 4, 1, File);

		for (int i = 0; i < Count; ++i)
		{
			int	SkillNumber, SkillLevel;

			fread(&SkillNumber, 4, 1, File);
			fread(&SkillLevel, 4, 1, File);

			CSkill* Skill = new CSkill;

			SkillInfo* Info = CSkillManager::GetInst()->FindSkillInfo(SkillNumber);

			Skill->Init(Info);

			Skill->SetSkillLevel(SkillLevel);

			m_vecSkillList.push_back(Skill);
		}


		fclose(File);
	}
}
