#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class UIManager;
class Unit;

class UIPanelBase
{
public:
	UIPanelBase(void);
	~UIPanelBase(void);

	void setTargetUnit(Unit* Target);
	void RemoveTarget();

private:
//	Unit* TargetUnit;
};

