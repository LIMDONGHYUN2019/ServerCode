#include "Navigation.h"
#include "NavCell.h"
#include "NavigationThread.h"
#include "ThreadManager.h"
#include <stack>

CNavigation::CNavigation() :
	m_Count(0),
	m_CountH(0),
	m_CountV(0),
	m_CellWidth(0),
	m_CellHeight(0),
	m_UseCount(0),
	m_FindPath(false)
{
}

CNavigation::CNavigation(const CNavigation& Nav)
{
	m_Count = Nav.m_Count;
	m_CountH = Nav.m_CountH;
	m_CountV = Nav.m_CountV;
	m_CellWidth = Nav.m_CellWidth;
	m_CellHeight = Nav.m_CellHeight;
	m_Min = Nav.m_Min;
	m_Max = Nav.m_Max;
	m_UseCount = 0;
	m_FindPath = false;

	size_t	Size = Nav.m_vecCell.size();

	m_vecCell.clear();

	m_vecCell.resize(Size);

	for (size_t i = 0; i < Size; ++i)
	{
		CNavCell* Cell = Nav.m_vecCell[i]->Clone();

		m_vecCell[i] = Cell;
	}

	m_vecUseCell.clear();

	m_vecUseCell.resize(m_vecCell.size());
}

CNavigation::~CNavigation()
{
	SAFE_DELETE_VECLIST(m_vecCell);
}

bool CNavigation::Init(const char* FullPath)
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, FullPath, "rb");

	if (!pFile)
		return false;

	int	NameCount;

	fread(&NameCount, 4, 1, pFile);
	char	NavName[32] = {};
	char	NavName1[256] = {};
	int		Tempo;

	fread(&Tempo, 4, 1, pFile);

	//fread(NavName, 1, NameCount + 1, pFile);
	fread(NavName, 1, Tempo, pFile);
	// �𸮾󿡼� int�� �����ϸ� uint�� ����ȴ��� ������
	// �߰��� �𸮾󿡼� ���ڿ��� �����ҋ� \0���� �����ϴ°Ű���.
	// �׷��� ���忭�տ� �� \0���� ������ ���ڿ� ���̸� ���ڿ��տ� �־��ش�. 

	fread(&m_Count, sizeof(int), 1, pFile); // ��ֹ� ����
	fread(&m_CountH, sizeof(int), 1, pFile);
	fread(&m_CountV, sizeof(int), 1, pFile);
	fread(&m_CellWidth, sizeof(float), 1, pFile);
	fread(&m_CellHeight, sizeof(float), 1, pFile);
	fread(&m_Min, sizeof(Vector3), 1, pFile);
	fread(&m_Max, sizeof(Vector3), 1, pFile);

	for (int i = 0; i < m_CountV; ++i)
	{
		for (int j = 0; j < m_CountH; ++j)
		{
			CNavCell* Cell = new CNavCell;

			Cell->m_CellType = NavCell_Type::Normal;
			Cell->m_IndexX = j;
			Cell->m_IndexY = i;
			Cell->m_Index = i * m_CountH + j;
			Cell->m_Width = m_CellWidth;
			Cell->m_Height = m_CellHeight;
			Cell->m_Center = m_Min + Vector3(i * m_CellHeight + m_CellHeight * 0.5f, j * m_CellWidth + m_CellWidth * 0.5f, 0.f);
			m_vecCell.push_back(Cell);
		}
	}

	for (int i = 0; i < m_Count; ++i)
	{
		int	IndexX = 0, IndexY = 0;
		fread(&IndexX, sizeof(int), 1, pFile);
		fread(&IndexY, sizeof(int), 1, pFile);

		int	Index = IndexY * m_CountH + IndexX;

		m_vecCell[Index]->m_CellType = NavCell_Type::Wall;
	}

	m_vecUseCell.resize(m_vecCell.size());


	fclose(pFile);


	return true;
}

