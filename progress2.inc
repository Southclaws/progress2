/*==============================================================================


	Progress Bar 2.0.2

		A SA:MP UI library for rendering progress bars used to visualise all
		manner of data from health to a countdown timer.

		Library originally written by Flávio Toribio: flavio_toribio@hotmail.com
		Now maintained by Southclaw in version 2+ with new features.


==============================================================================*/


#if defined _progress2_included
	#endinput
#endif

#if !defined _samp_included
	#tryinclude <a_samp>
	#if !defined _samp_included
		#error could not locate a_samp.inc file, please check your server includes
	#endif
#endif

#if !defined PB_DEBUG
	#define PB_DEBUG (false)
#endif


#define _progress2_included
#define _progress2_version 0x201

#define MAX_PLAYER_BARS             (MAX_PLAYER_TEXT_DRAWS / 3)
#define INVALID_PLAYER_BAR_VALUE    (Float:0xFFFFFFFF)
#define INVALID_PLAYER_BAR_ID       (PlayerBar:-1)

#if PB_DEBUG == true
	#define pb_debug(%0); printf(%0);
#else
	#define pb_debug(%0);
#endif


/*==============================================================================

	Setup

==============================================================================*/


enum
{
	BAR_DIRECTION_RIGHT,
	BAR_DIRECTION_LEFT,
	BAR_DIRECTION_UP,
	BAR_DIRECTION_DOWN
}

enum E_BAR_DATA
{
Float:		pbar_posX,
Float:		pbar_posY,
Float:		pbar_width,
Float:		pbar_height,
			pbar_colour,
Float:		pbar_maxValue,
Float:		pbar_progressValue,
			pbar_direction
}

enum E_BAR_TEXT_DRAW
{
PlayerText:	pbar_back,
PlayerText:	pbar_fill,
PlayerText:	pbar_main
}


static
			pbar_Data[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_DATA],
			pbar_TextDraw[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_TEXT_DRAW];

#if defined _Y_ITERATE_LOCAL_VERSION
	new Iterator:pbar_Index[MAX_PLAYERS]<MAX_PLAYER_BARS>;
#else
	static bool:pbar_Valid[MAX_PLAYERS][MAX_PLAYER_BARS];
#endif


forward PlayerBar:CreatePlayerProgressBar(playerid, Float:x, Float:y, Float:width=55.5, Float:height=3.2, colour = 0xFF1C1CFF, Float:max=100.0, direction=BAR_DIRECTION_RIGHT);
forward Float:GetPlayerProgressBarValue(playerid, PlayerBar:barid);


/*==============================================================================

	Utils

==============================================================================*/


stock Float:pb_percent(Float:x, Float:widthorheight, Float:max, Float:value, direction)
{
	new Float:result;

	switch(direction)
	{
		case BAR_DIRECTION_RIGHT:
			result = ((x - 3.0) + (((((x - 2.0) + widthorheight) - x) / max) * value));

		case BAR_DIRECTION_LEFT:
			result = ((x - 1.0) - (((((x + 2.0) - widthorheight) - x) / max) * -value)) - 4.0;

		case BAR_DIRECTION_UP:
			result = -((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) + 0.55);

		case BAR_DIRECTION_DOWN:
			result = ((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) - 0.55);
	}

	return result;
}

stock _bar_NewID(playerid)
{
#if defined _Y_ITERATE_LOCAL_VERSION
	return Iter_Free(pbar_Index[playerid]);
#else
	new i;

	for(i = 0; i < MAX_PLAYER_BARS; i++)
	{
		if(!pbar_Valid[playerid][i])
			break;
	}

	return (i == MAX_PLAYER_BARS) ? -1 : i;
#endif
}


/*==============================================================================

	Core

==============================================================================*/


