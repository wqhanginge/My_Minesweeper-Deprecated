#include "gamecore.h"


GameInfo Game;
GameScore Score;

int index2x(int index)
{
	return (index % Game.width);
}

int index2y(int index)
{
	return (index / Game.width);
}

int xy2index(int x, int y)
{
	return (y * Game.width) + x;
}

void getNeighbors(Neighbor & neighbor, int x, int y)
{
	neighbor[0] = xy2index(x, y);
	neighbor[1] = (x > 0 && y > 0) ? xy2index(x - 1, y - 1) : -1;
	neighbor[2] = (y > 0) ? xy2index(x, y - 1) : -1;
	neighbor[3] = (x < Game.width - 1 && y > 0) ? xy2index(x + 1, y - 1) : -1;
	neighbor[4] = (x > 0) ? xy2index(x - 1, y) : -1;
	neighbor[5] = (x < Game.width - 1) ? xy2index(x + 1, y) : -1;
	neighbor[6] = (x > 0 && y < Game.height - 1) ? xy2index(x - 1, y + 1) : -1;
	neighbor[7] = (y < Game.height - 1) ? xy2index(x, y + 1) : -1;
	neighbor[8] = (x < Game.width - 1 && y < Game.height - 1) ? xy2index(x + 1, y + 1) : -1;
}

void getNeighbors(Neighbor & neighbor, int index)
{
	getNeighbors(neighbor, index2x(index), index2y(index));
}

void setGameMode(byte mode, byte width/*=0*/, byte height/*=0*/, word mines/*=0*/)
{
	switch (mode) {
	case JUNIOR:
		Game.mode = JUNIOR;
		Game.state = INIT;
		Game.width = JUNIOR_WIDTH;
		Game.height = JUNIOR_HEIGHT;
		Game.size = JUNIOR_SIZE;
		Game.mines = JUNIOR_MINES;
		Game.mine_remains = JUNIOR_MINES;
		Game.uncov_units = 0;
		Game.time = 0;
		memset(Game.map, 0, sizeof(byte) * Game.size);
		break;
	case MIDDLE:
		Game.mode = MIDDLE;
		Game.state = INIT;
		Game.width = MIDDLE_WIDTH;
		Game.height = MIDDLE_HEIGHT;
		Game.size = MIDDLE_SIZE;
		Game.mines = MIDDLE_MINES;
		Game.mine_remains = MIDDLE_MINES;
		Game.uncov_units = 0;
		Game.time = 0;
		memset(Game.map, 0, sizeof(byte) * Game.size);
		break;
	case SENIOR:
		Game.mode = SENIOR;
		Game.state = INIT;
		Game.width = SENIOR_WIDTH;
		Game.height = SENIOR_HEIGHT;
		Game.size = SENIOR_SIZE;
		Game.mines = SENIOR_MINES;
		Game.mine_remains = SENIOR_MINES;
		Game.uncov_units = 0;
		Game.time = 0;
		memset(Game.map, 0, sizeof(byte) * Game.size);
		break;
	case CUSTOM:
		Game.mode = CUSTOM;
		Game.state = INIT;
		Game.width = (width < MIN_WIDTH) ? MIN_WIDTH : width;
		Game.width = (Game.width > MAX_WIDTH) ? MAX_WIDTH : Game.width;
		Game.height = (height < MIN_HEIGHT) ? MIN_HEIGHT : height;
		Game.height = (Game.height > MAX_HEIGHT) ? MAX_HEIGHT : Game.height;
		Game.size = Game.width * Game.height;
		Game.mines = (mines > MAX_MINES(Game.size)) ? MAX_MINES(Game.size) : mines;
		Game.mines = (Game.mines < MIN_MINES) ? MIN_MINES : Game.mines;
		Game.mine_remains = (short)Game.mines;
		Game.uncov_units = 0;
		Game.time = 0;
		memset(Game.map, 0, sizeof(byte) * Game.size);
		break;
	default:
		setGameMode(JUNIOR);
		break;
	}
}

void setGameState(byte state)
{
	if (state >= UNKNOW) return;
	Game.state = state;
}

void resetGame()
{
	Game.state = INIT;
	Game.mine_remains = Game.mines;
	Game.uncov_units = 0;
	Game.time = 0;
	memset(Game.map, 0, sizeof(byte)*Game.size);
}

void createGameMap(int x, int y)
{
	if (Game.state != INIT) return;

	Neighbor safepos;
	getNeighbors(safepos, x, y);

	//generate mines, 8 units around where clicked won't have mines
	//use shuffle algorithm
	memset(Game.map, MUM_MINE, sizeof(byte)*Game.mines);
	dword neicount = 0;
	for (int i = 0; i < 9; i++) neicount += (safepos[i] != -1);	//remember how many safe positions needed
	for (dword k = 0; k < Game.mines; k++) {	//shuffle algorithm, ignore tail
		dword index = rand() % (Game.size - neicount - k) + k;
		swap(Game.map[k], Game.map[index]);
	}
	if (safepos[0] >= Game.size / 2) {	//there may be overlap between safe area and neighbor area, need handling
		for (dword i = 0; i < neicount; i++) swap(Game.map[i], Game.map[Game.size - 1 - i]);
		neicount = Game.size;	//WARNING:the meaning of neicount has been changed
	}
	for (int i = 0; i < 9; i++) {	//move safe area to where it is
		if (safepos[i] != -1) {
			swap(Game.map[safepos[i]], Game.map[Game.size - neicount]);
			neicount--;
		}
	}

	//write neighbor mines
	for (dword i = 0; i < Game.size; i++) {
		if (MUISMINE(Game.map[i])) continue;
		int m = 0;
		Neighbor neipos;
		getNeighbors(neipos, i);
		for (int j = 1; j < 9; j++)
			if (neipos[j] != -1 && MUISMINE(Game.map[neipos[j]])) m++;
		SETMUMINES(m, Game.map[i]);
	}
}

