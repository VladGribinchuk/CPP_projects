// Class and stuctures

#include <Windows.h>
#include <list>
#include <ctime>

#define MAX_LOADSTRING		100
#define MAINWINDOWWIDTH		300
#define MAINWINDOWHEIGHT	600
#define WORKWINDOWWIDTH		800
#define WORKWINDOWHEIGHT	600
#define MAXNODESNAKE		32
#define BEGINNODESNAKE		2
#define LEVELNUM			2

#define BEGINMAPX 10	//px
#define BEGINMAPY 20	//px
#define MAPSTEP 20		//px
#define MAPX	780		//px
#define MAPY	400		//px
#define MAPSIZEX MAPX/MAPSTEP
#define MAPSIZEY MAPY/MAPSTEP

#define UNAME 100
#define OKRUN 0
#define REVERSERUN 1
#define CRASHRUN 2
#define EATITSELF 3
#define EATALLSTUFF 4
#define EATSTUFF 5
#define NOMATTER 0x00A0
#define MAXSTUFF 40
#define MAXBLOCK 100

struct _Table{
	//
	// struct for display actual information of
	// users progress
	//
	TCHAR UserName[UNAME];
	DWORD LevelNum;
	DOUBLE Scores;

};

struct _User{
	//
	// Info about current user
	//
	TCHAR UserName[UNAME];
	DWORD ProgressLevel;
	DWORD SNAKESPEED;
	DWORD isExtraSkin;
	DWORD isAllLevelOpen;
	DWORD SKIN;

	DWORD records[LEVELNUM];
};

struct _Level{
	//
	// Info about loaded level
	//
	TCHAR LevelName[UNAME];
	DWORD LevelNum;

	DWORD MaxStuff;

	DWORD AmountBlock;
	POINT PosBlock[MAXBLOCK];
};

struct _Map{
	//
	// Area of current level room
	//

	//logical size
	DWORD sizeX;
	DWORD sizeY;

	//DWORD AmountStuff;
	POINT PosStuff;
	DWORD MaxStuff;

	DWORD AmountBlock;
	POINT PosBlock[MAXBLOCK];

	//BOOL isEaten[MAXSTUFF];
};

class Snake{
	//
	// Main "engine"
	//

public:
	// snake consist of node
	// every node has coordinates
	struct _Node {
		long curPosX;
		long curPosY;
		long oldPosX;
		long oldPosY;
	};

private:
	std::list<_Node> lNode;
	long MAX_NODE;
	long COUNT_NODE;
	_Map Map;

	int CheckVectorHoriz;
	int CheckVectorVert;

	int Cond;

public:

	void InitMap(DWORD sizeX,
		DWORD sizeY,
		DWORD maxStuff,
		DWORD AmountBlock,
		POINT PosBlock[MAXBLOCK]
		)
	{
		Map.sizeX = sizeX;
		Map.sizeY = sizeY;
		Map.MaxStuff = maxStuff;
		
		// NOMATTER block will not show on
		// current level room map
		for (int i = 0; i < MAXBLOCK; i++)
		{
			Map.PosBlock[i].x = NOMATTER;
			Map.PosBlock[i].y = NOMATTER;
		}
		
		Map.AmountBlock = AmountBlock;
		for (int i = 0; i < AmountBlock; i++)
		{
			Map.PosBlock[i] = *(PosBlock + i);
		}
		CreatePosStuff();
	}

	void Init(long COUNT_MAX_NODE, long C_NOD, int STARTPOSX, int STARTPOSY)
	{
		srand(time(NULL));

		Cond = 0;

		lNode.clear();
		//
		// Set default settings map
		//
		MAX_NODE = COUNT_MAX_NODE;


		//
		// Defaut amount Node
		//
		COUNT_NODE = C_NOD;

		//
		// Set starting position
		// for each Node from Tail
		//
		for (int i = 0; i < COUNT_NODE; i++)
		{
			_Node Obj;
			Obj.curPosX = STARTPOSX;
			Obj.curPosY = STARTPOSY;
			lNode.push_back(Obj);
		}
		DefaultVector();
		
	}

	void CreatePosStuff()
	{
		BOOL repeat;
		POINT posStuff;
		do
		{
			repeat = FALSE;
			posStuff.x = rand() % Map.sizeX;
			posStuff.y = rand() % Map.sizeY;
			for (int i = 0; i < Map.AmountBlock; i++)
				if ((Map.PosBlock[i].x == posStuff.x) && (Map.PosBlock[i].y == posStuff.y))
					repeat = TRUE; 
		} while (repeat);
		
		Map.PosStuff = posStuff;
	}

