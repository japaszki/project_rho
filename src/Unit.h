#pragma once
#include "stdafx.h"

class Unit
{
public:
	Unit(ProjectRho* App, int ID);
	~Unit(void);

	int getUnitID();
	ProjectRho* getAppHandle();
	Physics* getPhysics();
	HUD* getHUD();
	Status* getStatus();
	Visuals* getVisuals();

	void UpdateUnit(Ogre::Real TimeStep); //Update Ogre Node to reflect current state
	bool ConditionalPathUpdate(); //Update path if counter has reached zero
	void RequestPathUpdate(); //Set path update counter to zero

private:
	ProjectRho* AppHandle;
	Ogre::SceneNode* UnitNode; //Handle to unit node
	Physics* UnitPhysics; //Handle to Physics specialist class
	HUD* UnitHUD; //Handle to HUD specialist
	Status* UnitStatus; //Handle to Status specialist
	Visuals* UnitVisuals; //Handle to Visuals specialist class

	int UnitID; //Unique indentifier of unit
	int UpdateCounter; //Frames since last orbital path update
	void UpdatePath(); //Recalculate path of orbit based on current velocity and position
};

