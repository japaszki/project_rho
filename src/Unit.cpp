#pragma once
#include "Unit.h"

Unit::Unit(ProjectRho* App, int ID)
{
	AppHandle = App;
	UnitPhysics = new Physics(this);
	UnitHUD = new HUD(this);
	UnitStatus = new Status(this);
	UnitVisuals = new Visuals(this);

	UpdateCounter = 0;
	UnitID = ID;
}

Unit::~Unit(void)
{
	AppHandle->getSceneManager()->destroySceneNode(UnitNode);
	delete UnitPhysics;
	delete UnitHUD;
}

void Unit::UpdateUnit(Ogre::Real TimeStep)
{
	UnitStatus->Update(TimeStep);

	for(int i = 1; i <= 20; i++) //20 steps of physics per step of graphics
	{ 
		UnitPhysics->IterateGrav(0.05 * TimeStep);
		UnitPhysics->PropagateRotation(0.05 * TimeStep);
	}
	UnitVisuals->Update(TimeStep);
	UnitHUD->UpdateHUD();
	UnitHUD->RedrawOrbit();
}

void Unit::UpdatePath()
{	
	UnitPhysics->RecalculateOrbitalParameters();
	//UnitHUD->RedrawOrbit();
}

bool Unit::ConditionalPathUpdate()
{
	if(UpdateCounter-- <= 0)
	{
		UpdatePath();
		UpdateCounter = 200;
		return true;
	}
	else
	{
		return false;
	}
}

void Unit::RequestPathUpdate()
{
	UpdateCounter = 0;
}

ProjectRho* Unit::getAppHandle()
{
	return AppHandle;
}

Physics* Unit::getPhysics()
{
	return UnitPhysics;
}

HUD* Unit::getHUD()
{
	return UnitHUD;
}

Status* Unit::getStatus()
{
	return UnitStatus;
}

Visuals* Unit::getVisuals()
{
	return UnitVisuals;
}

int Unit::getUnitID()
{
	return UnitID;
}