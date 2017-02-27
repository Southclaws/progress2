#if defined _progress2_included
	#endinput
#endif
#define _progress2_included

#define BAR_DIRECTION_RIGHT \
	0
#define BAR_DIRECTION_LEFT \
	1
#define BAR_DIRECTION_UP \
	2
#define BAR_DIRECTION_DOWN \
	3

#define BAR_TEXTDRAW_BACKGROUND \
	0
#define BAR_TEXTDRAW_FILL \
	1
#define BAR_TEXTDRAW_MAIN \
	2

enum PB_e_BAR
{
	PB_e_BAR_DIRECTION,
	Float:PB_e_BAR_X,
	Float:PB_e_BAR_Y,
	Float:PB_e_BAR_WIDTH,
	Float:PB_e_BAR_HEIGHT,
	PB_e_BAR_COLOR,
	PB_e_BAR_BACKGROUND_COLOR,
	Float:PB_e_BAR_VALUE,
	Float:PB_e_BAR_MAX_VALUE
};

/******************************
*
*
*   PLAYER PROGRESS BARS
*
*
******************************/

#if !defined MAX_PLAYER_BARS
	#define MAX_PLAYER_BARS \
		(MAX_PLAYER_TEXT_DRAWS / 3)
#endif

#define INVALID_PLAYER_BAR_ID \
	(PlayerBar:-1)

static PB_ePlayerBar[MAX_PLAYERS][PlayerBar:MAX_PLAYER_BARS][PB_e_BAR];
static PlayerText:PB_ptxtBar[MAX_PLAYERS][PlayerBar:MAX_PLAYER_BARS][3];
static bool:PB_bPlayerBarShown[MAX_PLAYERS][PlayerBar:MAX_PLAYER_BARS];
static PB_iPlayerBarCount[MAX_PLAYERS];

static Float:PB_GetBarPercent(Float:x, Float:widthorheight, Float:max, Float:value, direction)
{
	new Float:ret;
	switch(direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			ret = ((x - 3.0) + (((((x - 2.0) + widthorheight) - x) / max) * value));
		}

		case BAR_DIRECTION_LEFT:
		{
			ret = ((x - 1.0) - (((((x + 2.0) - widthorheight) - x) / max) * -value)) - 4.0;
		}

		case BAR_DIRECTION_UP:
		{
			ret = -((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) + 0.55);
		}

		case BAR_DIRECTION_DOWN:
		{
			ret = ((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) - 0.55);
		}
	}
	return ret;
}

stock static PB_CreatePlayerBarBackground(playerid, PlayerBar:barid)
{
	new direction = PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION],
		Float:x = PB_ePlayerBar[playerid][barid][PB_e_BAR_X],
	    Float:y = PB_ePlayerBar[playerid][barid][PB_e_BAR_Y],
	    Float:width = PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH],
	    Float:height = PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT],
	    bgcolor = PB_ePlayerBar[playerid][barid][PB_e_BAR_BACKGROUND_COLOR];

    switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], ((x + width) - 4.0), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (height / 10));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (height / 10));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_UP:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (-((height / 10) * 1.02) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], 1.0, ((height / 10) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}
	}
}

stock static PB_CreatePlayerBarFill(playerid, PlayerBar:barid)
{
	new direction = PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION],
		Float:x = PB_ePlayerBar[playerid][barid][PB_e_BAR_X],
	    Float:y = PB_ePlayerBar[playerid][barid][PB_e_BAR_Y],
	    Float:width = PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH],
	    Float:height = PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT],
	    color = PB_ePlayerBar[playerid][barid][PB_e_BAR_COLOR];

    switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL] = CreatePlayerTextDraw(playerid, (x + 1.2), (y + 2.15), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((x + width) - 5.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL] = CreatePlayerTextDraw(playerid, (x - 1.2), (y + 2.15), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_UP:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL] = CreatePlayerTextDraw(playerid, (x - 1.2), (y - 1.0), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1.0, -((height / 10) - 1.02));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL] = CreatePlayerTextDraw(playerid, (x - 1.2), (y + 1.0), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.55));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}
	}
}

