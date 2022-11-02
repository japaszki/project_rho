#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class UIManager;

class ControlPanelManager : public UIPanelBase
{
public:
	ControlPanelManager(UIManager* UIManagerHandle);
	~ControlPanelManager(void);

	void setTargetUnit(Unit* Target);
	void RemoveTarget();
	bool IsHit(CEGUI::Vector2 Coords); //Check if the given position hits this panel

	//testing visuals class state
	bool ChangeVisualStatus(const CEGUI::EventArgs &e);

private:
	UIManager* Parent;
	Unit* TargetUnit;
};

