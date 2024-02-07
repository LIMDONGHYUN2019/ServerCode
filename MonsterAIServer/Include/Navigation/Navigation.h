#pragma once

#include "../AIInfo.h"

class CNavigation
{
public:
	CNavigation();
	CNavigation(const CNavigation& Nav);
	~CNavigation();

private:
	vector<class CNavCell*>	m_vecCell;
	vector<class CNavCell*>	m_vecOpen;
	vector<class CNavCell*>	m_vecUseCell;
	int						m_UseCount;
	int	m_Count;
	int m_CountH;
	int m_CountV;
	float	m_CellWidth;
	float	m_CellHeight;
	Vector3	m_Min;
	Vector3	m_Max;
	bool	m_FindPath;

public:
	bool Init(const char* FullPath);
	bool Init(int CountX, int CountY);
	void SetCellType(int IndexX, int IndexY, NavCell_Type Type);
	CNavigation* Clone();
	bool FindPath(vector<Vector3>& vecResult, const Vector3& Start, const Vector3& End);

private:
	bool FindJumpPoint(class CNavCell* Node, class CNavCell* EndNode, const Vector3& EndPos, vector<Vector3>& vecResult);
	void AddDir(JumpPoint_Dir Dir, class CNavCell* Node);
	class CNavCell* GetJumpPointNode(JumpPoint_Dir Dir, class CNavCell* Node, class CNavCell* EndNode, const Vector3& EndPos);
	class CNavCell* GetLeftJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetRightJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetTopJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetBottomJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetLTJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetRTJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetLBJumpPoint(class CNavCell* Node, class CNavCell* EndNode);
	class CNavCell* GetRBJumpPoint(class CNavCell* Node, class CNavCell* EndNode);

private:
	//static int SortOpenList(const void* Src, const void* Dest);
	static bool SortOpenList(CNavCell* Src, CNavCell* Dest);
};


   