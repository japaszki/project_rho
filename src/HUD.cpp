#pragma once
#include "HUD.h"

//Forward declare any classes referenced
class Unit;

HUD::HUD(Unit * UnitReference)
{
	Parent = UnitReference;

	//PathObject = NULL;
	OrbitEntity = NULL;
	HUDSphere = Ogre::Sphere();
	
	HudNode = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode(); //Billboardset goes here
	PathNode = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode(); //Path object goes here

	//Initialise billboards
	Pred1BillboardSet = Parent->getAppHandle()->getSceneManager()->createBillboardSet(1);
	Pred1BillboardSet->setCullIndividually(true); //Cull billboards individually to avoid entire billboardset disappearing
	Pred1BillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 100); //Set infinite bounding box to disable culling entire billboardset
	Pred1BillboardSet->setMaterialName("WhiteDot");
	Pred1Billboard = Pred1BillboardSet->createBillboard(0,0,0);
	HudNode->attachObject(Pred1BillboardSet);
	
	Pred2BillboardSet = Parent->getAppHandle()->getSceneManager()->createBillboardSet(1);
	Pred2BillboardSet->setCullIndividually(true); //Cull billboards individually to avoid entire billboardset disappearing
	Pred2BillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 100); //Set infinite bounding box to disable culling entire billboardset
	Pred2BillboardSet->setMaterialName("WhiteDot");
	Pred2Billboard = Pred2BillboardSet->createBillboard(0,0,0);
	HudNode->attachObject(Pred2BillboardSet);

	//Initialise CEGUI overlays
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* Sheet = wmgr.getWindow("MainGUI/Sheet");
	UnitBox = wmgr.createWindow("RhoFuture/UnitBox");
	UnitBox->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.15, 0)));
	UnitBox->setVisible(false);
	Sheet->addChildWindow(UnitBox);

	UnitIcon = wmgr.createWindow("RhoFuture/SensorIcon");
	UnitIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0, 60), CEGUI::UDim(0, 60)));
	UnitIcon->setVisible(false);
	Sheet->addChildWindow(UnitIcon);

	ApoapsisIcon = wmgr.createWindow("RhoFuture/BlackDot");
	ApoapsisIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0,40), CEGUI::UDim(0,40)));
	ApoapsisIcon->setVisible(true);
	Sheet->addChildWindow(ApoapsisIcon);

	PeriapsisIcon = wmgr.createWindow("RhoFuture/BlackDot");
	PeriapsisIcon->setSize(CEGUI::UVector2(CEGUI::UDim(0,40), CEGUI::UDim(0,40)));
	PeriapsisIcon->setVisible(true);
	Sheet->addChildWindow(PeriapsisIcon);

	//UIManager-related functions
	Parent->getAppHandle()->mUIMgr->AddHUD(this);
	Highlighted = false;
	Selected = false;
}

HUD::~HUD(void)
{
	Parent->getAppHandle()->getSceneManager()->destroyBillboardSet(Pred1BillboardSet);
	Parent->getAppHandle()->getSceneManager()->destroyBillboardSet(Pred2BillboardSet);
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(HudNode);
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(PathNode);
	Parent->getAppHandle()->mUIMgr->RemoveHUD(this);

	//NOTE: Delete CEGUI overlay!
}

