#include "Physics.h"

//Forward declare any classes referenced
class Unit;

Physics::Physics(Unit * UnitReference)
{
	Parent = UnitReference;
	Orbit.MajorBody = NULL;

	Position = Ogre::Vector3::ZERO;
	Velocity = Ogre::Vector3::ZERO;
	RPosition = Ogre::Vector3::ZERO;
	RVelocity = Ogre::Vector3::ZERO;
	Acceleration = Ogre::Vector3::ZERO;
	RotationVector = Ogre::Vector3::ZERO;
	AngularVelocity = Ogre::Vector3::ZERO;
}

Physics::~Physics(void)
{
}

void Physics::IterateGrav(double TimeStep)
{
	if(Orbit.MajorBody == NULL)
	{
		Propagate(&Position, &Velocity, &Acceleration, true, true, TimeStep);

		Propagate(&Pred1Pos, &Pred1Vel, &Pred1Acc, true, false, TimeStep);
		Propagate(&Pred2Pos, &Pred2Vel, &Pred2Acc, true, false, TimeStep);
	}
	else
	{
		Propagate(&RPosition, &RVelocity, &Acceleration, false,  true, TimeStep);

		Position = RPosition + Orbit.MajorBody->getPosition();
		Velocity = RVelocity + Orbit.MajorBody->getVelocity();

		Propagate(&Pred1Pos, &Pred1Vel, &Pred1Acc, false, false, TimeStep);
		Propagate(&Pred2Pos, &Pred2Vel, &Pred2Acc, false, false, TimeStep);
	}
}

void Physics::Propagate(Ogre::Vector3 * Pos, Ogre::Vector3 * Vel, Ogre::Vector3 * Acc, bool Absolute, bool Engines, double dt)
{
	//Velocity Verlet method
	*Pos += *Vel * dt + 0.5 * *Acc * pow(dt,2);
	*Vel += 0.5 * *Acc * dt;

	//Is our reference frame absolute or relative to a major body?
	if(Absolute)
	{
		*Acc = Parent->getAppHandle()->mPlanetMgr->getAcceleration(*Pos);
	}
	else
	{
		*Acc = getMajorBodyAccel(*Pos);
	}

	//Should engine thrust be taken into account?
	if(Engines)
	{
		*Acc += getFacing() * Parent->getStatus()->getAcceleration();
	}

	*Vel += 0.5 * *Acc * dt;
}

void Physics::PropagateRotation(double dt)
{
	RotationVector += AngularVelocity * dt;

	//Reset rotationvector if magnitude exceeds 2pi
	if(RotationVector.squaredLength() >= (4*Ogre::Math::PI*Ogre::Math::PI))
	{
		RotationVector -= RotationVector.normalisedCopy() * 2 * Ogre::Math::PI;
	}

	AngularVelocity += Parent->getStatus()->getAngularAcceleration() * dt;
}

Ogre::Vector3 Physics::getMajorBodyAccel(Ogre::Vector3 Pos)
{
	return - Pos * Orbit.MajorBody->getPlanetData().GravityStrength / pow((double)Pos.squaredLength(), 1.5);
}

Ogre::Vector3 Physics::getPosition()
{
	return Position;
}

void Physics::setPosition(Ogre::Vector3 Coord)
{
	Position = Coord;
	CheckMajorBody();

	if(Orbit.MajorBody == NULL)
	{
		Acceleration = Parent->getAppHandle()->mPlanetMgr->getAcceleration(Position);
	}
	else
	{
		Acceleration = getMajorBodyAccel(Position);
	}
}

Ogre::Vector3 Physics::getVelocity()
{
	return Velocity;
}

void Physics::setVelocity(Ogre::Vector3 Coord)
{
	Velocity = Coord;

	if(Orbit.MajorBody != NULL)
	{
		RVelocity = Velocity - Orbit.MajorBody->getVelocity();
	}
	Parent->RequestPathUpdate();
}

void Physics::setFacing(Ogre::Quaternion Quat)
{
	Ogre::Radian Angle;
	Quat.ToAngleAxis(Angle, RotationVector);
	RotationVector *= Angle.valueRadians();
}

Ogre::Quaternion Physics::getFacing()
{
	Ogre::Quaternion Facing;
	Facing.FromAngleAxis(Ogre::Radian(RotationVector.length()), RotationVector.normalisedCopy());
	return Facing;
}