stock static PB_CreatePlayerBarMain(playerid, PlayerBar:barid)
{
	new direction = PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION],
		Float:x = PB_ePlayerBar[playerid][barid][PB_e_BAR_X],
	    Float:y = PB_ePlayerBar[playerid][barid][PB_e_BAR_Y],
	    Float:width = PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH],
	    Float:height = PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT],
	    Float:value = PB_ePlayerBar[playerid][barid][PB_e_BAR_VALUE],
	    Float:max_value = PB_ePlayerBar[playerid][barid][PB_e_BAR_MAX_VALUE],
	    color = PB_ePlayerBar[playerid][barid][PB_e_BAR_COLOR];

	switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN] = CreatePlayerTextDraw(playerid, (x + 1.2), (y + 2.15), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], PB_GetBarPercent(x, width, max_value, value, direction), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1.0, ((height / 10) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN] = CreatePlayerTextDraw(playerid, (x - 1.2), (y + 2.15), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], PB_GetBarPercent(x, width, max_value, value, direction), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1.0, ((height / 10) - 0.35));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_UP:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN] = CreatePlayerTextDraw(playerid, (x - 1.2), (y - 1.0), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], ((x - width) - 2.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1.0, PB_GetBarPercent(x, width, max_value, value, direction));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN] = CreatePlayerTextDraw(playerid, (x - 1.2), (y + 1.0), "_");
			PlayerTextDrawUseBox(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1);
			PlayerTextDrawTextSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], ((x - width) - 2.5), 0.0);
			PlayerTextDrawLetterSize(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], 1.0, PB_GetBarPercent(x, width, max_value, value, direction));
			PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], color);
		}
	}
}

/*
native PlayerBar: CreatePlayerProgressBar(playerid, Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, color = 0xFF1C1CFF, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT, bgcolor = 0x000000FF);
*/
stock PlayerBar:CreatePlayerProgressBar(playerid, Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, color = 0xFF1C1CFF, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT, bgcolor = 0x000000FF)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return INVALID_PLAYER_BAR_ID;
	}

	if (PB_iPlayerBarCount[playerid] == MAX_PLAYER_BARS)
	{
	    return INVALID_PLAYER_BAR_ID;
	}

	new PlayerBar:barid = PlayerBar:PB_iPlayerBarCount[playerid]++;

	PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION] = direction;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_X] = x;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_Y] = y;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH] = width;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT] = height;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_COLOR] = color;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_BACKGROUND_COLOR] = bgcolor;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_VALUE] = 0.0;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_MAX_VALUE] = max;

    PB_bPlayerBarShown[playerid][barid] = false;

	PB_CreatePlayerBarBackground(playerid, barid);
	PB_CreatePlayerBarFill(playerid, barid);
	PB_CreatePlayerBarMain(playerid, barid);
	return barid;
}

/*
native IsValidPlayerProgressBar(playerid, PlayerBar:barid);
*/
stock IsValidPlayerProgressBar(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (barid < PlayerBar:0 || barid >= PlayerBar:PB_iPlayerBarCount[playerid])
	{
	    return 0;
	}

	return 1;
}

/*
native DestroyPlayerProgressBar(playerid, PlayerBar:barid);
*/
stock DestroyPlayerProgressBar(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);

	for (new PlayerBar:i = barid; i < (PlayerBar:(PB_iPlayerBarCount[playerid] - 1)); i++)
	{
		PB_ePlayerBar[playerid][i][PB_e_BAR_DIRECTION] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_DIRECTION];
		PB_ePlayerBar[playerid][i][PB_e_BAR_X] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_X];
		PB_ePlayerBar[playerid][i][PB_e_BAR_Y] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_Y];
		PB_ePlayerBar[playerid][i][PB_e_BAR_WIDTH] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_WIDTH];
		PB_ePlayerBar[playerid][i][PB_e_BAR_HEIGHT] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_HEIGHT];
		PB_ePlayerBar[playerid][i][PB_e_BAR_COLOR] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_COLOR];
		PB_ePlayerBar[playerid][i][PB_e_BAR_BACKGROUND_COLOR] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_BACKGROUND_COLOR];
		PB_ePlayerBar[playerid][i][PB_e_BAR_VALUE] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_VALUE];
		PB_ePlayerBar[playerid][i][PB_e_BAR_MAX_VALUE] = PB_ePlayerBar[playerid][(i + PlayerBar:1)][PB_e_BAR_MAX_VALUE];

    	PB_bPlayerBarShown[playerid][i] = PB_bPlayerBarShown[playerid][(i + PlayerBar:1)];

		PB_ptxtBar[playerid][i][BAR_TEXTDRAW_BACKGROUND] = PB_ptxtBar[playerid][(i + PlayerBar:1)][BAR_TEXTDRAW_BACKGROUND];
		PB_ptxtBar[playerid][i][BAR_TEXTDRAW_FILL] = PB_ptxtBar[playerid][(i + PlayerBar:1)][BAR_TEXTDRAW_FILL];
		PB_ptxtBar[playerid][i][BAR_TEXTDRAW_MAIN] = PB_ptxtBar[playerid][(i + PlayerBar:1)][BAR_TEXTDRAW_MAIN];
	}

	PB_iPlayerBarCount[playerid]--;
	return 1;
}

