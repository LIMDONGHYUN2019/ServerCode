
#include "SkillManager.h"
#include "PathManager.h"
#include "SkillTree.h"
#include "Skill.h"

DEFINITION_SINGLE(CSkillManager)

CSkillManager::CSkillManager()
{
}

CSkillManager::~CSkillManager()
{
	auto	iter = m_mapSkillInfo.begin();
	auto	iterEnd = m_mapSkillInfo.end();

	for (; iter != iterEnd; ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	size_t	Size = m_vecSkillTree.size();

	for (size_t i = 0; i < Size; ++i)
	{
		SAFE_DELETE(m_vecSkillTree[i]);
	}
}

bool CSkillManager::Init()
{
	// �� ������ ��ųƮ��ProtoType�� �����´�.
	FILE* pFile = nullptr;

	for (int i = 0; i < (int)ESkillJob::End; ++i)
	{

		char	strPath[MAX_PATH] = {};

		const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

		if (pPath)
			strcpy_s(strPath, pPath);

		switch ((ESkillJob)i)
		{
		case ESkillJob::GrayStone:
			strcat_s(strPath, "GreyStoneSkillInfo.sif");
			break;
		case ESkillJob::TwinBlast:
			strcat_s(strPath, "TwinBlastSkillInfo.sif");
			break;
		case ESkillJob::Serath:
			strcat_s(strPath, "SerathSkillInfo.sif");
			break;
		}

		fopen_s(&pFile, strPath, "rb");

		if (!pFile)
			return false;

		int iSkillSize = 0;
		fread(&iSkillSize, sizeof(int), 1, pFile);

		PSkillInfo pSkill = new _tagSkillInfo;
		memset(pSkill, 0, sizeof(_tagSkillInfo));

		for (int i = 0; i < iSkillSize; ++i)
		{
			_tagSkillInfo* pSkillInfo = new _tagSkillInfo;
			memset(pSkillInfo, 0, sizeof(_tagSkillInfo));

			fread(&pSkillInfo->eType, sizeof(ESkillType), 1, pFile);
			fread(&pSkillInfo->eJob, sizeof(ESkillJob), 1, pFile);
			fread(&pSkillInfo->ePropt, sizeof(ESkillProperty), 1, pFile);
			fread(&pSkillInfo->iSkillNumber, sizeof(int), 1, pFile);
			fread(&pSkillInfo->iSkillMaxLevel, sizeof(int), 1, pFile);

			fread(&pSkillInfo->iRequiredLevel, sizeof(int), 1, pFile);
			fread(&pSkillInfo->fCooldown, sizeof(float), 1, pFile);

			fread(pSkillInfo->strName, NAME_SIZE, 1, pFile);
			fread(pSkillInfo->strDesc, DESC_SIZE, 1, pFile);

			int iCount = 0;
			fread(&iCount, sizeof(int), 1, pFile);
			pSkillInfo->vecOption.resize(iCount);
			if (iCount > 0)
				fread(&pSkillInfo->vecOption[0], sizeof(SkillOption), iCount, pFile);

			iCount = 0;
			fread(&iCount, sizeof(int), 1, pFile);
			pSkillInfo->vecAction.resize(iCount);
			if (iCount > 0)
				fread(&pSkillInfo->vecAction[0], sizeof(SkillAction), iCount, pFile);

			iCount = 0;
			fread(&iCount, sizeof(int), 1, pFile);
			pSkillInfo->vecNextSkillNumber.resize(iCount);
			if (iCount > 0)
				fread(&pSkillInfo->vecNextSkillNumber[0], sizeof(int), iCount, pFile);

			iCount = 0;
			fread(&iCount, sizeof(int), 1, pFile);
			pSkillInfo->vecPrevSkillNumber.resize(iCount);
			if (iCount > 0)
				fread(&pSkillInfo->vecPrevSkillNumber[0], sizeof(int), iCount, pFile);

			m_vecJobSkillInfo[(int)pSkillInfo->eJob].push_back(pSkillInfo);
			// make_pair�� �ϸ鼭 �ڵ������� ��ġ�� ������ΰ� �ƴ� �ڹٲٸ鼭 ����ִ´�.
			m_mapSkillInfo.insert(make_pair(pSkillInfo->iSkillNumber, pSkillInfo));
		}
	}
	// ������ ��ų�� ���� ��Ƽ� ��ųƮ���� �����Ѵ�.
	//auto	iter = m_mapSkillInfo.begin();
	//auto	iterEnd = m_mapSkillInfo.end();
	//for (; iter != iterEnd; ++iter)
	//{
	//	m_vecJobSkillInfo[(int)iter->second->eJob].push_back(iter->second);
	//}

	// ��ųƮ���� �����Ѵ�.
	// �������� �����.
	for (int i = 0; i < (int)ESkillJob::End; ++i)
	{
		CSkillTree* Tree = new CSkillTree;

		size_t	JobSkillCount = m_vecJobSkillInfo[i].size();

		for (size_t j = 0; j < JobSkillCount; ++j)
		{
			Tree->AddSkillInfo(m_vecJobSkillInfo[i][j]);
		}

		m_vecSkillTree.push_back(Tree);
	}
	fclose(pFile);

	return true;
}

SkillInfo* CSkillManager::FindSkillInfo(int SkillNumber)
{
	auto	iter = m_mapSkillInfo.find(SkillNumber);

	if (iter == m_mapSkillInfo.end())
		return nullptr;

	return iter->second;
}

SkillInfo* CSkillManager::FindSkillInfo(ESkillType Type, ESkillJob Job, int SerialNumber)
{
	int SkillNumber = (int)Type * 10000 + (int)Job * 1000 + SerialNumber;

	return FindSkillInfo(SkillNumber);
}

CSkillTree* CSkillManager::GetSkillTree(ESkillJob Job)
{
	return m_vecSkillTree[(int)Job];
}