	void DefaultVector()
	{
		//
		// default vector run
		// 
		CheckVectorHoriz = 1;
		CheckVectorVert = 0;
	}

	void Run(int Vhoriz, int Vvert)
	{
		if ((Vhoriz == -1) && (CheckVectorHoriz == 1) ||
			((Vhoriz == 1) && (CheckVectorHoriz == -1)) ||
			((Vvert == -1) && (CheckVectorVert == 1)) ||
			((Vvert == 1) && (CheckVectorVert == -1)))
			if (Vhoriz)
				if (Vhoriz == -1)
					Vhoriz = 1;
				else
					Vhoriz = -1;
			else
				if (Vvert == -1)
					Vvert = 1;
				else
					Vvert = -1;

		CheckVectorHoriz = Vhoriz;
		CheckVectorVert = Vvert;

		//
		// Set old position for each Node
		//
		std::list<_Node>::iterator it = lNode.begin();
		while (it != lNode.end())
		{
			(*it).oldPosX = (*it).curPosX;
			(*it).oldPosY = (*it).curPosY;
			it++;
		}

		//
		// Move Head
		//
		it = lNode.begin();
		(*it).curPosX += 1 * Vhoriz;
		(*it).curPosY += 1 * Vvert;

		if ((*it).curPosX == Map.sizeX)
			(*it).curPosX = 0;
		if ((*it).curPosX == -1)
			(*it).curPosX = Map.sizeX - 1;

		if ((*it).curPosY == Map.sizeY)
			(*it).curPosY = 0;
		if ((*it).curPosY == -1)
			(*it).curPosY = Map.sizeY - 1;

		//
		// Default Condition 
		//
		Cond = OKRUN;

		//
		// Check eat itself
		// 
		it = lNode.begin();
		while (it != lNode.end())
		{
			std::list<_Node>::iterator it2 = it;
			it2++;
			while (it2 != lNode.end())
			{
				if (((*it).curPosX == (*it2).curPosX) && ((*it).curPosY == (*it2).curPosY))
					Cond = EATITSELF;
				it2++;
			}
			it++;
		}

		//
		// Move each Node from Tail
		//
		it = lNode.begin();
		it++;
		while (it != lNode.end())
		{
			it--;
			long tmpcurPosX = (*it).oldPosX;
			long tmpcurPosY = (*it).oldPosY;
			it++;
			(*it).curPosX = tmpcurPosX;
			(*it).curPosY = tmpcurPosY;
			it++;
		}

		//
		// Check eat stuff & Check crash of block
		//
		it = lNode.begin();
		for (int j = 0; j < Map.AmountBlock; j++)
		{
			if (((*it).curPosX == Map.PosBlock[j].x) && ((*it).curPosY == Map.PosBlock[j].y))
				Cond = CRASHRUN;
		}
		
		if (((*it).curPosX == Map.PosStuff.x) && ((*it).curPosY == Map.PosStuff.y))
		{
			_Node Obj;
			Obj.curPosX = lNode.back().oldPosX;
			Obj.curPosY = lNode.back().oldPosY;
			lNode.push_back(Obj);
			COUNT_NODE++;
			if (COUNT_NODE >= Map.MaxStuff + BEGINNODESNAKE)
				Cond = EATALLSTUFF; 
			else
				Cond = EATSTUFF;
		}
	}

	int GetCond()
	{
		return Cond;
	}

	int GetCurNodePosX(long index)
	{
		std::list<_Node>::iterator it = lNode.begin();
		for (int i = 0; i < index; i++)it++;
		return (int)(*it).curPosX;
	}

	int GetCurNodePosY(long index)
	{
		std::list<_Node>::iterator it = lNode.begin();
		for (int i = 0; i < index; i++)it++;
		return (int)(*it).curPosY;
	}

	int GetCountNode(){
		return COUNT_NODE;
	}

	int GetMaxNode(){
		return MAX_NODE;
	}

	int GetPosStuffX(){
		return Map.PosStuff.x;
	}

	int GetPosStuffY(){
		return Map.PosStuff.y;
	}
	int GetAmountBlock(){
		return Map.AmountBlock;
	}

};