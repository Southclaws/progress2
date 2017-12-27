#define FILTERSCRIPT

#include <a_samp>
#include <sscanf2>

#include <YSI\y_iterate>
#include <YSI\y_inline>
#include <YSI\y_dialog>
#include <YSI\y_commands>
#include <progress2>
#include <filemanager>

#define STEALTH_GREEN          0x33DD1100
#define STEALTH_ORANGE         0xFF880000
#define STEALTH_YELLOW         0xFFFF00AA

#define MAX_PB_EDIT            (10)

#define EDIT_STATE_NONE        (0)
#define EDIT_STATE_OPEN        (1)

#define EDIT_MODE_NONE         (0)
#define EDIT_MODE_MOVEBAR      (1)

#define DB_SAVE_TYPE_POS       (0)
#define DB_SAVE_TYPE_WIDTH     (1)
#define DB_SAVE_TYPE_HEIGHT    (2)
#define DB_SAVE_TYPE_COLOR     (3)
#define DB_SAVE_TYPE_MAX       (4)
#define DB_SAVE_TYPE_BARVALUE  (5)
#define DB_SAVE_TYPE_DIRECTION (6)

#define RGBA(%0,%1,%2,%3) ((((%0) & 0xFF) << 24) | (((%1) & 0xFF) << 16) | (((%2) & 0xFF) << 8) | (((%3) & 0xFF) << 0))

static PlayerBar:EditBars[MAX_PLAYERS][MAX_PB_EDIT];
static EditState[MAX_PLAYERS];
static EditMode[MAX_PLAYERS];
static CurrEditIndex[MAX_PLAYERS];
static DB:ProjectDB[MAX_PLAYERS];
static msgline[144];
static Float:SavedXY[MAX_PLAYERS][2];

static const DirectionNames[4][] = {
	"Right",
	"Left",
	"Up",
	"Down"
};

static const DirectionEnumNames[4][] = {
	"BAR_DIRECTION_RIGHT",
	"BAR_DIRECTION_LEFT",
	"BAR_DIRECTION_UP",
	"BAR_DIRECTION_DOWN"
};

static webcolors[4096];
static WebColorsRGBA[216] = {
	0x000000FF,0x000033FF,0x000066FF,0x000099FF,0x0000CCFF,0x0000FFFF,
	0x003300FF,0x003333FF,0x003366FF,0x003399FF,0x0033CCFF,0x0033FFFF,
	0x006600FF,0x006633FF,0x006666FF,0x006699FF,0x0066CCFF,0x0066FFFF,
	0x009900FF,0x009933FF,0x009966FF,0x009999FF,0x0099CCFF,0x0099FFFF,
	0x00CC00FF,0x00CC33FF,0x00CC66FF,0x00CC99FF,0x00CCCCFF,0x00CCFFFF,
	0x00FF00FF,0x00FF33FF,0x00FF66FF,0x00FF99FF,0x00FFCCFF,0x00FFFFFF,
	0x330000FF,0x330033FF,0x330066FF,0x330099FF,0x3300CCFF,0x3300FFFF,
	0x333300FF,0x333333FF,0x333366FF,0x333399FF,0x3333CCFF,0x3333FFFF,
	0x336600FF,0x336633FF,0x336666FF,0x336699FF,0x3366CCFF,0x3366FFFF,
	0x339900FF,0x339933FF,0x339966FF,0x339999FF,0x3399CCFF,0x3399FFFF,
	0x33CC00FF,0x33CC33FF,0x33CC66FF,0x33CC99FF,0x33CCCCFF,0x33CCFFFF,
	0x33FF00FF,0x33FF33FF,0x33FF66FF,0x33FF99FF,0x33FFCCFF,0x33FFFFFF,
	0x660000FF,0x660033FF,0x660066FF,0x660099FF,0x6600CCFF,0x6600FFFF,
	0x663300FF,0x663333FF,0x663366FF,0x663399FF,0x6633CCFF,0x6633FFFF,
	0x666600FF,0x666633FF,0x666666FF,0x666699FF,0x6666CCFF,0x6666FFFF,
	0x669900FF,0x669933FF,0x669966FF,0x669999FF,0x6699CCFF,0x6699FFFF,
	0x66CC00FF,0x66CC33FF,0x66CC66FF,0x66CC99FF,0x66CCCCFF,0x66CCFFFF,
	0x66FF00FF,0x66FF33FF,0x66FF66FF,0x66FF99FF,0x66FFCCFF,0x66FFFFFF,
	0x990000FF,0x990033FF,0x990066FF,0x990099FF,0x9900CCFF,0x9900FFFF,
	0x993300FF,0x993333FF,0x993366FF,0x993399FF,0x9933CCFF,0x9933FFFF,
	0x996600FF,0x996633FF,0x996666FF,0x996699FF,0x9966CCFF,0x9966FFFF,
	0x999900FF,0x999933FF,0x999966FF,0x999999FF,0x9999CCFF,0x9999FFFF,
	0x99CC00FF,0x99CC33FF,0x99CC66FF,0x99CC99FF,0x99CCCCFF,0x99CCFFFF,
	0x99FF00FF,0x99FF33FF,0x99FF66FF,0x99FF99FF,0x99FFCCFF,0x99FFFFFF,
	0xCC0000FF,0xCC0033FF,0xCC0066FF,0xCC0099FF,0xCC00CCFF,0xCC00FFFF,
	0xCC3300FF,0xCC3333FF,0xCC3366FF,0xCC3399FF,0xCC33CCFF,0xCC33FFFF,
	0xCC6600FF,0xCC6633FF,0xCC6666FF,0xCC6699FF,0xCC66CCFF,0xCC66FFFF,
	0xCC9900FF,0xCC9933FF,0xCC9966FF,0xCC9999FF,0xCC99CCFF,0xCC99FFFF,
	0xCCCC00FF,0xCCCC33FF,0xCCCC66FF,0xCCCC99FF,0xCCCCCCFF,0xCCCCFFFF,
	0xCCFF00FF,0xCCFF33FF,0xCCFF66FF,0xCCFF99FF,0xCCFFCCFF,0xCCFFFFFF,
	0xFF0000FF,0xFF0033FF,0xFF0066FF,0xFF0099FF,0xFF00CCFF,0xFF00FFFF,
	0xFF3300FF,0xFF3333FF,0xFF3366FF,0xFF3399FF,0xFF33CCFF,0xFF33FFFF,
	0xFF6600FF,0xFF6633FF,0xFF6666FF,0xFF6699FF,0xFF66CCFF,0xFF66FFFF,
	0xFF9900FF,0xFF9933FF,0xFF9966FF,0xFF9999FF,0xFF99CCFF,0xFF99FFFF,
	0xFFCC00FF,0xFFCC33FF,0xFFCC66FF,0xFFCC99FF,0xFFCCCCFF,0xFFCCFFFF,
	0xFFFF00FF,0xFFFF33FF,0xFFFF66FF,0xFFFF99FF,0xFFFFCCFF,0xFFFFFFFF
};

public OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		CurrEditIndex[i] = -1;
		for(new j = 0; j < MAX_PB_EDIT; j++) EditBars[i][j] = INVALID_PLAYER_BAR_ID;
	}
	
	for(new i = 0; i < 216; i++) format(webcolors, sizeof(webcolors), "%s{%06x}00000000\n", webcolors, WebColorsRGBA[i] >>> 8);

	return 1;
}

public OnPlayerConnect(playerid)
{
	SendClientMessage(playerid, STEALTH_ORANGE, "Type /pbedit to start editing progress bars!");
	return 1;
}

public OnFilterScriptExit()
{
	for(new i = 0; i < MAX_PLAYERS; i++)
	{
		CleanupPlayer(i);
	}
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	CleanupPlayer(playerid);
	return 1;
}


CMD:pbedit(playerid, arg[])
{
	if(EditMode[playerid] == EDIT_MODE_MOVEBAR) return SendClientMessage(playerid, STEALTH_YELLOW, "Please finish editing before using any commands");
	ProgressProjectEdit(playerid);
	return 1;
}


static ProgressProjectEdit(playerid)
{
	switch(EditState[playerid])
	{
		// No progject is open
		case EDIT_STATE_NONE:
		{
			inline PBNewLoadMenu(pid, dialogid, response, listitem, string:text[])
			{
				#pragma unused listitem, dialogid, pid, text
				if(response)
				{
					if(listitem == 0) CreateNewProject(playerid);
					
					// Open Existing project
					else if(listitem == 1)
					{
						new dir:dHandle = dir_open("./scriptfiles/pbedit/");
						new item[40], type;
						new line[1024];
						new extension[3];
						new fcount;

						// Create a load list
						while(dir_list(dHandle, item, type))
						{
							if(type != FM_DIR)
							{
								// We need to check extension
								if(strlen(item) > 3)
								{
									format(extension, sizeof(extension), "%s%s", item[strlen(item) - 2],item[strlen(item) - 1]);

									// File is apparently a db
									if(!strcmp(extension, "db"))
									{
										format(line, sizeof(line), "%s\n%s", item, line);
										fcount++;
									}
								}
							}
						}

						// Files were found
						if(fcount > 0)
						{
							inline Select(spid, sdialogid, sresponse, slistitem, string:stext[])
							{
								#pragma unused slistitem, sdialogid, spid, stext

								// Player selected project to load
								if(sresponse)
								{
									if(LoadProject(playerid, stext))
									{
										EditState[playerid] = EDIT_STATE_OPEN;
										EditMode[playerid] = EDIT_MODE_NONE;
										SendClientMessage(playerid, STEALTH_GREEN, "Project has been loaded");

										for(new i = 0; i < MAX_PB_EDIT; i++)
										{
											if(EditBars[playerid][i] != INVALID_PLAYER_BAR_ID)
											{
												CurrEditIndex[playerid] = i;
												break;
											}
										}
										ProgressProjectEdit(playerid);
									}
								}
							}
							Dialog_ShowCallback(playerid, using inline Select, DIALOG_STYLE_LIST, "Progress Bar Editor", line, "Ok", "Cancel");
						}
						// No files found
						else
						{
							inline CreateProject(cpid, cdialogid, cresponse, clistitem, string:ctext[])
							{
								#pragma unused clistitem, cdialogid, cpid, ctext
								if(cresponse) CreateNewProject(playerid);
								else ProgressProjectEdit(playerid);
							}
							Dialog_ShowCallback(playerid, using inline CreateProject, DIALOG_STYLE_MSGBOX, "Progress Bar Editor", "There are no projects to load create a new project?", "Ok", "Cancel");
						}
						return 1;
					}
				}
			}
			Dialog_ShowCallback(playerid, using inline PBNewLoadMenu, DIALOG_STYLE_LIST, "Progress Bar Editor", "New\nLoad", "Ok", "Cancel");
		}
		
		// Project is open
		case EDIT_STATE_OPEN:
		{
			inline PBEditMenu(pid, dialogid, response, listitem, string:text[])
			{
				#pragma unused listitem, dialogid, pid, text
				if(response)
				{
					switch(listitem)
					{
						// Create a new progress bar edit
						case 0:
						{
							new index = CreateProgressBarEdit(playerid);
							if(index > -1)
							{
								CurrEditIndex[playerid] = index;
								format(msgline, sizeof(msgline), "A new progress bar has been created Index: %i", index);
								SendClientMessage(playerid, STEALTH_GREEN, msgline);
								ProgressProjectEdit(playerid);
							}
							else
							{
								SendClientMessage(playerid, STEALTH_YELLOW, "Too many progress bars created!");
								ProgressProjectEdit(playerid);
							}
						}
						
						// Select a progress bar
						case 1:
						{
							inline PBSelect(spid, sdialogid, sresponse, slistitem, string:stext[])
							{
								#pragma unused slistitem, sdialogid, spid, stext
								if(sresponse)
								{
									sscanf(stext, "s[144]i", msgline, CurrEditIndex[playerid]);
									format(msgline, sizeof(msgline), "You have selected progress bar index %i for editing", CurrEditIndex[playerid]);
									SendClientMessage(playerid, STEALTH_GREEN, msgline);
								}
								ProgressProjectEdit(playerid);
							}
							new line[256];
							for(new i = 0; i < MAX_PB_EDIT; i++)
							{
								if(EditBars[playerid][i] != INVALID_PLAYER_BAR_ID) format(line, sizeof(line), "%sIndex: %i\n", line, i);
							}
							Dialog_ShowCallback(playerid, using inline PBSelect, DIALOG_STYLE_LIST, "Progress Bar Editor", line, "Ok", "Cancel");
						}
						
						// Edit progress bar
						case 2:
						{
							if(CurrEditIndex[playerid] == -1)
							{
								SendClientMessage(playerid, STEALTH_YELLOW, "You must have a bar selected to use this feature!");
								ProgressProjectEdit(playerid);
							}
							else ProgressProjectPropEdit(playerid);
						}

						// Delete a progress bar
						case 3:
						{
							inline PBDelete(spid, sdialogid, sresponse, slistitem, string:stext[])
							{
								#pragma unused slistitem, sdialogid, spid, stext
								if(sresponse)
								{
									sscanf(stext, "s[144]i", msgline, CurrEditIndex[playerid]);
									format(msgline, sizeof(msgline), "You have deleted progress bar index %i for editing", CurrEditIndex[playerid]);
									SendClientMessage(playerid, STEALTH_GREEN, msgline);
									new q[128];
									format(q, sizeof(q), "DELETE FROM `ProgressBars` WHERE `IndexID` = %i", CurrEditIndex[playerid]);
									db_query(ProjectDB[playerid], q);
									DestroyPlayerProgressBar(playerid, EditBars[playerid][CurrEditIndex[playerid]]);
									EditBars[playerid][CurrEditIndex[playerid]] = INVALID_PLAYER_BAR_ID;
									CurrEditIndex[playerid] = -1;
									ProgressProjectEdit(playerid);
								}
								else ProgressProjectEdit(playerid);
							}
							new line[256];
							for(new i = 0; i < MAX_PB_EDIT; i++)
							{
								if(EditBars[playerid][i] != INVALID_PLAYER_BAR_ID) format(line, sizeof(line), "%sIndex: %i\n", line, i);
							}
							Dialog_ShowCallback(playerid, using inline PBDelete, DIALOG_STYLE_LIST, "Progress Bar Editor", line, "Ok", "Cancel");

						}

						// Export project
						case 4:
						{
							new exportproj[64];
							
							// Ask for a map name
							inline ExportProject(epid, edialogid, eresponse, elistitem, string:etext[])
							{
								#pragma unused elistitem, edialogid, epid
								if(eresponse)
								{
									// Was a project name supplied ?
									if(!isnull(etext))
									{
										// Check map name length
										if(strlen(etext) >= 20)
										{
											SendClientMessage(playerid, STEALTH_YELLOW, "Choose a shorter project name to export to...");
											return 1;
										}

										// Format the output name
										format(exportproj, sizeof(exportproj), "pbedit/%s.txt", etext);

										// Project exists ask to remove
										if(fexist(exportproj))
										{
											inline RemoveProject(rpid, rdialogid, rresponse, rlistitem, string:rtext[])
											{
												#pragma unused rlistitem, rdialogid, rpid, rtext

												// Remove map and export
												if(rresponse)
												{
													fremove(exportproj);
													ProjectExport(playerid, exportproj);
												}
												else ProgressProjectEdit(playerid);
											}
											Dialog_ShowCallback(playerid, using inline RemoveProject, DIALOG_STYLE_MSGBOX, "Progress Bar Editor", "A export exists with this name replace?", "Ok", "Cancel");
										}
										// We can start the export
										else ProjectExport(playerid, exportproj);
									}
									else
									{
										SendClientMessage(playerid, STEALTH_YELLOW, "You can't export a project with no name");
										Dialog_ShowCallback(playerid, using inline ExportProject, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a export project name", "Ok", "Cancel");
									}
								}
								else ProgressProjectEdit(playerid);
							}
							Dialog_ShowCallback(playerid, using inline ExportProject, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a export project name", "Ok", "Cancel");
						}
						
						// Close project
						case 5:
						{
							CleanupPlayer(playerid);
							SendClientMessage(playerid, STEALTH_GREEN, "Your project has been closed!");
						}
					}
				}
			}
			Dialog_ShowCallback(playerid, using inline PBEditMenu, DIALOG_STYLE_LIST, "Progress Bar Editor", "Create Progress Bar\nSelect Progress Bar\nEdit Progress Bar\nDelete Progress Bar\nExport Progress Bar\nClose Project", "Ok", "Cancel");
		}
	}
	return 1;
}

static ProjectExport(playerid, filename[])
{
	new File:f = fopen(filename, io_write);
	new templine[256], Float:x, Float:y, count;

	for(new i = 0; i < MAX_PB_EDIT; i++)
	{
		if(EditBars[playerid][i] != INVALID_PLAYER_BAR_ID)
		{
			GetPlayerProgressBarPos(playerid, EditBars[playerid][i], x, y);
			format(templine, sizeof(templine), "PlayerBar_%i[playerid] = CreatePlayerProgressBar(playerid, %f, %f, %f, %f, %i, %f, %s);\r\n",
				count,
				x,
				y,
				GetPlayerProgressBarWidth(playerid, EditBars[playerid][i]),
				GetPlayerProgressBarHeight(playerid, EditBars[playerid][i]),
				GetPlayerProgressBarColour(playerid, EditBars[playerid][i]),
				GetPlayerProgressBarMaxValue(playerid, EditBars[playerid][i]),
				DirectionEnumNames[GetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]])]
			);

			fwrite(f, templine);

			if(GetPlayerProgressBarValue(playerid, EditBars[playerid][i]) > 0.0)
			{
				format(templine, sizeof(templine), "PlayerBar_%i[playerid] = SetPlayerProgressBarValue(playerid, %f);\r\n",
					GetPlayerProgressBarValue(playerid, EditBars[playerid][i]));
				fwrite(f, templine);
			}
			count++;
		}
	}
	fclose(f);
	if(count) SendClientMessage(playerid, STEALTH_GREEN, "Project exported!");

	return 1;
}

