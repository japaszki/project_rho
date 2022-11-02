#include "stdafx.h"


PlanetManager::PlanetManager(ProjectRho* App)
{
	AppHandle = App;
}

PlanetManager::~PlanetManager(void)
{
}

Planet* PlanetManager::AddPlanet(std::string PlanetName)
{
	Planet* NewPlanet = new Planet(AppHandle, PlanetName);
	PlanetArray.push_back(NewPlanet);
	return NewPlanet;
}

void PlanetManager::UpdateAllPlanets(double TimeStep)
{
	for(int i = PlanetArray.size() - 1; i >= 0; i--)
	{
		PlanetArray[i]->UpdatePlanet(TimeStep);
	}
}

Ogre::Vector3 PlanetManager::getAcceleration(Ogre::Vector3 Position)
{
	Ogre::Vector3 CurrentAccel = Ogre::Vector3::ZERO; //Intermediate result of acceleration
	double DistFactor; // Intermediate value in calculating gravitational force
	double Strength;

	if(PlanetArray.size() > 0)
	{
		for(int i = PlanetArray.size() - 1; i >= 0; i--)
		{
			DistFactor = pow(	pow((static_cast<double>(PlanetArray[i]->getPosition().x) - Position.x),2) + 
				pow((static_cast<double>(PlanetArray[i]->getPosition().y) - Position.y),2) + 
				pow((static_cast<double>(PlanetArray[i]->getPosition().z) - Position.z),2)	,1.5);

			Strength = PlanetArray[i]->getPlanetData().GravityStrength; //Store strength value to reduce number of calls to getGravityStrength() function

			CurrentAccel.x += (PlanetArray[i]->getPosition().x - Position.x) * Strength / DistFactor;
			CurrentAccel.y += (PlanetArray[i]->getPosition().y - Position.y) * Strength / DistFactor;
			CurrentAccel.z += (PlanetArray[i]->getPosition().z - Position.z) * Strength / DistFactor;
		}
	}
	return CurrentAccel;
}

Planet* PlanetManager::getMajorBody(Ogre::Vector3 Position)
{
	std::vector<double> AccelerationList; //list of magnitudes of acceleration due to each planet

	if(PlanetArray.size() == 0)
	{
		return NULL;
	}
	else if(PlanetArray.size() == 1)
	{
		return PlanetArray[0];
	}
	else 
	{
		//generate list of magnitudes of accelerations due to each planet
		AccelerationList.reserve(PlanetArray.size());
		for(int i = 0; i < PlanetArray.size(); i++)
		{
			AccelerationList.push_back(PlanetArray[i]->getPlanetData().GravityStrength / (Position - PlanetArray[i]->getPosition()).squaredLength());
		}
		//find largest acceleration component
		//double LargestAcc = 0;
		int LargestIndex = 0;

		for(int i = AccelerationList.size() - 1; i >= 0; i--)
		{
			if(AccelerationList[i] > AccelerationList[LargestIndex])
			{
				LargestIndex = i;
			}
		}

		//find second largest component
		//initialise Second largest index to be not equal to the largest index
		int SecondLargestIndex;
		if(LargestIndex == 0)
		{
			SecondLargestIndex = 1;
		}
		else
		{
			SecondLargestIndex = 0;
		}

		for(int i = AccelerationList.size() - 1; i >= 0; i--)
		{

			if((AccelerationList[i] > AccelerationList[SecondLargestIndex])&&(i != LargestIndex))
			{
				SecondLargestIndex = i;
			}
		}

		//determine if largest component is much larger than the rest
		if(AccelerationList[LargestIndex] > 10 * AccelerationList[SecondLargestIndex])
		{
			return PlanetArray[LargestIndex];
		}
		else
		{
			return NULL;
		}
	}
}