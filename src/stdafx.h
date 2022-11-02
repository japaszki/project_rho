//Project Rho headers
#include "ProjectRho.h"
#include "Unit.h"
#include "Planet.h"
#include "Physics.h"
#include "PlanetHUD.h"
#include "HUD.h"
#include "UnitManager.h"
#include "PlanetManager.h"
#include "UIManager.h"
#include "CameraManager.h"
#include "Status.h"
#include "FuelPanelManager.h"
#include "ControlPanelManager.h"
#include "UIPanelBase.h"
#include "Visuals.h"
//#include "Utils.h"

//CEGUI headers
#include <CEGUI/include/CEGUI.h>
#include <CEGUI/include/RendererModules/Ogre/CEGUIOgreRenderer.h>

//Ogre headers
#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreBillboardSet.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>
 
// any other header can be included, as usual
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif