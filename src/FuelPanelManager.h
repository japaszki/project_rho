#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class UIManager;
class Unit;

class FuelPanelManager : public UIPanelBase
{
public:
	FuelPanelManager(UIManager* UIManagerHandle);
	~FuelPanelManager(void);

	void setTargetUnit(Unit* Target);
	void RemoveTarget();
	bool IsHit(CEGUI::Vector2 Coords); //Check if the given position hits this panel

private:
	UIManager* Parent;
	Unit* TargetUnit;
};