stock PlayerBar:CreatePlayerProgressBar(playerid, Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, colour = 0xFF1C1CFF, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT)
{
	new barid = _bar_NewID(playerid);

	if(barid == -1)
	{
		print("[CreatePlayerProgressBar] ERROR: MAX_PLAYER_BARS limit reached.");
		return INVALID_PLAYER_BAR_ID;
	}

	pb_debug("[CreatePlayerProgressBar] Creating progress bar %d at %f,%f width:%f height:%f max:%f direction:%d", barid, x, y, width, height, max, direction);

	switch(direction)
	{
		case BAR_DIRECTION_RIGHT:
		{
			pbar_TextDraw[playerid][barid][pbar_back] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_back], x + width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1.0, height / 10);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));

			pbar_TextDraw[playerid][barid][pbar_fill] = CreatePlayerTextDraw(playerid, x + 1.2, y + 2.15, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], x + width - 5.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1.0, height / 10 - 0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));

			pbar_TextDraw[playerid][barid][pbar_main] = CreatePlayerTextDraw(playerid, x + 1.2, y + 2.15, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_main], pb_percent(x, width, max, 0.0, direction), 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_main], 1.0, height / 10 - 0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);
		}

		case BAR_DIRECTION_LEFT:
		{
			pbar_TextDraw[playerid][barid][pbar_back] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_back], x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1.0, height / 10);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));

			pbar_TextDraw[playerid][barid][pbar_fill] = CreatePlayerTextDraw(playerid, x - 1.2, y + 2.15, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1.0, height / 10 - 0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));

			pbar_TextDraw[playerid][barid][pbar_main] = CreatePlayerTextDraw(playerid, x - 1.2, y + 2.15, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_main], pb_percent(x, width, max, 0.0, direction), 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_main], 1.0, height / 10 - 0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);
		}

		case BAR_DIRECTION_UP:
		{
			pbar_TextDraw[playerid][barid][pbar_back] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_back], x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1.0, -((height / 10) * 1.02) -0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));

			pbar_TextDraw[playerid][barid][pbar_fill] = CreatePlayerTextDraw(playerid, x - 1.2, y - 1.0, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1.0, -(height / 10.0) * 1.02);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));

			pbar_TextDraw[playerid][barid][pbar_main] = CreatePlayerTextDraw(playerid, x - 1.2, y - 1.0, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_main], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_main], x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_main], 0.0, -0.55);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);
		}

		case BAR_DIRECTION_DOWN:
		{
			pbar_TextDraw[playerid][barid][pbar_back] = CreatePlayerTextDraw(playerid, x, y, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_back], x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_back], 1.0, ((height / 10)) -0.35);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));

			pbar_TextDraw[playerid][barid][pbar_fill] = CreatePlayerTextDraw(playerid, x - 1.2, y + 1.0, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_fill], 1.0, (height / 10.0) - 0.55);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));

			pbar_TextDraw[playerid][barid][pbar_main] = CreatePlayerTextDraw(playerid, x - 1.2, y + 1.0, "_");
			PlayerTextDrawUseBox		(playerid, pbar_TextDraw[playerid][barid][pbar_main], 1);
			PlayerTextDrawTextSize		(playerid, pbar_TextDraw[playerid][barid][pbar_main], x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, pbar_TextDraw[playerid][barid][pbar_main], 0.0, -0.55);
			PlayerTextDrawBoxColor		(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);
		}
	}


	pbar_Data[playerid][barid][pbar_posX] = x;
	pbar_Data[playerid][barid][pbar_posY] = y;
	pbar_Data[playerid][barid][pbar_width] = width;
	pbar_Data[playerid][barid][pbar_height] = height;
	pbar_Data[playerid][barid][pbar_colour] = colour;
	pbar_Data[playerid][barid][pbar_maxValue] = max;
	pbar_Data[playerid][barid][pbar_progressValue] = 0.0;
	pbar_Data[playerid][barid][pbar_direction] = direction;

#if defined _Y_ITERATE_LOCAL_VERSION
	Iter_Add(pbar_Index[playerid], barid);
#else
	pbar_Valid[playerid][barid] = true;
#endif

	return PlayerBar:barid;
}

