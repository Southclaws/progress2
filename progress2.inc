/*
	Note 1: Background & filter TextDraws have normalized corner positions.
	Note 2: Bar TextDraw have corners that associated with min & max values (unnormalized).
	Note 3: _bar_percent is now DEPRECATED - do not ever use it.
*/
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

#tryinclude <open.mp>

#if !defined PlayerTextDrawBoxColour
	#include <a_samp>
	#define PlayerTextDrawBoxColour PlayerTextDrawBoxColor
#endif

#tryinclude <logger>
#tryinclude <YSI_Data\y_iterate>

#tryinclude <YSI_Coding\y_hooks>


#define MAX_PLAYER_BARS					(PlayerBar:(_:MAX_PLAYER_TEXT_DRAWS / 3))
#define INVALID_PLAYER_BAR_VALUE		(Float:0xFFFFFFFF)
#define INVALID_PLAYER_BAR_ID			(PlayerBar:-1)

enum progressbar_direction {
	BAR_DIRECTION_RIGHT,
	BAR_DIRECTION_LEFT,
	BAR_DIRECTION_HORIZONTAL_FROM_0,
	BAR_DIRECTION_UP,
	BAR_DIRECTION_DOWN,
	BAR_DIRECTION_VERTICAL_FROM_0,
};

enum E_BAR_DATA {
	bool:is_created,
	bool:pbar_show,
	Float:pbar_posX,
	Float:pbar_posY,
	Float:pbar_width,
	Float:pbar_height,
	pbar_colour,
	Float:pbar_minValue,
	Float:pbar_maxValue,
	Float:pbar_progressValue,
	Float:E_PADDING_X,
	Float:E_PADDING_Y,
	progressbar_direction:pbar_direction
}

static const Float:direction_size_mult[] = {
	1.0,	// BAR_DIRECTION_RIGHT
	-1.0,	// BAR_DIRECTION_LEFT
	1.0,	// BAR_DIRECTION_HORIZONTAL_FROM_0
	-1.0,	// BAR_DIRECTION_UP
	1.0,	// BAR_DIRECTION_DOWN
	-1.0	// BAR_DIRECTION_VERTICAL_FROM_0
};

enum E_BAR_TEXT_DRAW {
	PlayerText:pbar_back,
	PlayerText:pbar_fill,
	PlayerText:pbar_main
}

enum E_PROGRESSBAR_BOUNDRY {
	Float:E_PBAR_BACKGROUND_POS_X,
	Float:E_PBAR_BACKGROUND_POS_Y,
	Float:E_PBAR_BACKGROUND_RIGHT,
	Float:E_PBAR_BACKGROUND_HEIGHT,
	Float:E_PBAR_FILLER_POS_X,
	Float:E_PBAR_FILLER_POS_Y,
	Float:E_PBAR_FILLER_RIGHT,
	Float:E_PBAR_FILLER_HEIGHT,
	Float:E_PBAR_VALUE_POS_X,
	Float:E_PBAR_VALUE_POS_Y,
	Float:E_PBAR_VALUE_RIGHT,
	Float:E_PBAR_VALUE_HEIGHT,
};

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
	const progressbar_direction:direction = BAR_DIRECTION_RIGHT
);
forward Float:GetPlayerProgressBarValue(const playerid, const PlayerBar:barid);

