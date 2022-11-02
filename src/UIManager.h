#pragma once
#include "stdafx.h"
#include "Utils.h"

//Forward declare any classes referenced
class ProjectRho;
class Unit;
class HUD;
class Planet;
class PlanetHUD;

class UIManager : public Utils
{
public:
	UIManager(ProjectRho* App, Ogre::Camera* PlayerCamera);
	~UIManager(void);

	void AddHUD(HUD* HUDObject);
	void RemoveHUD(HUD * HUDObject);
	void AddPlanetHUD(PlanetHUD* HUDObject);
	void RemovePlanetHUD(PlanetHUD * HUDObject);
	void MouseMoved(OIS::MouseState State);
	void MousePressed(OIS::MouseState State, OIS::MouseButtonID id);
	void MouseReleased(OIS::MouseState State, OIS::MouseButtonID id);
	void UpdateUI(double TimeStep);
	bool isOccludedByPlanet(Ogre::Vector3 Position);

	ProjectRho* getAppHandle();
	Ogre::Camera* getCamera();

private:
	void ProcessSelectionBox(); //Select units in box
	void DeselectList(); //Deselect every unit in SelectedUnitList
	void CheckSelectedUnitList(); //Check if SelectedUnitList has only one element, if so move to SelectedUnit

	void setPanelTarget(Unit* Target); //set target of all GUI panels
	void RemovePanelTarget(); //remove target of all GUI panels
	bool IsMouseOverPanel(); //return true if mouse is over a GUI panel

	ProjectRho* AppHandle;
	CEGUI::OgreRenderer* mRenderer; //CEGUI Renderer!
	CameraManager* mCameraManager;
	FuelPanelManager* mFuelPanelManager;
	ControlPanelManager* mControlPanelManager;

	std::vector<HUD*> HUDList; //List of unit HUD objects
	std::vector<PlanetHUD*> PlanetHUDList; //List of planet HUD objects
	Unit* HighlightedUnit;
	Planet* HighlightedPlanet;
	Unit* SelectedUnit;
	Planet* SelectedPlanet;
	std::vector<Unit*> SelectedUnitList;
	OIS::MouseState CurrentMouseState;
	Ogre::Camera* PlayerCameraHandle;
	CEGUI::Window* SelectionBox;
	Ogre::Vector2 InitialSelectionPoint; //Starting point of selection box
	Ogre::Real TargetTime;

	bool SelectionMode; //Is a selection box being drawn?
	bool LMB_Down; //Is the Left Mouse Button down? Only used for drawing of selection box!
};