static LoadProject(playerid, filename[])
{
	new file[64];
	format(file, sizeof(file), "pbedit/%s", filename);
	ProjectDB[playerid] = db_open(file);

	new DBResult:r = db_query(ProjectDB[playerid], "SELECT * FROM `ProgressBars`");
	
	if(db_num_rows(r))
	{
		new Field[32], index, Float:x, Float:y, Float:w, Float:h, color, Float:bmax, Float:pbv, direction;

		for(new i = 0; i < db_num_rows(r); i++)
		{
			db_get_field_assoc(r, "IndexID", Field, 32);
			index = strval(Field);

			db_get_field_assoc(r, "BarX", Field, 32);
			x = floatstr(Field);

			db_get_field_assoc(r, "BarY", Field, 32);
			y = floatstr(Field);

			db_get_field_assoc(r, "BarWidth", Field, 32);
			w = floatstr(Field);

			db_get_field_assoc(r, "BarHeight", Field, 32);
			h = floatstr(Field);

			db_get_field_assoc(r, "BarColor", Field, 32);
			color = strval(Field);

			db_get_field_assoc(r, "BarMaxValue", Field, 32);
			bmax = floatstr(Field);
			
			db_get_field_assoc(r, "BarProgressValue", Field, 32);
			pbv = floatstr(Field);
			
			db_get_field_assoc(r, "BarDirection", Field, 32);
			direction = strval(Field);
			
			EditBars[playerid][index] = CreatePlayerProgressBar(playerid, x, y, w, h, color, bmax, direction);
			ShowPlayerProgressBar(playerid, EditBars[playerid][index]);
			SetPlayerProgressBarValue(playerid, EditBars[playerid][index], pbv);

			db_next_row(r);
		}
		db_free_result(r);
	}
	else
	{
		SendClientMessage(playerid, STEALTH_YELLOW, "There was no progress bars to load please make a new project");
		db_free_result(r);
		db_close(ProjectDB[playerid]);
		return 0;
	}
	return 1;
}