/*
native ShowPlayerProgressBar(playerid, PlayerBar:barid);
*/
stock ShowPlayerProgressBar(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);

	PB_bPlayerBarShown[playerid][barid] = true;
	return 1;
}

/*
native HidePlayerProgressBar(playerid, PlayerBar:barid);
*/
stock HidePlayerProgressBar(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PlayerTextDrawHide(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawHide(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawHide(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);

	PB_bPlayerBarShown[playerid][barid] = false;
	return 1;
}

/*
native GetPlayerProgressBarDirection(playerid, PlayerBar:barid);
*/
stock GetPlayerProgressBarDirection(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION];
}

/*
native GetPlayerProgressBarPos(playerid, PlayerBar:barid, &Float:x, &Float:y);
*/
stock GetPlayerProgressBarPos(playerid, PlayerBar:barid, &Float:x, &Float:y)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	x = PB_ePlayerBar[playerid][barid][PB_e_BAR_X];
	y = PB_ePlayerBar[playerid][barid][PB_e_BAR_X];
	return 1;
}

/*
native Float:GetPlayerProgressBarHeight(playerid, PlayerBar:barid);
*/
stock Float:GetPlayerProgressBarHeight(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0.0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0.0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT];
}

/*
native Float:GetPlayerProgressBarWidth(playerid, PlayerBar:barid);
*/
stock Float:GetPlayerProgressBarWidth(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0.0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0.0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH];
}

/*
native GetPlayerProgressBarBGColor(playerid, PlayerBar:barid);
*/
stock GetPlayerProgressBarBGColor(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_BACKGROUND_COLOR];
}

/*
native GetPlayerProgressBarColor(playerid, PlayerBar:barid);
*/
stock GetPlayerProgressBarColor(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_COLOR];
}

#define GetPlayerProgressBarColour \
    GetPlayerProgressBarColor

/*
native Float:GetPlayerProgressBarValue(playerid, PlayerBar:barid);
*/
stock Float:GetPlayerProgressBarValue(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0.0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0.0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_VALUE];
}

/*
native Float:GetPlayerProgressBarMaxValue(playerid, PlayerBar:barid);
*/
stock Float:GetPlayerProgressBarMaxValue(playerid, PlayerBar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0.0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0.0;
	}

	return PB_ePlayerBar[playerid][barid][PB_e_BAR_MAX_VALUE];
}

/*
native SetPlayerProgressBarDirection(playerid, PlayerBar:barid);
*/
stock SetPlayerProgressBarDirection(playerid, PlayerBar:barid, direction)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

    PB_ePlayerBar[playerid][barid][PB_e_BAR_DIRECTION] = direction;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarBackground(playerid, barid);
	PB_CreatePlayerBarFill(playerid, barid);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native SetPlayerProgressBarPos(playerid, PlayerBar:barid, &Float:x, &Float:y);
*/
stock SetPlayerProgressBarPos(playerid, PlayerBar:barid, Float:x, Float:y)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PB_ePlayerBar[playerid][barid][PB_e_BAR_X] = x;
	PB_ePlayerBar[playerid][barid][PB_e_BAR_X] = y;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarBackground(playerid, barid);
	PB_CreatePlayerBarFill(playerid, barid);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native SetPlayerProgressBarHeight(playerid, PlayerBar:barid, Float:height);
