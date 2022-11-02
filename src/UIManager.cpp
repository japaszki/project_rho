#pragma once
#include "UIManager.h"

UIManager::UIManager(ProjectRho* App, Ogre::Camera* PlayerCamera)
{
	AppHandle = App;
	PlayerCameraHandle = PlayerCamera;
	mCameraManager = new CameraManager(this, PlayerCamera);
	HighlightedUnit = NULL;
	SelectedUnit = NULL;
	HighlightedPlanet = NULL;
	SelectedPlanet =  NULL;
	SelectionMode = false;
	LMB_Down = false;

	//Initialise CEGUI
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("RhoFuture.scheme");

	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	//Initialise CEGUI UI
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *Sheet = wmgr.createWindow("DefaultWindow", "MainGUI/Sheet");
	CEGUI::System::getSingleton().setGUISheet(Sheet);
	
	mFuelPanelManager = new FuelPanelManager(this);
	mControlPanelManager = new ControlPanelManager(this);

	//Initialise quit button
	CEGUI::Window *Quit = wmgr.createWindow("RhoFuture/Button", "MainGUI/QuitButton");
	Quit->setText("Quit");
	Quit->setProperty("Font", "LCDMono2-18");
	Quit->setProperty("NormalTextColour", "ff37abc8");
	Quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
	Sheet->addChildWindow(Quit);
	Quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ProjectRho::quit, AppHandle));

	//Initialise selection box
	SelectionBox = wmgr.createWindow("RhoFuture/SelectionBox");
	SelectionBox->setVisible(false);
	Sheet->addChildWindow(SelectionBox);
}

UIManager::~UIManager(void)
{
	delete mCameraManager;
	//NOTE: Delete list & CEGUI overlay!
}

ProjectRho* UIManager::getAppHandle()
{
	return AppHandle;
}

void UIManager::AddHUD(HUD* HUDObject)
{
	HUDList.push_back(HUDObject);
}

void UIManager::RemoveHUD(HUD* HUDObject)
{
	//Remove from HUDList
	for(int i = HUDList.size(); i > 0; i--)
	{
		if(HUDList[i-1] == HUDObject)
		{
			HUDList.erase(HUDList.begin()+i-1);
		}
	}
	//Remove from selections, if any
	if(HighlightedUnit == HUDObject->getParent())
	{
		HUDObject->Unhighlight();
		HighlightedUnit = NULL;
	}
	if(SelectedUnit == HUDObject->getParent())
	{
		HUDObject->Deselect();
		SelectedUnit = NULL;
	}
	for(int i = SelectedUnitList.size(); i > 0; i--)
	{
		if(SelectedUnitList[i-1] == HUDObject->getParent())
		{
			SelectedUnitList.erase(SelectedUnitList.begin()+i-1);
		}
	}
	CheckSelectedUnitList();
}

void UIManager::AddPlanetHUD(PlanetHUD* HUDObject)
{
	PlanetHUDList.push_back(HUDObject);
}

void UIManager::RemovePlanetHUD(PlanetHUD* HUDObject)
{
	for(int i = PlanetHUDList.size(); i > 0; i--)
	{
		if(PlanetHUDList[i-1] == HUDObject)
		{
			PlanetHUDList.erase(PlanetHUDList.begin()+i-1);
		}
	}
}