static CreateNewProject(playerid)
{
	inline PBNewProject(npid, ndialogid, nresponse, nlistitem, string:ntext[])
	{
		#pragma unused nlistitem, ndialogid, npid, ntext
		if(nresponse)
		{
			if(!isnull(ntext))
			{
				new fname[64];
				format(fname, sizeof(fname), "pbedit/%s.db", ntext);

				if(fexist(fname))
				{
					inline PBOverWrite(opid, odialogid, oresponse, olistitem, string:otext[])
					{
						#pragma unused olistitem, odialogid, opid, otext
						if(oresponse)
						{
							fremove(fname);
							CreateProgressBarProject(playerid, fname);
							SendClientMessage(playerid, STEALTH_GREEN, "You have created a new project!");
							ProgressProjectEdit(playerid);
						}
						else ProgressProjectEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline PBOverWrite, DIALOG_STYLE_MSGBOX, "Progress Bar Editor", "Overwrite?", "Ok", "Cancel");
				}
				else
				{
					printf("New project");
					CreateProgressBarProject(playerid, fname);
					ProgressProjectEdit(playerid);
					SendClientMessage(playerid, STEALTH_GREEN, "You have created a new project!");
				}
			}
			else
			{
				SendClientMessage(playerid, STEALTH_YELLOW, "You need to supply a filename to save to!");
				Dialog_ShowCallback(playerid, using inline PBNewProject, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a filename for your project", "Ok", "Cancel");
			}
		}
		else ProgressProjectEdit(playerid);
	}
	Dialog_ShowCallback(playerid, using inline PBNewProject, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a filename for your project", "Ok", "Cancel");
	return 1;
}

static ProgressProjectPropEdit(playerid)
{
	new Float:x, Float:y;
	
	inline PBPropEditMenu(pid, dialogid, response, listitem, string:text[])
	{
		#pragma unused listitem, dialogid, pid, text
		if(response)
		{
			switch(listitem)
			{
				// Set X-Position
				case 0:
				{
					inline SetXPos(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:pos;
								if(sscanf(stext, "f", pos))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetXPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new X Value", "Ok", "Back");
								}
								else if(pos < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetXPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new X Value", "Ok", "Back");
								}
								else if(pos > 640.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 640.0!");
									Dialog_ShowCallback(playerid, using inline SetXPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new X Value", "Ok", "Back");
								}
								else
								{
									GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);
									SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], pos, y);
									SendClientMessage(playerid, STEALTH_GREEN, "X Position has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_POS);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetXPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new X Value", "Ok", "Back");
				}
				
				// Set Y Pos
				case 1:
				{
					inline SetYPos(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:pos;
								if(sscanf(stext, "f", pos))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetYPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new Y Value", "Ok", "Back");
								}
								else if(pos < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetYPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new Y Value", "Ok", "Back");
								}
								else if(pos > 480.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 480.0!");
									Dialog_ShowCallback(playerid, using inline SetYPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new Y Value", "Ok", "Back");
								}
								else
								{
									GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);
									SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, pos);
									SendClientMessage(playerid, STEALTH_GREEN, "Y Position has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_POS);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetYPos, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new Y Value", "Ok", "Back");
				}
				
				// Set width
				case 2:
				{
					inline SetWidth(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:width;
								if(sscanf(stext, "f", width))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetWidth, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new width Value", "Ok", "Back");
								}
								else if(width < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetWidth, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new width Value", "Ok", "Back");
								}
								else if(width > 640.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 640.0!");
									Dialog_ShowCallback(playerid, using inline SetWidth, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new width Value", "Ok", "Back");
								}
								else
								{
									SetPlayerProgressBarWidth(playerid, EditBars[playerid][CurrEditIndex[playerid]], width);
									SendClientMessage(playerid, STEALTH_GREEN, "Width has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_WIDTH);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetWidth, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new width Value", "Ok", "Back");
				}
				
				// Set height
				case 3:
				{
					inline SetHeight(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:height;
								if(sscanf(stext, "f", height))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetHeight, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new height Value", "Ok", "Back");
								}
								else if(height < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetHeight, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new height Value", "Ok", "Back");
								}
								else if(height > 640.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 640.0!");
									Dialog_ShowCallback(playerid, using inline SetHeight, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new height Value", "Ok", "Back");
								}
								else
								{
									SetPlayerProgressBarHeight(playerid, EditBars[playerid][CurrEditIndex[playerid]], height);
									SendClientMessage(playerid, STEALTH_GREEN, "Height has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_HEIGHT);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetHeight, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new height Value", "Ok", "Back");
				}

				// Set Color
				case 4:
				{
					inline SelectColorMet(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							switch(slistitem)
							{
								case 0:
								{
									inline SelectHexColor(hpid, hdialogid, hresponse, hlistitem, string:htext[])
									{
										#pragma unused hlistitem, hdialogid, hpid, htext
										if(hresponse)
										{
											new hex;
											if(sscanf(htext, "x", hex))
											{
												SendClientMessage(playerid, STEALTH_YELLOW, "Invalid hex value!");
												ProgressProjectPropEdit(playerid);
											}
											else
											{
												SetPlayerProgressBarColour(playerid, EditBars[playerid][CurrEditIndex[playerid]], hex);
												SendClientMessage(playerid, STEALTH_GREEN, "Color has been updated");
												UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_COLOR);
												ProgressProjectPropEdit(playerid);
												
											}
										}
										else ProgressProjectPropEdit(playerid);
									}
									Dialog_ShowCallback(playerid, using inline SelectHexColor, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Hex color ( 0x00000000 ) ARGB", "Ok", "Cancel");
								}
								case 1:
								{
									new red, green, blue, alpha;
									inline SelectRed(redpid, reddialogid, redresponse, redlistitem, string:redtext[])
									{
										#pragma unused redlistitem, reddialogid, redpid, redtext
										if(redresponse)
										{
											red = strval(redtext);
											if(red < 0 || red > 255) Dialog_ShowCallback(playerid, using inline SelectRed, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Red Value <0 - 255>", "Ok", "Cancel");
											else
											{
												inline SelectGreen(greenpid, greendialogid, greenresponse, greenlistitem, string:greentext[])
												{
													#pragma unused greenlistitem, greendialogid, greenpid, greentext
													if(greenresponse)
													{
														green = strval(greentext);
														if(green < 0 || green > 255) Dialog_ShowCallback(playerid, using inline SelectGreen, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Green Value <0 - 255>", "Ok", "Cancel");
														else
														{
															inline SelectBlue(bluepid, bluedialogid, blueresponse, bluelistitem, string:bluetext[])
															{
																#pragma unused bluelistitem, bluedialogid, bluepid, bluetext
																if(blueresponse)
																{
																	blue = strval(bluetext);
																	if(blue < 0 || blue > 255) Dialog_ShowCallback(playerid, using inline SelectBlue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Blue Value <0 - 255>", "Ok", "Cancel");
																	else
																	{
																		inline SelectAlpha(alphapid, alphadialogid, alpharesponse, alphalistitem, string:alphatext[])
																		{
																			#pragma unused alphalistitem, alphadialogid, alphapid, alphatext
																			if(alpharesponse)
																			{
																				if(isnull(alphatext)) alpha = 255;
																				else alpha = strval(alphatext);
																				if(alpha < 0 || alpha > 255) Dialog_ShowCallback(playerid, using inline SelectAlpha, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Alpha Value <0 - 255>\nNote: Leaving this empty is full alpha 255", "Ok", "Cancel");
																				else
																				{
																					ProgressProjectPropEdit(playerid);
																					SetPlayerProgressBarColour(playerid, EditBars[playerid][CurrEditIndex[playerid]], RGBA(red, green, blue, alpha));
																					SendClientMessage(playerid, STEALTH_GREEN, "Color has been updated");
																					UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_COLOR);
																					ProgressProjectPropEdit(playerid);
																				}
																			}
																			else ProgressProjectPropEdit(playerid);
																		}
																		Dialog_ShowCallback(playerid, using inline SelectAlpha, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Alpha Value <0 - 255>\nNote: Leaving this empty is full alpha 255", "Ok", "Cancel");
																	}
																}
																else ProgressProjectPropEdit(playerid);
															}
															Dialog_ShowCallback(playerid, using inline SelectBlue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Blue Value <0 - 255>", "Ok", "Cancel");
														}
													}
													else ProgressProjectPropEdit(playerid);
												}
												Dialog_ShowCallback(playerid, using inline SelectGreen, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Green Value <0 - 255>", "Ok", "Cancel");
											}
										}
										else ProgressProjectPropEdit(playerid);
									}
									Dialog_ShowCallback(playerid, using inline SelectRed, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter Red Value <0 - 255>", "Ok", "Cancel");
								}
								case 2:
								{
									inline SelectWebColor(wpid, wdialogid, wresponse, wlistitem, string:wtext[])
									{
										#pragma unused wlistitem, wdialogid, wpid, wtext
										if(wresponse)
										{
											SetPlayerProgressBarColour(playerid, EditBars[playerid][CurrEditIndex[playerid]], WebColorsRGBA[wlistitem]);
											SendClientMessage(playerid, STEALTH_GREEN, "Color has been updated");
											UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_COLOR);
											ProgressProjectPropEdit(playerid);
										}
										else ProgressProjectPropEdit(playerid);
									}
									Dialog_ShowCallback(playerid, using inline SelectWebColor, DIALOG_STYLE_LIST, "Progress Bar Editor", webcolors, "Ok", "Cancel");
								}
							}
						}
					}
					Dialog_ShowCallback(playerid, using inline SelectColorMet, DIALOG_STYLE_LIST, "Progress Bar Editor", "Hex Value\nCombinator\nWeb Colors", "Ok", "Cancel");
				}

				// Set Max value
				case 5:
				{
					inline SetMax(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:maxv;
								if(sscanf(stext, "f", maxv))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetMax, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new max Value", "Ok", "Back");
								}
								else if(maxv < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetMax, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new max Value", "Ok", "Back");
								}
								else if(maxv > 640.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 640.0!");
									Dialog_ShowCallback(playerid, using inline SetMax, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new max Value", "Ok", "Back");
								}
								else
								{
									SetPlayerProgressBarMaxValue(playerid, EditBars[playerid][CurrEditIndex[playerid]], maxv);
									SendClientMessage(playerid, STEALTH_GREEN, "Max has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_MAX);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetMax, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new max Value", "Ok", "Back");
				}
			
				// Set bar value
				case 6:
				{
					inline SetBarValue(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							if(!isnull(stext))
							{
								new Float:barv;
								if(sscanf(stext, "f", barv))
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be a float!");
									Dialog_ShowCallback(playerid, using inline SetBarValue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new bar Value", "Ok", "Back");
								}
								else if(barv < 0.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must not be negative!");
									Dialog_ShowCallback(playerid, using inline SetBarValue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new bar Value", "Ok", "Back");
								}
								else if(barv > 640.0)
								{
									SendClientMessage(playerid, STEALTH_YELLOW, "Value must be less than 640.0!");
									Dialog_ShowCallback(playerid, using inline SetBarValue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new bar Value", "Ok", "Back");
								}
								else
								{
									SetPlayerProgressBarValue(playerid, EditBars[playerid][CurrEditIndex[playerid]], barv);
									SendClientMessage(playerid, STEALTH_GREEN, "Bar value has been updated");
									UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_BARVALUE);
									ProgressProjectPropEdit(playerid);
								}
							}
							else ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetBarValue, DIALOG_STYLE_INPUT, "Progress Bar Editor", "Enter a new bar Value", "Ok", "Back");
				}
				
				// Set progress bar direction
				case 7:
				{
					inline SetBarDirection(spid, sdialogid, sresponse, slistitem, string:stext[])
					{
						#pragma unused slistitem, sdialogid, spid, stext
						if(sresponse)
						{
							switch(slistitem)
							{
								case 0: SetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]], BAR_DIRECTION_RIGHT);
								case 1: SetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]], BAR_DIRECTION_LEFT);
								case 2: SetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]], BAR_DIRECTION_UP);
								case 3: SetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]], BAR_DIRECTION_DOWN);
							}
							SendClientMessage(playerid, STEALTH_GREEN, "Bar direction has been updated");
							UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_DIRECTION);
							ProgressProjectPropEdit(playerid);
						}
						else ProgressProjectPropEdit(playerid);
					}
					Dialog_ShowCallback(playerid, using inline SetBarDirection, DIALOG_STYLE_LIST, "Progress Bar Editor", "Right\nLeft\nUp\nDown", "Ok", "Back");
				}
				
				// Set XY with keys
				case 8:
				{
					EditMode[playerid] = EDIT_MODE_MOVEBAR;
					GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], SavedXY[playerid][0], SavedXY[playerid][1]);
					// TogglePlayerControllable(playerid, false);
					SendClientMessage(playerid, STEALTH_GREEN, "Use the arrow keys to set the position press 'WALK KEY' when finished!");
				}
			}
		}
		else ProgressProjectEdit(playerid);
	}
	new line[256];

	GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);

	format(line, sizeof(line), "XPos: %f\nYPos: %f\nWidth: %f\nHeight: %f\nColor: {%06x}XXXXXXXX{FFFFFF}\nMax Value:%f\nBar Value: %f\nDirection: %s\nSet XY with keys",
		x,
		y,
		GetPlayerProgressBarWidth(playerid, EditBars[playerid][CurrEditIndex[playerid]]),
		GetPlayerProgressBarHeight(playerid, EditBars[playerid][CurrEditIndex[playerid]]),
		GetPlayerProgressBarColour(playerid, EditBars[playerid][CurrEditIndex[playerid]]) >>> 8,
		GetPlayerProgressBarMaxValue(playerid, EditBars[playerid][CurrEditIndex[playerid]]),
		GetPlayerProgressBarValue(playerid, EditBars[playerid][CurrEditIndex[playerid]]),
		DirectionNames[GetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]])]
	);

	Dialog_ShowCallback(playerid, using inline PBPropEditMenu, DIALOG_STYLE_LIST, "Progress Bar Editor", line, "Ok", "Back");
	return 1;
}