stock DestroyPlayerProgressBar(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][_:barid][pbar_back]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][_:barid][pbar_fill]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][_:barid][pbar_main]);

	pbar_Data[playerid][_:barid][pbar_posX] = 0.0;
	pbar_Data[playerid][_:barid][pbar_posY] = 0.0;
	pbar_Data[playerid][_:barid][pbar_width] = 0.0;
	pbar_Data[playerid][_:barid][pbar_height] = 0.0;
	pbar_Data[playerid][_:barid][pbar_maxValue] = 0.0;
	pbar_Data[playerid][_:barid][pbar_progressValue] = 0.0;
	pbar_Data[playerid][_:barid][pbar_colour] = 0;

#if defined _Y_ITERATE_LOCAL_VERSION
	Iter_Remove(pbar_Index[playerid], _:barid);
#else
	pbar_Valid[playerid][_:barid] = false;
#endif

	return 1;
}

stock ShowPlayerProgressBar(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][_:barid][pbar_back]);
	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][_:barid][pbar_fill]);
	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][_:barid][pbar_main]);

	return 1;
}

stock HidePlayerProgressBar(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][_:barid][pbar_back]);
	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][_:barid][pbar_fill]);
	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][_:barid][pbar_main]);

	return 1;
}


/*==============================================================================

	Hooks and Internal

==============================================================================*/


#if defined _Y_ITERATE_LOCAL_VERSION

	#if defined FILTERSCRIPT

		public OnFilterScriptInit()
		{
			Iter_Init(pbar_Index);

			#if defined ppb_OnFilterScriptInit
				return ppb_OnFilterScriptInit();
			#else
				return 1;
			#endif
		}
		#if defined _ALS_OnFilterScriptInit
			#undef OnFilterScriptInit
		#else
			#define _ALS_OnFilterScriptInit
		#endif
		 
		#define OnFilterScriptInit ppb_OnFilterScriptInit
		#if defined ppb_OnFilterScriptInit
			forward ppb_OnFilterScriptInit();
		#endif

	#else

		public OnGameModeInit()
		{
			Iter_Init(pbar_Index);

			#if defined ppb_OnGameModeInit
				return ppb_OnGameModeInit();
			#else
				return 1;
			#endif
		}
		#if defined _ALS_OnGameModeInit
			#undef OnGameModeInit
		#else
			#define _ALS_OnGameModeInit
		#endif
		 
		#define OnGameModeInit ppb_OnGameModeInit
		#if defined ppb_OnGameModeInit
			forward ppb_OnGameModeInit();
		#endif

	#endif

#endif

public OnPlayerDisconnect(playerid, reason)
{
	for(new i; i < MAX_PLAYER_BARS; i++)
		DestroyPlayerProgressBar(playerid, PlayerBar:i);

	#if defined ppb_OnPlayerDisconnect
		return ppb_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
 
#define OnPlayerDisconnect ppb_OnPlayerDisconnect
#if defined ppb_OnPlayerDisconnect
	forward ppb_OnPlayerDisconnect(playerid, reason);
#endif


/*==============================================================================

	Interface

==============================================================================*/


stock IsValidPlayerProgressBar(playerid, PlayerBar:barid)
{
#if defined _Y_ITERATE_LOCAL_VERSION
	return Iter_Contains(pbar_Index[playerid], _:barid);
#else
	if(!(PlayerBar:0 <= barid < PlayerBar:MAX_PLAYER_BARS))
		return false;

	return pbar_Valid[playerid][_:barid];
#endif
}

// pbar_posX
// pbar_posY
stock GetPlayerProgressBarPos(playerid, PlayerBar:barid, &Float:x, &Float:y)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	x = pbar_Data[playerid][_:barid][pbar_posX];
	y = pbar_Data[playerid][_:barid][pbar_posY];

	return 1;
}

/*stock SetPlayerProgressBarPos(playerid, PlayerBar:barid, Float:x, Float:y)
{
	if(!IsPlayerConnected(playerid))
		return false;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return false;

	pbar_Data[playerid][barid][pbar_posX] = x;
	pbar_Data[playerid][barid][pbar_posY] = y;

	return true;
}*/

// pbar_width
stock Float:GetPlayerProgressBarWidth(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return INVALID_PLAYER_BAR_VALUE;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return INVALID_PLAYER_BAR_VALUE;

	return pbar_Data[playerid][_:barid][pbar_width];
}