*/
stock SetPlayerProgressBarHeight(playerid, PlayerBar:barid, Float:height)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PB_ePlayerBar[playerid][barid][PB_e_BAR_HEIGHT] = height;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarBackground(playerid, barid);
	PB_CreatePlayerBarFill(playerid, barid);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native SetPlayerProgressBarWidth(playerid, PlayerBar:barid, Float:width);
*/
stock SetPlayerProgressBarWidth(playerid, PlayerBar:barid, Float:width)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PB_ePlayerBar[playerid][barid][PB_e_BAR_WIDTH] = width;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarBackground(playerid, barid);
	PB_CreatePlayerBarFill(playerid, barid);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native SetPlayerProgressBarBGColor(playerid, PlayerBar:barid, bgcolor);
*/
stock SetPlayerProgressBarBGColor(playerid, PlayerBar:barid, bgcolor)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

    PB_ePlayerBar[playerid][barid][PB_e_BAR_BACKGROUND_COLOR] = bgcolor;

	PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_BACKGROUND]);
	}
	return 1;
}

/*
native SetPlayerProgressBarColor(playerid, PlayerBar:barid, color);
*/
stock SetPlayerProgressBarColor(playerid, PlayerBar:barid, color)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

    PB_ePlayerBar[playerid][barid][PB_e_BAR_COLOR] = color;

	PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
	PlayerTextDrawBoxColor(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN], color);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_FILL]);
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

#define SetPlayerProgressBarColour \
    SetPlayerProgressBarColor

/*
native SetPlayerProgressBarValue(playerid, PlayerBar:barid, Float:value);
*/
stock SetPlayerProgressBarValue(playerid, PlayerBar:barid, Float:value)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PB_ePlayerBar[playerid][barid][PB_e_BAR_VALUE] = value;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native SetPlayerProgressBarMaxValue(playerid, PlayerBar:barid, Float:maxvalue);
*/
stock SetPlayerProgressBarMaxValue(playerid, PlayerBar:barid, Float:maxvalue)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidPlayerProgressBar(playerid, barid))
	{
	    return 0;
	}

	PB_ePlayerBar[playerid][barid][PB_e_BAR_MAX_VALUE] = maxvalue;

	PlayerTextDrawDestroy(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	PB_CreatePlayerBarMain(playerid, barid);
	if (PB_bPlayerBarShown[playerid][barid])
	{
		PlayerTextDrawShow(playerid, PB_ptxtBar[playerid][barid][BAR_TEXTDRAW_MAIN]);
	}
	return 1;
}

/*
native CountPlayerProgressBars(playerid);
*/
stock CountPlayerProgressBars(playerid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	return PB_iBarCount;
}

/******************************
*
*
*   GLOBAL PROGRESS BARS
*
*
******************************/

#if !defined MAX_BARS
	#define MAX_BARS \
		(MAX_TEXT_DRAWS / 3)
#endif

#define INVALID_BAR_ID \
	(Bar:-1)

static PB_eBar[Bar:MAX_BARS][PB_e_BAR];
static Text:PB_txtBar[Bar:MAX_BARS][3];
static bool:PB_bBarShownForPlayer[Bar:MAX_BARS][MAX_PLAYERS];
static PB_iBarCount;

stock static PB_CreateBarBackground(Bar:barid)
{
	new direction = PB_eBar[barid][PB_e_BAR_DIRECTION],
		Float:x = PB_eBar[barid][PB_e_BAR_X],
	    Float:y = PB_eBar[barid][PB_e_BAR_Y],
	    Float:width = PB_eBar[barid][PB_e_BAR_WIDTH],
	    Float:height = PB_eBar[barid][PB_e_BAR_HEIGHT],
	    bgcolor = PB_eBar[barid][PB_e_BAR_BACKGROUND_COLOR];

    switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND] = TextDrawCreate(x, y, "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], ((x + width) - 4.0), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (height / 10));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND] = TextDrawCreate(x, y, "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (height / 10));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_UP:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND] = TextDrawCreate(x, y, "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1.0, (-((height / 10) * 1.02) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND] = TextDrawCreate(x, y, "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], ((x - width) - 4.0), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], 1.0, ((height / 10) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);
		}
	}
}