Ogre::Vector3 Physics::getAngularVelocity()
{
	return AngularVelocity;
}

void Physics::setAngularVelocity(Ogre::Vector3 Coord)
{
	AngularVelocity = Coord;
}

Ogre::Quaternion Physics::getPrograde()
{
	if(Orbit.MajorBody == NULL)
	{
		return Ogre::Vector3::UNIT_Z.getRotationTo(Velocity);
	}
	else
	{
		return Ogre::Vector3::UNIT_Z.getRotationTo(RVelocity);
	}
}

Ogre::Quaternion Physics::getRetrograde()
{
	if(Orbit.MajorBody == NULL)
	{
		return Ogre::Vector3::UNIT_Z.getRotationTo(Velocity * -1);
	}
	else
	{
		return Ogre::Vector3::UNIT_Z.getRotationTo(RVelocity* -1);
	}
}

Ogre::Quaternion Physics::getNormal()
{
	//Orbit normal vector only defined when major body exists
	if(Orbit.MajorBody == NULL)
	{
		return Ogre::Quaternion::IDENTITY;
	}
	else
	{
		Ogre::Vector3 RPos;
		RPos = Position - Orbit.MajorBody->getPosition();
		return Ogre::Vector3::UNIT_Z.getRotationTo(RPos.crossProduct(Velocity));
	}
}

Ogre::Quaternion Physics::getAntiNormal()
{
	//Orbit antinormal vector only defined when major body exists
	if(Orbit.MajorBody == NULL)
	{
		return Ogre::Quaternion::IDENTITY;
	}
	else
	{
		Ogre::Vector3 RPos;
		RPos = Position - Orbit.MajorBody->getPosition();
		return Ogre::Vector3::UNIT_Z.getRotationTo(-RPos.crossProduct(Velocity));
	}
}

Ogre::Quaternion Physics::getCorrectionRoll(Ogre::Quaternion UpAxis)
{
	Ogre::Vector3 LocalY;
	LocalY = getFacing() * Ogre::Vector3::UNIT_Y;

	Ogre::Quaternion Roll;
	Ogre::Vector3 Init, Mid, Final, Temp;

	Init = LocalY.normalisedCopy();
	Final = UpAxis * Ogre::Vector3::UNIT_Z;
	Final = Final.normalisedCopy();
	Mid = Init.midPoint(Final).normalisedCopy();

	//Vector representing axis of rotation (vector component of quaternion)
	Temp = Ogre::Math::Sin(Init.angleBetween(Mid)) * Init.crossProduct(Final).normalisedCopy();
	
	Roll.w = Ogre::Math::Cos(Init.angleBetween(Mid));
	Roll.x = Temp.x;
	Roll.y = Temp.y;
	Roll.z = Temp.z;

	return Roll;
}

void Physics::CheckMajorBody()
{
	Planet* NewMajorBody = Parent->getAppHandle()->mPlanetMgr->getMajorBody(Position);

	//if entered sphere of influence of major body
	if((Orbit.MajorBody == NULL) && (NewMajorBody != NULL))
	{
		RPosition = Position - NewMajorBody->getPosition();
		RVelocity = Velocity - NewMajorBody->getVelocity();
	}
	//if leaving sphere of influence of major body
	if((Orbit.MajorBody != NULL) && (NewMajorBody == NULL))
	{
		Position = RPosition + Orbit.MajorBody->getPosition();
		Velocity = RVelocity + Orbit.MajorBody->getVelocity();
	}
	//if jumping from one sphere of influence to another
	if((Orbit.MajorBody != NULL) && (NewMajorBody != NULL) && (Orbit.MajorBody != NewMajorBody))
	{
		Position = RPosition + Orbit.MajorBody->getPosition();
		Velocity = RVelocity + Orbit.MajorBody->getVelocity();

		RPosition = Position - NewMajorBody->getPosition();
		RVelocity = Velocity - NewMajorBody->getVelocity();
	}
	//Update major body
	Orbit.MajorBody = NewMajorBody;
}