stock PlayerBar:CreatePlayerProgressBar(
	const playerid,
	const Float:x, const Float:y,
	const Float:width = 55.5, const Float:height = 3.2,
	const colour = 0xFF1C1CFF,
	const Float:max = 100.0,
	const progressbar_direction:direction = BAR_DIRECTION_RIGHT
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
	pbar_Data[playerid][barid][pbar_minValue] = 0.0;
	pbar_Data[playerid][barid][pbar_maxValue] = max;
	pbar_Data[playerid][barid][pbar_progressValue] = 0.0;
	pbar_Data[playerid][barid][pbar_direction] = direction;
	pbar_Data[playerid][barid][E_PADDING_X] = 1.2;
	pbar_Data[playerid][barid][E_PADDING_Y] = 1.0;

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

	PlayerTextDrawBoxColour(playerid, pbar_TextDraw[playerid][barid][pbar_back], 0x00000000 | (colour & 0x000000FF));
	PlayerTextDrawBoxColour(playerid, pbar_TextDraw[playerid][barid][pbar_fill], (colour & 0xFFFFFF00) | (0x66 & ((colour & 0x000000FF) / 2)));
	PlayerTextDrawBoxColour(playerid, pbar_TextDraw[playerid][barid][pbar_main], colour);

	return 1;
}

stock Float:GetPlayerProgressBarMinValue(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return INVALID_PLAYER_BAR_VALUE;
	}
	return pbar_Data[playerid][barid][pbar_minValue];
}

stock SetPlayerProgressBarMinValue(const playerid, const PlayerBar:barid, const Float:min_value) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}
	pbar_Data[playerid][barid][pbar_minValue] = min_value;
	SetPlayerProgressBarValue(playerid, barid, pbar_Data[playerid][barid][pbar_progressValue]);
	return 1;
}
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
		boundry[E_PROGRESSBAR_BOUNDRY],
		Float:min_value = pbar_Data[playerid][barid][pbar_minValue],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		color = pbar_Data[playerid][barid][pbar_colour],
		PlayerText:ptd_main = pbar_TextDraw[playerid][barid][pbar_main]
	;
	if( value < min_value ) {
		value = min_value;
	} else if( value > max_value ) {
		value = max_value;
	}
	pbar_Data[playerid][barid][pbar_progressValue] = value;
	PlayerBarUI_computeBoundry(playerid, barid, boundry);

	PlayerTextDrawDestroy(playerid, ptd_main);

	ptd_main = CreatePlayerTextDraw(
		playerid,
		boundry[E_PBAR_VALUE_POS_X],
		boundry[E_PBAR_VALUE_POS_Y],
		"_"
	);
	PlayerTextDrawTextSize(playerid, ptd_main, boundry[E_PBAR_VALUE_RIGHT], 0.0);
	PlayerTextDrawLetterSize(playerid, ptd_main, 1.0, boundry[E_PBAR_VALUE_HEIGHT]);
	PlayerTextDrawUseBox(playerid, ptd_main, (value > min_value));
	SetPlayerProgressBarColour(playerid, barid, color);
	pbar_TextDraw[playerid][barid][pbar_main] = ptd_main;

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
stock progressbar_direction:GetPlayerProgressBarDirection(const playerid, const PlayerBar:barid) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return progressbar_direction:BAR_DIRECTION_RIGHT;
	}

	return pbar_Data[playerid][barid][pbar_direction];
}

stock SetPlayerProgressBarDirection(const playerid, const PlayerBar:barid, const progressbar_direction:direction) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}

	pbar_Data[playerid][barid][pbar_direction] = direction;
	_progress2_renderBar(playerid, barid);

	return 1;
}

