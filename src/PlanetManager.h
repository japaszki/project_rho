#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class Planet;
class ProjectRho;

class PlanetManager
{
public:
	PlanetManager(ProjectRho* App);
	~PlanetManager(void);

	void UpdateAllPlanets(double TimeStep);
	Planet* AddPlanet(std::string PlanetName);
	Planet* getPlanet(std::string PlanetName);
	Ogre::Vector3 getAcceleration(Ogre::Vector3 Position); //Acceleration of a body at a given position due to combined gravity of planets
	Planet* getMajorBody(Ogre::Vector3 Position);
private:
	ProjectRho* AppHandle;
	std::vector<Planet*> PlanetArray;
};

