#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class Unit;

//Define structures used by this class

enum CarriedUnitType
{
	CARRIEDUNITTYPE_MISSILE,
	CARRIEDUNITTYPE_BOOSTER, //can fire engines while being carried. uses its own fuel
	CARRIEDUNITTYPE_FUELTANK, //parent unit can use this unit's fuel to power its engines
	CARRIEDUNITTYPE_PASSENGER //doesn't do anything while being carried
};

struct Engine
{
	//Constant parameters
	std::string EngineType; // Type of engine; helps AI choose engine to use
	Ogre::Quaternion Direction; //Direction of acceleration when active (MUST be unit quaternion)
	Ogre::Vector3 ThrustPosition; //Position at which thrust is applied
	double Thrust; //Thrust at full throttle
	double FuelConsumption; //Rate of fuel consumption at full throttle
	std::string FuelType; //Type of fuel used

	//Variable parameters
	double Throttle; //Current throttle setting
};

struct Fuel
{
	//Constant parameters
	std::string FuelType; //Type of fuel stored
	double Capacity; //Mass of fuel stored when full
	Ogre::Vector3 Position; //Position of point mass to represent inertia

	//Variable parameters
	double Amount; //fraction of tank full
};

struct Coolant
{
	//Constant parameters
	double SpecificHeatCapacity;
	double MinTemp;
	double MaxTemp;
	double Capacity; //Mass of coolant stored when full
	Ogre::Vector3 Position; //Position of point mass to represent inertia

	//Variable parameters
	double Temp; //current temperature
	double Amount; //fraction of tank full
};

struct Hull
{
	//Constant parameters
	double Mass; //dry mass of unit
	Ogre::Vector3 Inertia; //dry moment of inertia (note: CG of empty hull is taken to be at origin of entity)
};

//Forward declare struct before defining CarriedUnit
struct UnitParameters;

struct CarriedUnit
{
	UnitParameters* mUnitParameters; //reference to carried unit data
	CarriedUnitType mCarriedUnitType; //function of carried unit
	Ogre::Vector3 Position; //Position relative to CG of carrier unit
};

class TurretStatus
{
public:
	//constant parameters:
	std::string TurretName; //used to pass commands to TurretVisuals
	Ogre::Radian MinElevation;
	Ogre::Radian MaxElevation;
	Ogre::Radian MaxElevationSpeed;
	Ogre::Radian MinAzimuth; //azimuth must be between -pi and pi
	Ogre::Radian MaxAzimuth; //if MinAzimuth == MaxAzimuth, turret can rotate full circle
	Ogre::Radian MaxAzimuthSpeed;
	//Note: it is up to the user to make sure the following match up to the data given to the Visuals class:
	Ogre::Vector3 TurretAxis; //azimuth azis of rotation (relative to root)
	Ogre::Vector3 BarrelAxis; //elevation axis of rotation (relative to turret)
	Ogre::Vector3 BaseAxis; //Facing of gun at 0 elevation and 0 azimuth
	Ogre::Vector3 TurretPosition; //Origin of gun relative to unit centre

