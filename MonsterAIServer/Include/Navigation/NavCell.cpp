
#include "NavCell.h"

CNavCell::CNavCell() :
	m_CellType(NavCell_Type::Normal),
	m_NodeType(NavNode_Type::None),
	m_Parent(nullptr),
	m_G(FLT_MAX),
	m_H(FLT_MAX),
	m_Total(FLT_MAX)
{
}

CNavCell::CNavCell(const CNavCell& Cell)
{
	*this = Cell;

	m_Parent = nullptr;
}

CNavCell::~CNavCell()
{
}

CNavCell* CNavCell::Clone()
{
	return new CNavCell(*this);
}