stock static PB_CreateBarFill(Bar:barid)
{
	new direction = PB_eBar[barid][PB_e_BAR_DIRECTION],
		Float:x = PB_eBar[barid][PB_e_BAR_X],
	    Float:y = PB_eBar[barid][PB_e_BAR_Y],
	    Float:width = PB_eBar[barid][PB_e_BAR_WIDTH],
	    Float:height = PB_eBar[barid][PB_e_BAR_HEIGHT],
	    color = PB_eBar[barid][PB_e_BAR_COLOR];

    switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_FILL] = TextDrawCreate((x + 1.2), (y + 2.15), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((x + width) - 5.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_FILL] = TextDrawCreate((x - 1.2), (y + 2.15), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_UP:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_FILL] = TextDrawCreate((x - 1.2), (y - 1.0), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1.0, -((height / 10) - 1.02));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_FILL] = TextDrawCreate((x - 1.2), (y + 1.0), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((x - width) - 2.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_FILL], 1.0, ((height / 10) - 0.55));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
		}
	}
}

stock static PB_CreateBarMain(Bar:barid)
{
	new direction = PB_eBar[barid][PB_e_BAR_DIRECTION],
		Float:x = PB_eBar[barid][PB_e_BAR_X],
	    Float:y = PB_eBar[barid][PB_e_BAR_Y],
	    Float:width = PB_eBar[barid][PB_e_BAR_WIDTH],
	    Float:height = PB_eBar[barid][PB_e_BAR_HEIGHT],
	    Float:value = PB_eBar[barid][PB_e_BAR_VALUE],
	    Float:max_value = PB_eBar[barid][PB_e_BAR_MAX_VALUE],
	    color = PB_eBar[barid][PB_e_BAR_COLOR];

	switch (direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_MAIN] = TextDrawCreate((x + 1.2), (y + 2.15), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], PB_GetBarPercent(x, width, max_value, value, direction), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1.0, ((height / 10) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_LEFT:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_MAIN] = TextDrawCreate((x - 1.2), (y + 2.15), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], PB_GetBarPercent(x, width, max_value, value, direction), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1.0, ((height / 10) - 0.35));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_UP:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_MAIN] = TextDrawCreate((x - 1.2), (y - 1.0), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], ((x - width) - 2.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1.0, PB_GetBarPercent(x, width, max_value, value, direction));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], color);
		}

		case BAR_DIRECTION_DOWN:
		{
			PB_txtBar[barid][BAR_TEXTDRAW_MAIN] = TextDrawCreate((x - 1.2), (y + 1.0), "_");
			TextDrawUseBox(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1);
			TextDrawTextSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], ((x - width) - 2.5), 0.0);
			TextDrawLetterSize(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], 1.0, PB_GetBarPercent(x, width, max_value, value, direction));
			TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], color);
		}
	}
}

/*
native Bar: CreateProgressBar(Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, color = 0xFF1C1CFF, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT, bgcolor = 0x000000FF);
*/
stock Bar:CreateProgressBar(Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, color = 0xFF1C1CFF, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT, bgcolor = 0x000000FF)
{
	if (PB_iBarCount == MAX_BARS)
	{
	    return INVALID_BAR_ID;
	}

	new Bar:barid = Bar:PB_iBarCount++;

	PB_eBar[barid][PB_e_BAR_DIRECTION] = direction;
	PB_eBar[barid][PB_e_BAR_X] = x;
	PB_eBar[barid][PB_e_BAR_Y] = y;
	PB_eBar[barid][PB_e_BAR_WIDTH] = width;
	PB_eBar[barid][PB_e_BAR_HEIGHT] = height;
	PB_eBar[barid][PB_e_BAR_COLOR] = color;
	PB_eBar[barid][PB_e_BAR_BACKGROUND_COLOR] = bgcolor;
	PB_eBar[barid][PB_e_BAR_VALUE] = 0.0;
	PB_eBar[barid][PB_e_BAR_MAX_VALUE] = max;

	for (new i; i < MAX_PLAYERS; i++)
	{
	    PB_bBarShownForPlayer[barid][i] = false;
	}

	PB_CreateBarBackground(barid);
	PB_CreateBarFill(barid);
	PB_CreateBarMain(barid);
	return barid;
}

/*
native IsValidProgressBar(Bar:barid);
*/
stock IsValidProgressBar(Bar:barid)
{
	if (barid < Bar:0 || barid >= Bar:PB_iBarCount)
	{
	    return 0;
	}

	return 1;
}

