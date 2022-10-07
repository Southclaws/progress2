// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_progress2
	#undef _inc_progress2
#endif
// custom include-guard to ensure we don't duplicate
#if defined _progress2_included
	#endinput
#endif
#define _progress2_included

#include <a_samp>
#tryinclude <logger>
#tryinclude <YSI_Data\y_iterate>

#tryinclude <YSI_Coding\y_hooks>


#define MAX_PLAYER_BARS					(PlayerBar:(_:MAX_PLAYER_TEXT_DRAWS / 3))
#define INVALID_PLAYER_BAR_VALUE		(Float:0xFFFFFFFF)
#define INVALID_PLAYER_BAR_ID			(PlayerBar:-1)

enum {
	BAR_DIRECTION_RIGHT,
	BAR_DIRECTION_LEFT,
	BAR_DIRECTION_UP,
	BAR_DIRECTION_DOWN
}

enum E_BAR_DATA {
	bool:is_created,
	bool:pbar_show,
	Float:pbar_posX,
	Float:pbar_posY,
	Float:pbar_width,
	Float:pbar_height,
	pbar_colour,
	Float:pbar_maxValue,
	Float:pbar_progressValue,
	pbar_direction
}

enum E_BAR_TEXT_DRAW {
	PlayerText:pbar_back,
	PlayerText:pbar_fill,
	PlayerText:pbar_main
}

static pbar_TextDraw[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_TEXT_DRAW];

new
	#if(defined _INC_y_iterate)
		Iterator:pbar_Index[MAX_PLAYERS]<PlayerBar:(_:MAX_PLAYER_BARS)>,
	#endif
	pbar_Data[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_DATA]
;

//	Reset info of player progress bar.
stock PlayerBarUI_ResetPlayerItem(const playerid, const PlayerBar:barid) {
	pbar_TextDraw[playerid][barid][pbar_back] = PlayerText:INVALID_TEXT_DRAW;
	pbar_TextDraw[playerid][barid][pbar_fill] = PlayerText:INVALID_TEXT_DRAW;
	pbar_TextDraw[playerid][barid][pbar_main] = PlayerText:INVALID_TEXT_DRAW;
	pbar_Data[playerid][barid][is_created] = false;
}

//	Reset info of all bars for one player.
stock PlayerBarUI_ResetPlayer(const playerid) {
	for(new PlayerBar:barid = PlayerBar:0; barid < MAX_PLAYER_BARS; ++barid) {
		PlayerBarUI_ResetPlayerItem(playerid, barid);
	}
}
//	Reset all bars for all players.
stock PlayerBarUI_ResetAll() {
	for(new playerid = 0; playerid < MAX_PLAYERS; ++playerid) {
		PlayerBarUI_ResetPlayer(playerid);
	}
}
//	Returns free player progress bar.
stock PlayerBar:PlayerBarUI_FindFree(const playerid) {
	for(new PlayerBar:barid = PlayerBar:0; barid < MAX_PLAYER_BARS; ++barid) {
		if( !pbar_Data[playerid][barid][is_created] ) {
			return barid;
		}
	}
	return INVALID_PLAYER_BAR_ID;
}

forward PlayerBar:CreatePlayerProgressBar(
	const playerid,
	const Float:x, const Float:y,
	const Float:width = 55.5, const Float:height = 3.2,
	const colour = 0xFF1C1CFF,
	const Float:max = 100.0,
	const direction = BAR_DIRECTION_RIGHT
);
forward Float:GetPlayerProgressBarValue(const playerid, const PlayerBar:barid);
forward Float:_bar_percent(const Float:x, const Float:widthorheight, const Float:max, const Float:value, const direction);

