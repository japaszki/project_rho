#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class Unit;
class Planet;

//Define structures used by this class
struct OrbitalParameters
{
	Planet * MajorBody; //Planet unit is orbiting around;
	double SemiMajorAxis;
	double SemiMinorAxis;
	double Period; //If orbit elliptical value is zero
	double OrbitalEnergy; //Specific orbital energy
	double Eccentricity;

	Ogre::Vector3 AngularMomentum; //Specific relative angular momentum
	Ogre::Vector3 PeriapsisVector; //Position of Pe relative to major body
	Ogre::Vector3 ApoapsisVector; //Position of Ap relative to major body, if orbit elliptical, value is 0
};

class Physics
{
public:
	Physics(Unit* UnitReference);
	~Physics(void);

	Ogre::Vector3 Pred1Pos; //First predicted position
	Ogre::Vector3 Pred1Vel;  
	Ogre::Vector3 Pred1Acc;

	Ogre::Vector3 Pred2Pos; //Second predicted position
	Ogre::Vector3 Pred2Vel;
	Ogre::Vector3 Pred2Acc;

	OrbitalParameters Orbit;

	void CheckMajorBody(); // Check if there is a single gravitational source providing most of the acceleration

	Ogre::Quaternion getPrograde(); //Return prograde direction
	Ogre::Quaternion getRetrograde();
	Ogre::Quaternion getNormal();
	Ogre::Quaternion getAntiNormal();
	Ogre::Quaternion getCorrectionRoll(Ogre::Quaternion UpAxis); //Determine rotation necessary to point local +Y axis at given facing

	Ogre::Vector3 getPosition(); //Return current position
	void setPosition(Ogre::Vector3 Coord); //Change position
	Ogre::Vector3 getVelocity(); //Return current velocity
	void setVelocity(Ogre::Vector3 Coord); //Change velocity
	Ogre::Quaternion getFacing(); //Return facing quaternion
	void setFacing(Ogre::Quaternion Quat); //Change facing
	Ogre::Vector3 getAngularVelocity();
	void setAngularVelocity(Ogre::Vector3 Coord);

	void RecalculateOrbitalParameters();
	void IterateGrav(double TimeStep); //Apply gravitational acceleration to unit
	void Propagate(Ogre::Vector3 * Pos, Ogre::Vector3 * Vel, Ogre::Vector3 * Acc, bool Absolute, bool Engines, double dt); //Gives position and velocity after one time step
	void PropagateRotation(double dt); //Update facing and angular velocity after one time step
	Ogre::Vector3 getMajorBodyAccel(Ogre::Vector3 Pos); //!!Only call if major body is a valid pointer!!

private:
	Unit* Parent; //Reference to parent unit

	Ogre::Vector3 Position; //Current position in space (absolute)
	Ogre::Vector3 Velocity; //Current velocity
	Ogre::Vector3 RPosition; //Current position relative to major body, only valid if major body exists
	Ogre::Vector3 RVelocity; 
	Ogre::Vector3 Acceleration; //Current acceleration
	Ogre::Vector3 RotationVector; //Vector representation of facing
	Ogre::Vector3 AngularVelocity;
};

