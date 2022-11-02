#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class Unit;

//Define structures used by this class
struct NodeElement 
{
	std::string ElementName; //Each element must have unique name
	std::string MeshName; //Filename of mesh
	Ogre::Vector3 AxisOffset; //Offset of element axis from mesh axis
	Ogre::SceneNode* ElementNodeHandle; //Visuals class overwrites this: leave empty; "Inner" scenenode
	Ogre::SceneNode* AxisNodeHandle; //"Outer" scenenode, use this node to move and rotate
	Ogre::Vector3 BasePosition; //Need to be zero for root node
	Ogre::Quaternion BaseOrientation; //Need to be zero for root node
	std::vector<NodeElement> SubNodes;
};

struct Track
{
	std::string ElementName; //name of node to apply track to
	Ogre::NodeAnimationTrack* AnimationTrack;
};

struct ActiveTrack
{
	Ogre::NodeAnimationTrack* AnimationTrack;
	Ogre::SceneNode* TargetNode;
	Ogre::Real Duration;
	Ogre::Real CurrentTime;
};

class Animation
{
public:
	std::string AnimName;
	Ogre::Real Duration;
	std::vector<Track> TrackList;

	void AddTrack(Track NewTrack)
	{
		TrackList.push_back(NewTrack);
	}
};

struct TurretVisuals
{
	std::string AssemblyName; //name of turret assembly for commands
	std::string TurretName; //name of turret element in nodetree
	std::string BarrelName; //name of barrel element
	Ogre::Vector3 TurretAxis; //Axis of rotation of turret (azimuth)
	Ogre::Vector3 BarrelAxis; //Axis of rotation of barrel (elevation)
	//leave empty:
	Ogre::SceneNode* TurretNode;
	Ogre::SceneNode* BarrelNode;
	Ogre::Quaternion TurretBaseOrientation;
	Ogre::Quaternion BarrelBaseOrientation;
};

class Visuals
{
public:
	Visuals(Unit* UnitReference);
	~Visuals(void);
	void setNodeTree(NodeElement NodeTree);
	void Update(Ogre::Real TimeStep);
	void AddAnimation(Animation NewAnimation);
	void StartAnim(std::string Animation);
	void AddTurret(TurretVisuals NewTurret);
	void setTurretOrientation(std::string Turret, Ogre::Radian Azimuth, Ogre::Radian Elevation); //BaseOrientation of turret and barrel taken as zero

private:
	Unit* Parent; //Reference to parent unit
	NodeElement mNodeTree;
	std::vector<Animation> mAnimationList;
	std::list<ActiveTrack> mActiveTrackList;
	std::vector<TurretVisuals> mTurretList;

	void SetupSubNodes(NodeElement* Node, Ogre::SceneNode* ParentNode);
	Ogre::SceneNode* getTreeNode(std::string Name, NodeElement NodeTree); //recursively find scenenode associated with Name
	NodeElement* getTreeElement(std::string Name, NodeElement* NodeTree);
};