void UIManager::UpdateUI(double TimeStep)
{
	// get window height and width
	Ogre::Real ScreenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
	Ogre::Real ScreenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();
 
	// convert to 0-1 offset
	Ogre::Real OffsetX = CurrentMouseState.X.abs / ScreenWidth;
	Ogre::Real OffsetY = CurrentMouseState.Y.abs / ScreenHeight;
 
	// set up the ray
	Ogre::Ray MouseRay = PlayerCameraHandle->getCameraToViewportRay(OffsetX, OffsetY);

	//Check if any units are highlighted
	HighlightedUnit = NULL;
	HighlightedPlanet = NULL;
	for(int i = HUDList.size(); i > 0; i--)
	{
		HUDList[i-1]->Unhighlight(); //Unhighlight all units
		if(MouseRay.intersects(HUDList[i-1]->getHUDSphere()).first)
		{
			HighlightedUnit = HUDList[i-1]->getParent(); 
		}
	}
	//Check if any planets are highlighted
	for(int i = PlanetHUDList.size(); i > 0; i--)
	{
		PlanetHUDList[i-1]->Unhighlight(); //Unhighlight all planets
		if(MouseRay.intersects(PlanetHUDList[i-1]->getHUDSphere()).first)
		{
			HighlightedPlanet = PlanetHUDList[i-1]->getParent();
		}
	}
	//Unit takes precedence over planet; thus set HighlightedPlanet to NULL if there is a highlighted unit
	if(HighlightedUnit != NULL)
	{
		HighlightedUnit->getHUD()->Highlight();
		HighlightedPlanet = NULL;
	}
	else if(HighlightedPlanet != NULL)
	{
		HighlightedPlanet->getHUD()->Highlight();
	}

	//Set target of camera manager to highlighted unit
	const float NearFollowDistance = 750; //Distance to start following
	const float FarFollowDistance = 2000; //Distance to stop following
	const float TargetPersistenceTime = 0.15;
	if(HighlightedUnit != NULL)
	{
		if(CurrentMouseState.Z.rel > 0)//Set target only if zooming towards highlighted unit
		{
			TargetTime = 0;

			if(MouseRay.intersects(HighlightedUnit->getHUD()->getHUDSphere()).second < NearFollowDistance) //follow unit if close enough
			{
				mCameraManager->setTargetUnit(HighlightedUnit);
			}
			else //target unit's position if too far
			{
				mCameraManager->setTargetPosition(HighlightedUnit->getPhysics()->getPosition());
			}
		}
	}
	else if(HighlightedPlanet != NULL) //Unit takes precedence over planet
	{
		if(CurrentMouseState.Z.rel > 0)//Set target only if zooming towards highlighted unit
		{
			TargetTime = 0;

			if(MouseRay.intersects(HighlightedPlanet->getHUD()->getHUDSphere()).second < NearFollowDistance) //follow unit if close enough
			{
				mCameraManager->setTargetPlanet(HighlightedPlanet);
			}
			else //target unit's position if too far
			{
				mCameraManager->setTargetPosition(HighlightedPlanet->getPosition());
			}
		}
	}
	else //If no units or planets highlighted and not following any unit or planet
	{
		if(mCameraManager->getTargetUnitDistance() > FarFollowDistance)// if too far to keep following current target
		{
			TargetTime += TimeStep;
			if(TargetTime > TargetPersistenceTime) //allow target to persist for some time before removing
			{
				mCameraManager->RemoveTarget();
			}
		}
	}
	mCameraManager->Update(TimeStep);
}

void UIManager::MouseMoved(OIS::MouseState State)
{
	CurrentMouseState = State;
	mCameraManager->MouseMoved(State);

	if(LMB_Down)
	{
		SelectionMode = true;
	}

	const int SelectionBoxBorder = 8; //size of glow border around selection box

	//Draw selection box
	if(SelectionMode)
	{
		if(!((InitialSelectionPoint.x == CurrentMouseState.X.abs)&&(InitialSelectionPoint.y == CurrentMouseState.Y.abs)))
		{
			// get window height and width
			Ogre::Real ScreenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
			Ogre::Real ScreenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();

			// convert to 0-1 offset
			Ogre::Real InitialX = InitialSelectionPoint.x / ScreenWidth;
			Ogre::Real InitialY = InitialSelectionPoint.y / ScreenHeight;
			Ogre::Real CurrentX = CurrentMouseState.X.abs / ScreenWidth;
			Ogre::Real CurrentY = CurrentMouseState.Y.abs / ScreenHeight;

			SelectionBox->setVisible(true);
			SelectionBox->setPosition(CEGUI::UVector2(CEGUI::UDim(std::min(InitialX, CurrentX), -SelectionBoxBorder), CEGUI::UDim(std::min(InitialY, CurrentY), -SelectionBoxBorder)));
			SelectionBox->setSize(CEGUI::UVector2(CEGUI::UDim(std::abs(InitialX - CurrentX), 2*SelectionBoxBorder), CEGUI::UDim(std::abs(InitialY - CurrentY), 2*SelectionBoxBorder)));
		}
		else
		{
			SelectionBox->setVisible(false);
		}
	}
}

void UIManager::MousePressed(OIS::MouseState State, OIS::MouseButtonID id)
{
	CurrentMouseState = State;
	mCameraManager->MousePressed(State, id);
	switch(id)
	{
	case OIS::MB_Left:
		if(!IsMouseOverPanel())
		{
			LMB_Down = true;
		}
		InitialSelectionPoint = Ogre::Vector2(State.X.abs, State.Y.abs);
		break;
	default:
		break;
	}
}

void UIManager::MouseReleased(OIS::MouseState State, OIS::MouseButtonID id)
{
	CurrentMouseState = State;
	mCameraManager->MouseReleased(State, id);

	switch(id)
	{
	case OIS::MB_Left: 

		LMB_Down = false;

		//Only select/deselect if mouse not over a GUI window
		if(!IsMouseOverPanel())
		{
			//Deselect previously selected units and planets
			//Prevent stray pointer if nothing selected
			if(SelectedUnit != NULL)
			{
				SelectedUnit->getHUD()->Deselect();
				RemovePanelTarget();
				SelectedUnit = NULL;
			}
			//Deselect previous selection list
			if(SelectedUnitList.size() != 0)
			{
				DeselectList();
			}
			if(SelectedPlanet != NULL)
			{
				SelectedPlanet->getHUD()->Deselect();
				SelectedPlanet = NULL;
			}

			//Stop drawing selection box and check for any units selected
			if(SelectionMode)
			{
				SelectionMode = false;
				SelectionBox->setVisible(false);
				ProcessSelectionBox();
			}
			else //Select single planet or unit if not in selection mode
			{
				if(HighlightedUnit != NULL) //Select highlighted unit
				{
					HighlightedUnit->getHUD()->Select();
					SelectedUnit = HighlightedUnit;
					setPanelTarget(SelectedUnit);
				}
				else if(HighlightedPlanet != NULL)//Select planet if no unit highlighted
				{
					HighlightedPlanet->getHUD()->Select();
					SelectedPlanet = HighlightedPlanet;
				}
			}
		}
		else //Exit selection mode without selecting units if selectionbox dragged onto GUI window
		{
			SelectionMode = false;
			SelectionBox->setVisible(false);
		}
		break;
	default:
		break;
	}
}

