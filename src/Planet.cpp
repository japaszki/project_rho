#include "Planet.h"

Planet::Planet(ProjectRho* App, std::string PlanetName)
{
	AppHandle = App;
	mPlanetData.MajorBody = NULL;
	mPlanetData.Sun = false;
	Name = PlanetName;
	Time = 0;
	pHUD = new PlanetHUD(this);
}

Planet::~Planet(void)
{
}

ProjectRho* Planet::getAppHandle()
{
	return AppHandle;
}

std::string Planet::getName()
{
	return Name;
}

Ogre::SceneNode * Planet::getNode()
{
	return PlanetNode;
}

PlanetData Planet::getPlanetData()
{
	return mPlanetData;
}

void Planet::UpdateNode()
{
	PlanetNode->setPosition(Position);
}

void Planet::UpdatePlanet(double TimeStep)
{
	if(!mPlanetData.Sun)
	{
		Time += TimeStep;
		if(Time >= mPlanetData.TimeOfPeriapsisPassage + 0.5 * mPlanetData.OrbitalPeriod)
		{
			Time -= mPlanetData.OrbitalPeriod;
		}

		Ogre::Radian TrueAnomaly = getTrueAnomaly();

		double Radius = mPlanetData.SemiMajorAxis * (1 - pow(mPlanetData.Eccentricity, 2)) / (1 - mPlanetData.Eccentricity * Ogre::Math::Cos(TrueAnomaly + Ogre::Radian(Ogre::Math::PI)));
		Ogre::Quaternion Lan, Inc, Arg;

		Lan.FromAngleAxis(mPlanetData.LongitudeOfAscendingNode, Ogre::Vector3::UNIT_Y);
		Inc.FromAngleAxis(mPlanetData.Inclination, Ogre::Vector3::NEGATIVE_UNIT_X);
		Arg.FromAngleAxis(TrueAnomaly - mPlanetData.ArgumentOfPeriapsis + Ogre::Radian(Ogre::Math::HALF_PI), Ogre::Vector3::UNIT_Y);

		Ogre::Vector3 RPos = Lan * Inc * Arg * Ogre::Vector3(0, 0, Radius);

		//Calculate velocty; change to relfect major body velocity
		
		if(mPlanetData.MajorBody == NULL)
		{
			Velocity = (RPos - Position)/TimeStep;
			Position = RPos;
		}
		else
		{
			Position = RPos + mPlanetData.MajorBody->getPosition();
			Velocity = (RPos - Position)/TimeStep - mPlanetData.MajorBody->getVelocity();
		}

		UpdateNode();
	}

	pHUD->UpdateHUD();
	pHUD->RedrawOrbit();
}

Ogre::Vector3 Planet::getPosition()
{
	return Position;
}

Ogre::Vector3 Planet::getVelocity()
{
	return Velocity;
}

PlanetHUD* Planet::getHUD()
{
	return pHUD;
}

void Planet::setPlanetData(PlanetData NewPlanetData, std::string Mesh)
{
	mPlanetData = NewPlanetData;
	mPlanetData.Sun = false;

	pHUD->setHUDSphereRadius(mPlanetData.HUDRadius);
	pHUD->setOcclusionSphereRadius(mPlanetData.OcclusionRadius);

	double EccentricityFactor = sqrt((1-mPlanetData.Eccentricity)/(1+mPlanetData.Eccentricity));

	//generate position lookup table

	for(int i = 0; i < 1001; i++)
	{
		TrueAnomalyList[i] = Ogre::Radian(-Ogre::Math::PI + 0.001 * i * Ogre::Math::TWO_PI);

		double EccentricAnomaly;
		if(Ogre::Math::Abs(TrueAnomalyList[i]) != Ogre::Radian(Ogre::Math::PI)) //check for special case where formula is undefined due to tan(pi/2)
		{
			EccentricAnomaly = 2 * atan(EccentricityFactor * tan(TrueAnomalyList[i].valueRadians() * 0.5));
		}
		else
		{
			EccentricAnomaly = TrueAnomalyList[i].valueRadians();
		}

		TimeList[i] = mPlanetData.TimeOfPeriapsisPassage + mPlanetData.OrbitalPeriod / (Ogre::Math::TWO_PI) * (EccentricAnomaly - mPlanetData.Eccentricity * sin(EccentricAnomaly));
	}

	//Create planet node and entity
	//Note that the old entity is not destroyed if this function is called again.
	PlanetNode = AppHandle->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* PlanetEntity = AppHandle->getSceneManager()->createEntity(Mesh);
	PlanetNode->attachObject(PlanetEntity);
	PlanetNode->setPosition(0, 0, 0);
}

void Planet::IsSun(PlanetData SunData, std::string Mesh)
{
	mPlanetData.GravityStrength = SunData.GravityStrength;
	mPlanetData.Inclination = 0;
	mPlanetData.ArgumentOfPeriapsis = 0;
	mPlanetData.LongitudeOfAscendingNode = 0;
	mPlanetData.SemiMajorAxis = 0;
	mPlanetData.Eccentricity = 0;
	mPlanetData.TimeOfPeriapsisPassage = 0;
	mPlanetData.OrbitalPeriod = 0;
	mPlanetData.MajorBody = NULL;
	mPlanetData.HUDRadius = SunData.HUDRadius;
	mPlanetData.OcclusionRadius = SunData.OcclusionRadius;
	Position = Ogre::Vector3::ZERO;
	Velocity = Ogre::Vector3::ZERO;

	//double EccentricityFactor = sqrt((1-PlanetData.Eccentricity)/(1+PlanetData.Eccentricity));
	mPlanetData.Sun = true;

	pHUD->setHUDSphereRadius(mPlanetData.HUDRadius);
	pHUD->setOcclusionSphereRadius(mPlanetData.OcclusionRadius);

	//Create planet node and entity
	//Note that the old entity is not destroyed if this function is called again.
	PlanetNode = AppHandle->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* PlanetEntity = AppHandle->getSceneManager()->createEntity(Mesh);
	PlanetEntity->setCastShadows(false);
	PlanetNode->attachObject(PlanetEntity);
	PlanetNode->setPosition(0, 0, 0);

	Ogre::BillboardSet* SunBillboardSet = AppHandle->getSceneManager()->createBillboardSet(1);
	SunBillboardSet->setCullIndividually(true); //Cull billboards individually to avoid entire billboardset disappearing
	SunBillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 100); //Set infinite bounding box to disable culling entire billboardset
	SunBillboardSet->setMaterialName("SunHalo");
	SunBillboardSet->setDefaultDimensions(4800, 4800);
	Ogre::Billboard* SunBillboard = SunBillboardSet->createBillboard(0,0,0);
	PlanetNode->attachObject(SunBillboardSet);
}

Ogre::Radian Planet::getTrueAnomaly()
{
	//weird glitch causing these values to be changed
	TrueAnomalyList[0] = Ogre::Radian(-Ogre::Math::PI);
	TrueAnomalyList[1] = Ogre::Radian(-Ogre::Math::PI + 0.001 * Ogre::Math::TWO_PI);
	//replace with cubic interpolation
	for(int i = 0; i < 1000; i++)
	{
		if((Time >= TimeList[i]) && (Time < TimeList[i+1]))
		{
			return TrueAnomalyList[i] + (Time - TimeList[i])/(TimeList[i+1] - TimeList[i])*(TrueAnomalyList[i+1] - TrueAnomalyList[i]);
		}
	}

	return Ogre::Radian(Ogre::Math::PI);
}