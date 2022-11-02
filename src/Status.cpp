#include "Status.h"


Status::Status(Unit* UnitReference)
{
	Parent = UnitReference;
}

Status::~Status(void)
{
}

void Status::setUnitParameters(UnitParameters UnitParams)
{
	mUnitParameters = UnitParams;
	RecalculateMassAndInertia();
}

UnitParameters Status::getUnitParameters()
{
	return mUnitParameters;
}

double Status::getMass()
{
	return OverallMass;
}

Ogre::Vector3 Status::getInertia()
{
	return OverallInertia;
}

void Status::RecalculateMassAndInertia()
{
	OverallMass = getLevelMass(&mUnitParameters);
	CGOffset = getLevelCG(&mUnitParameters);
	OverallInertia = getLevelInertia(&mUnitParameters);
}

Ogre::Vector3 Status::getCGOffset()
{
	return CGOffset;
}

Ogre::Vector3 Status::getLevelCG(UnitParameters* UnitParams)
{
	Ogre::Vector3 VectorSum = Ogre::Vector3::ZERO;
	double ScalarSum = 0;

	ScalarSum += UnitParams->mHull.Mass;

	for(int x = UnitParams->FuelList.size() - 1; x >= 0; x--)
	{
		double CurrentFuelMass = UnitParams->FuelList[x].Capacity * UnitParams->FuelList[x].Amount;
		ScalarSum += CurrentFuelMass;
		VectorSum += CurrentFuelMass * UnitParams->FuelList[x].Position;
	}
	for(int x = UnitParams->CoolantList.size() - 1; x >= 0; x--)
	{
		double CurrentCoolantMass = UnitParams->CoolantList[x].Capacity * UnitParams->CoolantList[x].Amount;
		ScalarSum += CurrentCoolantMass;
		VectorSum += CurrentCoolantMass * UnitParams->CoolantList[x].Position;
	}
	for(int x = UnitParams->CarriedUnitList.size() - 1; x >= 0; x--)
	{
		double CurrentUnitMass = getLevelMass(UnitParams->CarriedUnitList[x].mUnitParameters);
		ScalarSum += CurrentUnitMass;
		VectorSum += CurrentUnitMass * (UnitParams->CarriedUnitList[x].Position + getLevelCG(UnitParams->CarriedUnitList[x].mUnitParameters));
	}

	if(ScalarSum == 0)
	{
		return Ogre::Vector3::ZERO;
	}
	else
	{
		return VectorSum / ScalarSum;
	}
}

double Status::getLevelMass(UnitParameters* UnitParams)
{
	double LevelMass = 0;

	LevelMass += UnitParams->mHull.Mass;
	for(int x = UnitParams->CarriedUnitList.size() - 1; x >= 0; x--)
	{
		LevelMass += getLevelMass(UnitParams->CarriedUnitList[x].mUnitParameters);
	}
	return LevelMass;
}

Ogre::Vector3 Status::getLevelInertia(UnitParameters* UnitParams)
{
	Ogre::Vector3 LevelInertia = Ogre::Vector3::ZERO;
	Ogre::Vector3 LevelCG = getLevelCG(UnitParams);

	LevelInertia += UnitParams->mHull.Inertia;

	for(int x = UnitParams->FuelList.size() - 1; x >= 0; x--)
	{
		double CurrentFuelMass = UnitParams->FuelList[x].Capacity * UnitParams->FuelList[x].Amount;
		
		LevelInertia.x += CurrentFuelMass * pow((UnitParams->FuelList[x].Position.x - LevelCG.x),2);
		LevelInertia.y += CurrentFuelMass * pow((UnitParams->FuelList[x].Position.y - LevelCG.y),2);
		LevelInertia.z += CurrentFuelMass * pow((UnitParams->FuelList[x].Position.z - LevelCG.z),2);
	}
	for(int x = UnitParams->CoolantList.size() - 1; x >= 0; x--)
	{
		double CurrentCoolantMass = UnitParams->CoolantList[x].Capacity * UnitParams->CoolantList[x].Amount;
		
		LevelInertia.x += CurrentCoolantMass * pow((UnitParams->CoolantList[x].Position.x - LevelCG.x),2);
		LevelInertia.y += CurrentCoolantMass * pow((UnitParams->CoolantList[x].Position.y - LevelCG.y),2);
		LevelInertia.z += CurrentCoolantMass * pow((UnitParams->CoolantList[x].Position.z - LevelCG.z),2);
	}
	for(int x = UnitParams->CarriedUnitList.size() - 1; x >= 0; x--)
	{
		double CurrentUnitMass = getLevelMass(UnitParams->CarriedUnitList[x].mUnitParameters);
		
		//add moment of inertia of carried unit
		LevelInertia += getLevelInertia(UnitParams->CarriedUnitList[x].mUnitParameters);

		//apply parallel axis theorem to shift axes of carried unit's CG
		LevelInertia.x += CurrentUnitMass * pow((UnitParams->CarriedUnitList[x].Position.x + getLevelCG(UnitParams->CarriedUnitList[x].mUnitParameters).x - LevelCG.x),2);
		LevelInertia.y += CurrentUnitMass * pow((UnitParams->CarriedUnitList[x].Position.y + getLevelCG(UnitParams->CarriedUnitList[x].mUnitParameters).y - LevelCG.y),2);
		LevelInertia.z += CurrentUnitMass * pow((UnitParams->CarriedUnitList[x].Position.z + getLevelCG(UnitParams->CarriedUnitList[x].mUnitParameters).z - LevelCG.z),2);
	}

	return LevelInertia;
}

