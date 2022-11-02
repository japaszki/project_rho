#include "ControlPanelManager.h"


ControlPanelManager::ControlPanelManager(UIManager* UIManagerHandle)
{
	Parent = UIManagerHandle;
	TargetUnit = NULL;

	//Define control panel
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *ControlPanel = wmgr.createWindow("RhoFuture/DataPanel", "MainGUI/ControlPanel");
	ControlPanel->setSize(CEGUI::UVector2(CEGUI::UDim(0.22, 0), CEGUI::UDim(0.5, 0)));
	ControlPanel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.78, 0), CEGUI::UDim(0.2, 0)));
	wmgr.getWindow("MainGUI/Sheet")->addChildWindow(ControlPanel);
	wmgr.getWindow("MainGUI/ControlPanel")->setVisible(false);

	CEGUI::Window *ControlPanelTitle = wmgr.createWindow("RhoFuture/StaticText", "MainGUI/ControlPanel/Title");
	ControlPanelTitle->setSize(CEGUI::UVector2(CEGUI::UDim(0.7, 0), CEGUI::UDim(0, 40)));
	ControlPanelTitle->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0, 10)));
	ControlPanelTitle->setText("Commands");
	ControlPanelTitle->setProperty("FrameEnabled", "False");
	ControlPanelTitle->setProperty("BackgroundEnabled", "False");
	ControlPanelTitle->setProperty("HorizontalAlignment", "Centre");
	ControlPanelTitle->setProperty("Font", "LCDMono2-18");
	ControlPanelTitle->setProperty("TextColours", "tl:ff37abc8 tr:ff37abc8 bl:ff37abc8 br:ff37abc8");
	ControlPanel->addChildWindow(ControlPanelTitle);

	CEGUI::Window *ChangeOrbitButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/ChangeOrbitButton");
	ChangeOrbitButton->setText("Change Orbit");
	ChangeOrbitButton->setProperty("Font", "LCDMono2-16");
	ChangeOrbitButton->setProperty("NormalTextColour", "ff37abc8");
	ChangeOrbitButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	ChangeOrbitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 48)));
	ControlPanel->addChildWindow(ChangeOrbitButton);

	CEGUI::Window *EscapeTrajectoryButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/EscapeTrajectoryButton");
	EscapeTrajectoryButton->setText("Escape Trajectory");
	EscapeTrajectoryButton->setProperty("Font", "LCDMono2-16");
	EscapeTrajectoryButton->setProperty("NormalTextColour", "ff37abc8");
	EscapeTrajectoryButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	EscapeTrajectoryButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 77)));
	ControlPanel->addChildWindow(EscapeTrajectoryButton);

	CEGUI::Window *RendezvousButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/RendezvousButton");
	RendezvousButton->setText("Rendezvous");
	RendezvousButton->setProperty("Font", "LCDMono2-16");
	RendezvousButton->setProperty("NormalTextColour", "ff37abc8");
	RendezvousButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	RendezvousButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 106)));
	ControlPanel->addChildWindow(RendezvousButton);

	CEGUI::Window *InterceptButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/InterceptButton");
	InterceptButton->setText("Intercept");
	InterceptButton->setProperty("Font", "LCDMono2-16");
	InterceptButton->setProperty("NormalTextColour", "ff37abc8");
	InterceptButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	InterceptButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 135)));
	ControlPanel->addChildWindow(InterceptButton);

	CEGUI::Window *ManualManeouvreButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/ManualManeouvreButton");
	ManualManeouvreButton->setText("Manual Maneouvre");
	ManualManeouvreButton->setProperty("Font", "LCDMono2-16");
	ManualManeouvreButton->setProperty("NormalTextColour", "ff37abc8");
	ManualManeouvreButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	ManualManeouvreButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 164)));
	ControlPanel->addChildWindow(ManualManeouvreButton);

	CEGUI::Window *EvasionButton = wmgr.createWindow("RhoFuture/Button", "MainGUI/EvasionButton");
	EvasionButton->setText("Evasion");
	EvasionButton->setProperty("Font", "LCDMono2-16");
	EvasionButton->setProperty("NormalTextColour", "ff37abc8");
	EvasionButton->setSize(CEGUI::UVector2(CEGUI::UDim(1, -50), CEGUI::UDim(0, 30)));
	EvasionButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 25), CEGUI::UDim(0, 193)));
	//testing visuals class state
	EvasionButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ControlPanelManager::ChangeVisualStatus, this));
	ControlPanel->addChildWindow(EvasionButton);
}


ControlPanelManager::~ControlPanelManager(void)
{
}

bool ControlPanelManager::ChangeVisualStatus(const CEGUI::EventArgs &e)
{
	//TargetUnit->getVisuals()->setState("Open");
	TargetUnit->getVisuals()->StartAnim("Open");
	return true;
}

void ControlPanelManager::setTargetUnit(Unit* Target)
{
	TargetUnit = Target;
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.getWindow("MainGUI/ControlPanel")->setVisible(true);
}

void ControlPanelManager::RemoveTarget()
{
	TargetUnit = NULL;
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.getWindow("MainGUI/ControlPanel")->setVisible(false);
}

bool ControlPanelManager::IsHit(CEGUI::Vector2 Coords)
{
	if(TargetUnit == NULL)
	{
		return false;
	}
	else
	{
		CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
		return wmgr.getWindow("MainGUI/ControlPanel")->isHit(Coords, true);
	}
}