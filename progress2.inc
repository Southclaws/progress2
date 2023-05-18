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

#if !defined _INC_open_mp
	#include <a_samp>

	#if !defined PlayerTextDrawBoxColour
		#define PlayerTextDrawBoxColour PlayerTextDrawBoxColor
	#endif
#endif

#tryinclude <logger>
#tryinclude <YSI_Data\y_iterate>

#tryinclude <YSI_Coding\y_hooks>

#if defined MAX_PLAYER_BARS
	#if (MAX_PLAYER_BARS - PlayerBar:0)==PlayerBar:0
		#error You forgot to set value of MAX_PLAYER_BARS. Type something like: #define MAX_PLAYER_BARS (PlayerBar:30).
	#endif
	#if (MAX_PLAYER_BARS < PlayerBar:1)
		#error You are trying to allocate invalid number of PlayerBars. Set value of MAX_PLAYER_BARS greather than 0.
	#elseif MAX_PLAYER_BARS > PlayerBar:(MAX_PLAYER_TEXT_DRAWS / 3)
		#error You are trying to allocate too many PlayerBars. Set value of MAX_PLAYER_BARS below or equals to (MAX_PLAYER_TEXT_DRAWS/3).
	#endif
#else
	#define MAX_PLAYER_BARS					(PlayerBar:(_:MAX_PLAYER_TEXT_DRAWS / 3))
#endif


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

static PlayerText:pbar_TextDraw[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_TEXT_DRAW];

new
	#if(defined _INC_y_iterate)
		Iterator:pbar_Index[MAX_PLAYERS]<PlayerBar:(_:MAX_PLAYER_BARS)>,
	#endif
	pbar_Data[MAX_PLAYERS][MAX_PLAYER_BARS][E_BAR_DATA]