stock PlayerBar:CreatePlayerProgressBar(
	const playerid,
	const Float:x, const Float:y,
	const Float:width = 55.5, const Float:height = 3.2,
	const colour = 0xFF1C1CFF,
	const Float:max = 100.0,
	const direction = BAR_DIRECTION_RIGHT
) {
	if( !IsPlayerConnected(playerid) ) {
		#if(defined _logger_included)
		Logger_Err("attempt to create player progress bar for invalid player",
			Logger_I("playerid", playerid));
		#endif
		return INVALID_PLAYER_BAR_ID;
	}

	#if(defined _INC_y_iterate)
	new PlayerBar:barid = PlayerBar:Iter_Free(pbar_Index[playerid]);

	if( barid == PlayerBar:ITER_NONE ) {
		#if(defined _logger_included)
		Logger_Err("MAX_PLAYER_BARS limit reached.");
		#endif
		return INVALID_PLAYER_BAR_ID;
	}
	#else
	new PlayerBar:barid = PlayerBarUI_FindFree(playerid);
	if( barid == INVALID_PLAYER_BAR_ID ) {
		#if(defined _logger_included)
		Logger_Err("MAX_PLAYER_BARS limit reached.");
		#endif
		return INVALID_PLAYER_BAR_ID;
	}
	#endif

	pbar_TextDraw[playerid][barid][pbar_back] = PlayerText:INVALID_TEXT_DRAW;
	pbar_TextDraw[playerid][barid][pbar_fill] = PlayerText:INVALID_TEXT_DRAW;
	pbar_TextDraw[playerid][barid][pbar_main] = PlayerText:INVALID_TEXT_DRAW;
	pbar_Data[playerid][barid][is_created] = true;
	pbar_Data[playerid][barid][pbar_show] = false;
	pbar_Data[playerid][barid][pbar_posX] = x;
	pbar_Data[playerid][barid][pbar_posY] = y;
	pbar_Data[playerid][barid][pbar_width] = width;
	pbar_Data[playerid][barid][pbar_height] = height;
	pbar_Data[playerid][barid][pbar_colour] = colour;
	pbar_Data[playerid][barid][pbar_maxValue] = max;
	pbar_Data[playerid][barid][pbar_progressValue] = 0.0;
	pbar_Data[playerid][barid][pbar_direction] = direction;

	#if(defined _INC_y_iterate)
	Iter_Add(pbar_Index[playerid], barid);
	#endif

	_progress2_renderBar(playerid, barid);

	return barid;
}

stock DestroyPlayerProgressBar(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_back]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_fill]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_main]);

	pbar_Data[playerid][barid][is_created] = false;
	#if(defined _INC_y_iterate)
	Iter_Remove(pbar_Index[playerid], barid);
	#endif

	return 1;
}

stock bool:IsPlayerProgressBarVisible(const playerid, const PlayerBar:barid) {
	return (IsValidPlayerProgressBar(playerid, barid) && pbar_Data[playerid][barid][pbar_show]);
}
stock ShowPlayerProgressBar(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_show] = true;

	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][barid][pbar_back]);
	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][barid][pbar_fill]);
	PlayerTextDrawShow(playerid, pbar_TextDraw[playerid][barid][pbar_main]);

	return 1;
}

stock HidePlayerProgressBar(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_show] = false;

	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][barid][pbar_back]);
	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][barid][pbar_fill]);
	PlayerTextDrawHide(playerid, pbar_TextDraw[playerid][barid][pbar_main]);

	return 1;
}

stock IsValidPlayerProgressBar(const playerid, const PlayerBar:barid) {
	#if(defined _INC_y_iterate)
		return Iter_Contains(pbar_Index[playerid], barid);
	#else
		if( INVALID_PLAYER_BAR_ID < barid && barid < MAX_PLAYER_BARS ) {
			return pbar_Data[playerid][barid][is_created];
		}
		return false;
	#endif
}

// pbar_posX
// pbar_posY
stock GetPlayerProgressBarPos(const playerid, const PlayerBar:barid, &Float:x, &Float:y) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	x = pbar_Data[playerid][barid][pbar_posX];
	y = pbar_Data[playerid][barid][pbar_posY];

	return 1;
}

stock SetPlayerProgressBarPos(const playerid, const PlayerBar:barid, const Float:x, const Float:y) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return false;
	}

	pbar_Data[playerid][barid][pbar_posX] = x;
	pbar_Data[playerid][barid][pbar_posY] = y;

	_progress2_renderBar(playerid, barid);

	return true;
}

// pbar_width
stock Float:GetPlayerProgressBarWidth(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return INVALID_PLAYER_BAR_VALUE;
	}

	return pbar_Data[playerid][barid][pbar_width];
}

stock SetPlayerProgressBarWidth(const playerid, const PlayerBar:barid, const Float:width) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_width] = width;

	_progress2_renderBar(playerid, barid);

	return 1;
}

// pbar_height
stock Float:GetPlayerProgressBarHeight(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return INVALID_PLAYER_BAR_VALUE;
	}

	return pbar_Data[playerid][barid][pbar_height];
}

stock SetPlayerProgressBarHeight(const playerid, const PlayerBar:barid, const Float:height) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_height] = height;

	_progress2_renderBar(playerid, barid);

	return 1;
}

