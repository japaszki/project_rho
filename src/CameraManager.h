#pragma once
#include "stdafx.h"
#include "UIManager.h"
#include "Utils.h"

//Forward declare any classes referenced
class UIManager;
class Unit;
class Planet;

enum TargetDataType
{
	TARGETDATATYPE_NONE,
	TARGETDATATYPE_POSITION,
	TARGETDATATYPE_UNIT,
	TARGETDATATYPE_PLANET
};

struct TargetData
{
	//int Type; //0=none, 1=position, 2=unit, 3=planet
	TargetDataType Type;
	Ogre::Vector3 Position;
	Unit* TargetUnit;
	Planet* TargetPlanet;
};

class CameraManager : public Utils
{
public:
	CameraManager(UIManager* UIManagerHandle, Ogre::Camera* PlayerCamera);
	~CameraManager(void);

	void MouseMoved(OIS::MouseState State);
	void MousePressed(OIS::MouseState State, OIS::MouseButtonID id);
	void MouseReleased(OIS::MouseState State, OIS::MouseButtonID id);
	void Update(double TimeStep);

	//Tell the camera manager which unit has been selected so that it knows to follow it
	void setTargetUnit(Unit* Target);
	void setTargetPosition(Ogre::Vector3 TargetPosition);
	void RemoveTargetUnit(Unit* Target); //Remove target if target is this unit
	void setTargetPlanet(Planet* Target);
	void RemoveTargetPlanet(Planet* Target); //Remove target if target is this planet
	void RemoveTarget();
	Ogre::Real getTargetUnitDistance();

private:
	UIManager* Parent;
	Ogre::Camera* PlayerCameraHandle;
	//Ogre::SceneNode* CameraTransNode; Not used?
	Ogre::SceneNode* CameraYawNode;
	Ogre::SceneNode* CameraPitchNode;
	Ogre::Vector3 MouseVel;
	OIS::MouseState mMouseState;

	Ogre::Real TranslateProgress; //What fraction of the translation to the target pos has been completed
	Ogre::Vector3 InitialPositionDifference;
	TargetData mTargetData; //Target to follow

	void setMousePosition(int x, int y); //move mouse to this position (in pixels)

	bool RotationMode; //Is the camera being rotated?
};

