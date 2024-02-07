#include "Inventory.h"
#include "PathManager.h"
#include "User.h"
#include "Stream.h"

CInventory::CInventory()
{
}

CInventory::~CInventory()
{
	// 인벤토리에 있는 아이템을 파일에 갱신해준다.
	FILE* pFile = nullptr;

	fopen_s(&pFile, m_strFullPath, "rb");

	if (pFile)
	{
		int	iCount = (int)m_vecItem.size();

		fwrite(&iCount, 4, 1, pFile);

		for (int i = 0; i < iCount; ++i)
		{
			fwrite(m_vecItem[i]->strName, 1, 32, pFile);
			fwrite(&m_vecItem[i]->Type, sizeof(ItemType), 1, pFile);
			fwrite(&m_vecItem[i]->Price, sizeof(int), 1, pFile);
			fwrite(&m_vecItem[i]->Sell, sizeof(int), 1, pFile);

			int	iOptionCount = (int)m_vecItem[i]->vecOption.size();

			fwrite(&iOptionCount, 4, 1, pFile);

			fwrite(&m_vecItem[i]->vecOption[0], sizeof(_tagItemOption), iOptionCount, pFile);
		}

		fclose(pFile);
	}

	ZeroMemory(m_strFullPath, MAX_PATH);
}

bool CInventory::Init(const char* pFileName)
{
	FILE* pFile = nullptr;
	char	strPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

	if (pPath)
		strcpy_s(strPath, pPath);

	strcat_s(strPath, pFileName);

	strcpy_s(m_strFullPath, strPath);

	fopen_s(&pFile, strPath, "rb");

	if (pFile)
	{
		int	iInvenCount = 0;

		// 이 캐릭터의 인벤토리를 생성한다.

		fread(&iInvenCount, 4, 1, pFile);

		for (int i = 0; i < iInvenCount; ++i)
		{
			// 아이템 정보들을 읽어온다.   
			_tagItemInfo* pItem = new _tagItemInfo;

			m_vecItem.push_back(pItem);

			fread(pItem->strName, 1, 32, pFile);
			fread(&pItem->Grade, sizeof(EItemGrade), 1, pFile);
			fread(&pItem->Type, sizeof(ItemType), 1, pFile);
			fread(&pItem->Price, sizeof(int), 1, pFile);
			fread(&pItem->Sell, sizeof(int), 1, pFile);

			int	iOptionCount = 0;

			fread(&iOptionCount, 4, 1, pFile);

			pItem->vecOption.resize(iOptionCount);
			
			fread(&pItem->vecOption[0], sizeof(_tagItemOption), iOptionCount, pFile);
		}

		fclose(pFile);
	}

	return true;
}

void CInventory::SendItemList(CUser* pSession)
{
	BYTE	Packet[MAX_BUFFER_LENGTH] = {};
	CStream	stream;
	stream.SetBuffer((char*)Packet);

	int	iCount = (int)m_vecItem.size();
	stream.Write<int>(&iCount, sizeof(int));

	for (int i = 0; i < iCount; ++i)
	{
		stream.Write(m_vecItem[i]->strName, ITEMNAME_SIZE);
		stream.Write<EItemGrade>(&m_vecItem[i]->Grade, sizeof(EItemGrade));
		stream.Write<ItemType>(&m_vecItem[i]->Type, sizeof(ItemType));
		stream.Write<int>(&m_vecItem[i]->Price, sizeof(int));
		stream.Write<int>(&m_vecItem[i]->Sell, sizeof(int));

		int	iOptionCount = (int)m_vecItem[i]->vecOption.size();

		stream.Write<int>(&iOptionCount, sizeof(int));

		stream.Write(&m_vecItem[i]->vecOption[0], sizeof(_tagItemOption) * iOptionCount);
	}

	pSession->WritePacket(GP_INVENTORYLIST, Packet, stream.GetLength());
}

bool CInventory::AddItem(_tagItemInfo* pItem)
{ 
	if (m_vecItem.size() < INVENTORY_MAX)
	{
		m_vecItem.push_back(pItem);
		return true;
	}
	else
	{
		return false;
	}
}

bool CInventory::PopItem(_tagItemInfo* pItem)
{
	if (m_vecItem.size() < INVENTORY_MAX)
	{
		auto iter = m_vecItem.begin();
		auto iterend = m_vecItem.end();
		for (; iter < iterend; ++iter)
		{
			if (strncmp(pItem->strName, (*iter)->strName, ITEMNAME_SIZE) == 0)
			{
				m_vecItem.erase(iter);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CInventory::SaveItem()
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, m_strFullPath, "wb");

	if (pFile)
	{
		int	iCount = (int)m_vecItem.size();

		fwrite(&iCount, 4, 1, pFile);

		for (int i = 0; i < iCount; ++i)
		{
			fwrite(m_vecItem[i]->strName, 1, 32, pFile);
			fwrite(&m_vecItem[i]->Grade, sizeof(EItemGrade), 1, pFile);
			fwrite(&m_vecItem[i]->Type, sizeof(ItemType), 1, pFile);
			fwrite(&m_vecItem[i]->Price, sizeof(int), 1, pFile);
			fwrite(&m_vecItem[i]->Sell, sizeof(int), 1, pFile);

			int	iOptionCount = (int)m_vecItem[i]->vecOption.size();

			fwrite(&iOptionCount, 4, 1, pFile);

			fwrite(&m_vecItem[i]->vecOption[0], sizeof(_tagItemOption), iOptionCount, pFile);
		}

		fclose(pFile);
	}

	return true;
}
