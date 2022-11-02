#pragma once
#include "stdafx.h"

//Forward declare any classes referenced
class Unit;
class ProjectRho;

class UnitManager
{
public:
	UnitManager(ProjectRho* App);
	~UnitManager();

	Unit* CreateUnit();
	void DeleteUnit(int Index);
	Unit* getUnit(int Index); //WARNING: indices and pointers to Unit* are invalidated after list-rearranging methods are called
	Unit* getUnitByID(int ID);
	void UpdateAllUnits(double TimeStep);
private:
	ProjectRho* AppHandle; //Handle to game application
	std::vector<Unit*> UnitArray;
	int NextID; //ID of next unit to be created
};