void HUD::UpdateHUD()
{
	Pred1Billboard->setPosition(Parent->getPhysics()->Pred1Pos);
	Pred2Billboard->setPosition(Parent->getPhysics()->Pred2Pos);

	//Set selection sphere radius and position
	const Ogre::Real HUDSphereRadius = 0.04; //Factor to be multiplied by distance to camera
	const Ogre::Real MinSphereRadius = 80; //Minimum radius
	const Ogre::Real MaxSphereRadius = 300; //Maximum radius
	double DistanceToCamera = (Parent->getAppHandle()->mUIMgr->getCamera()->getRealPosition() - Parent->getPhysics()->getPosition()).length();
	double CurrentSelectionRadius = HUDSphereRadius * DistanceToCamera;

	HUDSphere.setCenter(Parent->getPhysics()->getPosition());

	if(CurrentSelectionRadius > MaxSphereRadius)
	{
		CurrentSelectionRadius = MaxSphereRadius;
	}
	else if(CurrentSelectionRadius < MinSphereRadius)
	{
		CurrentSelectionRadius = MinSphereRadius;
	}
	HUDSphere.setRadius(CurrentSelectionRadius);

	//Determine screen coordinates to position unit box
	Ogre::Vector2 HUDCoords = ConvertWorldToScreenPosition(Parent->getPhysics()->getPosition(), Parent->getAppHandle()->mUIMgr->getCamera());
	const double UnitBoxSize = 100;
	const double MinUnitBoxSize = 0.02;
	const double UnitBoxTransitionSize = 0.065;
	const double MaxUnitBoxSize = 0.45;
	const Ogre::Real IconDisplayDistance = 4000; // distance to begin fading in
	const Ogre::Real IconHideDistance = 30000; // distance to begin fading out
	const Ogre::Real IconNearFadeDistance = 2000; //Distance over which icon fades in
	const Ogre::Real IconFarFadeDistance = 30000; //Distance over which icon fades out

	double CurrUnitBoxSize = UnitBoxSize / DistanceToCamera;

	if(CurrUnitBoxSize < MinUnitBoxSize)
	{
		CurrUnitBoxSize = MinUnitBoxSize;
		UnitBox->disable();
	}
	else if(CurrUnitBoxSize < UnitBoxTransitionSize)
	{
		UnitBox->disable();
	}
	else
	{
		UnitBox->enable();

		if(CurrUnitBoxSize > MaxUnitBoxSize)
		{
			CurrUnitBoxSize = MaxUnitBoxSize;
		}
	}

	//Show UnitIcon when far away
	if(DistanceToCamera >= IconDisplayDistance)
	{
		if(DistanceToCamera < (IconHideDistance + IconFarFadeDistance))
		{
			if((DistanceToCamera - IconDisplayDistance) >= IconNearFadeDistance)
			{
				if(DistanceToCamera >= IconHideDistance)
				{
					UnitIcon->setAlpha(1.0 - ((DistanceToCamera - IconHideDistance) / IconFarFadeDistance));
					UnitBox->setAlpha(1.0 - ((DistanceToCamera - IconHideDistance) / IconFarFadeDistance)); //UnitBox fades away together with icon
					ApoapsisIcon->setAlpha(1.0 - ((DistanceToCamera - IconHideDistance) / IconFarFadeDistance));
					PeriapsisIcon->setAlpha(1.0 - ((DistanceToCamera - IconHideDistance) / IconFarFadeDistance));
				}
				else
				{
					UnitIcon->setAlpha(1.0);
					UnitBox->setAlpha(1.0);
					ApoapsisIcon->setAlpha(1.0);
					PeriapsisIcon->setAlpha(1.0);
				}
			}
			else
			{
				UnitIcon->setAlpha((DistanceToCamera - IconDisplayDistance) / IconNearFadeDistance);
				UnitBox->setAlpha(1.0);
				ApoapsisIcon->setAlpha(1.0);
				PeriapsisIcon->setAlpha(1.0);
			}
			
			UnitIcon->setVisible(true);
			UnitIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(HUDCoords.x ,-30),CEGUI::UDim(HUDCoords.y ,-30)));
		}
		else
		{
			UnitIcon->setVisible(false);
			UnitBox->setAlpha(0.0);
			ApoapsisIcon->setAlpha(0.0);
			PeriapsisIcon->setAlpha(0.0);
		}
	}
	else
	{
		UnitIcon->setVisible(false);
		UnitBox->setAlpha(1.0);
		ApoapsisIcon->setAlpha(1.0);
		PeriapsisIcon->setAlpha(1.0);
	}

	if(Selected)
	{
		//Display CEGUI overlay
		UnitBox->setVisible(true);
		UnitBox->setPosition(CEGUI::UVector2(CEGUI::UDim(HUDCoords.x - CurrUnitBoxSize,0),CEGUI::UDim(HUDCoords.y - CurrUnitBoxSize,0)));
		UnitBox->setSize(CEGUI::UVector2(CEGUI::UDim(2 * CurrUnitBoxSize, 0), CEGUI::UDim(2 * CurrUnitBoxSize, 0)));
	}
	else if(Highlighted)
	{
		//Display CEGUI overlay
		UnitBox->setVisible(true);
		UnitBox->setPosition(CEGUI::UVector2(CEGUI::UDim(HUDCoords.x - CurrUnitBoxSize,0),CEGUI::UDim(HUDCoords.y - CurrUnitBoxSize,0)));
		UnitBox->setSize(CEGUI::UVector2(CEGUI::UDim(2 * CurrUnitBoxSize, 0), CEGUI::UDim(2 * CurrUnitBoxSize, 0)));
	}
	else
	{
		//Hide CEGUI overlay
		UnitBox->setVisible(false);
	}

	//Set positions of apsis points
	if(Parent->getPhysics()->Orbit.MajorBody != NULL)
	{
		Ogre::Vector2 ApCoords = ConvertWorldToScreenPosition(Parent->getPhysics()->Orbit.MajorBody->getPosition() 
																+ Parent->getPhysics()->Orbit.PeriapsisVector, 
																Parent->getAppHandle()->mUIMgr->getCamera());
		ApoapsisIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(ApCoords.x ,-20),CEGUI::UDim(ApCoords.y ,-20)));
		ApoapsisIcon->setVisible(true);

		if(Parent->getPhysics()->Orbit.Eccentricity < 1)
		{
			Ogre::Vector2 PeCoords = ConvertWorldToScreenPosition(Parent->getPhysics()->Orbit.MajorBody->getPosition() 
																	+ Parent->getPhysics()->Orbit.ApoapsisVector,
																	Parent->getAppHandle()->mUIMgr->getCamera());
			PeriapsisIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(PeCoords.x ,-20),CEGUI::UDim(PeCoords.y ,-20)));
			PeriapsisIcon->setVisible(true);
		}
		else
		{
			PeriapsisIcon->setPosition(CEGUI::UVector2(CEGUI::UDim(0 ,0),CEGUI::UDim(0 ,0)));
			PeriapsisIcon->setVisible(false);
		}

		//Check occlusion by planet
		if(Parent->getAppHandle()->mUIMgr->isOccludedByPlanet(Parent->getPhysics()->Orbit.MajorBody->getPosition() 
		+ Parent->getPhysics()->Orbit.PeriapsisVector))
			PeriapsisIcon->setVisible(false);

		if(Parent->getAppHandle()->mUIMgr->isOccludedByPlanet(Parent->getPhysics()->Orbit.MajorBody->getPosition() 
		+ Parent->getPhysics()->Orbit.ApoapsisVector))
			ApoapsisIcon->setVisible(false);
	}
	else
	{
		ApoapsisIcon->setVisible(false);
		PeriapsisIcon->setVisible(false);
	}
}