void Physics::RecalculateOrbitalParameters()
{
	CheckMajorBody();
	if(Orbit.MajorBody == NULL)
	{
		Ogre::Vector3 Pos = Position;
		Ogre::Vector3 Vel = Velocity;
		Ogre::Vector3 Acc = Parent->getAppHandle()->mPlanetMgr->getAcceleration(Position);

		//NOTE: Sort out time step to use for position predicition
		double TimeStep;
		TimeStep = 1;

		for(int n=0; n<4001; n++)
		{
			Propagate(&Pos, &Vel, &Acc, true, false, TimeStep); 

			if(n == 2000) //initialise first predicted position
			{
				Pred1Pos = Pos;
				Pred1Vel = Vel;
				Pred1Acc = Acc;
			}
			if(n == 4000) //initialise second predicted position
			{
				Pred2Pos = Pos;
				Pred2Vel = Vel;
				Pred2Acc = Acc;
			}
		}
	}
	else
	{
		Ogre::Vector3 RPos = RPosition;
		Ogre::Vector3 RVel = RVelocity;
		Ogre::Vector3 Acc = getMajorBodyAccel(RPosition);

		Orbit.OrbitalEnergy = RVel.squaredLength()/2 - Orbit.MajorBody->getPlanetData().GravityStrength/RPos.length();
		Orbit.SemiMajorAxis = - Orbit.MajorBody->getPlanetData().GravityStrength/(2*Orbit.OrbitalEnergy);
		Orbit.AngularMomentum = RPos.crossProduct(RVel); 
		Orbit.Eccentricity = pow(1 + 2 * Orbit.OrbitalEnergy * Orbit.AngularMomentum.squaredLength() / pow(Orbit.MajorBody->getPlanetData().GravityStrength,2),0.5);
		Orbit.SemiMinorAxis = Orbit.SemiMajorAxis * sqrt(abs(1 - pow(Orbit.Eccentricity,2)));

		if(Orbit.Eccentricity < 1)
		{
			Orbit.Period = 2 * Ogre::Math::PI * pow(pow(Orbit.SemiMajorAxis,3)/Orbit.MajorBody->getPlanetData().GravityStrength,0.5);
		}

		Ogre::Quaternion PeRot;
		Ogre::Radian PeRotAngle;

		if(RPos.dotProduct(RVel) <= 0) //if approaching periapsis
		{
			PeRotAngle = Ogre::Math::ACos(Orbit.SemiMajorAxis * (1 - pow(Orbit.Eccentricity,2))/(RPos.length() * Orbit.Eccentricity) - 1/Orbit.Eccentricity);
		}
		else //if moving away from periapsis
		{
			PeRotAngle = -Ogre::Math::ACos(Orbit.SemiMajorAxis * (1 - pow(Orbit.Eccentricity,2))/(RPos.length() * Orbit.Eccentricity) - 1/Orbit.Eccentricity);
		}

		//Create quaternion to represent rotation in plane of orbit
		PeRot.FromAngleAxis(PeRotAngle, Orbit.AngularMomentum.normalisedCopy());
		PeRot.normalise();

		Orbit.PeriapsisVector = PeRot * RPos.normalisedCopy(); //rotate unit vector to direction of periapsis
		if(Orbit.Eccentricity < 1) //Only an elliptical orbit has a defined apoapsis
		{
			Orbit.ApoapsisVector = -Orbit.PeriapsisVector; //apoapsis is in opposite direction to periapsis
			Orbit.ApoapsisVector *= Orbit.SemiMajorAxis * (1+Orbit.Eccentricity); //multiply unit vector by magnitude of apoapsis radius
		}
		else
		{
			Orbit.ApoapsisVector = 0;
		}

		Orbit.PeriapsisVector *= Orbit.SemiMajorAxis * (1-Orbit.Eccentricity); //multiply unit vector by magnitude of periapsis radius

		//NOTE: Sort out time step to use for position predicition
		double TimeStep;
		if(Orbit.Eccentricity < 1)
		{
			TimeStep = Orbit.Period / 20000;
		}
		else
		{
			TimeStep = 1;
		}

		for(int n=0; n<4001; n++)
		{
			Propagate(&RPos, &RVel, &Acc, false, false, TimeStep); 

			if(n == 2000) //initialise first predicted position
			{
				Pred1Pos = RPos + Orbit.MajorBody->getPosition();
				Pred1Vel = RVel + Orbit.MajorBody->getVelocity();
				Pred1Acc = Acc;
			}
			if(n == 4000) //initialise second predicted position
			{
				Pred2Pos = RPos + Orbit.MajorBody->getPosition();
				Pred2Vel = RVel + Orbit.MajorBody->getVelocity();
				Pred2Acc = Acc;
			}
		}
	}
}