bool CNavigation::Init(int CountX, int CountY)
{
	m_CountH = CountX;
	m_CountV = CountY;

	m_Count = m_CountH * m_CountV;

	m_CellWidth = 1.f;
	m_CellHeight = 1.f;

	m_Min = Vector3(0.f, 0.f, 0.f);
	m_Max = Vector3(m_CountV, m_CountH, 0.f);

	for (int i = 0; i < m_CountV; ++i)
	{
		for (int j = 0; j < m_CountH; ++j)
		{
			CNavCell* Cell = new CNavCell;

			Cell->m_CellType = NavCell_Type::Normal;
			Cell->m_IndexX = j;
			Cell->m_IndexY = i;
			Cell->m_Index = i * m_CountH + j;
			Cell->m_Center.x = m_CountV - (i + m_CellHeight * 0.5f);
			Cell->m_Center.y = j + m_CellWidth * 0.5f;
			Cell->m_Width = m_CellWidth;
			Cell->m_Height = m_CellHeight;

			m_vecCell.push_back(Cell);
		}
	}

	m_vecUseCell.resize(m_vecCell.size());

	return true;
}

void CNavigation::SetCellType(int IndexX, int IndexY, NavCell_Type Type)
{
	m_vecCell[IndexY * m_CountH + IndexX]->m_CellType = Type;
}

CNavigation* CNavigation::Clone()
{
	return new CNavigation(*this);
}

bool CNavigation::FindPath(vector<Vector3>& vecResult, const Vector3& Start, const Vector3& End)
{
	// ������ġ�� ������ġ�� �̿��ؼ� �ش� ��带 ã�´�.
	if (m_Min.x > Start.x || m_Max.x < Start.x || m_Min.y > Start.y || m_Max.y < Start.y)
		return false;

	if (m_Min.x > End.x || m_Max.x < End.x || m_Min.y > End.y || m_Max.y < End.y)
		return false;

	float	Height = m_Max.x - m_Min.x;

	// ��ġ���� Min���� ���ش�. ������ �������� �� ��ġ�� ��ȯ�� ���ִ� ���̴�.
	Vector3	ConvertStart = Start - m_Min;
	Vector3	ConvertEnd = End - m_Min;

	//ConvertStart.x = Height - ConvertStart.x;
	//ConvertEnd.x = Height - ConvertEnd.x;

	// ��ĭ�� ũ�⸦ �������־ ���� ���� ũ�Ⱑ 1x1 �� ��ġ�� ��ȯ���ش�.
	ConvertStart.y /= m_CellWidth;
	ConvertStart.x /= m_CellHeight;

	ConvertEnd.y /= m_CellWidth;
	ConvertEnd.x /= m_CellHeight;

	int	IndexWidth, IndexHeight;

	IndexWidth = (int)ConvertStart.y;
	IndexHeight = (int)ConvertStart.x;

	int	Index = IndexHeight * m_CountH + IndexWidth;

	CNavCell* StartCell = m_vecCell[Index];

	if (StartCell->m_CellType == NavCell_Type::Wall)
		return false;

	IndexWidth = (int)ConvertEnd.y;
	IndexHeight = (int)ConvertEnd.x;

	Index = IndexHeight * m_CountH + IndexWidth;

	CNavCell* EndCell = m_vecCell[Index];

	// ���� �� ������ ��ó�� ã�ư��� �ؾ� �ȴٸ� ���� ���� �߽����� �� �� �ִ� ���� ã�´�.
	if (EndCell->m_CellType == NavCell_Type::Wall)
		return false;

	vecResult.clear();

	// ���� ���� �̵��ؾ� �Ǵ� ��� �ش� ��ġ�� �ٷ� �ش�.
	if (StartCell == EndCell)
	{
		vecResult.push_back(End);
		return true;
	}

	for (int i = 0; i < m_UseCount; ++i)
	{
		m_vecUseCell[i]->m_NodeType = NavNode_Type::None;
		m_vecUseCell[i]->m_Parent = nullptr;
		m_vecUseCell[i]->m_G = FLT_MAX;
		m_vecUseCell[i]->m_H = FLT_MAX;
		m_vecUseCell[i]->m_Total = FLT_MAX;
		m_vecUseCell[i]->m_vecFindDir.clear();
	}

	m_UseCount = 0;

	// ���� ���� ������Ͽ� �־�д�.
	m_vecOpen.push_back(StartCell);

	m_vecUseCell[m_UseCount] = StartCell;
	++m_UseCount;

	StartCell->m_NodeType = NavNode_Type::Open;
	StartCell->m_G = 0.f;
	StartCell->m_H = 0.f;
	StartCell->m_Total = 0.f;

	for (int i = 0; i < JPD_MAX; ++i)
	{
		StartCell->m_vecFindDir.push_back((JumpPoint_Dir)i);
	}

	m_FindPath = false;

	// ������ �����Ѵ�.
	while (!m_vecOpen.empty())
	{
		CNavCell* Node = m_vecOpen.back();
		m_vecOpen.pop_back();

		Node->m_NodeType = NavNode_Type::Close;

		// 8������ �˻��Ͽ� ��������Ʈ�� �ִ°����� ���ؼ� ������Ͽ� �־��ش�.
		if (FindJumpPoint(Node, EndCell, End, vecResult))
			break;

		// �����Ѵ�.
		if (m_vecOpen.size() >= 2)
			std::sort(m_vecOpen.begin(), m_vecOpen.end(), CNavigation::SortOpenList);
			//qsort(&m_vecOpen[0], m_vecOpen.size(), sizeof(CNavCell*), CNavigation::SortOpenList);
	}

	if (vecResult.empty())
		int a = 0;

	m_vecOpen.clear();

	return !vecResult.empty();
}

