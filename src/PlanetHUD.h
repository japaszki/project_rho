#pragma once
#include "stdafx.h"
#include "Utils.h"

class PlanetHUD : public Utils
{
public:
	PlanetHUD(Planet * PlanetReference);
	~PlanetHUD(void);

	void UpdateHUD();
	void RedrawOrbit();
	void setHUDSphereRadius(double Radius);
	void setOcclusionSphereRadius(double Radius);
	Ogre::Sphere getHUDSphere();
	Ogre::Sphere getOcclusionSphere();
	Planet* getParent();

	void Select();
	void Deselect();
	void Highlight();
	void Unhighlight();

private:
	Planet * Parent;

	//Ogre::ManualObject* PathObject; //Handle to orbital path
	Ogre::Entity* OrbitEntity; //Handle to orbital path model
	Ogre::SceneNode* PathNode; //Handle to node containing orbital path
	Ogre::SceneNode* HudNode; //Handle to node containing hud elements
	//Ogre::BillboardSet* HudBillboardSet; //Handle to HUD
	//Ogre::Billboard* Pred1Billboard; //Handle to billboard at 1st predicted position
	//Ogre::Billboard* Pred2Billboard; //Handle to billboard at 2nd predicted position
	//Ogre::Billboard* ApoapsisBillboard; //Handle to billboard at apoapsis
	//Ogre::Billboard* PeriapsisBillboard; //Handle to billboard at periapsis

	CEGUI::Window* Overlay; //CEGUI overlay

	bool Highlighted; //Has this planet been highlighted by the user?
	bool Selected; //Has this planet been selected by the user?
	Ogre::Sphere HUDSphere; //Sphere representing selectable region in UI
	Ogre::Sphere OcclusionSphere; //Sphere to check if HUD elements are occluded by planet
};

