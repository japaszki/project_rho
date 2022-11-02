#include "Visuals.h"


Visuals::Visuals(Unit* UnitReference)
{
	Parent = UnitReference;
	mNodeTree.AxisNodeHandle = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
}


Visuals::~Visuals(void)
{
}

void Visuals::setNodeTree(NodeElement NodeTree)
{
	//clear current nodetree (MUST be done before new nodetree is copied)
	Parent->getAppHandle()->getSceneManager()->destroySceneNode(mNodeTree.AxisNodeHandle); 
	//copy new nodetree
	mNodeTree = NodeTree;

	//Create new SceneNode hierarchy
	mNodeTree.AxisNodeHandle = Parent->getAppHandle()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mNodeTree.ElementNodeHandle = mNodeTree.AxisNodeHandle->createChildSceneNode();
	Ogre::Entity* TempEntity = Parent->getAppHandle()->getSceneManager()->createEntity(mNodeTree.MeshName);
	TempEntity->setMaterialName("PPL_diffuse_and_specular");
	mNodeTree.ElementNodeHandle->attachObject(TempEntity);
	mNodeTree.ElementNodeHandle->setPosition(-mNodeTree.AxisOffset);
	mNodeTree.AxisNodeHandle->setScale(20,20,20); //increase the size of the ship for testing purposes

	for(int x = mNodeTree.SubNodes.size() - 1; x >= 0; x--)
	{
		SetupSubNodes(&mNodeTree.SubNodes[x], mNodeTree.AxisNodeHandle);
	}
}

void Visuals::SetupSubNodes(NodeElement* Node, Ogre::SceneNode* ParentNode)
{
	Node->AxisNodeHandle = ParentNode->createChildSceneNode();
	Node->AxisNodeHandle->setPosition(Node->BasePosition);
	Node->AxisNodeHandle->setOrientation(Node->BaseOrientation);
	Node->ElementNodeHandle = Node->AxisNodeHandle->createChildSceneNode();
	Ogre::Entity* TempEntity = Parent->getAppHandle()->getSceneManager()->createEntity(Node->MeshName);
	TempEntity->setMaterialName("PPL_diffuse_and_specular");
	Node->ElementNodeHandle->attachObject(TempEntity);
	Node->ElementNodeHandle->setPosition(-Node->AxisOffset);

	for(int x = Node->SubNodes.size() - 1; x >= 0; x--)
	{
		SetupSubNodes(&Node->SubNodes[x], Node->AxisNodeHandle);
	}
}

void Visuals::Update(Ogre::Real TimeStep)
{
	mNodeTree.AxisNodeHandle->setPosition(Parent->getPhysics()->getPosition());
	mNodeTree.AxisNodeHandle->setOrientation(Parent->getPhysics()->getFacing());

	//update each running animation
	for(std::list<ActiveTrack>::iterator mIterator = mActiveTrackList.begin(); mIterator != mActiveTrackList.end(); mIterator++)
	{
		//cull if animation complete
		if(mIterator->CurrentTime >= mIterator->Duration)
		{
			mIterator = mActiveTrackList.erase(mIterator);
		}

		//apply animation track and increment time
		if(mIterator->CurrentTime < mIterator->Duration)
		{
			mIterator->AnimationTrack->applyToNode(mIterator->TargetNode, mIterator->CurrentTime);
			mIterator->CurrentTime += TimeStep;

			if(mIterator->CurrentTime > mIterator->Duration)
				mIterator->CurrentTime = mIterator->Duration;
			else if(mIterator->CurrentTime < 0.0f)
				mIterator->CurrentTime = 0.0f;
		}
	}
}

void Visuals::AddAnimation(Animation NewAnimation)
{
	mAnimationList.push_back(NewAnimation);
}

void Visuals::AddTurret(TurretVisuals NewTurret)
{
	NodeElement* Turret;
	NodeElement* Barrel;
	Turret = getTreeElement(NewTurret.TurretName, &mNodeTree);
	Barrel = getTreeElement(NewTurret.BarrelName, &mNodeTree);
	NewTurret.TurretNode = Turret->AxisNodeHandle;
	NewTurret.BarrelNode = Barrel->AxisNodeHandle;
	NewTurret.TurretBaseOrientation = Turret->BaseOrientation;
	NewTurret.BarrelBaseOrientation = Barrel->BaseOrientation;
	mTurretList.push_back(NewTurret);
}

void Visuals::setTurretOrientation(std::string Turret, Ogre::Radian Azimuth, Ogre::Radian Elevation)
{
	for(int x=mTurretList.size() - 1; x >= 0; x--)
	{
		if(mTurretList[x].AssemblyName == Turret)
		{
			Ogre::Quaternion TurretRotation, BarrelRotation;
			TurretRotation.FromAngleAxis(Azimuth, mTurretList[x].TurretAxis);
			BarrelRotation.FromAngleAxis(Elevation, mTurretList[x].BarrelAxis);
			mTurretList[x].TurretNode->setOrientation(TurretRotation * mTurretList[x].TurretBaseOrientation);
			mTurretList[x].BarrelNode->setOrientation(BarrelRotation * mTurretList[x].BarrelBaseOrientation);
		}
	}
}

/*void Visuals::setState(std::string NewState)
{
	for(int i=0; i<mVisualStateList.size(); i++)
	{
		if(mVisualStateList[i].StateName == NewState)
		{
			for(int j=0; j<mVisualStateList[i].NodePositionList.size(); j++)
			{
				Ogre::SceneNode* CurrentElement = getTreeNode(mVisualStateList[i].NodePositionList[j].ElementName, mNodeTree);
				if(CurrentElement != NULL)
				{
					CurrentElement->setPosition(mVisualStateList[i].NodePositionList[j].ElementPosition);
					CurrentElement->setOrientation(mVisualStateList[i].NodePositionList[j].ElementOrientation);
					CurrentElement->setVisible(mVisualStateList[i].NodePositionList[j].ElementVisibility);
				}
			}
			break; //stop searching for other states
		}
	}
}*/

void Visuals::StartAnim(std::string Animation)
{
	//Start animation
	for(int x=mAnimationList.size() - 1; x >= 0; x--)
	{
		if(mAnimationList[x].AnimName == Animation)
		{
			//set up each node track
			for(int y=mAnimationList[x].TrackList.size() - 1; y >= 0; y--)
			{
				Ogre::SceneNode* CurrentElement = getTreeNode(mAnimationList[x].TrackList[y].ElementName, mNodeTree);
				if(CurrentElement != NULL)
				{
					//Add to list of running animations
					ActiveTrack CurrentTrack;
					CurrentTrack.AnimationTrack = mAnimationList[x].TrackList[y].AnimationTrack;
					CurrentTrack.TargetNode = CurrentElement;
					CurrentTrack.Duration = mAnimationList[x].Duration;
					CurrentTrack.CurrentTime = 0.0f;
					mActiveTrackList.push_front(CurrentTrack);
				}
			}
		}
	}
}

Ogre::SceneNode* Visuals::getTreeNode(std::string Name, NodeElement NodeTree)
{
	return getTreeElement(Name, &NodeTree)->AxisNodeHandle;
}

NodeElement* Visuals::getTreeElement(std::string Name, NodeElement* NodeTree)
{
	if(!NodeTree->ElementName.compare(Name))
	{
		return NodeTree;
	}
	else
	{
		for(int x = NodeTree->SubNodes.size() - 1; x >= 0; x--)
		{		
			NodeElement* Element = getTreeElement(Name, &NodeTree->SubNodes[x]);
			if(Element != NULL)
			{
				return Element;
			}
		}
	}
	return NULL;
}