bool CNavigation::FindJumpPoint(CNavCell* Node, CNavCell* EndNode, const Vector3& EndPos, vector<Vector3>& vecResult)
{
	// �� ���⺰�� ��������Ʈ�� �ִ����� �Ǵ��ؾ� �Ѵ�.
	for (size_t i = 0; i < Node->m_vecFindDir.size(); ++i)
	{
		CNavCell* Corner = GetJumpPointNode(Node->m_vecFindDir[i], Node, EndNode, EndPos);

		if (!Corner)
			continue;

		// ã�ƿ� ��尡 ��������� ���
		if (Corner == EndNode)
		{
			m_vecUseCell[m_UseCount] = Corner;
			++m_UseCount;

			vecResult.push_back(EndPos);

			CNavCell* PathNode = Node;

			while (PathNode)
			{
				vecResult.push_back(PathNode->m_Center);
				PathNode = PathNode->m_Parent;
			}

			// ���� ��� ����
			vecResult.pop_back();

			return true;
		}

		// �̵� ����� �����ش�.
		float	Cost = 0.f;

		switch (Node->m_vecFindDir[i])
		{
		case JPD_TOP:
			Cost = Node->m_G + abs(Node->m_Center.x - Corner->m_Center.x);
			break;
		case JPD_TL:
			Cost = Node->m_G + Node->m_Center.Distance(Corner->m_Center);
			break;
		case JPD_LEFT:
			Cost = Node->m_G + abs(Node->m_Center.y - Corner->m_Center.y);
			break;
		case JPD_BL:
			Cost = Node->m_G + Node->m_Center.Distance(Corner->m_Center);
			break;
		case JPD_BOTTOM:
			Cost = Node->m_G + abs(Node->m_Center.x - Corner->m_Center.x);
			break;
		case JPD_BR:
			Cost = Node->m_G + Node->m_Center.Distance(Corner->m_Center);
			break;
		case JPD_RIGHT:
			Cost = Node->m_G + abs(Node->m_Center.y - Corner->m_Center.y);
			break;
		case JPD_TR:
			Cost = Node->m_G + Node->m_Center.Distance(Corner->m_Center);
			break;
		}


		if (Corner->m_NodeType == NavNode_Type::None)
		{
			Corner->m_NodeType = NavNode_Type::Open;
			Corner->m_Parent = Node;
			Corner->m_G = Cost;
			Corner->m_H = Corner->m_Center.Distance(EndPos);
			Corner->m_Total = Corner->m_G + Corner->m_H;

			m_vecOpen.push_back(Corner);

			m_vecUseCell[m_UseCount] = Corner;
			++m_UseCount;

			AddDir(Node->m_vecFindDir[i], Corner);
		}

		else
		{
			if (Cost < Corner->m_G)
			{
				Corner->m_G = Cost;
				Corner->m_Total = Corner->m_G + Corner->m_H;
				Corner->m_Parent = Node;

				AddDir(Node->m_vecFindDir[i], Corner);
			}
		}
	}

	return false;
}