/*
native DestroyProgressBar(Bar:barid);
*/
stock DestroyProgressBar(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);

	for (new Bar:i = barid; i < Bar:(PB_iBarCount - 1); i++)
	{
		PB_eBar[i][PB_e_BAR_DIRECTION] = PB_eBar[(i + Bar:1)][PB_e_BAR_DIRECTION];
		PB_eBar[i][PB_e_BAR_X] = PB_eBar[(i + Bar:1)][PB_e_BAR_X];
		PB_eBar[i][PB_e_BAR_Y] = PB_eBar[(i + Bar:1)][PB_e_BAR_Y];
		PB_eBar[i][PB_e_BAR_WIDTH] = PB_eBar[(i + Bar:1)][PB_e_BAR_WIDTH];
		PB_eBar[i][PB_e_BAR_HEIGHT] = PB_eBar[(i + Bar:1)][PB_e_BAR_HEIGHT];
		PB_eBar[i][PB_e_BAR_COLOR] = PB_eBar[(i + Bar:1)][PB_e_BAR_COLOR];
		PB_eBar[i][PB_e_BAR_BACKGROUND_COLOR] = PB_eBar[(i + Bar:1)][PB_e_BAR_BACKGROUND_COLOR];
		PB_eBar[i][PB_e_BAR_VALUE] = PB_eBar[(i + Bar:1)][PB_e_BAR_VALUE];
		PB_eBar[i][PB_e_BAR_MAX_VALUE] = PB_eBar[(i + Bar:1)][PB_e_BAR_MAX_VALUE];

		for (new x; x < MAX_PLAYERS; x++)
		{
		    PB_bBarShownForPlayer[i][x] = PB_bBarShownForPlayer[(i + Bar:1)][x];
		}

		PB_txtBar[i][BAR_TEXTDRAW_BACKGROUND] = PB_txtBar[(i + Bar:1)][BAR_TEXTDRAW_BACKGROUND];
		PB_txtBar[i][BAR_TEXTDRAW_FILL] = PB_txtBar[(i + Bar:1)][BAR_TEXTDRAW_FILL];
		PB_txtBar[i][BAR_TEXTDRAW_MAIN] = PB_txtBar[(i + Bar:1)][BAR_TEXTDRAW_MAIN];
	}

	PB_iBarCount--;
	return 1;
}

/*
native ShowProgressBarForPlayer(playerid, Bar:barid);
*/
stock ShowProgressBarForPlayer(playerid, Bar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	TextDrawShowForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawShowForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawShowForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);

	PB_bBarShownForPlayer[barid][playerid] = true;
	return 1;
}

/*
native HideProgressBarForPlayer(playerid, Bar:barid);
*/
stock HideProgressBarForPlayer(playerid, Bar:barid)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
	    return 0;
	}

	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	TextDrawHideForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawHideForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawHideForPlayer(playerid, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);

	PB_bBarShownForPlayer[barid][playerid] = false;
	return 1;
}

/*
native ShowProgressBarForAll(Bar:barid);
*/
stock ShowProgressBarForAll(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	TextDrawShowForAll(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawShowForAll(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawShowForAll(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
	    if (IsPlayerConnected(i))
	    {
			PB_bBarShownForPlayer[barid][i] = true;
		}
	}
	return 1;
}

/*
native HideProgressBarForAll(Bar:barid);
*/
stock HideProgressBarForAll(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	TextDrawHideForAll(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawHideForAll(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawHideForAll(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
	    if (IsPlayerConnected(i))
	    {
			PB_bBarShownForPlayer[barid][i] = false;
		}
	}
	return 1;
}
/*
native GetProgressBarDirection(Bar:barid);
*/
stock GetProgressBarDirection(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	return PB_eBar[barid][PB_e_BAR_DIRECTION];
}

/*
native GetProgressBarPos(Bar:barid, &Float:x, &Float:y);
*/
stock GetProgressBarPos(Bar:barid, &Float:x, &Float:y)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	x = PB_eBar[barid][PB_e_BAR_X];
	y = PB_eBar[barid][PB_e_BAR_X];
	return 1;
}

/*
native Float:GetProgressBarHeight(Bar:barid);
*/
stock Float:GetProgressBarHeight(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0.0;
	}

	return PB_eBar[barid][PB_e_BAR_HEIGHT];
}

/*
native Float:GetProgressBarWidth(Bar:barid);
*/
stock Float:GetProgressBarWidth(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0.0;
	}

	return PB_eBar[barid][PB_e_BAR_WIDTH];
}

/*
native GetProgressBarBGColor(Bar:barid);
*/
stock GetProgressBarBGColor(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	return PB_eBar[barid][PB_e_BAR_BACKGROUND_COLOR];
}

/*
native GetProgressBarColor(Bar:barid);
*/
stock GetProgressBarColor(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	return PB_eBar[barid][PB_e_BAR_COLOR];
}

#define GetProgressBarColour \
    GetProgressBarColor

/*
native Float:GetProgressBarValue(Bar:barid);
*/
stock Float:GetProgressBarValue(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0.0;
	}

	return PB_eBar[barid][PB_e_BAR_VALUE];
}