public OnPlayerUpdate(playerid)
{
	if(EditMode[playerid] == EDIT_MODE_MOVEBAR)
	{
		new Keys,ud,lr, Float:x, Float:y;

		GetPlayerKeys(playerid,Keys,ud,lr);

		if(Keys & KEY_WALK)
		{
			inline SavePosition(spid, sdialogid, sresponse, slistitem, string:stext[])
			{
				#pragma unused slistitem, sdialogid, spid, stext
				if(sresponse)
				{
					SendClientMessage(playerid, STEALTH_GREEN, "Position Updated");
					UpdateDB(playerid, CurrEditIndex[playerid], DB_SAVE_TYPE_POS);
				}
				else
				{
					SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], SavedXY[playerid][0], SavedXY[playerid][1]);
					SendClientMessage(playerid, STEALTH_YELLOW, "Position update cancelled");
				}
				ProgressProjectPropEdit(playerid);
			}
			EditMode[playerid] = EDIT_MODE_NONE;
			Dialog_ShowCallback(playerid, using inline SavePosition, DIALOG_STYLE_MSGBOX, "Progress Bar Editor", "Update Position?", "Update", "Cancel");
		}
		else
		{
			GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);

			if(ud == KEY_UP) SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y-10.0);
			else if(ud == KEY_DOWN) SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y+10.0);
			if(lr == KEY_LEFT) SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x-10.0, y);
			else if(lr == KEY_RIGHT) SetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x+10.0, y);

			new line[128];
			GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);
			format(line, sizeof(line), "~w~~n~~n~~n~x: %1.f y: %1.f", x, y);

			GameTextForPlayer(playerid, line, 2000, 5);
		}
	}
	return 1;
}