	TurretStatus()
	{
		Elevation = Ogre::Radian(0);
		Azimuth = Ogre::Radian(0);
		ElevationSpeed = Ogre::Radian(0);
		AzimuthSpeed = Ogre::Radian(0);
	}
	Ogre::Radian getElevation()
	{
		return Elevation;
	}
	Ogre::Radian getAzimuth()
	{
		return Azimuth;
	}
	Ogre::Radian getElevationSpeed()
	{
		return ElevationSpeed;
	}
	Ogre::Radian getAzimuthSpeed()
	{
		return AzimuthSpeed;
	}
	void setSpeed(Ogre::Radian NewElevationSpeed, Ogre::Radian NewAzimuthSpeed)
	{
		if(NewElevationSpeed > MaxElevationSpeed)
			ElevationSpeed = MaxElevationSpeed;
		else if(NewElevationSpeed < -MaxElevationSpeed)
			ElevationSpeed = -MaxElevationSpeed;
		else
			ElevationSpeed = NewElevationSpeed;

		if(NewAzimuthSpeed > MaxAzimuthSpeed)
			AzimuthSpeed = MaxAzimuthSpeed;
		else if(NewAzimuthSpeed < -MaxAzimuthSpeed)
			AzimuthSpeed = -MaxAzimuthSpeed;
		else
			AzimuthSpeed = NewAzimuthSpeed;
	}
	void Update(Ogre::Real TimeStep)
	{
		Elevation += ElevationSpeed * TimeStep;
		Azimuth += AzimuthSpeed * TimeStep;
		
		if(Azimuth > Ogre::Radian(Ogre::Math::PI))
			Azimuth -= Ogre::Radian(Ogre::Math::TWO_PI);
		if(Azimuth < Ogre::Radian(-Ogre::Math::PI))
			Azimuth += Ogre::Radian(Ogre::Math::TWO_PI);

		if(MinAzimuth != MaxAzimuth)
		{
			if(Azimuth > MaxAzimuth)
				Azimuth = MaxAzimuth;
			else if(Azimuth < MinAzimuth)
				Azimuth = MinAzimuth;
		}

		if(Elevation > MaxElevation)
			Elevation = MaxElevation;
		else if(Elevation < MinElevation)
			Elevation = MinElevation;
	}
private:
	Ogre::Radian Elevation;
	Ogre::Radian Azimuth;
	Ogre::Radian ElevationSpeed;
	Ogre::Radian AzimuthSpeed;
};

class UnitParameters //Parameters of current unit only, does not include specs of carried units
{
public:
	Hull mHull;
	std::vector<Engine> EngineList;
	std::vector<Fuel> FuelList; //preferably only one entry per fuel type
	std::vector<Coolant> CoolantList;
	std::vector<CarriedUnit> CarriedUnitList;
	std::vector<TurretStatus> TurretList;

	void AddEngine(Engine NewEngine)
	{
		EngineList.push_back(NewEngine);
	}
	void AddFuel(Fuel NewFuel)
	{
		FuelList.push_back(NewFuel);
	}
	void AddCoolant(Coolant NewCoolant)
	{
		CoolantList.push_back(NewCoolant);
	}
	void AddCarriedUnit(CarriedUnit NewCarriedUnit)
	{
		CarriedUnitList.push_back(NewCarriedUnit);
	}
	void AddTurret(TurretStatus NewTurret)
	{
		TurretList.push_back(NewTurret);
	}
};

class Status
{
public:
	Status(Unit* UnitReference);
	~Status(void);

	double getMass();
	Ogre::Vector3 getInertia();
	Ogre::Vector3 getCGOffset();
	Ogre::Vector3 getAcceleration(); //return acceleration relative to unit facing. doesn't automatically update mass
	Ogre::Vector3 getAngularAcceleration(); //angular acceleration relative to unit facing. doesn't automatically update inertia
	void setUnitParameters(UnitParameters UnitParams);
	UnitParameters getUnitParameters();
	
	void RecalculateMassAndInertia(); //update mass, inertia and cg
	void Update(Ogre::Real TimeStep); //update fuel, thrust, temperature; doesn't update total mass, inertia or CG

private:
	Unit* Parent; //Reference to parent unit

	UnitParameters mUnitParameters; //Parameters of this unit
	double OverallMass; //Total mass of unit and all material carried
	Ogre::Vector3 OverallInertia; //Moment of inertia about each principal axis, products of inertia are necessarily zero so are not specified
	Ogre::Vector3 CGOffset; //Offset of net CG from empty hull CG
	Ogre::Vector3 NetForce; //Total force exerted by all engines
	Ogre::Vector3 NetTorque;  //Total torque exerted by all engines

	Ogre::Vector3 getLevelCG(UnitParameters* UnitParams); //recursively calculate the CG position vector
	double getLevelMass(UnitParameters* UnitParams); //recursively calculate mass of this unit and that of its carried units
	Ogre::Vector3 getLevelInertia(UnitParameters* UnitParams); //recursively calculate moment of inertia about each axis
	void UpdateLevel(UnitParameters* UnitParams, Ogre::Real TimeStep, Ogre::Vector3 ThrustOffset, Ogre::Vector3* NetThrust, Ogre::Vector3* NetTorque); //recursively update fuel, thrust, temperature
};

