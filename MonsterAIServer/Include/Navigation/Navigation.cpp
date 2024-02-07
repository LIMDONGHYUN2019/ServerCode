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
	// 언리얼에서 int로 저장하면 uint로 저장된느거 같은데
	// 추가로 언리얼에서 문자열을 저장할떄 \0까지 저장하는거같다.
	// 그래서 문장열앞에 이 \0까지 포함한 문자열 길이를 문자열앞에 넣어준다. 

	fread(&m_Count, sizeof(int), 1, pFile); // 장애물 갯수
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
	// 시작위치와 도착위치를 이용해서 해당 노드를 찾는다.
	if (m_Min.x > Start.x || m_Max.x < Start.x || m_Min.y > Start.y || m_Max.y < Start.y)
		return false;

	if (m_Min.x > End.x || m_Max.x < End.x || m_Min.y > End.y || m_Max.y < End.y)
		return false;

	float	Height = m_Max.x - m_Min.x;

	// 위치에서 Min값을 빼준다. 원점을 시작으로 한 위치로 변환을 해주는 것이다.
	Vector3	ConvertStart = Start - m_Min;
	Vector3	ConvertEnd = End - m_Min;

	//ConvertStart.x = Height - ConvertStart.x;
	//ConvertEnd.x = Height - ConvertEnd.x;

	// 한칸의 크기를 나누어주어서 가로 세로 크기가 1x1 한 위치로 변환해준다.
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

	// 만약 이 내용을 근처를 찾아가게 해야 된다면 현재 셀을 중심으로 갈 수 있는 셀을 찾는다.
	if (EndCell->m_CellType == NavCell_Type::Wall)
		return false;

	vecResult.clear();

	// 같은 셀을 이동해야 되는 경우 해당 위치를 바로 준다.
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

	// 시작 셀을 열린목록에 넣어둔다.
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

	// 노드들을 조사한다.
	while (!m_vecOpen.empty())
	{
		CNavCell* Node = m_vecOpen.back();
		m_vecOpen.pop_back();

		Node->m_NodeType = NavNode_Type::Close;

		// 8방향을 검사하여 점프포인트가 있는곳들을 구해서 열린목록에 넣어준다.
		if (FindJumpPoint(Node, EndCell, End, vecResult))
			break;

		// 정렬한다.
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
	// 각 방향별로 점프포인트가 있는지를 판단해야 한다.
	for (size_t i = 0; i < Node->m_vecFindDir.size(); ++i)
	{
		CNavCell* Corner = GetJumpPointNode(Node->m_vecFindDir[i], Node, EndNode, EndPos);

		if (!Corner)
			continue;

		// 찾아온 노드가 도착노드일 경우
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

			// 시작 노드 제거
			vecResult.pop_back();

			return true;
		}

		// 이동 비용을 구해준다.
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
		// 1칸씩 왼쪽으로 가면서 코너가 있는지를 판단한다.
		--IndexX;

		if (IndexX < 0)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[Node->m_IndexY * m_CountH + IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		// 현재 위치가 도착 노드인지를 판단한다.
		else if (CheckNode == EndNode)
			return CheckNode;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// 왼쪽으로 갈때의 코너는 위쪽, 아래쪽 코너 뿐이다.
		// 위쪽 코너는 현 위치에서 위쪽이 막혀있고 왼쪽 위가 열려있으면 위쪽 코너이다.
		// 아래쪽 코너는 현 위치에서 아래쪽이 막혀있고 왼쪽 아래가 열려있으면 아래쪽 코너이다.
		// 위쪽 먼저 검사한다. 위쪽 셀을 얻어온다.
		int	CornerX = IndexX;
		int	CornerY = Node->m_IndexY - 1; // 아래쪽 체크

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
		// 1칸씩 왼쪽으로 가면서 코너가 있는지를 판단한다.
		++IndexX;

		if (IndexX >= m_CountH)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[Node->m_IndexY * m_CountH + IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// 현재 위치가 도착 노드인지를 판단한다.
		else if (CheckNode == EndNode)
			return CheckNode;

		// 왼쪽으로 갈때의 코너는 위쪽, 아래쪽 코너 뿐이다.
		// 위쪽 코너는 현 위치에서 위쪽이 막혀있고 왼쪽 위가 열려있으면 위쪽 코너이다.
		// 아래쪽 코너는 현 위치에서 아래쪽이 막혀있고 왼쪽 아래가 열려있으면 아래쪽 코너이다.
		// 위쪽 먼저 검사한다. 위쪽 셀을 얻어온다.
		int	CornerX = IndexX;
		int	CornerY = Node->m_IndexY - 1; // 위쪽 체크

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
		// 1칸씩 왼쪽으로 가면서 코너가 있는지를 판단한다.
		--IndexY;

		if (IndexY < 0)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[IndexY * m_CountH + Node->m_IndexX];
		//  현재 위치에서 위쪽 위치를 가져온다.

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// 현재 위치가 도착 노드인지를 판단한다.
		else if (CheckNode == EndNode)
			return CheckNode;
		 
		// 왼쪽으로 갈때의 코너는 위쪽, 아래쪽 코너 뿐이다.
		// 위쪽 코너는 현 위치에서 위쪽이 막혀있고 왼쪽 위가 열려있으면 위쪽 코너이다.
		// 아래쪽 코너는 현 위치에서 아래쪽이 막혀있고 왼쪽 아래가 열려있으면 아래쪽 코너이다.
		// 위쪽 먼저 검사한다. 위쪽 셀을 얻어온다.
		int	CornerX = Node->m_IndexX - 1;
		int	CornerY = IndexY; // Node->m_IndexY - 1 이거 아닌가? → 아님
		
		// 코너가 살아있다면 코너로 갈수있는 함수가 호출되도록하기위해
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
		// 1칸씩 왼쪽으로 가면서 코너가 있는지를 판단한다.
		++IndexY;

		if (IndexY >=m_CountV)
			return nullptr;

		CNavCell* CheckNode = m_vecCell[IndexY * m_CountH + Node->m_IndexX];

		if (CheckNode->m_CellType == NavCell_Type::Wall)
			return nullptr;

		else if (CheckNode->m_NodeType == NavNode_Type::Close)
			return nullptr;

		// 현재 위치가 도착 노드인지를 판단한다.
		else if (CheckNode == EndNode)
			return CheckNode;

		// 왼쪽으로 갈때의 코너는 위쪽, 아래쪽 코너 뿐이다.
		// 위쪽 코너는 현 위치에서 위쪽이 막혀있고 왼쪽 위가 열려있으면 위쪽 코너이다.
		// 아래쪽 코너는 현 위치에서 아래쪽이 막혀있고 왼쪽 아래가 열려있으면 아래쪽 코너이다.
		// 위쪽 먼저 검사한다. 위쪽 셀을 얻어온다.
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