// pbar_colour
stock GetPlayerProgressBarColour(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	return pbar_Data[playerid][barid][pbar_colour];
}

stock SetPlayerProgressBarColour(const playerid, const PlayerBar:barid, const colour) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_colour] = colour;

	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));
	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));
	PlayerTextDrawBoxColor(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);

	return 1;
}

// pbar_maxValue
stock Float:GetPlayerProgressBarMaxValue(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return INVALID_PLAYER_BAR_VALUE;
	}

	return pbar_Data[playerid][barid][pbar_maxValue];
}

stock SetPlayerProgressBarMaxValue(const playerid, const PlayerBar:barid, const Float:max) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_maxValue] = max;
	SetPlayerProgressBarValue(playerid, barid, pbar_Data[playerid][barid][pbar_progressValue]);

	return 1;
}

// pbar_progressValue
stock SetPlayerProgressBarValue(const playerid, const PlayerBar:barid, Float:value) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	new
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		direction = pbar_Data[playerid][barid][pbar_direction],
		Float:boundry_size,
		Float:adopted_size,
		PlayerText:ptr_main = pbar_TextDraw[playerid][barid][pbar_main]
	;
	if( value < 0.0 ) {
		value = 0.0;
	} else if( value > max_value ) {
		value = max_value;
	}
	if( direction == BAR_DIRECTION_RIGHT || direction == BAR_DIRECTION_LEFT ) {
		boundry_size = pbar_Data[playerid][barid][pbar_width];
	} else {
		boundry_size = pbar_Data[playerid][barid][pbar_height];
	}
	adopted_size = _bar_percent(
		pbar_Data[playerid][barid][pbar_posX],
		boundry_size,
		max_value,
		value,
		direction
	);

	PlayerTextDrawUseBox(playerid, ptr_main, value > 0.0);

	pbar_Data[playerid][barid][pbar_progressValue] = value;

	switch(direction) {
		case BAR_DIRECTION_RIGHT, BAR_DIRECTION_LEFT: {
			PlayerTextDrawTextSize(playerid, ptr_main, adopted_size, 0.0);
		}
		case BAR_DIRECTION_UP, BAR_DIRECTION_DOWN: {
			PlayerTextDrawLetterSize(playerid, ptr_main, 1.0, adopted_size);
		}
	}

	if( pbar_Data[playerid][barid][pbar_show] ) {
		ShowPlayerProgressBar(playerid, barid);
	}

	return 1;
}

stock Float:GetPlayerProgressBarValue(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return INVALID_PLAYER_BAR_VALUE;
	}

	return pbar_Data[playerid][barid][pbar_progressValue];
}

// pbar_direction
stock GetPlayerProgressBarDirection(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	return pbar_Data[playerid][barid][pbar_direction];
}

stock SetPlayerProgressBarDirection(const playerid, const PlayerBar:barid, const direction) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_direction] = direction;
	_progress2_renderBar(playerid, barid);

	return 1;
}

/*
	Internal
*/

