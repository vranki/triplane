/* 
 * Triplane Classic - a side-scrolling dogfighting game.
 * Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * tjt@users.sourceforge.net
 */

#include <assert.h>
#include <cstdio>
#include <SDL.h>
#include <cstring>
#include "sdl_compat.h"
#include "io/joystick.h"
#include "settings.h"
#include "util/wutil.h"

/*
 * min/max value given by joystick axis is multiplied by this to give
 * the threshold for when a direction is active
 */
#define JOYSTICK_THRESHOLD_MULTIPLIER 0.8

joystick_configuration joystick_config[5];

SDL_Joystick *joydev[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

/**
 * Initializes joystick configuration.
 * @return Bitwise or of JOY1X, JOY1Y, JOY2X and JOY2Y as
 * appropriate. 1 bit means that a device providing the axis is available
 */
short init_joysticks() {
    /* set default config, used if the configuration file does not exist */
    joystick_config[0].up.type = 2;
    joystick_config[0].up.n = 1;
    joystick_config[0].up.threshold = (Sint16) (32767 * JOYSTICK_THRESHOLD_MULTIPLIER);
    joystick_config[0].down.type = 2;
    joystick_config[0].down.n = 1;
    joystick_config[0].down.threshold = (Sint16) (-32767 * JOYSTICK_THRESHOLD_MULTIPLIER);
    joystick_config[0].power.type = 2;
    joystick_config[0].power.n = 0;
    joystick_config[0].power.threshold = (Sint16) (32767 * JOYSTICK_THRESHOLD_MULTIPLIER);
    joystick_config[0].brake.type = 2;
    joystick_config[0].brake.n = 0;
    joystick_config[0].brake.threshold = (Sint16) (-32767 * JOYSTICK_THRESHOLD_MULTIPLIER);
    joystick_config[0].guns.type = 1;
    joystick_config[0].guns.n = 0;
    joystick_config[0].guns.threshold = 0;
    joystick_config[0].bombs.type = 1;
    joystick_config[0].bombs.n = 1;
    joystick_config[0].bombs.threshold = 0;
    joystick_config[0].roll.type = 0;
    joystick_config[0].roll.n = 0;
    joystick_config[0].roll.threshold = 0;
    /*
     * use these for noautoroll:
     * joystick_config[0].roll.type = 1;
     * joystick_config[0].roll.n = 2;
     * joystick_config[0].roll.threshold = 0;
     */

    memcpy(&joystick_config[1], &joystick_config[0], sizeof(joystick_configuration));
	memcpy(&joystick_config[2], &joystick_config[0], sizeof(joystick_configuration));
	memcpy(&joystick_config[3], &joystick_config[0], sizeof(joystick_configuration));

	//for solo
	memcpy(&joystick_config[4], &joystick_config[0], sizeof(joystick_configuration));

	if (SDL_NumJoysticks() >= 4)
        return JOY1 | JOY2 | JOY3 |JOY4;
	else if (SDL_NumJoysticks() == 3)
        return JOY1 | JOY2 | JOY3;
    else if (SDL_NumJoysticks() == 2)
        return JOY1 | JOY2 ;
    else if (SDL_NumJoysticks() == 1)
        return JOY1;
    else
        return 0;
}

/**
 * Loads saved joystic presence and calibration data.
 * @return 1 if succeeded in loading something
 */
int load_joysticks_data(const char *filename) {
    FILE *fp;

    fp = settings_open(filename, "rb");

    if (fp != nullptr) {
        fread(&joystick_config, sizeof(joystick_configuration), 5, fp);
        fclose(fp);
        return 1;
    }

    return 0;
}

/**
 * Saves joystic precense and calibration data.
 */

void save_joysticks_data(const char *filename) {
    FILE *fp = settings_open(filename, "wb");
    fwrite(&joystick_config, sizeof(joystick_configuration), 5, fp);
    fclose(fp);
}

/**
 * Open and close joystick devices according to the arguments.
 * @param joy1 = 1 if joystick 1 should be opened
 * @param joy2 = 1 if joystick 2 should be opened
 */
void open_close_joysticks(int joy1, int joy2, int joy3, int joy4) {
    if (SDL_NumJoysticks() >= 1) {
        if (!joy1 && SDL_JoystickOpened(0)) {
            SDL_JoystickClose(joydev[0]);
            joydev[0] = nullptr;
			joydev[4] = nullptr;
        }
        if (joy1 && !SDL_JoystickOpened(0)) {
            joydev[0] = joydev[4] = SDL_JoystickOpen(0);
        }
    }
    if (SDL_NumJoysticks() >= 2) {
        if (!joy2 && SDL_JoystickOpened(1)) {
            SDL_JoystickClose(joydev[1]);
            joydev[1] = nullptr;
        }
        if (joy2 && !SDL_JoystickOpened(1)) {
            joydev[1] = SDL_JoystickOpen(1);
        }
    }
	if (SDL_NumJoysticks() >= 3) {
        if (!joy3 && SDL_JoystickOpened(2)) {
            SDL_JoystickClose(joydev[2]);
            joydev[2] = nullptr;
        }
        if (joy3 && !SDL_JoystickOpened(2)) {
            joydev[2] = SDL_JoystickOpen(2);
        }
    }
	if (SDL_NumJoysticks() >= 4) {
        if (!joy4 && SDL_JoystickOpened(3)) {
            SDL_JoystickClose(joydev[3]);
            joydev[3] = nullptr;
        }
        if (joy4 && !SDL_JoystickOpened(3)) {
            joydev[3] = SDL_JoystickOpen(3);
        }
    }
}

static int is_joystick_action_active(int t, const joystick_action * a) {
    if (a->type == 1) {
        return SDL_JoystickGetButton(joydev[t], a->n);
    } else if (a->type == 2) {
        Sint16 v = SDL_JoystickGetAxis(joydev[t], a->n);
        if (a->threshold_dir == 1)      /* upper bound */
            return (v < a->threshold);
        else
            return (v > a->threshold);
    } else {
        return 0;
    }
}

/**
 * Updates player action {*down,*up,*power,*roll,*guns,*bombs}
 * according to current state of joystick t
 * @param t index of joystick (0 or 1)
 * @param inmenu = 1 if the player is in a menu (e.g. hangar menu)
 */
void get_joystick_action(int t, int inmenu, int *down, int *up, int *power, int *roll, int *guns, int *bombs
	,int power_on_off, int power_reverse, unsigned char *controls_power2, int in_closing
	,int *power_break_active
	) {
    /*
     * Special joystick actions for hangar menu are disabled here because
     * they are unintuitive on gamepads and other non-default joystick
     * configurations. Comment out next line to re-enable.
     * FIXME perhaps make this a command-line option or something?
     */
    inmenu = 0;
    if (inmenu) {
        *down = is_joystick_action_active(t, &joystick_config[t].down);
        *up = is_joystick_action_active(t, &joystick_config[t].up);
        *power = is_joystick_action_active(t, &joystick_config[t].guns);
        *roll = is_joystick_action_active(t, &joystick_config[t].bombs);
        *guns = is_joystick_action_active(t, &joystick_config[t].power);
        *bombs = is_joystick_action_active(t, &joystick_config[t].brake);
    } else {
        *roll = is_joystick_action_active(t, &joystick_config[t].roll);
        *guns = is_joystick_action_active(t, &joystick_config[t].guns);
        *bombs = is_joystick_action_active(t, &joystick_config[t].bombs);
        if (is_joystick_action_active(t, &joystick_config[t].brake)) {
            *power = 0;
            *down = 1;
            *up = 1;
        } else {
            //*power = is_joystick_action_active(t, &joystick_config[t].power);
			int joy_power = is_joystick_action_active(t, &joystick_config[t].power);
            *down = is_joystick_action_active(t, &joystick_config[t].down);
            *up = is_joystick_action_active(t, &joystick_config[t].up);

			if (!power_on_off) {
				// Toggle power is off
                    if (!power_reverse) {
						// ..and no reverse
                        if (joy_power)
                            *power = 1;
                        else
                            *power = 0;
                    } else {
						// Toggle off but reverse - we need to prevent takeoff with
						// power_break_active
						if (joy_power)
						{
							// Power is pressed
							*power = 0;
							if (*power_break_active)
								*power_break_active = 0;
						}
						else
						{
							// Power is not pressed
							if (!*power_break_active)
								*power = 1;
							else
								*power = 0;
						}						
                    }
                } else {
                    if (joy_power) {
                        if (!*controls_power2) {
                            if (*power)
                                *power = 0;
                            else
                                *power = 1;
                        }
                        *controls_power2 = 1;

                    } else
                        *controls_power2 = 0;

                    if (in_closing)
                        *power = 0;
                }
        }
    }
}

/** Does joystick t have a roll button? */
int joystick_has_roll_button(int t) {
    return (joystick_config[t].roll.type != 0);
}

/** Allocate enough memory to hold state of axes of given joy. */
Sint16 *allocate_axis_state(int joy) {
    return (Sint16 *) walloc(SDL_JoystickNumAxes(joydev[joy]) * sizeof(Sint16));
}

/** Save state of all axes of a given joystick */
void save_axis_state(Sint16 * axes, int joy) {
    int i, num;

    num = SDL_JoystickNumAxes(joydev[joy]);
    assert(num >= 0);

    for (i = 0; i < num; i++) {
        axes[i] = SDL_JoystickGetAxis(joydev[joy], i);
    }
}

/* Find axis that has changed most between idle and current state */
void find_changed_axis(struct joystick_action *act, Sint16 * idle, Sint16 * current, int joy) {
    int i, num, max_index, max_value;

    num = SDL_JoystickNumAxes(joydev[joy]);
    assert(num >= 0);

    max_index = 0;
    max_value = abs(idle[0] - current[0]);
    for (i = 1; i < num; i++) {
        int val;
        val = abs(idle[i] - current[i]);
        if (val > max_value) {
            max_value = val;
            max_index = i;
        }
    }

    act->type = 2;
    act->n = max_index;
    if (idle[max_index] < current[max_index]) {
        act->threshold_dir = 0; /* lower bound */
    } else {
        act->threshold_dir = 1; /* upper bound */
    }
    act->threshold = ((Sint16) (idle[max_index] + JOYSTICK_THRESHOLD_MULTIPLIER * ((double) current[max_index] - (double) idle[max_index])));	
}

/** Find button that is down. Returns 0 if no buttons are pressed. */
int find_changed_button(struct joystick_action *act, int joy) {
    int i, num;

    num = SDL_JoystickNumButtons(joydev[joy]);
    assert(num >= 0);

    for (i = 0; i < num; i++) {
        if (SDL_JoystickGetButton(joydev[joy], i)) {
            act->type = 1;
            act->n = i;
            return 1;
        }
    }
    return 0;
}

void set_disabled_action(struct joystick_action *act) {
    act->type = 0;
}

char *get_joy_action_string(struct joystick_action *act) {
    char *buf = (char *) walloc(15);

    if (act->type == 0) {
        strcpy(buf, "-");
    } else if (act->type == 1) {
        sprintf(buf, "B%d", act->n);
    } else if (act->type == 2) {
        sprintf(buf, "A%d%c", act->n, act->threshold_dir == 0 ? '+' : '-');
    }
    return buf;
}
