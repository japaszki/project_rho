#include "FuelPanelManager.h"


FuelPanelManager::FuelPanelManager(UIManager* UIManagerHandle)
{
	Parent = UIManagerHandle;
	TargetUnit = NULL;

	//Define fuel panel
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *FuelPanel = wmgr.createWindow("RhoFuture/DataPanel", "MainGUI/FuelPanel");
	FuelPanel->setSize(CEGUI::UVector2(CEGUI::UDim(0.22, 0), CEGUI::UDim(0.28, 0)));
	FuelPanel->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.2, 0)));
	wmgr.getWindow("MainGUI/Sheet")->addChildWindow(FuelPanel);
	wmgr.getWindow("MainGUI/FuelPanel")->setVisible(false);

	CEGUI::ProgressBar *FuelBar1 = static_cast<CEGUI::ProgressBar*>(wmgr.createWindow("RhoFuture/ProgressBar_Yellow", "MainGUI/FuelPanel/FuelBar1"));
	FuelBar1->setSize(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0, 30)));
	FuelBar1->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.25, 0)));
	FuelBar1->setProgress(0.2);
	FuelPanel->addChildWindow(FuelBar1);

	CEGUI::ProgressBar *FuelBar2 = static_cast<CEGUI::ProgressBar*>(wmgr.createWindow("RhoFuture/ProgressBar_Green", "MainGUI/FuelPanel/FuelBar2"));
	FuelBar2->setSize(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0, 30)));
	FuelBar2->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.45, 0)));
	FuelBar2->setProgress(0.4);
	FuelPanel->addChildWindow(FuelBar2);

	CEGUI::ProgressBar *FuelBar3 = static_cast<CEGUI::ProgressBar*>(wmgr.createWindow("RhoFuture/ProgressBar_Red", "MainGUI/FuelPanel/FuelBar3"));
	FuelBar3->setSize(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0, 30)));
	FuelBar3->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.65, 0)));
	FuelBar3->setProgress(1.0);
	FuelPanel->addChildWindow(FuelBar3);

	CEGUI::Window *FuelPanelTitle = wmgr.createWindow("RhoFuture/StaticText", "MainGUI/FuelPanel/Title");
	FuelPanelTitle->setSize(CEGUI::UVector2(CEGUI::UDim(0.6, 0), CEGUI::UDim(0, 40)));
	FuelPanelTitle->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2, 0), CEGUI::UDim(0, 10)));
	FuelPanelTitle->setText("Fuel");
	FuelPanelTitle->setProperty("FrameEnabled", "False");
	FuelPanelTitle->setProperty("BackgroundEnabled", "False");
	FuelPanelTitle->setProperty("HorizontalAlignment", "Centre");
	FuelPanelTitle->setProperty("Font", "LCDMono2-18");
	FuelPanelTitle->setProperty("TextColours", "tl:ff37abc8 tr:ff37abc8 bl:ff37abc8 br:ff37abc8");
	FuelPanel->addChildWindow(FuelPanelTitle);
}


FuelPanelManager::~FuelPanelManager(void)
{
}

void FuelPanelManager::setTargetUnit(Unit* Target)
{
	TargetUnit = Target;
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.getWindow("MainGUI/FuelPanel")->setVisible(true);
}

void FuelPanelManager::RemoveTarget()
{
	TargetUnit = NULL;
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.getWindow("MainGUI/FuelPanel")->setVisible(false);
}

bool FuelPanelManager::IsHit(CEGUI::Vector2 Coords)
{
	if(TargetUnit == NULL)
	{
		return false;
	}
	else
	{
		CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
		return wmgr.getWindow("MainGUI/FuelPanel")->isHit(Coords, true);
	}
}