void HUD::RedrawOrbit()
{
	//Delete previous orbit entity
	if(OrbitEntity != NULL)
	{
		Parent->getAppHandle()->getSceneManager()->destroyEntity(OrbitEntity);
	}

	if(Parent->getPhysics()->Orbit.MajorBody == NULL)
	{
		OrbitEntity = NULL;
	}
	else
	{

		//Draw elliptical orbit
		if(Parent->getPhysics()->Orbit.Eccentricity < 1)
		{
			//Define orbital path coordinates, scale and facing ****************************************
			OrbitEntity = Parent->getAppHandle()->getSceneManager()->createEntity("Circle.mesh");
			OrbitEntity->setMaterialName("EmissiveWhite");
			PathNode->attachObject(OrbitEntity);
			PathNode->setScale(Parent->getPhysics()->Orbit.SemiMinorAxis, 1, Parent->getPhysics()->Orbit.SemiMajorAxis);
			PathNode->setPosition(Parent->getPhysics()->Orbit.MajorBody->getPosition() + Parent->getPhysics()->Orbit.PeriapsisVector.midPoint(Parent->getPhysics()->Orbit.ApoapsisVector));
			PathNode->setOrientation(Ogre::Vector3::UNIT_Z.getRotationTo(Parent->getPhysics()->Orbit.ApoapsisVector));
		}
		else //Draw hyperbolic orbit
		{
			//Define orbital path coordinates, scale and facing ****************************************
			OrbitEntity = Parent->getAppHandle()->getSceneManager()->createEntity("Hyperbola.mesh");
			OrbitEntity->setMaterialName("EmissiveWhite");
			PathNode->attachObject(OrbitEntity);
			PathNode->setScale(Parent->getPhysics()->Orbit.SemiMinorAxis, 1, -1*Parent->getPhysics()->Orbit.SemiMajorAxis); //SemiMajorAxis sign flipped as it is negative in a hyperbola
			PathNode->setPosition(Parent->getPhysics()->Orbit.MajorBody->getPosition() + Parent->getPhysics()->Orbit.PeriapsisVector);
			PathNode->setOrientation(-Ogre::Vector3::UNIT_Z.getRotationTo(Parent->getPhysics()->Orbit.PeriapsisVector));
		}

		//Rotate about line of apsides to ensure rendered path is in the correct plane **************
		Ogre::Vector3 LocalY;
		LocalY = PathNode->getOrientation() * Ogre::Vector3::UNIT_Y;

		Ogre::Quaternion Roll;
		Ogre::Vector3 Init, Mid, Final, Temp;

		Init = LocalY.normalisedCopy();
		Final = Parent->getPhysics()->Orbit.AngularMomentum.normalisedCopy();
		Mid = Init.midPoint(Final).normalisedCopy();

		//Vector representing axis of rotation (vector component of quaternion)
		Temp = Ogre::Math::Sin(Init.angleBetween(Mid)) * Init.crossProduct(Final).normalisedCopy();

		Roll.w = Ogre::Math::Cos(Init.angleBetween(Mid));
		Roll.x = Temp.x;
		Roll.y = Temp.y;
		Roll.z = Temp.z;

		PathNode->setOrientation(Roll * PathNode->getOrientation());
	}
}

Ogre::Sphere HUD::getHUDSphere()
{
	return HUDSphere;
}

Unit* HUD::getParent()
{
	return Parent;
}

void HUD::Select()
{
	Selected = true;
}

void HUD::Deselect()
{
	Selected = false;
}

void HUD::Highlight()
{
	Highlighted = true;
}

void HUD::Unhighlight()
{
	Highlighted = false;
}