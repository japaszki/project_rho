#pragma once
#include "stdafx.h"

//Define structures used by this class
struct PlanetData
{
	double GravityStrength; //Indicates strength of gravity, equal to G*M
	Ogre::Radian Inclination;
	Ogre::Radian ArgumentOfPeriapsis;
	Ogre::Radian LongitudeOfAscendingNode;
	double SemiMajorAxis;
	double Eccentricity; //Elliptical and circular orbits only, ie. 0 <= e < 1
	double TimeOfPeriapsisPassage;
	double OrbitalPeriod;
	Planet* MajorBody; //Planet that this object is orbiting, NULL if not orbiting any other planet

	double RotationPeriod;
	Ogre::Radian InitialRotation;
	double HUDRadius;
	double OcclusionRadius;

	bool Sun; //Is this object a sun?
};

class Planet
{
public:
	Planet(ProjectRho* App, std::string PlanetName);
	~Planet(void);

	std::string getName();

	void UpdatePlanet(double TimeStep);
	void UpdateNode(); //Update Ogre Node to reflect current state

	ProjectRho* getAppHandle();
	Ogre::Vector3 getPosition(); //Return current position
	Ogre::Vector3 getVelocity(); //Return current velocity
	Ogre::Quaternion getFacing(); //Return facing quaternion

	void setPlanetData(PlanetData NewPlanetData, std::string Mesh);
	void IsSun(PlanetData SunData, std::string Mesh); //Calling this function will cause the planet to be stationary and at the centre of the coordinate system
	PlanetData getPlanetData();
	PlanetHUD* getHUD();

	Ogre::SceneNode* getNode();

private:
	ProjectRho* AppHandle;
	PlanetHUD* pHUD;
	Ogre::SceneNode* PlanetNode; //Handle to planet scenenode
	Ogre::Vector3 Position; //Current position in space
	Ogre::Vector3 Velocity;
	Ogre::Quaternion Facing; //Direction planet is facing

	std::string Name;
	PlanetData mPlanetData;

	double Time;

	double TimeList[1000]; //Lookup tables for position of planet
	Ogre::Radian TrueAnomalyList[1001];
	Ogre::Radian getTrueAnomaly();
};