;
static bool:is_progressbar_initialised = false;

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
	is_progressbar_initialised = true;
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
//	Returns true if value-textdraw needs to be drawn due of current value relation to min/max values.
static stock bool:PlayerBarUI_isNeedToDrawValue(const progressbar_direction:direction, const Float:cur_value, const Float:min_value, const Float:max_value) {
	if( direction == BAR_DIRECTION_HORIZONTAL_FROM_0 || direction == BAR_DIRECTION_VERTICAL_FROM_0 ) {
		if( max_value < 0.0 ) {
			return (cur_value < max_value);
		} else if( min_value > 0.0 ) {
			return (cur_value > min_value);
		} else {
			return (floatabs(cur_value) > 0.001 * (max_value - min_value));
		}
	} else {
		return (cur_value > min_value);
	}
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
	if( !is_progressbar_initialised ) {	// auto initialisation at first progress bar creation.
		PlayerBarUI_ResetAll();
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

	_ptextdraw_destroy_array(playerid, pbar_TextDraw[playerid][barid]);

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
		boundary[E_PROGRESSBAR_BOUNDRY],
		Float:min_value = pbar_Data[playerid][barid][pbar_minValue],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		progressbar_direction:direction = pbar_Data[playerid][barid][pbar_direction],
		bool:draw_main,
		color = pbar_Data[playerid][barid][pbar_colour],
		PlayerText:ptd_main = pbar_TextDraw[playerid][barid][pbar_main]
	;
	if( value < min_value ) {
		value = min_value;
	} else if( value > max_value ) {
		value = max_value;
	}
	pbar_Data[playerid][barid][pbar_progressValue] = value;
	draw_main = PlayerBarUI_isNeedToDrawValue(direction, value, min_value, max_value);
	PlayerBarUI_computeBoundry(playerid, barid, boundary);

	#if defined PlayerTextDrawSetPos
	{
		PlayerTextDrawSetPos(playerid, ptd_main, boundary[E_PBAR_VALUE_POS_X], boundary[E_PBAR_VALUE_POS_Y]);
		PlayerTextDrawTextSize(playerid, ptd_main, boundary[E_PBAR_VALUE_RIGHT], 0.0);
		PlayerTextDrawLetterSize(playerid, ptd_main, 1.0, boundary[E_PBAR_VALUE_HEIGHT]);
		PlayerTextDrawUseBox(playerid, ptd_main, draw_main);
	}
	#else
	{	// Textdraw set position not found. Set with recreation.
		PlayerTextDrawDestroy(playerid, ptd_main);
		ptd_main = CreatePlayerTextDraw(
			playerid,
			boundary[E_PBAR_VALUE_POS_X],
			boundary[E_PBAR_VALUE_POS_Y],
			"_"
		);
		new PlayerText:ptd_back = pbar_TextDraw[playerid][barid][pbar_back],
			PlayerText:ptd_fill = pbar_TextDraw[playerid][barid][pbar_fill]
		;
		if(	// Check in new main textdraw is not under filler/background.
			(ptd_fill != PlayerText:INVALID_TEXT_DRAW && ptd_fill < ptd_main) ||
			(ptd_back != PlayerText:INVALID_TEXT_DRAW && ptd_back < ptd_main)
		) {
			PlayerTextDrawTextSize(playerid, ptd_main, boundary[E_PBAR_VALUE_RIGHT], 0.0);
			PlayerTextDrawLetterSize(playerid, ptd_main, 1.0, boundary[E_PBAR_VALUE_HEIGHT]);
			PlayerTextDrawUseBox(playerid, ptd_main, draw_main);
			pbar_TextDraw[playerid][barid][pbar_main] = ptd_main;
		} else {	// Rare case: main/value textdraw was created UNDER(with lower ID than) filler/background textdraw -> recreate whole bar again.
			PlayerTextDrawDestroy(playerid, ptd_back);
			PlayerTextDrawDestroy(playerid, ptd_fill);
			PlayerTextDrawDestroy(playerid, ptd_main);
			PlayerBarUI_createGeometry(playerid, barid, boundary, color, draw_main);
		}
	}
	#endif
	SetPlayerProgressBarColour(playerid, barid, color);

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
static PlayerBarUI_createGeometry(const playerid, const PlayerBar:barid, const boundary[E_PROGRESSBAR_BOUNDRY], const color, const bool:draw_main) {
	new PlayerText:ptd_back, PlayerText:ptd_fill, PlayerText:ptd_main;
	ptd_back = CreatePlayerTextDraw(playerid, boundary[E_PBAR_BACKGROUND_POS_X], boundary[E_PBAR_BACKGROUND_POS_Y], "_");
	PlayerTextDrawTextSize		(playerid, ptd_back, boundary[E_PBAR_BACKGROUND_RIGHT], 0.0);
	PlayerTextDrawLetterSize	(playerid, ptd_back, 1.0, boundary[E_PBAR_BACKGROUND_HEIGHT]);
	PlayerTextDrawUseBox		(playerid, ptd_back, true);

	ptd_fill = CreatePlayerTextDraw(playerid, boundary[E_PBAR_FILLER_POS_X], boundary[E_PBAR_FILLER_POS_Y], "_");
	PlayerTextDrawTextSize		(playerid, ptd_fill, boundary[E_PBAR_FILLER_RIGHT], 0.0);
	PlayerTextDrawLetterSize	(playerid, ptd_fill, 1.0, boundary[E_PBAR_FILLER_HEIGHT]);
	PlayerTextDrawUseBox		(playerid, ptd_fill, true);

	ptd_main = CreatePlayerTextDraw(playerid, boundary[E_PBAR_VALUE_POS_X], boundary[E_PBAR_VALUE_POS_Y], "_");
	PlayerTextDrawTextSize(playerid, ptd_main, boundary[E_PBAR_VALUE_RIGHT], 0.0);
	PlayerTextDrawLetterSize(playerid, ptd_main, 1.0, boundary[E_PBAR_VALUE_HEIGHT]);
	PlayerTextDrawUseBox		(playerid, ptd_main, draw_main);

	pbar_TextDraw[playerid][barid][pbar_back] = ptd_back;
	pbar_TextDraw[playerid][barid][pbar_fill] = ptd_fill;
	pbar_TextDraw[playerid][barid][pbar_main] = ptd_main;
	SetPlayerProgressBarColour(playerid, barid, color);
}

_progress2_renderBar(const playerid, const PlayerBar:barid) {
	if(
		!IsPlayerConnected(playerid) ||
		!IsValidPlayerProgressBar(playerid, barid)
	) {
		return false;
	}

	new
		PlayerText:old_textdraw_array[E_BAR_TEXT_DRAW],
		boundary[E_PROGRESSBAR_BOUNDRY],
		Float:min_value = pbar_Data[playerid][barid][pbar_minValue],
		Float:max_value = pbar_Data[playerid][barid][pbar_maxValue],
		Float:cur_value = pbar_Data[playerid][barid][pbar_progressValue],
		progressbar_direction:direction = pbar_Data[playerid][barid][pbar_direction],
		bool:draw_main,
		color = pbar_Data[playerid][barid][pbar_colour],
		PlayerText:ptd_back,
		PlayerText:ptd_fill,
		PlayerText:ptd_main
	;
	old_textdraw_array = pbar_TextDraw[playerid][barid];
	_ptextdraw_destroy_array(playerid, old_textdraw_array);

	//	Computing normalized sizes and corner positions in canvas pixels.
	draw_main = PlayerBarUI_isNeedToDrawValue(direction, cur_value, min_value, max_value);
	PlayerBarUI_computeBoundry(playerid, barid, boundary);

	PlayerBarUI_createGeometry(playerid, barid, boundary, color, draw_main);
	ptd_back = pbar_TextDraw[playerid][barid][pbar_main];
	ptd_fill = pbar_TextDraw[playerid][barid][pbar_fill];
	ptd_main = pbar_TextDraw[playerid][barid][pbar_back];

	//	Fixing very special bug:
	//		Past versions of progress bars never hide destroyed textdraws from previous state (PlayerTextDrawDestroy doesn't hiding anything).
	//		ProgressBar was always hopes that textdraw IDs will be same after bar recreation.
	//		That can lead to some of bar's textdraw still drawn on player screen.
	//		You can reproduce:
	//		1) Create some of your textdraws.
	//		2) Create progress bar.
	//		3) Remove some previous textdraws.
	//		4) Recreate progress bar.
	//		5) Do not create & show textdraws anymore.
	{
		new E_BAR_TEXT_DRAW:td_slot_old,
			E_BAR_TEXT_DRAW:td_slot_new,
			PlayerText:old_textdraw_item
		;
		for(td_slot_old = E_BAR_TEXT_DRAW:0; td_slot_old < E_BAR_TEXT_DRAW:sizeof(old_textdraw_array); ++td_slot_old) {
			old_textdraw_item = old_textdraw_array[td_slot_old];
			for(td_slot_new = E_BAR_TEXT_DRAW:0; td_slot_new < E_BAR_TEXT_DRAW:sizeof(old_textdraw_array); ++td_slot_new) {
				if(
					old_textdraw_item != ptd_back &&
					old_textdraw_item != ptd_fill &&
					old_textdraw_item != ptd_main
				) {
					PlayerTextDrawHide(playerid, old_textdraw_item);	// Hiding after destroying? Just send RPC.
				}
			}
		}
	}

	if( pbar_Data[playerid][barid][pbar_show] ) {
		ShowPlayerProgressBar(playerid, barid);
	}

	return true;
}
#if(defined _INC_y_hooks)
hook OnScriptInit() {
	PlayerBarUI_ResetAll();
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

static stock _ptextdraw_destroy_array(const playerid, const PlayerText:ptd_array[E_BAR_TEXT_DRAW]) {
	for(new E_BAR_TEXT_DRAW:ptd_slot = E_BAR_TEXT_DRAW:0; ptd_slot < E_BAR_TEXT_DRAW:3; ++ptd_slot) {
		PlayerTextDrawDestroy(playerid, ptd_array[ptd_slot]);
	}
}

static stock _normalise_range_f(&Float:value_a, &Float:value_b) {
	if( value_a > value_b ) {
		new Float:temp_value = value_b;
		value_b = value_a;
		value_a = temp_value;
	}
}

static stock PlayerBarUI_computeBoundry(const playerid, const PlayerBar:barid, boundary[E_PROGRESSBAR_BOUNDRY]) {
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

	boundary[E_PBAR_BACKGROUND_POS_X] = pos_x;
	boundary[E_PBAR_BACKGROUND_POS_Y] = pos_y;
	boundary[E_PBAR_BACKGROUND_RIGHT] = outer_pos_x2;
	boundary[E_PBAR_BACKGROUND_HEIGHT] = 0.1 * (outer_pos_y2 - pos_y);
	boundary[E_PBAR_FILLER_POS_X] = inner_pos_x1;
	boundary[E_PBAR_FILLER_POS_Y] = inner_pos_y1;
	boundary[E_PBAR_FILLER_RIGHT] = inner_pos_x2;
	boundary[E_PBAR_FILLER_HEIGHT] = 0.1 * (inner_pos_y2 - inner_pos_y1);
	boundary[E_PBAR_VALUE_POS_X] = value_pos_x1;
	boundary[E_PBAR_VALUE_POS_Y] = value_pos_y1;
	boundary[E_PBAR_VALUE_RIGHT] = value_pos_x2;
	boundary[E_PBAR_VALUE_HEIGHT] = 0.1 * (value_pos_y2 - value_pos_y1);
}
