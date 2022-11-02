#include "PlanetHUD.h"

PlanetHUD::PlanetHUD(Planet * PlanetReference)
{
	Parent = PlanetReference;

	HudNode = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode(); //Billboardset goes here
	PathNode = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode(); //Path object goes here

	//PathObject = NULL;
	OrbitEntity = NULL;

	OrbitEntity = Parent->getAppHandle()->getSceneManager()->createEntity("circle.mesh");
	PathNode->attachObject(OrbitEntity);

	//Initialise billboards
	/*HudBillboardSet = Parent->getAppHandle()->getSceneManager()->createBillboardSet(5);
	HudBillboardSet->setCullIndividually(true); //Cull billboards individually to avoid entire billboardset disappearing
	HudBillboardSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 100); //Set infinite bounding box to disable culling entire billboardset
	HudBillboardSet->setMaterialName("Green");

	Pred1Billboard = HudBillboardSet->createBillboard(0,0,0);
	Pred2Billboard = HudBillboardSet->createBillboard(0,0,0);
	ApoapsisBillboard = HudBillboardSet->createBillboard(0,0,0);
	PeriapsisBillboard = HudBillboardSet->createBillboard(0,0,0);
	HudNode->attachObject(HudBillboardSet);*/

	//Initialise CEGUI overlay
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* Sheet = wmgr.getWindow("MainGUI/Sheet");
	Overlay = wmgr.createWindow("TaharezLook/Button");
	Overlay->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
	Overlay->setVisible(false);
	Sheet->addChildWindow(Overlay);

	//UIManager-related functions
	double HUDSphereRadius = 500;

	HUDSphere.setRadius(HUDSphereRadius);
	Parent->getAppHandle()->mUIMgr->AddPlanetHUD(this);
	Highlighted = false;
	Selected = false;
}

PlanetHUD::~PlanetHUD(void)
{
	//Parent->getAppHandle()->getSceneManager()->destroyBillboardSet(HudBillboardSet);
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(HudNode);
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(PathNode);
	Parent->getAppHandle()->mUIMgr->RemovePlanetHUD(this);

	//NOTE: Delete CEGUI overlay!
}

void PlanetHUD::UpdateHUD()
{
	HUDSphere.setCenter(Parent->getPosition());
	OcclusionSphere.setCenter(Parent->getPosition());

	//Determine screen coordinates to position overlay
	Ogre::Vector2 HUDCoords = ConvertWorldToScreenPosition(Parent->getPosition(), Parent->getAppHandle()->mUIMgr->getCamera());

	if(Selected)
	{
		//Display CEGUI overlay
		Overlay->setVisible(true);
		Overlay->setPosition(CEGUI::UVector2(CEGUI::UDim(HUDCoords.x,0),CEGUI::UDim(HUDCoords.y,0)));

	}
	else if(Highlighted)
	{
		//Display CEGUI overlay
		Overlay->setVisible(true);
		Overlay->setPosition(CEGUI::UVector2(CEGUI::UDim(HUDCoords.x,0),CEGUI::UDim(HUDCoords.y,0)));
	}
	else
	{
		//Hide CEGUI overlay
		Overlay->setVisible(false);
	}
}

void PlanetHUD::RedrawOrbit()
{
	if(!Parent->getPlanetData().Sun) //Sun does not have major body
	{
		double SemiMinorAxis = Parent->getPlanetData().SemiMajorAxis * pow(1 - pow(Parent->getPlanetData().Eccentricity, 2), 0.5);
		PathNode->setScale(SemiMinorAxis, 1, Parent->getPlanetData().SemiMajorAxis);

		Ogre::Quaternion Lan, Inc, Arg;
		Lan.FromAngleAxis(Parent->getPlanetData().LongitudeOfAscendingNode, Ogre::Vector3::UNIT_Y);
		Inc.FromAngleAxis(Parent->getPlanetData().Inclination, Ogre::Vector3::NEGATIVE_UNIT_X);
		Arg.FromAngleAxis(Parent->getPlanetData().ArgumentOfPeriapsis - Ogre::Radian(Ogre::Math::HALF_PI), Ogre::Vector3::NEGATIVE_UNIT_Y);

		PathNode->setOrientation(Lan * Inc * Arg);
		PathNode->setPosition(Parent->getPlanetData().MajorBody->getPosition() - Lan * Inc * Arg * Ogre::Vector3(0, 0, Parent->getPlanetData().SemiMajorAxis * Parent->getPlanetData().Eccentricity));
	}

}

void PlanetHUD::setHUDSphereRadius(double Radius)
{
	HUDSphere.setRadius(Radius);
}

void PlanetHUD::setOcclusionSphereRadius(double Radius)
{
	OcclusionSphere.setRadius(Radius);
}

Ogre::Sphere PlanetHUD::getHUDSphere()
{
	return HUDSphere;
}

Ogre::Sphere PlanetHUD::getOcclusionSphere()
{
	return OcclusionSphere;
}

Planet* PlanetHUD::getParent()
{
	return Parent;
}

void PlanetHUD::Select()
{
	Selected = true;
}

void PlanetHUD::Deselect()
{
	Selected = false;
}

void PlanetHUD::Highlight()
{
	Highlighted = true;
}

void PlanetHUD::Unhighlight()
{
	Highlighted = false;
}