void CNavigation::AddDir(JumpPoint_Dir Dir, CNavCell* Node)
{
	Node->m_vecFindDir.clear();

	switch (Dir)
	{
	case JPD_TOP:
		Node->m_vecFindDir.push_back(JPD_TOP);
		Node->m_vecFindDir.push_back(JPD_TL);
		Node->m_vecFindDir.push_back(JPD_TR);
		break;
	case JPD_TR:
		Node->m_vecFindDir.push_back(JPD_TR);
		Node->m_vecFindDir.push_back(JPD_TOP);
		Node->m_vecFindDir.push_back(JPD_TL);
		Node->m_vecFindDir.push_back(JPD_RIGHT);
		Node->m_vecFindDir.push_back(JPD_BR);
		break;
	case JPD_TL:
		Node->m_vecFindDir.push_back(JPD_TL);
		Node->m_vecFindDir.push_back(JPD_TOP);
		Node->m_vecFindDir.push_back(JPD_LEFT);
		Node->m_vecFindDir.push_back(JPD_BL);
		Node->m_vecFindDir.push_back(JPD_TR);
		break;

	case JPD_LEFT:
		Node->m_vecFindDir.push_back(JPD_LEFT);
		Node->m_vecFindDir.push_back(JPD_TL);
		Node->m_vecFindDir.push_back(JPD_BL);
		break;
	case JPD_RIGHT:
		Node->m_vecFindDir.push_back(JPD_RIGHT);
		Node->m_vecFindDir.push_back(JPD_BR);
		Node->m_vecFindDir.push_back(JPD_TR);
		break;

	case JPD_BOTTOM:
		Node->m_vecFindDir.push_back(JPD_BOTTOM);
		Node->m_vecFindDir.push_back(JPD_BR);
		Node->m_vecFindDir.push_back(JPD_BL);
		break;
	case JPD_BL:
		Node->m_vecFindDir.push_back(JPD_BL);
		Node->m_vecFindDir.push_back(JPD_BOTTOM);
		Node->m_vecFindDir.push_back(JPD_LEFT);
		Node->m_vecFindDir.push_back(JPD_TL);
		Node->m_vecFindDir.push_back(JPD_BR);
		break;
	case JPD_BR:
		Node->m_vecFindDir.push_back(JPD_BR);
		Node->m_vecFindDir.push_back(JPD_BOTTOM);
		Node->m_vecFindDir.push_back(JPD_RIGHT);
		Node->m_vecFindDir.push_back(JPD_BL);
		Node->m_vecFindDir.push_back(JPD_TR);
		break;
	}
}

CNavCell* CNavigation::GetJumpPointNode(JumpPoint_Dir Dir, CNavCell* Node, CNavCell* EndNode, const Vector3& EndPos)
{
	switch (Dir)
	{
	case JPD_TOP:
		return GetTopJumpPoint(Node, EndNode);
	case JPD_TL:
		return GetLTJumpPoint(Node, EndNode);
	case JPD_LEFT:
		return GetLeftJumpPoint(Node, EndNode);
	case JPD_BL:
		return GetLBJumpPoint(Node, EndNode);
	case JPD_BOTTOM:
		return GetBottomJumpPoint(Node, EndNode);
	case JPD_BR:
		return GetRBJumpPoint(Node, EndNode);
	case JPD_RIGHT:
		return GetRightJumpPoint(Node, EndNode);
	case JPD_TR:
		return GetRTJumpPoint(Node, EndNode);
	}

	return nullptr;
}