stock GetPlayerProgressBarPadding(const playerid, const PlayerBar:barid, &Float:padding_x, &Float:padding_y) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}
	padding_x = pbar_Data[playerid][barid][E_PADDING_X];
	padding_y = pbar_Data[playerid][barid][E_PADDING_Y];
	return 1;
}
stock SetPlayerProgressBarPadding(const playerid, const PlayerBar:barid, const Float:padding_x, const Float:padding_y) {
	if( !IsValidPlayerProgressBar(playerid, barid) ) {
		return 0;
	}
	pbar_Data[playerid][barid][E_PADDING_X] = padding_x;
	pbar_Data[playerid][barid][E_PADDING_Y] = padding_y;
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
		Float:padding_x, Float:padding_y,
		Float:width = pbar_Data[playerid][barid][pbar_width],
		Float:height = pbar_Data[playerid][barid][pbar_height],
		Float:min_value = pbar_Data[playerid][barid][pbar_minValue],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		Float:cur_value = pbar_Data[playerid][barid][pbar_progressValue],
		progressbar_direction:direction = pbar_Data[playerid][barid][pbar_direction],
		color = pbar_Data[playerid][barid][pbar_colour],
		Float:outer_pos_x2, Float:outer_pos_y2,	// 'back' pos (right bottom corner)
		Float:inner_pos_x1, Float:inner_pos_y1,	// 'fill' pos (left upper corner)
		Float:inner_pos_x2, Float:inner_pos_y2,	// 'fill' pos (right bottom corner)
		Float:inner_size_x, Float:inner_size_y,	// 'fill' size
		Float:value_pos_x1, Float:value_pos_y1,
		Float:value_pos_x2, Float:value_pos_y2,
		Float:value_size_y,
		Float:size_multiplier = direction_size_mult[direction],
		bool:is_vertical = (direction > BAR_DIRECTION_HORIZONTAL_FROM_0),
		Float:ratio_from,
		Float:ratio_to,
		PlayerText:ptd_back,
		PlayerText:ptd_fill,
		PlayerText:ptd_main
	;
	GetPlayerProgressBarPos(playerid, barid, pos_x, pos_y);
	GetPlayerProgressBarPadding(playerid, barid, padding_x, padding_y);
	//	Computing normalized sizes and corner positions in canvas pixels.
	outer_pos_x2 = pos_x + width;
	outer_pos_y2 = pos_y + height;
	inner_pos_x1 = pos_x + padding_x;
	inner_pos_x2 = outer_pos_x2 - padding_x;
	inner_pos_y1 = pos_y + padding_y;
	inner_pos_y2 = outer_pos_y2 - padding_y;
	inner_size_x = inner_pos_x2 - inner_pos_x1;
	inner_size_y = inner_pos_y2 - inner_pos_y1;
	bar_getRatios(direction, cur_value, min_value, max_value, ratio_from, ratio_to);
	ratio_from *= size_multiplier;
	ratio_to *= size_multiplier;
	if( is_vertical ) {
		value_pos_x1 = inner_pos_x1;
		value_pos_x2 = inner_pos_x2;
	} else {	// is horizontal.
		value_pos_y1 = inner_pos_y1;
		value_pos_y2 = inner_pos_y2;
	}
	switch(direction) {
		case BAR_DIRECTION_RIGHT:				{ value_pos_x1 = inner_pos_x1; }
		case BAR_DIRECTION_LEFT:				{ value_pos_x1 = inner_pos_x2; }
		case BAR_DIRECTION_HORIZONTAL_FROM_0:	{ value_pos_x1 = inner_pos_x1; }
		case BAR_DIRECTION_UP:					{ value_pos_y1 = inner_pos_y2; }
		case BAR_DIRECTION_DOWN:				{ value_pos_y1 = inner_pos_y1; }
		case BAR_DIRECTION_VERTICAL_FROM_0:		{ value_pos_y1 = inner_pos_y2; }
	}
	if( is_vertical ) {
		value_pos_y2 = value_pos_y1;
		value_pos_y1 += inner_size_y * ratio_from;
		value_pos_y2 += inner_size_y * ratio_to;
	} else {
		value_pos_x2 = value_pos_x1;
		value_pos_x1 += inner_size_x * ratio_from;
		value_pos_x2 += inner_size_x * ratio_to;
	}
	// normalising ranges.
	{
		new Float:temp_value;
		if( value_pos_x1 > value_pos_x2 ) {
			temp_value = value_pos_x2;
			value_pos_x2 = value_pos_x1;
			value_pos_x1 = temp_value;
		}
		if( value_pos_y1 > value_pos_y2 ) {
			temp_value = value_pos_y2;
			value_pos_y2 = value_pos_y1;
			value_pos_y1 = temp_value;
		}
	}
	new Float:correction_x = 1.25;
	value_pos_x1 += correction_x;
	value_pos_x2 -= correction_x;
	inner_pos_x1 += correction_x;
	inner_pos_x2 -= correction_x;

	value_size_y = value_pos_y2 - value_pos_y1;

	ptd_back = CreatePlayerTextDraw(playerid, pos_x, pos_y, "_");
	ptd_fill = CreatePlayerTextDraw(playerid, inner_pos_x1, inner_pos_y1, "_");
	ptd_main = CreatePlayerTextDraw(playerid, value_pos_x1, value_pos_y1, "_");
	PlayerTextDrawTextSize(playerid, ptd_main, value_pos_x2, 0.0);
	PlayerTextDrawLetterSize(playerid, ptd_main, 1.0, value_size_y / 10.0);

	PlayerTextDrawTextSize		(playerid, ptd_back, outer_pos_x2, 0.0);
	PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, height / 10.0);
	PlayerTextDrawTextSize		(playerid, ptd_fill, inner_pos_x2, 0.0);
	PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, inner_size_y / 10.0);

	pbar_TextDraw[playerid][barid][pbar_back] = ptd_back;
	pbar_TextDraw[playerid][barid][pbar_fill] = ptd_fill;
	pbar_TextDraw[playerid][barid][pbar_main] = ptd_main;
	PlayerTextDrawUseBox		(playerid, ptd_back, true);
	PlayerTextDrawUseBox		(playerid, ptd_fill, true);
	PlayerTextDrawUseBox		(playerid, ptd_main, true);
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

