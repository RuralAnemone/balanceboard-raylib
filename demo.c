/*
 *	wiiuse
 *
 *	Written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	Copyright 2006-2007
 *
 *	This file is part of wiiuse.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	$Header$
 *
 */

/**
 *	@file
 *
 *	@brief Example using the wiiuse API.
 *
 *	This file is an example of how to use the wiiuse library.
 */

#include <stdio.h>                      /* for printf */
#include <stdlib.h>
#include <math.h>

#include "wiiuse.h"                     /* for wiimote_t, classic_ctrl_t, etc */
#include "raylib.h"

#ifndef WIIUSE_WIN32
#include <unistd.h>                     /* for usleep */
#endif

#define MAX_WIIMOTES				4
#define GUI_WIDTH 1820
#define GUI_HEIGHT 980

#define CURSOR_RADIUS 20
#define ITEM_RADIUS 7

// global variables
int score = 0;
typedef struct position {
	int x;
	int y;
} position;
position getUVFromPos(float x, float y);
position itemPos;
position cursorPos;
bool touchingItem(void);
void resetItem(void);

/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {
	printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

	/* if a button is pressed, report it */
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) {
		printf("A pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
		printf("B pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP)) {
		printf("UP pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))	{
		printf("DOWN pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))	{
		printf("LEFT pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))	{
		printf("RIGHT pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))	{
		printf("MINUS pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))	{
		printf("PLUS pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
		printf("ONE pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
		printf("TWO pressed\n");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))	{
		printf("HOME pressed\n");
	}

	/* show events specific to supported expansions */
	/* wii balance board */
	struct wii_board_t* wb = (wii_board_t*)&wm->exp.wb;
	float total = wb->tl + wb->tr + wb->bl + wb->br;
	float x = ((wb->tr + wb->br) / total) * 2 - 1;
	float y = ((wb->tl + wb->tr) / total) * 2 - 1;
	// printf("Weight: %f kg @ (%f, %f)\n", total, x, y);
	// printf("Interpolated weight: TL:%f  TR:%f  BL:%f  BR:%f\n", wb->tl, wb->tr, wb->bl, wb->br);
	// printf("Raw: TL:%d  TR:%d  BL:%d  BR:%d\n", wb->rtl, wb->rtr, wb->rbl, wb->rbr); 

	// update screen
	
	if(touchingItem()) resetItem();
	
	ClearBackground(BLACK);
	BeginDrawing();
	DrawLine(GUI_WIDTH/2,0,GUI_WIDTH/2,GUI_HEIGHT,WHITE);
	DrawLine(0,GUI_HEIGHT/2,GUI_WIDTH,GUI_HEIGHT/2,WHITE);
	
	cursorPos = getUVFromPos(x, y);

	DrawCircle(cursorPos.x, cursorPos.y, CURSOR_RADIUS, RED);
	DrawCircle(itemPos.x, itemPos.y, ITEM_RADIUS, YELLOW);
	printf("item pos: x:%i y:%i", itemPos.x, itemPos.y);

	DrawText("weight:", 10, 50, 20, WHITE);
	char total_str[50];
	sprintf(total_str, "%f", total);
	DrawText(total_str, 80, 50, 20, WHITE);
	DrawText("kg", 190, 50, 20, WHITE);
	DrawText("weight:", 10, 70, 20, WHITE);
	float lbs = total * 2.20462262185;
	char lbs_str[50];
	sprintf(lbs_str, "%f", lbs);
	DrawText(lbs_str, 80, 70, 20, WHITE);
	DrawText("lbs", 190, 70, 20, WHITE);

	DrawText("score:", 10, 10, 20, WHITE);
	char score_str[50];
	sprintf(score_str, "%i", score);
	DrawText(score_str, 80, 10, 20, WHITE);
	EndDrawing();
}

/**
 *	@brief Callback that handles a read event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *	@param data		Pointer to the filled data block.
 *	@param len		Length in bytes of the data block.
 *
 *	This function is called automatically by the wiiuse library when
 *	the wiimote has returned the full data requested by a previous
 *	call to wiiuse_read_data().
 *
 *	You can read data on the wiimote, such as Mii data, if
 *	you know the offset address and the length.
 *
 *	The \a data pointer was specified on the call to wiiuse_read_data().
 *	At the time of this function being called, it is not safe to deallocate
 *	this buffer.
 */
void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
	int i = 0;

	printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	printf("finished read of size %i\n", len);
	for (; i < len; ++i) {
		if (!(i % 16)) {
			printf("\n");
		}
		printf("%x ", data[i]);
	}
	printf("\n\n");
}

position getUVFromPos(float x, float y) {
	position p;
	p.x = (int) (x * (GUI_WIDTH / 2) + (GUI_WIDTH / 2));
	p.y = (int) (-y * (GUI_HEIGHT / 2) + (GUI_HEIGHT / 2));

	return p;
}

bool touchingItem(void) {
	
	return pow((cursorPos.x-itemPos.x),2) + pow((cursorPos.y-itemPos.y),2) <= pow((CURSOR_RADIUS+ITEM_RADIUS),2);
}

void resetItem() {
	score++;
	itemPos.x = rand() % GUI_WIDTH;
	itemPos.y = rand() % GUI_HEIGHT;
}

/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiiuse_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t* wm) {
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", wm->exp.type);
	printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	printf("ir:              %i\n", WIIUSE_USING_IR(wm));
	printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	printf("battery:         %f %%\n", wm->battery_level);
}


/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
	printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}


void test(struct wiimote_t* wm, byte* data, unsigned short len) {
	printf("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
}

short any_wiimote_connected(wiimote** wm, int wiimotes) {
	int i;
	if (!wm) {
		return 0;
	}

	for (i = 0; i < wiimotes; i++) {
		if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
			return 1;
		}
	}

	return 0;
}


/**
 *	@brief main()
 *
 *	Connect to up to two wiimotes and print any events
 *	that occur on either device.
 */
int main(int argc, char** argv) {
	wiimote** wiimotes;
	int found, connected;

	// initialize raylib
	InitWindow(GUI_WIDTH, GUI_HEIGHT, "Balance Board Demo");

	itemPos.x = rand() % GUI_WIDTH;
	itemPos.y = rand() % GUI_HEIGHT;

	BeginDrawing();
	DrawText("starting demo...", 10, 10, 20, WHITE);
	EndDrawing();

	/*
	 *	Initialize an array of wiimote objects.
	 *
	 *	The parameter is the number of wiimotes I want to create.
	 */
	wiimotes =  wiiuse_init(MAX_WIIMOTES);

	/*
	 *	Find wiimote devices
	 *
	 *	Now we need to find some wiimotes.
	 *	Give the function the wiimote array we created, and tell it there
	 *	are MAX_WIIMOTES wiimotes we are interested in.
	 *
	 *	Set the timeout to be 5 seconds.
	 *
	 *	This will return the number of actual wiimotes that are in discovery mode.
	 */
	BeginDrawing();
	DrawText("finding wiimotes... NOTE: DISCONNECT AND PRESS SYNC BUTTON", 10, 30, 20, WHITE);
	EndDrawing();

	found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
	if (!found) {
		printf("No wiimotes found.\n");
		BeginDrawing();
		DrawText("no wiimotes found. exiting...", 10, 50, 20, WHITE);
		EndDrawing();
		return 0;
	}

	/*
	 *	Connect to the wiimotes
	 *
	 *	Now that we found some wiimotes, connect to them.
	 *	Give the function the wiimote array and the number
	 *	of wiimote devices we found.
	 *
	 *	This will return the number of established connections to the found wiimotes.
	 */
	connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
	if (connected) {
		char s[50];
		sprintf(s, "connected to %i wiimotes (of %i found).", connected, found);
		printf("Connected to %i wiimotes (of %i found).\n", connected, found);

		BeginDrawing();
		DrawText(s, 10, 70, 20, WHITE);
		EndDrawing();
	} else {
		printf("Failed to connect to any wiimote.\n");
		BeginDrawing();
		DrawText("Failed to connect to any wiimote.", 10, 70, 20, WHITE);
		EndDrawing();
		return 0;
	}

	BeginDrawing();
	DrawText("connected!", 10, 90, 20, WHITE);
	EndDrawing();

	/*
	 *	Now set the LEDs and rumble for a second so it's easy
	 *	to tell which wiimotes are connected (just like the wii does).
	 */
	BeginDrawing();
	DrawText("setting LEDs...", 10, 110, 20, WHITE);
	EndDrawing();
	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
	wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
	wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);
	wiiuse_rumble(wiimotes[0], 1);
	wiiuse_rumble(wiimotes[1], 1);

#ifndef WIIUSE_WIN32
	usleep(200000);
#else
	Sleep(200);
#endif

	wiiuse_rumble(wiimotes[0], 0);
	wiiuse_rumble(wiimotes[1], 0);

	printf("\nControls:\n");
	printf("\tB toggles rumble.\n");
	printf("\t+ to start Wiimote accelerometer reporting, - to stop\n");
	printf("\tUP to start IR camera (sensor bar mode), DOWN to stop.\n");
	printf("\t1 to start Motion+ reporting, 2 to stop.\n");
	printf("\n\n");

	/*
	 *	Maybe I'm interested in the battery power of the 0th
	 *	wiimote.  This should be WIIMOTE_ID_1 but to be sure
	 *	you can get the wiimote associated with WIIMOTE_ID_1
	 *	using the wiiuse_get_by_id() function.
	 *
	 *	A status request will return other things too, like
	 *	if any expansions are plugged into the wiimote or
	 *	what LEDs are lit.
	 */
	/* wiiuse_status(wiimotes[0]); */

	/*
	 *	This is the main loop
	 *
	 *	wiiuse_poll() needs to be called with the wiimote array
	 *	and the number of wiimote structures in that array
	 *	(it doesn't matter if some of those wiimotes are not used
	 *	or are not connected).
	 *
	 *	This function will set the event flag for each wiimote
	 *	when the wiimote has things to report.
	 */
	while (any_wiimote_connected(wiimotes, MAX_WIIMOTES) && !WindowShouldClose() && IsWindowReady()) {
		if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
			/*
			 *	This happens if something happened on any wiimote.
			 *	So go through each one and check if anything happened.
			 */
			int i = 0;
			for (; i < MAX_WIIMOTES; ++i) {
				switch (wiimotes[i]->event) {
					case WIIUSE_EVENT:
						/* a generic event occurred */
						handle_event(wiimotes[i]);
						break;

					case WIIUSE_STATUS:
						/* a status event occurred */
						handle_ctrl_status(wiimotes[i]);
						break;

					case WIIUSE_DISCONNECT:
					case WIIUSE_UNEXPECTED_DISCONNECT:
						/* the wiimote disconnected */
						handle_disconnect(wiimotes[i]);
						break;

					case WIIUSE_READ_DATA:
						/*
						 *	Data we requested to read was returned.
						 *	Take a look at wiimotes[i]->read_req
						 *	for the data.
						 */
						break;

					case WIIUSE_WII_BOARD_CTRL_INSERTED:
						printf("Balance board controller inserted.\n");
						break;

					case WIIUSE_WII_BOARD_CTRL_REMOVED:
						/* some expansion was removed */
						handle_ctrl_status(wiimotes[i]);
						printf("An expansion was removed.\n");
						break;

					default:
						break;
				}
			}
		}
	}

	/*
	 *	Disconnect the wiimotes
	 */
	wiiuse_cleanup(wiimotes, MAX_WIIMOTES);

	return 0;
}
