#pragma once
#include "stdafx.h"
#include "Utils.h"

//Forward declare any classes referenced
class Unit;

class HUD : public Utils
{
public:
	HUD(Unit * UnitReference);
	~HUD(void);

	void UpdateHUD();
	void RedrawOrbit();
	Ogre::Sphere getHUDSphere();
	Unit* getParent();

	void Select();
	void Deselect();
	void Highlight();
	void Unhighlight();

private:
	Unit* Parent; //Reference to parent unit

	//Ogre::ManualObject* PathObject; //Handle to orbital path
	Ogre::Entity* OrbitEntity; //Handle to orbital path model
	Ogre::SceneNode* PathNode; //Handle to node containing orbital path
	Ogre::SceneNode* HudNode; //Handle to node containing hud elements
	//Handles to billboardsets containing HUD billboards.
	Ogre::BillboardSet* Pred1BillboardSet;
	Ogre::BillboardSet* Pred2BillboardSet;
	//Ogre::BillboardSet* ApoapsisBillboardSet;
	//Ogre::BillboardSet* PeriapsisBillboardSet;
	Ogre::Billboard* Pred1Billboard; //Handle to billboard at 1st predicted position
	Ogre::Billboard* Pred2Billboard; //Handle to billboard at 2nd predicted position
	//Ogre::Billboard* ApoapsisBillboard; //Handle to billboard at apoapsis
	//Ogre::Billboard* PeriapsisBillboard; //Handle to billboard at periapsis

	CEGUI::Window* UnitBox; //CEGUI overlay
	CEGUI::Window* UnitIcon;
	CEGUI::Window* ApoapsisIcon;
	CEGUI::Window* PeriapsisIcon;

	bool Highlighted; //Has this unit been highlighted by the user?
	bool Selected; //Has this unit been selected by the user?
	Ogre::Sphere HUDSphere; //Sphere representing selectable region in UI
};