_progress2_renderBar(const playerid, const PlayerBar:barid) {
	if(
		!IsPlayerConnected(playerid) ||
		!IsValidPlayerProgressBar(playerid, barid)
	) {
		return false;
	}

	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_back]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_fill]);
	PlayerTextDrawDestroy(playerid, pbar_TextDraw[playerid][barid][pbar_main]);
	new
		Float:pos_x,
		Float:pos_y,
		Float:width = pbar_Data[playerid][barid][pbar_width],
		Float:height = pbar_Data[playerid][barid][pbar_height],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		Float:cur_value = pbar_Data[playerid][barid][pbar_progressValue],
		direction = pbar_Data[playerid][barid][pbar_direction],
		color = pbar_Data[playerid][barid][pbar_colour],
		PlayerText:ptd_back,
		PlayerText:ptd_fill,
		PlayerText:ptd_main
	;
	GetPlayerProgressBarPos(playerid, barid, pos_x, pos_y);
	ptd_back = CreatePlayerTextDraw(playerid, pos_x, pos_y, "_");
	switch(direction) {
		case BAR_DIRECTION_RIGHT: {
			PlayerTextDrawTextSize		(playerid, ptd_back, pos_x + width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, height / 10.0);

			ptd_fill = CreatePlayerTextDraw(playerid, pos_x + 1.2, pos_y + 2.15, "_");
			PlayerTextDrawTextSize		(playerid, ptd_fill, pos_x + width - 5.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, height / 10.0 - 0.35);

			ptd_main = CreatePlayerTextDraw(playerid, pos_x + 1.2, pos_y + 2.15, "_");
			PlayerTextDrawTextSize		(playerid, ptd_main, _bar_percent(pos_x, width, max_value, cur_value, direction), 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_main, 1.0, height / 10.0 - 0.35);
		}
		case BAR_DIRECTION_LEFT: {
			PlayerTextDrawTextSize		(playerid, ptd_back, pos_x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, height / 10.0);

			ptd_fill = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y + 2.15, "_");
			PlayerTextDrawTextSize		(playerid, ptd_fill, pos_x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, height / 10.0 - 0.35);

			ptd_main = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y + 2.15, "_");
			PlayerTextDrawTextSize		(playerid, ptd_main, _bar_percent(pos_x, width, max_value, cur_value, direction), 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_main, 1.0, height / 10.0 - 0.35);
		}
		case BAR_DIRECTION_UP: {
			PlayerTextDrawTextSize		(playerid, ptd_back, pos_x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, -((height / 10.0) * 1.02) -0.35);

			ptd_fill = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y - 1.0, "_");
			PlayerTextDrawTextSize		(playerid, ptd_fill, pos_x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, -(height / 10.0) * 1.02);

			ptd_main = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y - 1.0, "_");
			PlayerTextDrawTextSize		(playerid, ptd_main, pos_x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_main, 1.0, _bar_percent(pos_x, height, max_value, cur_value, direction));
		}
		case BAR_DIRECTION_DOWN: {
			PlayerTextDrawTextSize		(playerid, ptd_back, pos_x - width - 4.0, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, ((height / 10.0)) - 0.35);

			ptd_fill = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y + 1.0, "_");
			PlayerTextDrawTextSize		(playerid, ptd_fill, pos_x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, (height / 10.0) - 0.55);

			ptd_main = CreatePlayerTextDraw(playerid, pos_x - 1.2, pos_y + 1.0, "_");
			PlayerTextDrawTextSize		(playerid, ptd_main, pos_x - width - 2.5, 0.0);
			PlayerTextDrawLetterSize	(playerid, ptd_main, 1.0, _bar_percent(pos_x, height, max_value, cur_value, direction));
		}
	}
	pbar_TextDraw[playerid][barid][pbar_back] = ptd_back;
	pbar_TextDraw[playerid][barid][pbar_fill] = ptd_fill;
	pbar_TextDraw[playerid][barid][pbar_main] = ptd_main;
	PlayerTextDrawUseBox		(playerid, ptd_back, 1);
	PlayerTextDrawUseBox		(playerid, ptd_fill, 1);
	PlayerTextDrawUseBox		(playerid, ptd_main, 1);
	SetPlayerProgressBarColour(playerid, barid, color);

	if( pbar_Data[playerid][barid][pbar_show] ) {
		ShowPlayerProgressBar(playerid, barid);
	}

	return true;
}
#if(defined _INC_y_hooks)
hook OnScriptInit() {
	#if(defined _INC_y_iterate)
	Iter_Init(pbar_Index);
	#endif
}

hook OnPlayerDisconnect(playerid, reason) {
	#if(defined _INC_y_iterate)
	Iter_Clear(pbar_Index[playerid]);
	#endif
}

hook OnScriptExit() {
	for(new i = 0; i < MAX_PLAYERS; i++) {
		if( IsPlayerConnected(i) ) {
			DestroyAllPlayerProgressBars(i);
		}
	}
}
#endif

stock DestroyAllPlayerProgressBars(const playerid) {
	for(new PlayerBar:i = PlayerBar:0; i < MAX_PLAYER_BARS; i++) {
		DestroyPlayerProgressBar(playerid, i);
	}

	return 1;
}

stock Float:_bar_percent(const Float:x, const Float:widthorheight, const Float:max, const Float:value, const direction) {
	new Float:result;

	switch(direction) {
		case BAR_DIRECTION_RIGHT: {
			result = ((x - 3.0) + (((((x - 2.0) + widthorheight) - x) / max) * value));
		}
		case BAR_DIRECTION_LEFT: {
			result = ((x - 1.0) - (((((x + 2.0) - widthorheight) - x) / max) * -value)) - 4.0;
		}
		case BAR_DIRECTION_UP: {
			result = -((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) + 0.55);
		}
		case BAR_DIRECTION_DOWN: {
			result = ((((((widthorheight / 10.0) - 0.45) * 1.02) / max) * value) - 0.55);
		}
	}

	return result;
}
