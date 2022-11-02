#ifndef __ProjectRho_h_
#define __ProjectRho_h_

#include "stdafx.h"

class ProjectRho : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener//, OgreBites::SdkTrayListener
{
public:
    ProjectRho(void);
    virtual ~ProjectRho(void);

	virtual void go(void);
	
	UnitManager* mUnitMgr;
	UIManager* mUIMgr;
	PlanetManager* mPlanetMgr;
	Ogre::SceneManager* getSceneManager();
	OIS::Mouse* getMouse();
	bool quit(const CEGUI::EventArgs &e);

protected:
	bool setup();
    bool configure(void);
    void chooseSceneManager(void);
    void createCamera(void);
    void destroyScene(void);
    void createViewports(void);
    void setupResources(void);
    void createResourceListener(void);
    void loadResources(void);

    void createScene(void);
	void createFrameListener(void);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Ogre::WindowEventListener
    //Adjust mouse clipping area
    void windowResized(Ogre::RenderWindow* rw);
    //Unattach OIS before window shutdown (very important under Linux)
    void windowClosed(Ogre::RenderWindow* rw);

    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
	Ogre::Viewport* mViewport;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    bool mCursorWasVisible;                    // was cursor visible before dialog appeared
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;
};

#endif // #ifndef __ProjectRho_h_