CNavCell* CNavigation::GetLeftJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int	IndexX = Node->m_IndexX;

	while (true)
	{
		// 1ĭ�� �������� ���鼭 �ڳʰ� �ִ����� �Ǵ��Ѵ�.
		--IndexX;

		if (IndexX < 0)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[Node->m_IndexY * m_CountH + IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		// ���� ��ġ�� ���� ��������� �Ǵ��Ѵ�.
		else if (CheckNode == EndNode)
			return CheckNode;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// �������� ������ �ڳʴ� ����, �Ʒ��� �ڳ� ���̴�.
		// ���� �ڳʴ� �� ��ġ���� ������ �����ְ� ���� ���� ���������� ���� �ڳ��̴�.
		// �Ʒ��� �ڳʴ� �� ��ġ���� �Ʒ����� �����ְ� ���� �Ʒ��� ���������� �Ʒ��� �ڳ��̴�.
		// ���� ���� �˻��Ѵ�. ���� ���� ���´�.
		int	CornerX = IndexX;
		int	CornerY = Node->m_IndexY - 1; // �Ʒ��� üũ

		if (CornerY >= 0 && CornerX - 1 >= 0)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[CornerY * m_CountH + (CornerX - 1)]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}

		CornerX = IndexX;
		CornerY = Node->m_IndexY + 1;

		if (CornerY < m_CountV && CornerX - 1 >= 0)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[CornerY * m_CountH + (CornerX - 1)]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}
	}

	return nullptr;
}

CNavCell* CNavigation::GetRightJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int	IndexX = Node->m_IndexX;

	while (true)
	{
		// 1ĭ�� �������� ���鼭 �ڳʰ� �ִ����� �Ǵ��Ѵ�.
		++IndexX;

		if (IndexX >= m_CountH)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[Node->m_IndexY * m_CountH + IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// ���� ��ġ�� ���� ��������� �Ǵ��Ѵ�.
		else if (CheckNode == EndNode)
			return CheckNode;

		// �������� ������ �ڳʴ� ����, �Ʒ��� �ڳ� ���̴�.
		// ���� �ڳʴ� �� ��ġ���� ������ �����ְ� ���� ���� ���������� ���� �ڳ��̴�.
		// �Ʒ��� �ڳʴ� �� ��ġ���� �Ʒ����� �����ְ� ���� �Ʒ��� ���������� �Ʒ��� �ڳ��̴�.
		// ���� ���� �˻��Ѵ�. ���� ���� ���´�.
		int	CornerX = IndexX;
		int	CornerY = Node->m_IndexY - 1; // ���� üũ

		if (CornerY >= 0 && CornerX + 1 < m_CountH)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[CornerY * m_CountH + (CornerX + 1)]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}

		CornerX = IndexX;
		CornerY = Node->m_IndexY + 1;

		if (CornerY < m_CountV && CornerX + 1 < m_CountH)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[CornerY * m_CountH + (CornerX + 1)]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}
	}

	return nullptr;
}

