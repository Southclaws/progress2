#include <crashdetect> // needs to be included before YSI

#define RUN_TESTS
#include <ut_mock_players>
#include <YSI\y_testing>

#include "progress2.inc"

Test:PlayerProgressBar() {
	new playerid = 68;

	new
		PlayerBar:bar1 = CreatePlayerProgressBar(playerid, 310.0, 200.0, 50.0, 10.0, 0x11acFFFF, 10.0, BAR_DIRECTION_LEFT),
		PlayerBar:bar2 = CreatePlayerProgressBar(playerid, 320.0, 200.0, 10.0, 50.0, 0xcfcf11FF, 10.0, BAR_DIRECTION_UP),
		PlayerBar:bar3 = CreatePlayerProgressBar(playerid, 320.0, 215.0, 10.0, 50.0, 0xac11FFFF, 10.0, BAR_DIRECTION_DOWN),
		PlayerBar:bar4 = CreatePlayerProgressBar(playerid, 320.0, 200.0, 50.0, 10.0, 0xcfcaf1FF, 10.0, BAR_DIRECTION_RIGHT);

	ASSERT(IsValidPlayerProgressBar(playerid, bar1) == 1);
	ASSERT(IsValidPlayerProgressBar(playerid, bar2) == 1);
	ASSERT(IsValidPlayerProgressBar(playerid, bar3) == 1);
	ASSERT(IsValidPlayerProgressBar(playerid, bar4) == 1);

	new Float:x, Float:y;
	GetPlayerProgressBarPos(playerid, bar4, x, y);
	ASSERT(x == 320.0 && y == 200.0);
	SetPlayerProgressBarPos(playerid, bar4, 100.0, 250.0);
	GetPlayerProgressBarPos(playerid, bar4, x, y);
	ASSERT(x == 100.0 && y == 250.0);

	new Float:width = GetPlayerProgressBarWidth(playerid, bar4);
	ASSERT(width == 50.0);
	SetPlayerProgressBarWidth(playerid, bar4, 75.0);
	width = GetPlayerProgressBarWidth(playerid, bar4);
	ASSERT(width == 75.0);

	new Float:height = GetPlayerProgressBarHeight(playerid, bar4);
	ASSERT(height == 10.0);
	SetPlayerProgressBarHeight(playerid, bar4, 25.0);
	height = GetPlayerProgressBarHeight(playerid, bar4);
	ASSERT(height == 25.0);

	new colour = GetPlayerProgressBarColour(playerid, bar4);
	ASSERT(colour == 0xcfcaf1FF);

	SetPlayerProgressBarColour(playerid, bar4, 0x11acFFFF);
	colour = GetPlayerProgressBarColour(playerid, bar4);
	ASSERT(colour == 0x11acFFFF);

	new Float:maxvalue = GetPlayerProgressBarMaxValue(playerid, bar4);
	ASSERT(maxvalue == 10.0);

	SetPlayerProgressBarMaxValue(playerid, bar4, 88.8);
	maxvalue = GetPlayerProgressBarMaxValue(playerid, bar4);
	ASSERT(maxvalue == 88.8);

	new Float:value = GetPlayerProgressBarValue(playerid, bar4);
	ASSERT(value == 0.0);

	SetPlayerProgressBarValue(playerid, bar4, 44.4);
	value = GetPlayerProgressBarValue(playerid, bar4);
	ASSERT(value == 44.4);

	new direction = GetPlayerProgressBarDirection(playerid, bar4);
	ASSERT(direction == BAR_DIRECTION_RIGHT);
	SetPlayerProgressBarDirection(playerid, bar4, BAR_DIRECTION_UP);
	direction = GetPlayerProgressBarDirection(playerid, bar4);
	ASSERT(direction == BAR_DIRECTION_UP);
}
