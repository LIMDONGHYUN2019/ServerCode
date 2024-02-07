#pragma once

#include "../AIInfo.h"

class CNavCell
{
	friend class CNavigation;

private:
	CNavCell();
	CNavCell(const CNavCell& Cell);
	~CNavCell();

private:
	CNavCell* m_Parent;
	NavCell_Type	m_CellType;
	NavNode_Type	m_NodeType;
	int				m_IndexX;
	int				m_IndexY;
	int				m_Index;
	float			m_Width;
	float			m_Height;

	float			m_G;
	float			m_H;
	float			m_Total;

	Vector3			m_Center;
	vector<JumpPoint_Dir>	m_vecFindDir;

public:
	CNavCell* Clone();
};