/*
native Float:GetProgressBarMaxValue(Bar:barid);
*/
stock Float:GetProgressBarMaxValue(Bar:barid)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0.0;
	}

	return PB_eBar[barid][PB_e_BAR_MAX_VALUE];
}

/*
native SetProgressBarDirection(Bar:barid);
*/
stock SetProgressBarDirection(Bar:barid, direction)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

    PB_eBar[barid][PB_e_BAR_DIRECTION] = direction;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarBackground(barid);
	PB_CreateBarFill(barid);
	PB_CreateBarMain(barid);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native SetProgressBarPos(Bar:barid, Float:x, Float:y);
*/
stock SetProgressBarPos(Bar:barid, Float:x, Float:y)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	PB_eBar[barid][PB_e_BAR_X] = x;
	PB_eBar[barid][PB_e_BAR_X] = y;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarBackground(barid);
	PB_CreateBarFill(barid);
	PB_CreateBarMain(barid);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native SetProgressBarHeight(Bar:barid, Float:height);
*/
stock SetProgressBarHeight(Bar:barid, Float:height)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	PB_eBar[barid][PB_e_BAR_HEIGHT] = height;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarBackground(barid);
	PB_CreateBarFill(barid);
	PB_CreateBarMain(barid);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native SetBarWidth(Bar:barid, Float:width);
*/
stock SetProgressBarWidth(Bar:barid, Float:width)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	PB_eBar[barid][PB_e_BAR_WIDTH] = width;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarBackground(barid);
	PB_CreateBarFill(barid);
	PB_CreateBarMain(barid);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native SetProgressBarBGColor(Bar:barid, bgcolor);
*/
stock SetProgressBarBGColor(Bar:barid, bgcolor)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

    PB_eBar[barid][PB_e_BAR_BACKGROUND_COLOR] = bgcolor;

	TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND], bgcolor);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_BACKGROUND]);
		}
	}
	return 1;
}

/*
native SetProgressBarColor(Bar:barid, color);
*/
stock SetProgressBarColor(Bar:barid, color)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

    PB_eBar[barid][PB_e_BAR_COLOR] = color;

	TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_FILL], ((color & 0xFFFFFF00) | (0x66 & ((color & 0x000000FF) / 2))));
	TextDrawBoxColor(PB_txtBar[barid][BAR_TEXTDRAW_MAIN], color);

	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_FILL]);
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

#define SetProgressBarColour \
    SetProgressBarColor

/*
native SetProgressBarValue(Bar:barid, Float:value);
*/
stock SetProgressBarValue(Bar:barid, Float:value)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	PB_eBar[barid][PB_e_BAR_VALUE] = value;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarMain(barid);
	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native SetProgressBarMaxValue(Bar:barid, Float:maxvalue);
*/
stock SetProgressBarMaxValue(Bar:barid, Float:maxvalue)
{
	if (!IsValidProgressBar(barid))
	{
	    return 0;
	}

	PB_eBar[barid][PB_e_BAR_MAX_VALUE] = maxvalue;

	TextDrawDestroy(PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
	PB_CreateBarMain(barid);
	for (new i, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if (IsPlayerConnected(i) && PB_bBarShownForPlayer[barid][i])
		{
			TextDrawShowForPlayer(i, PB_txtBar[barid][BAR_TEXTDRAW_MAIN]);
		}
	}
	return 1;
}

/*
native CountProgressBars();
*/
stock CountProgressBars()
{
	return PB_iBarCount;
}
