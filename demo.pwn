#include <ZCMD>
#include <strlib>
#include <sscanf2>

#include "progress2.inc"


new
	PlayerBar:gBar[4],
	Float:gBarValue[4],
	Float:gInc[4];

public OnFilterScriptExit() {
	for(new i; i < 1024; i++) {
		PlayerTextDrawDestroy(0, PlayerText:i);
	}

	return 1;
}

funtests() {
	//gBar[0] = CreatePlayerProgressBar(0, 300.0, 180.0, 100.0, 4.4, 0x11acFFFF, 10.0, BAR_DIRECTION_RIGHT);
	//gBar[1] = CreatePlayerProgressBar(0, 300.0, 190.0, 100.0, 4.4, 0xcfcf11FF, 15.0, BAR_DIRECTION_RIGHT);
	//gBar[2] = CreatePlayerProgressBar(0, 300.0, 180.0, 100.0, 4.4, 0xac11FFFF, 18.0, BAR_DIRECTION_LEFT);
	//gBar[3] = CreatePlayerProgressBar(0, 300.0, 190.0, 100.0, 4.4, 0xcfcaf1FF, 8.0, BAR_DIRECTION_LEFT);

	gBar[0] = CreatePlayerProgressBar(0, 310.0, 200.0, 50.0, 10.0, 0x11acFFFF, 10.0, BAR_DIRECTION_LEFT);
	gBar[1] = CreatePlayerProgressBar(0, 320.0, 200.0, 10.0, 50.0, 0xcfcf11FF, 10.0, BAR_DIRECTION_UP);
	gBar[2] = CreatePlayerProgressBar(0, 320.0, 215.0, 10.0, 50.0, 0xac11FFFF, 10.0, BAR_DIRECTION_DOWN);
	gBar[3] = CreatePlayerProgressBar(0, 320.0, 200.0, 50.0, 10.0, 0xcfcaf1FF, 10.0, BAR_DIRECTION_RIGHT);

	SetPlayerProgressBarValue(0, gBar[0], 0.0001);
	SetPlayerProgressBarValue(0, gBar[1], 5.0);
	SetPlayerProgressBarValue(0, gBar[2], 0.0001);
	SetPlayerProgressBarValue(0, gBar[3], 5.0);

	ShowPlayerProgressBar(0, gBar[0]);
	ShowPlayerProgressBar(0, gBar[1]);
	ShowPlayerProgressBar(0, gBar[2]);
	ShowPlayerProgressBar(0, gBar[3]);

	gBarValue[0] = 2.0;
	gBarValue[1] = 5.0;
	gBarValue[2] = 7.0;
	gBarValue[3] = 3.344;

	gInc[0] = 1.0;
	gInc[1] = 1.0;
	gInc[2] = 1.0;
	gInc[3] = 1.0;

	SetTimer("updatebar", 100, true);
}

forward updatebar();
public updatebar() {
	for(new i; i < 4; i++) 	{
		if(gBarValue[i] >= GetPlayerProgressBarMaxValue(0, gBar[i])) {
			gInc[i] = -1.0;
		}

		if(gBarValue[i] <= 0.0) {
			gInc[i] = 1.0;
		}

		SetPlayerProgressBarValue(0, gBar[i], gBarValue[i]);
		gBarValue[i] += gInc[i];
	}
}

CMD:bars(playerid, params[]) {
	funtests();
	return 1;
}

CMD:showbar(playerid, params[]) {
	ShowPlayerProgressBar(playerid, gBar[0]);
	return 1;
}

CMD:hidebar(playerid, params[]) {
	HidePlayerProgressBar(playerid, gBar[0]);
	return 1;
}

CMD:delbar(playerid, params[]) {
	DestroyPlayerProgressBar(playerid, gBar[0]);
	return 1;
}

CMD:barv(playerid, params[]) {
	SetPlayerProgressBarValue(playerid, gBar[0], floatstr(params));
	SendClientMessage(playerid, -1, sprintf("Value: %f", GetPlayerProgressBarValue(playerid, gBar[0])));

	return 1;
}

CMD:barm(playerid, params[]) {
	SetPlayerProgressBarMaxValue(playerid, gBar[0], floatstr(params));
	SendClientMessage(playerid, -1, sprintf("Value: %f", GetPlayerProgressBarMaxValue(playerid, gBar[0])));

	return 1;
}

CMD:barc(playerid, params[]) {
	new colour;
	sscanf(params, "x", colour);
	SetPlayerProgressBarColour(playerid, gBar[0], colour);
	return 1;
}

CMD:barp(playerid, params[]) {
	new
		Float:x,
		Float:y,
		Float:w,
		Float:h;

	sscanf(params, "ffff", x, y, w, h);

	SetPlayerProgressBarPos(playerid, gBar[0], x, y);
	SetPlayerProgressBarWidth(playerid, gBar[1], w);
	SetPlayerProgressBarHeight(playerid, gBar[1], h);

	return 1;
}