/*stock SetPlayerProgressBarWidth(playerid, PlayerBar:barid, Float:width)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	pbar_Data[playerid][_:barid][pbar_width] = width;

	// TODO: Re-render

	return 1;
}*/

// pbar_height
stock Float:GetPlayerProgressBarHeight(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return INVALID_PLAYER_BAR_VALUE;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return INVALID_PLAYER_BAR_VALUE;

	return pbar_Data[playerid][_:barid][pbar_height];
}

/*stock SetPlayerProgressBarHeight(playerid, PlayerBar:barid, Float:height)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	pbar_Data[playerid][_:barid][pbar_height] = height;

	// TODO: Re-render

	return 1;
}*/

// pbar_colour
stock GetPlayerProgressBarColour(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	return pbar_Data[playerid][_:barid][pbar_colour];
}

stock SetPlayerProgressBarColour(playerid, PlayerBar:barid, colour)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	pbar_Data[playerid][_:barid][pbar_colour] = colour;
	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][_:barid][pbar_back], 0x00000000 | (colour & 0x000000FF));

	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][_:barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));

	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][_:barid][pbar_main], colour);

	return 1;
}

// pbar_maxValue
stock Float:GetPlayerProgressBarMaxValue(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return INVALID_PLAYER_BAR_VALUE;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return INVALID_PLAYER_BAR_VALUE;

	return pbar_Data[playerid][_:barid][pbar_maxValue];
}

stock SetPlayerProgressBarMaxValue(playerid, PlayerBar:barid, Float:max)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	pbar_Data[playerid][_:barid][pbar_maxValue] = max;
	SetPlayerProgressBarValue(playerid, barid, pbar_Data[playerid][_:barid][pbar_progressValue]);

	return 1;
}

// pbar_progressValue
stock SetPlayerProgressBarValue(playerid, PlayerBar:barid, Float:value)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	value = (value < 0.0) ? (0.0) : (value > pbar_Data[playerid][_:barid][pbar_maxValue]) ? (pbar_Data[playerid][_:barid][pbar_maxValue]) : (value);

	PlayerTextDrawUseBox(playerid, pbar_TextDraw[playerid][_:barid][pbar_main], value > 0.0);

	pbar_Data[playerid][_:barid][pbar_progressValue] = value;

	switch(pbar_Data[playerid][_:barid][pbar_direction])
	{
		case BAR_DIRECTION_RIGHT, BAR_DIRECTION_LEFT:
		{

			PlayerTextDrawTextSize(playerid, pbar_TextDraw[playerid][_:barid][pbar_main],
				pb_percent(
					pbar_Data[playerid][_:barid][pbar_posX],
					pbar_Data[playerid][_:barid][pbar_width],
					pbar_Data[playerid][_:barid][pbar_maxValue],
					value,
					pbar_Data[playerid][_:barid][pbar_direction]), 0.0);
		}

		case BAR_DIRECTION_UP, BAR_DIRECTION_DOWN:
		{
			PlayerTextDrawLetterSize(playerid, pbar_TextDraw[playerid][_:barid][pbar_main], 1.0,
				pb_percent(
					pbar_Data[playerid][_:barid][pbar_posX],
					pbar_Data[playerid][_:barid][pbar_height],
					pbar_Data[playerid][_:barid][pbar_maxValue],
					value,
					pbar_Data[playerid][_:barid][pbar_direction]));
		}
	}

	ShowPlayerProgressBar(playerid, barid);

	return 1;
}

stock Float:GetPlayerProgressBarValue(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return INVALID_PLAYER_BAR_VALUE;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return INVALID_PLAYER_BAR_VALUE;

	return pbar_Data[playerid][_:barid][pbar_progressValue];
}

// pbar_direction
stock GetPlayerProgressBarDirection(playerid, PlayerBar:barid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	return pbar_Data[playerid][_:barid][pbar_direction];
}
/*
stock SetPlayerProgressBarDirection(playerid, PlayerBar:barid, direction)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(!IsValidPlayerProgressBar(playerid, barid))
		return 0;

	pbar_Data[playerid][_:barid][pbar_direction] = direction;

	// TODO: Re-render

	return 1;
}
*/