CNavCell* CNavigation::GetTopJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int	IndexY = Node->m_IndexY;

	while (true)
	{
		// 1ĭ�� �������� ���鼭 �ڳʰ� �ִ����� �Ǵ��Ѵ�.
		--IndexY;

		if (IndexY < 0)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[IndexY * m_CountH + Node->m_IndexX];
		//  ���� ��ġ���� ���� ��ġ�� �����´�.

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// ���� ��ġ�� ���� ��������� �Ǵ��Ѵ�.
		else if (CheckNode == EndNode)
			return CheckNode;
		 
		// �������� ������ �ڳʴ� ����, �Ʒ��� �ڳ� ���̴�.
		// ���� �ڳʴ� �� ��ġ���� ������ �����ְ� ���� ���� ���������� ���� �ڳ��̴�.
		// �Ʒ��� �ڳʴ� �� ��ġ���� �Ʒ����� �����ְ� ���� �Ʒ��� ���������� �Ʒ��� �ڳ��̴�.
		// ���� ���� �˻��Ѵ�. ���� ���� ���´�.
		int	CornerX = Node->m_IndexX - 1;
		int	CornerY = IndexY; // Node->m_IndexY - 1 �̰� �ƴѰ�? �� �ƴ�
		
		// �ڳʰ� ����ִٸ� �ڳʷ� �����ִ� �Լ��� ȣ��ǵ����ϱ�����
		if (CornerY - 1 >= 0 && CornerX >= 0)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(CornerY - 1) * m_CountH + CornerX]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}


		CornerX = Node->m_IndexX + 1;
		CornerY = IndexY;

		if (CornerY - 1 >= 0 && CornerX < m_CountH)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(CornerY - 1) * m_CountH + CornerX]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}
	}

	return nullptr;
}

CNavCell* CNavigation::GetBottomJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int	IndexY = Node->m_IndexY;

	while (true)
	{
		// 1ĭ�� �������� ���鼭 �ڳʰ� �ִ����� �Ǵ��Ѵ�.
		++IndexY;

		if (IndexY >=m_CountV)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[IndexY * m_CountH + Node->m_IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// ���� ��ġ�� ���� ��������� �Ǵ��Ѵ�.
		else if (CheckNode == EndNode)
			return CheckNode;

		// �������� ������ �ڳʴ� ����, �Ʒ��� �ڳ� ���̴�.
		// ���� �ڳʴ� �� ��ġ���� ������ �����ְ� ���� ���� ���������� ���� �ڳ��̴�.
		// �Ʒ��� �ڳʴ� �� ��ġ���� �Ʒ����� �����ְ� ���� �Ʒ��� ���������� �Ʒ��� �ڳ��̴�.
		// ���� ���� �˻��Ѵ�. ���� ���� ���´�.
		int	CornerX = Node->m_IndexX - 1;
		int	CornerY = IndexY;

		if (CornerY + 1 < m_CountV && CornerX >= 0)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(CornerY + 1) * m_CountH + CornerX]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}

		CornerX = Node->m_IndexX + 1;
		CornerY = IndexY;

		if (CornerY + 1 < m_CountV && CornerX < m_CountH)
		{
			if (m_vecCell[CornerY * m_CountH + CornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(CornerY + 1) * m_CountH + CornerX]->m_CellType == NavCell_Type::Normal)
				return CheckNode;
		}
	}

	return nullptr;
}

CNavCell* CNavigation::GetLTJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int iIndexY = Node->m_IndexY;
	int iIndexX = Node->m_IndexX;

	while (true)
	{
		--iIndexY;
		--iIndexX;

		if (iIndexY < 0 || iIndexX < 0)
			return nullptr;

		CNavCell* pCheckNode = m_vecCell[iIndexY * m_CountH + iIndexX]; 

		if (pCheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;
		else if (pCheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;
		else if (pCheckNode == EndNode)
			return pCheckNode;

		int iCornerX = iIndexX;
		int iCornerY = iIndexY + 1;

		if (iCornerY < m_CountV && iCornerX - 1 >= 0)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY) * m_CountH + (iCornerX - 1)]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		iCornerX = iIndexX + 1;
		iCornerY = iIndexY;

		if (iCornerY - 1 >= 0 && iCornerX < m_CountH)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY - 1) * m_CountH + iCornerX]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		CNavCell* FindNode = nullptr;

		FindNode = GetTopJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;

		FindNode = GetLeftJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;
		
	}

	return nullptr;
}