static CreateProgressBarProject(playerid, filename[])
{
	ProjectDB[playerid] = db_open(filename);
	db_query(ProjectDB[playerid], "CREATE TABLE IF NOT EXISTS `ProgressBars` (IndexID INTEGER, BarX REAL, BarY REAL, BarWidth REAL, BarHeight REAL, BarColor INTEGER, BarMaxValue REAL, BarProgressValue REAL, BarDirection INTEGER)");
	EditState[playerid] = EDIT_STATE_OPEN;
	return 1;
}

static CreateProgressBarEdit(playerid)
{
	for(new i = 0; i < MAX_PB_EDIT; i++)
	{
		if(EditBars[playerid][i] == INVALID_PLAYER_BAR_ID)
		{
			EditBars[playerid][i] = CreatePlayerProgressBar(playerid, 320.0, 320.0, 55.5, 3.2, 0xFF1C1CFF, 100.0, BAR_DIRECTION_RIGHT);
			ShowPlayerProgressBar(playerid, EditBars[playerid][i]);

			new q[256];
			format(q, sizeof(q), "INSERT INTO `ProgressBars` (`IndexID`, `BarX`, `BarY`, `BarWidth`, `BarHeight`, `BarColor`, `BarMaxValue`, `BarProgressValue`, `BarDirection`) \
				VALUES(%i, %f, %f, %f, %f, %i, %f, %f, %i)",
				i,
				320.0,
				320.0,
				55.5,
				3.2,
				0xFF1C1CFF,
				100.0,
				0.0,
				BAR_DIRECTION_RIGHT
			);

			db_query(ProjectDB[playerid], q);

			return i;
		}
	}
	return -1;
}

