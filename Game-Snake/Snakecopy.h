// Class and stuctures

#include <Windows.h>
#include <list>

#define MAX_LOADSTRING		100
#define MAINWINDOWWIDTH		300
#define MAINWINDOWHEIGHT	600
#define WORKWINDOWWIDTH		400
#define WORKWINDOWHEIGHT	600
#define MAXNODESNAKE		20
#define BEGINNODESNAKE		2
//#define RUNSPEED			600 //ms

#define BEGINMAPX 10	//px
#define BEGINMAPY 20	//px
#define MAPSTEP 15		//px
#define MAPX	375		//px
#define MAPY	405		//px
#define MAPSIZEX MAPX/MAPSTEP
#define MAPSIZEY MAPY/MAPSTEP
#define ENCRYPT_FACTOR (unsigned char)22

#define UNAME 100
#define OKRUN 0
#define REVERSERUN 1
#define CRASHRUN 2
#define EATITSELF 3
#define EATALLSTUFF 4
#define NOMATTER 0x1000
#define MAXSTUFF 20
#define MAXBLOCK 20

typedef struct _Crypt{
	BYTE val;
	BOOL param;
} Crypt;

struct _User{

	TCHAR UserName[UNAME];
	DWORD ProgressLevel;
	DWORD SNAKESPEED;
	DWORD isExtraSkin;
	DWORD isAllLevelOpen;
	DWORD SKIN;
};

struct _Level{

	TCHAR LevelName[UNAME];
	DWORD LevelNum;

	DWORD AmountStuff;
	POINT PosStuff[MAXSTUFF];

	DWORD AmountBlock;
	POINT PosBlock[MAXBLOCK];
};

struct _Map{

	//logical size
	DWORD sizeX;
	DWORD sizeY;

	DWORD AmountStuff;
	POINT PosStuff[MAXSTUFF];

	DWORD AmountBlock;
	POINT PosBlock[MAXBLOCK];

	BOOL isEaten[MAXSTUFF];
};

class Snake
{
public:
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

public:

	void InitMap(DWORD sizeX,
		DWORD sizeY,
		DWORD AmountStuff,
		POINT PosStuff[20],
		DWORD AmountBlock,
		POINT PosBlock[20]
		)
	{
		Map.sizeX = sizeX;
		Map.sizeY = sizeY;
		Map.AmountStuff = AmountStuff;
		for (int i = 0; i < 20; i++)
		{
			Map.PosStuff[i] = *(PosStuff + i);
			if (Map.PosStuff[i].x >= NOMATTER)
				Map.isEaten[i] = TRUE;
			else
				Map.isEaten[i] = FALSE;
		}
		Map.AmountBlock = AmountBlock;
		for (int i = 0; i < 20; i++)
		{
			Map.PosBlock[i] = *(PosBlock + i);
		}
	}

	void Init(long COUNT_MAX_NODE, long C_NOD, int STARTPOSX, int STARTPOSY)
	{
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

	void DefaultVector()
	{
		CheckVectorHoriz = 1;
		CheckVectorVert = 0;
	}

	int Run(int Vhoriz, int Vvert)
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
					return EATITSELF;
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
		for (int j = 0; j < 20; j++)
		{
			if (((*it).curPosX == Map.PosBlock[j].x) && ((*it).curPosY == Map.PosBlock[j].y))
				return CRASHRUN;

			if (((*it).curPosX == Map.PosStuff[j].x) && ((*it).curPosY == Map.PosStuff[j].y))
			{
				Map.isEaten[j] = TRUE;
				Map.PosStuff[j].x = NOMATTER;
				Map.PosStuff[j].y = NOMATTER;
				_Node Obj;
				Obj.curPosX = lNode.back().oldPosX;
				Obj.curPosY = lNode.back().oldPosY;
				lNode.push_back(Obj);
				COUNT_NODE++;
			}
		}

		int count = 0;
		for (int j = 0; j < 20; j++)
		{
			if (Map.isEaten[j] == TRUE)
				count++;
		}
		if (count == 20)
			return EATALLSTUFF;


		return OKRUN;
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

	int GetPosStuffX(int index){
		return Map.PosStuff[index].x;
	}

	int GetPosStuffY(int index){
		return Map.PosStuff[index].y;
	}

};