CNavCell* CNavigation::GetRTJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int iIndexY = Node->m_IndexY;
	int iIndexX = Node->m_IndexX;

	while (true)
	{
		--iIndexY;
		++iIndexX;

		if (iIndexY < 0 || iIndexX >= m_CountH)
			return nullptr;

		CNavCell* pCheckNode = m_vecCell[iIndexY * m_CountH + iIndexX];

		if (pCheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;
		else if (pCheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;
		else if (pCheckNode == EndNode)
			return pCheckNode;

		int iCornerX = iIndexX - 1;
		int iCornerY = iIndexY;

		if (iCornerY - 1 >= 0 && iCornerX >= 0)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY - 1)*m_CountH + iCornerX]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		iCornerX = iIndexX;
		iCornerY = iIndexY + 1;

		if (iCornerY < m_CountV && iCornerX + 1 < m_CountH)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[iCornerY * m_CountH + (iCornerX + 1)]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		CNavCell* FindNode = nullptr;

		FindNode = GetTopJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;

		FindNode = GetRightJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;
	}

	return nullptr;
}

CNavCell* CNavigation::GetLBJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int iIndexY = Node->m_IndexY;
	int iIndexX = Node->m_IndexX;

	while (true)
	{
		++iIndexY;
		--iIndexX;

		if (iIndexY >= m_CountV || iIndexX < 0)
			return nullptr;

		CNavCell* pCheckNode = m_vecCell[iIndexY * m_CountH + iIndexX];

		if (pCheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;
		else if (pCheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;
		else if (pCheckNode == EndNode)
			return pCheckNode;


		int iCornerX = iIndexX + 1;
		int iCornerY = iIndexY;

		if (iCornerY + 1 < m_CountV && iCornerX < m_CountH)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY + 1)*m_CountH + iCornerX]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		iCornerX = iIndexX;
		iCornerY = iIndexY - 1;

		if (iCornerY >= 0 && iCornerX - 1 >= 0)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY) * m_CountH + (iCornerX - 1)]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		CNavCell* FindNode = nullptr;

		FindNode = GetBottomJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;

		FindNode = GetLeftJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;
	}

	return nullptr;
}

CNavCell* CNavigation::GetRBJumpPoint(CNavCell* Node, class CNavCell* EndNode)
{
	int iIndexY = Node->m_IndexY;
	int iIndexX = Node->m_IndexX;

	while (true)
	{
		++iIndexY;
		++iIndexX;

		if (iIndexY >= m_CountV || iIndexX >= m_CountH)
			return nullptr;

		CNavCell* pCheckNode = m_vecCell[iIndexY * m_CountH + iIndexX];

		if (pCheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;
		else if (pCheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;
		else if (pCheckNode == EndNode)
			return pCheckNode;

		int iCornerX = iIndexX - 1;
		int iCornerY = iIndexY;

		if (iCornerY + 1 < m_CountV && iCornerX >= 0)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY + 1)*m_CountH + iCornerX]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		iCornerX = iIndexX;
		iCornerY = iIndexY - 1;

		if (iCornerY >= 0 && iCornerX + 1 < m_CountH)
		{
			if (m_vecCell[iCornerY * m_CountH + iCornerX]->m_CellType == NavCell_Type::Wall &&
				m_vecCell[(iCornerY) * m_CountH + (iCornerX + 1)]->m_CellType == NavCell_Type::Normal)
				return pCheckNode;
		}

		CNavCell* FindNode = nullptr;

		FindNode = GetBottomJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;

		FindNode = GetRightJumpPoint(pCheckNode, EndNode);

		if (FindNode)
			return pCheckNode;
	}

	return nullptr;
}

bool CNavigation::SortOpenList(CNavCell* Src, CNavCell* Dest)
{
	return Src->m_Total > Dest->m_Total;
}

//int CNavigation::SortOpenList(const void* Src, const void* Dest)
//{
//	CNavCell* SrcCell = (CNavCell*)Src;
//	CNavCell* DestCell = (CNavCell*)Dest;
//
//	if (SrcCell->m_Total < DestCell->m_Total)
//		return 1;
//
//	else if (SrcCell->m_Total > DestCell->m_Total)
//		return -1;
//
//	return 0;
//}