static UpdateDB(playerid, index, type)
{
	new q[128];
	switch(type)
	{
		case DB_SAVE_TYPE_POS:
		{
			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, EditBars[playerid][CurrEditIndex[playerid]], x, y);
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarX` = %f, `BarY` = %f WHERE `IndexID` = %i",
				x, y, index);
		}
		
		case DB_SAVE_TYPE_WIDTH:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarWidth` = %f WHERE `IndexID` = %i",
				GetPlayerProgressBarWidth(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
		
		case DB_SAVE_TYPE_HEIGHT:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarHeight` = %f WHERE `IndexID` = %i",
				GetPlayerProgressBarHeight(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
		
		case DB_SAVE_TYPE_COLOR:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarColor` = %i WHERE `IndexID` = %i",
				GetPlayerProgressBarColour(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
		
		case DB_SAVE_TYPE_MAX:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarMaxValue` = %f WHERE `IndexID` = %i",
				GetPlayerProgressBarMaxValue(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
	
		case DB_SAVE_TYPE_BARVALUE:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarProgressValue` = %f WHERE `IndexID` = %i",
				GetPlayerProgressBarValue(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
		
		case DB_SAVE_TYPE_DIRECTION:
		{
			format(q, sizeof(q), "UPDATE `ProgressBars` SET `BarDirection` = %i WHERE `IndexID` = %i",
				GetPlayerProgressBarDirection(playerid, EditBars[playerid][CurrEditIndex[playerid]]), index);
		}
	}

	db_query(ProjectDB[playerid], q);
	return 1;
}

static CleanupPlayer(playerid)
{
	if(EditState[playerid] == EDIT_STATE_OPEN)
	{
		db_close(ProjectDB[playerid]);
		CurrEditIndex[playerid] = -1;
		EditState[playerid] = EDIT_STATE_NONE;
		EditMode[playerid] = EDIT_MODE_NONE;
		for(new i = 0; i < MAX_PB_EDIT; i++)
		{
			if(EditBars[playerid][i] != INVALID_PLAYER_BAR_ID) DestroyPlayerProgressBar(playerid, EditBars[playerid][i]);
			EditBars[playerid][i] = INVALID_PLAYER_BAR_ID;
		}
	}
	return 1;
}