Ogre::Vector3 Status::getAcceleration()
{
	return NetForce / OverallMass;
}

Ogre::Vector3 Status::getAngularAcceleration()
{
	Ogre::Vector3 AngularAcceleration;
	AngularAcceleration.x = NetTorque.x / OverallInertia.x;
	AngularAcceleration.y = NetTorque.y / OverallInertia.y;
	AngularAcceleration.z = NetTorque.z / OverallInertia.z;

	return AngularAcceleration;
}

void Status::Update(Ogre::Real TimeStep)
{
	UpdateLevel(&mUnitParameters, TimeStep, CGOffset, &NetForce, &NetTorque);
}

void Status::UpdateLevel(UnitParameters* UnitParams, Ogre::Real TimeStep, Ogre::Vector3 ThrustOffset, Ogre::Vector3* NetThrust, Ogre::Vector3* NetTorque)
{
	*NetThrust = Ogre::Vector3::ZERO;
	*NetTorque = Ogre::Vector3::ZERO;
	//iterate through engine list
	for(int x = UnitParams->EngineList.size() - 1; x >= 0; x--)
	{
		//check if engine is active
		if(UnitParams->EngineList[x].Throttle >= 0.0)
		{
			//iterate through fuel list
			for(int y = UnitParams->FuelList.size() - 1; y >= 0; y--)
			{
				//if correct fuel type and not empty
				if((UnitParams->FuelList[y].FuelType == UnitParams->EngineList[x].FuelType)&&(UnitParams->FuelList[y].Amount > 0.0))
				{
					double CurrentThrust;
					CurrentThrust = UnitParams->EngineList[x].Thrust * UnitParams->EngineList[x].Throttle;

					//deduct fuel mass
					UnitParams->FuelList[y].Amount -= UnitParams->EngineList[x].FuelConsumption * UnitParams->EngineList[x].Throttle * TimeStep / UnitParams->FuelList[y].Capacity;
					//add to net thrust
					*NetThrust += CurrentThrust * UnitParams->EngineList[x].Direction * Ogre::Vector3::UNIT_Z;
					//add to net torque
					Ogre::Vector3 PerpendicularDistance;
					Ogre::Vector3 NetOffset;

					NetOffset = UnitParams->EngineList[x].ThrustPosition - ThrustOffset;
					PerpendicularDistance =  NetOffset - NetOffset.dotProduct(UnitParams->EngineList[x].Direction * Ogre::Vector3::UNIT_Z) * UnitParams->EngineList[x].Direction * Ogre::Vector3::UNIT_Z;
					*NetTorque += (UnitParams->EngineList[x].Direction * Ogre::Vector3::UNIT_Z).crossProduct(PerpendicularDistance) * CurrentThrust;

					//stop searching for fuel
					break;
				}
			}
		}
	}

	//iterate through turret list
	for(int x = UnitParams->TurretList.size()-1; x >= 0; x--)
	{
		UnitParams->TurretList[x].Update(TimeStep);
		Parent->getVisuals()->setTurretOrientation(UnitParams->TurretList[x].TurretName, UnitParams->TurretList[x].getAzimuth(), UnitParams->TurretList[x].getElevation());
	}

	//Update carried units
	for(int x = UnitParams->CarriedUnitList.size()-1; x >= 0; x--)
	{
		if(UnitParams->CarriedUnitList[x].mCarriedUnitType == CARRIEDUNITTYPE_BOOSTER)
		{
			Ogre::Vector3 TempThrust, TempTorque;
			UpdateLevel(UnitParams->CarriedUnitList[x].mUnitParameters, TimeStep, ThrustOffset, &TempThrust, &TempTorque);
			*NetThrust += TempThrust;
			*NetTorque += TempTorque;
		}
	}
}