static stock bar_getRatios(const progressbar_direction:direction, const Float:cur_value, const Float:min_value, const Float:max_value, &Float:ratio_from, &Float:ratio_to) {
	new Float:range_value = max_value - min_value;
	if( direction == BAR_DIRECTION_HORIZONTAL_FROM_0 || direction == BAR_DIRECTION_VERTICAL_FROM_0 ) {	// From 0.
		if( max_value < 0.0 ) {
			ratio_from = 1.0;
		} else if( min_value > 0.0 ) {
			ratio_from = 0.0;
		} else {
			ratio_from = -min_value / range_value;
		}
	} else {
		ratio_from = 0.0;
	}
	ratio_to = (cur_value - min_value) / range_value;
}

static stock _normalise_range_f(&Float:value_a, &Float:value_b) {
	if( value_a > value_b ) {
		new Float:temp_value = value_b;
		value_b = value_a;
		value_a = temp_value;
	}
}

static stock PlayerBarUI_computeBoundry(const playerid, const PlayerBar:barid, boundry[E_PROGRESSBAR_BOUNDRY]) {
	new
		Float:pos_x, Float:pos_y,
		Float:padding_x, Float:padding_y,
		Float:width = pbar_Data[playerid][barid][pbar_width],
		Float:height = pbar_Data[playerid][barid][pbar_height],
		Float:min_value = pbar_Data[playerid][barid][pbar_minValue],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		Float:cur_value = pbar_Data[playerid][barid][pbar_progressValue],
		progressbar_direction:direction = pbar_Data[playerid][barid][pbar_direction],
		Float:outer_pos_x2, Float:outer_pos_y2,	// 'back' pos (right bottom corner)
		Float:inner_pos_x1, Float:inner_pos_y1,	// 'fill' pos (left upper corner)
		Float:inner_pos_x2, Float:inner_pos_y2,	// 'fill' pos (right bottom corner)
		Float:inner_size_x, Float:inner_size_y,	// 'fill' size
		Float:value_pos_x1, Float:value_pos_y1,
		Float:value_pos_x2, Float:value_pos_y2,
		Float:size_multiplier = direction_size_mult[direction],
		bool:is_vertical = (direction > BAR_DIRECTION_HORIZONTAL_FROM_0),
		Float:ratio_from, Float:ratio_to
	;
	GetPlayerProgressBarPos(playerid, barid, pos_x, pos_y);
	GetPlayerProgressBarPadding(playerid, barid, padding_x, padding_y);
	//	Computing normalized sizes and corner positions in canvas pixels.
	outer_pos_x2 = pos_x + width;
	outer_pos_y2 = pos_y + height;
	inner_pos_x1 = pos_x + padding_x;
	inner_pos_x2 = outer_pos_x2 - padding_x;
	inner_pos_y1 = pos_y + padding_y;
	inner_pos_y2 = outer_pos_y2 - padding_y;
	inner_size_x = inner_pos_x2 - inner_pos_x1;
	inner_size_y = inner_pos_y2 - inner_pos_y1;
	bar_getRatios(direction, cur_value, min_value, max_value, ratio_from, ratio_to);
	ratio_from *= size_multiplier;
	ratio_to *= size_multiplier;
	if( is_vertical ) {
		value_pos_x1 = inner_pos_x1;
		value_pos_x2 = inner_pos_x2;
	} else {	// is horizontal.
		value_pos_y1 = inner_pos_y1;
		value_pos_y2 = inner_pos_y2;
	}
	switch(direction) {
		case BAR_DIRECTION_RIGHT:				{ value_pos_x1 = inner_pos_x1; }
		case BAR_DIRECTION_LEFT:				{ value_pos_x1 = inner_pos_x2; }
		case BAR_DIRECTION_HORIZONTAL_FROM_0:	{ value_pos_x1 = inner_pos_x1; }
		case BAR_DIRECTION_UP:					{ value_pos_y1 = inner_pos_y2; }
		case BAR_DIRECTION_DOWN:				{ value_pos_y1 = inner_pos_y1; }
		case BAR_DIRECTION_VERTICAL_FROM_0:		{ value_pos_y1 = inner_pos_y2; }
	}
	if( is_vertical ) {
		value_pos_y2 = value_pos_y1;
		value_pos_y1 += inner_size_y * ratio_from;
		value_pos_y2 += inner_size_y * ratio_to;
	} else {
		value_pos_x2 = value_pos_x1;
		value_pos_x1 += inner_size_x * ratio_from;
		value_pos_x2 += inner_size_x * ratio_to;
	}
	// normalising ranges.
	{
		_normalise_range_f(value_pos_x1, value_pos_x2);
		_normalise_range_f(value_pos_y1, value_pos_y2);
	}
	new Float:correction_x = 1.25;
	value_pos_x1 += correction_x;
	value_pos_x2 -= correction_x;
	inner_pos_x1 += correction_x;
	inner_pos_x2 -= correction_x;

	boundry[E_PBAR_BACKGROUND_POS_X] = pos_x;
	boundry[E_PBAR_BACKGROUND_POS_Y] = pos_y;
	boundry[E_PBAR_BACKGROUND_RIGHT] = outer_pos_x2;
	boundry[E_PBAR_BACKGROUND_HEIGHT] = 0.1 * (outer_pos_y2 - pos_y);
	boundry[E_PBAR_FILLER_POS_X] = inner_pos_x1;
	boundry[E_PBAR_FILLER_POS_Y] = inner_pos_y1;
	boundry[E_PBAR_FILLER_RIGHT] = inner_pos_x2;
	boundry[E_PBAR_FILLER_HEIGHT] = 0.1 * (inner_pos_y2 - inner_pos_y1);
	boundry[E_PBAR_VALUE_POS_X] = value_pos_x1;
	boundry[E_PBAR_VALUE_POS_Y] = value_pos_y1;
	boundry[E_PBAR_VALUE_RIGHT] = value_pos_x2;
	boundry[E_PBAR_VALUE_HEIGHT] = 0.1 * (value_pos_y2 - value_pos_y1);
}