void createGameMap(int index)
{
	createGameMap(index2x(index), index2y(index));
}

int clickUnit(int x, int y)
{
	return clickUnit(xy2index(x, y));
}

int clickUnit(int index)
{
	if (Game.state != PROGRESS) return -2;
	if (index < 0 || index >= (int)Game.size) return -2;
	if (GETMUSTATE(Game.map[index]) != MUS_COVER && GETMUSTATE(Game.map[index]) != MUS_MARK) return -2;

	Game.uncov_units++;
	SETMUUPDATE(Game.map[index]);
	if (MUISMINE(Game.map[index])) {
		SETMUSTATE(MUS_BOMB, Game.map[index]);
		return -1;
	}
	else {
		SETMUSTATE(MUS_UNCOV, Game.map[index]);
		return GETMUMINES(Game.map[index]);
	}
}

int openBlanks(int x, int y)
{
	return openBlanks(xy2index(x, y));
}

int openBlanks(int index)
{
	if (Game.state != PROGRESS) return -1;
	if (index < 0 || index >= (int)Game.size) return -1;
	if (GETMUSTATE(Game.map[index]) != MUS_UNCOV) return -1;
	if (GETMUMINES(Game.map[index]) != 0) return -1;

	Neighbor pos;
	getNeighbors(pos, index);
	for (int i = 1; i < 9; i++) {
		int ret = clickUnit(pos[i]);
		if (ret == 0) openBlanks(pos[i]);
	}
	return 0;
}

int digNeighbors(int x, int y)
{
	return digNeighbors(xy2index(x, y));
}

int digNeighbors(int index)
{
	if (Game.state != PROGRESS) return -2;
	if (index < 0 || index >= (int)Game.size) return -2;
	if (GETMUSTATE(Game.map[index]) != MUS_UNCOV) return -2;

	Neighbor pos;
	getNeighbors(pos, index);

	//you can open neighbors only when flags EQ mines around neighbors
	int flags = 0;
	for (int i = 1; i < 9; i++)
		if (pos[i] != -1 && GETMUSTATE(Game.map[pos[i]]) == MUS_FLAG) flags++;
	if (GETMUMINES(Game.map[pos[0]]) != flags) return -2;

	Neighbor info;
	flags = 0;	//WARNING:the meaning of flags has been changed
	//open neighbors
	for (int i = 1; i < 9; i++) {
		info[i] = clickUnit(pos[i]);
		if (info[i] == 0) openBlanks(pos[i]);
		if (info[i] == -1) flags = -1;	//if bombed
	}
	return flags;
}

void uncovAllMines()
{
	if (Game.state == FAIL) {
		for (word i = 0; i < Game.size; i++) {
			if (MUISMINE(Game.map[i]) && (GETMUSTATE(Game.map[i]) == MUS_COVER || GETMUSTATE(Game.map[i]) == MUS_MARK)) {
				SETMUSTATE(MUS_UNCOV, Game.map[i]);
				SETMUUPDATE(Game.map[i]);
			}
			else if (!MUISMINE(Game.map[i]) && GETMUSTATE(Game.map[i]) == MUS_FLAG) {
				SETMUSTATE(MUS_WRONG, Game.map[i]);
				SETMUUPDATE(Game.map[i]);
			}
		}
	}
	else if (Game.state == SUCCESS) {
		for (word i = 0; i < Game.size; i++) {
			if (MUISMINE(Game.map[i]) && (GETMUSTATE(Game.map[i]) == MUS_COVER || GETMUSTATE(Game.map[i]) == MUS_MARK)) {
				SETMUSTATE(MUS_FLAG, Game.map[i]);
				SETMUUPDATE(Game.map[i]);
			}
		}
	}
}

void resetRecord()
{
	Score = { MAX_TIME,MAX_TIME,MAX_TIME,_T(DEF_SCORE_NAME_EN),_T(DEF_SCORE_NAME_EN),_T(DEF_SCORE_NAME_EN) };
}

dword getRecordTime(byte gamemode)
{
	switch (gamemode) {
	case JUNIOR:	return Score.junior_time;
	case MIDDLE:	return Score.middle_time;
	case SENIOR:	return Score.senior_time;
	default:		break;
	}
	return -1;
}

TCHAR *getpRecordName(byte gamemode)
{
	switch (gamemode) {
	case JUNIOR:	return Score.junior_name;
	case MIDDLE:	return Score.middle_name;
	case SENIOR:	return Score.senior_name;
	default:		break;
	}
	return nullptr;
}

void setRecordTime(byte gamemode)
{
	switch (gamemode) {
	case JUNIOR:
		Score.junior_time = Game.time;
		break;
	case MIDDLE:
		Score.middle_time = Game.time;
		break;
	case SENIOR:
		Score.senior_time = Game.time;
		break;
	default:
		break;
	}
}