void UIManager::ProcessSelectionBox()
{
	// get window height and width
	Ogre::Real ScreenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
	Ogre::Real ScreenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();

	// convert to 0-1 offset
	Ogre::Real InitialX = InitialSelectionPoint.x / ScreenWidth;
	Ogre::Real InitialY = InitialSelectionPoint.y / ScreenHeight;
	Ogre::Real CurrentX = CurrentMouseState.X.abs / ScreenWidth;
	Ogre::Real CurrentY = CurrentMouseState.Y.abs / ScreenHeight;

	//sort upper and lower bounds of selection box
	Ogre::Real UpperX = std::max(InitialX, CurrentX);
	Ogre::Real UpperY = std::max(InitialY, CurrentY);
	Ogre::Real LowerX = std::min(InitialX, CurrentX);
	Ogre::Real LowerY = std::min(InitialY, CurrentY);

	//Clear selection list
	SelectedUnitList.clear();

	//Search HUDList for units which fit inside the box
	for(int i = HUDList.size(); i > 0; i--)
	{
		Ogre::Vector2 ScreenPosition = ConvertWorldToScreenPosition(HUDList[i-1]->getParent()->getPhysics()->getPosition(), PlayerCameraHandle);
		if(ScreenPosition.x >= LowerX)
		{
			if(ScreenPosition.x <= UpperX)
			{
				if(ScreenPosition.y >= LowerY)
				{
					if(ScreenPosition.y <= UpperY)
					{
						HUDList[i-1]->Select();
						SelectedUnitList.push_back(HUDList[i-1]->getParent());
					}
				}
			}
		}
		else
		{
			HUDList[i-1]->Deselect();
		}
	}

	//Check if only one unit has been selected. If so, replace lone selectedunitlist entry with selectedunit
	CheckSelectedUnitList();

}

void UIManager::DeselectList()
{
	for(int i = SelectedUnitList.size(); i > 0; i--)
	{
		SelectedUnitList[i-1]->getHUD()->Deselect();
	}
	SelectedUnitList.clear();
}

void UIManager::CheckSelectedUnitList()
{
	if(SelectedUnitList.size() == 1)
	{
		SelectedUnit = SelectedUnitList[0];
		SelectedUnitList.clear();
		setPanelTarget(SelectedUnit);
	}
}

void UIManager::setPanelTarget(Unit* Target)
{
	mFuelPanelManager->setTargetUnit(Target);
	mControlPanelManager->setTargetUnit(Target);
}

void UIManager::RemovePanelTarget()
{
	mFuelPanelManager->RemoveTarget();
	mControlPanelManager->RemoveTarget();
}

bool UIManager::IsMouseOverPanel()
{
	if(mControlPanelManager->IsHit(CEGUI::Vector2(CurrentMouseState.X.abs, CurrentMouseState.Y.abs)))
	{
		return true;
	}
	else if(mFuelPanelManager->IsHit(CEGUI::Vector2(CurrentMouseState.X.abs, CurrentMouseState.Y.abs)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

Ogre::Camera* UIManager::getCamera()
{
	return PlayerCameraHandle;
}


//CHECK IMPLEMENTATION!!!
bool UIManager::isOccludedByPlanet(Ogre::Vector3 Position)
{
	//Check if any planets are intersected
	Ogre::Vector3 CameraPosition = getCamera()->getDerivedPosition();
	double DistanceToCamera = (Position - CameraPosition).length();
	Ogre::Ray TestRay;
	TestRay = Ogre::Ray(CameraPosition, (Position - CameraPosition).normalisedCopy());

	//debug:
	Ogre::ManualObject* RayObject = AppHandle->getSceneManager()->getManualObject("RayObject");
	RayObject->clear();
	RayObject->begin("EmissiveWhite", Ogre::RenderOperation::OT_LINE_LIST);
	RayObject->position(CameraPosition);
	RayObject->position(Position);
	RayObject->end();


	for(int i = PlanetHUDList.size(); i > 0; i--)
	{
		//Occluded if intersects and point of intersection is closer than Position
		if((TestRay.intersects(PlanetHUDList[i-1]->getOcclusionSphere()).first)&&
		((TestRay.intersects(PlanetHUDList[i-1]->getOcclusionSphere()).second) >= DistanceToCamera))
			return true;
	}
	return false; //not occluded
}