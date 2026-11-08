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

/* Terrain drawing code */

#include "terrain.h"
#include "../menus/tripmenu.h"
#include "../triplane.h"
#include "../world/constants.h"
#include "../world/plane.h"
#include "../world/tripai.h"
#include "../world/tripmis.h"

#include <cstdlib>


namespace world::terrain {

namespace {

/* Clip rectangle, inclusive, as Bitmap::blit() and putpix() take it. */
struct Clip {
  int x1, y1, x2, y2;
};

/* The whole 320x200 screen: what Bitmap::blit() clips to by default. */
constexpr Clip vga_screen{0, 0, 319, 199};

/* Screen position and clip rectangle of one sprite. */
struct Placement {
  int x, y;
  Clip clip;
};

void draw(Bitmap *sprite, const Placement &p) {
  sprite->blit(p.x, p.y, p.clip.x1, p.clip.y1, p.clip.x2, p.clip.y2);
}

void draw(const std::unique_ptr<Bitmap> &sprite, const Placement &p) {
  draw(sprite.get(), p);
}

/*
 * How the world is mapped onto the screen in one drawing pass.
 *
 * Split: one of the four viewports of the 320x200 split screen (player l).
 *        The world scrolls with the player and is clipped to the viewport
 *        x1_raja..x2_raja, y1_raja..y2_raja.
 * Solo:  the single 320x200 viewport of the solo player l, which scrolls
 *        horizontally only.
 * Svga:  the 800x600 screen shows the whole world as rows of screen_width
 *        pixels, each 196 lines high. A sprite goes to the row its left
 *        edge is in; a sprite sticking out past the right edge of its row
 *        may get a second copy at the start of the next row (see wraps()).
 */
struct View {
  enum class Kind { Split, Solo, Svga };

  Kind kind;
  int l; /* the player whose viewport this is (Split and Solo) */

  /*
   * Where a sprite whose top-left corner is at world (wx, wy) lands.
   * Airborne sprites (bombs, planes, flying objects) are clipped
   * differently from those on the ground: in the split screen the hangar
   * menu sliding down (in_closing) covers them, and in 800x600 mode they
   * are clipped to the top of their own row. rows_down shifts the sprite
   * that many rows further down (800x600 only).
   */
  Placement place(int wx, int wy, bool airborne, int rows_down) const {
    if (kind == Kind::Split)
      return {wx + player_shown_x[l] - player_x_8[l] + x_muutos[l],
              wy + player_shown_y[l] - player_y_8[l] + y_muutos[l],
              {x1_raja[l], y1_raja[l] + (airborne ? in_closing[l] : 0),
               x2_raja[l], y2_raja[l]}};

    if (kind == Kind::Solo)
      return {wx + player_shown_x[l] - player_x_8[l], wy, vga_screen};

    const int row = wx / screen_width + rows_down;
    return {wx - row * screen_width,
            wy + row * 196 - 4,
            {0, airborne ? row * 196 : 0, screen_width_less,
             screen_height_less}};
  }

  Placement ground(int wx, int wy, int rows_down = 0) const {
    return place(wx, wy, false, rows_down);
  }

  Placement air(int wx, int wy, int rows_down = 0) const {
    return place(wx, wy, true, rows_down);
  }

  /*
   * Does a sprite `width` pixels wide with its left edge at world x `wx`
   * stick out past the right edge of its 800x600 row? Never in the
   * 320x200 modes.
   */
  bool wraps(int wx, int width) const {
    return kind == Kind::Svga &&
           wx - (wx / screen_width) * screen_width + width > screen_width;
  }
};

/*
 * The layers of the world, each in its own function. The passes below
 * draw them in their own order; the order matters where sprites overlap.
 */

void draw_structures(const View &v) {
  for (int l2 = 0; l2 < MAX_STRUCTURES; l2++) {
    if ((structures[l2][struct_state[l2]] != nullptr) &&
        (!leveldata.struct_hit[l2]))
      draw(structures[l2][struct_state[l2]],
           v.ground(leveldata.struct_x[l2], leveldata.struct_y[l2]));
  }
}

void draw_hangar_doors(const View &v) {
  for (int l2 = 0; l2 < 4; l2++) {
    if (!hangar_x[l2])
      continue;
    draw(ovi[hangar_door_frame[l2]],
         v.ground(hangar_x[l2] + 27, hangar_y[l2] + 3));
  }
}

void draw_flags(const View &v) {
  if (!config.flags)
    return;
  for (int l2 = 0; l2 < MAX_FLAGS; l2++) {
    if (flags_x[l2])
      draw(flags[flags_owner[l2]][flags_frame[l2]],
           v.ground(flags_x[l2], flags_y[l2]));
  }
}

/* A gun in status 2 animates at half speed. */
const std::unique_ptr<Bitmap> &aa_gun_sprite(int l2) {
  const int frame =
      kkbase_status[l2] != 2 ? kkbase_frame[l2] : kkbase_frame[l2] >> 1;
  return kkbase[kkbase_type[l2]][kkbase_status[l2]][frame];
}

void draw_aa_guns(const View &v) {
  for (int l2 = 0; l2 < MAX_AA_GUNS; l2++) {
    if (!kkbase_x[l2])
      continue;

    draw(aa_gun_sprite(l2), v.ground(kkbase_x[l2], kkbase_y[l2]));

    if (v.wraps(kkbase_x[l2], 26))
      draw(aa_gun_sprite(l2), v.ground(kkbase_x[l2], kkbase_y[l2], 1));
  }
}

Bitmap *infantry_sprite(int l2) {
  const int country = infan_country[l2];
  const int direction = infan_direction[l2];
  const int frame = infan_frame[l2];

  switch (infan_state[l2]) {
  case 0:
    if (frame < 12)
      return infantry_walking[country][direction][frame].get();
    if (frame == 12)
      return infantry_dropping[country][direction].get();
    return infantry_after_drop[country][direction].get();

  case 1:
    return infantry_aiming[country][direction][frame].get();

  case 2:
    return infantry_shooting[country][direction][frame].get();

  case 3:
    return infantry_dying[country][direction][frame >> 1].get();

  case 4:
    return infantry_wavedeath[country][direction][frame >> 1].get();

  case 5:
    return infantry_bdying[country][direction][frame >> 1].get();

  default:
    return nullptr;
  }
}

void draw_infantry(const View &v) {
  for (int l2 = 0; l2 < MAX_INFANTRY; l2++) {
    if (!infan_x[l2])
      continue;

    if (Bitmap *sprite = infantry_sprite(l2))
      draw(sprite, v.ground(infan_x[l2], infan_y[l2]));
  }
}

Bitmap *mechanic_sprite(int l2) {
  switch (mekan_status[l2]) {
  case 1:
    return mekan_running[mekan_frame[l2]][mekan_direction[l2]].get();

  case 2:
    return mekan_pushing[0][mekan_frame[l2]][mekan_direction[l2]].get();

  case 3:
    return mekan_pushing[1][mekan_frame[l2]][mekan_direction[l2]].get();

  default:
    return nullptr;
  }
}

void draw_mechanics(const View &v) {
  for (int l2 = 0; l2 < 4; l2++) {
    if (!mekan_x[l2])
      continue;

    if (Bitmap *sprite = mechanic_sprite(l2))
      draw(sprite, v.ground(mekan_x[l2], mekan_y[l2]));
  }
}

void draw_bombs(const View &v) {
  for (int l2 = 0; l2 < MAX_BOMBS; l2++) {
    if (bomb_x[l2])
      draw(bomb[(bomb_angle[l2] >> 8) / 6],
           v.air((bomb_x[l2] >> 8) - 4, (bomb_y[l2] >> 8) - 4));
  }
}

bool plane_flying(int l2) {
  return !in_closing[l2] && player_exists[l2] && !plane_coming[l2];
}

bool plane_crashing(int l2) {
  return in_closing[l2] <= 12 && player_exists[l2] && !plane_coming[l2];
}

Bitmap *plane_sprite(int l2) {
  return planes[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]]
               [player_upsidedown[l2]]
                   .get();
}

/*
 * Radar blip for plane l2 at the edge of player l's viewport when it is
 * off-screen. There is no radar in 800x600 mode, where the whole world is
 * visible.
 */
void draw_radar_blip(const View &v, int l2) {
  const int l = v.l;

  if (v.kind == View::Kind::Split) {
    int tempx = player_x_8[l2] + player_shown_x[l] - player_x_8[l];
    int tempy = player_y_8[l2] + player_shown_y[l] - player_y_8[l];

    if (!computer_active[l] &&
        (tempx > 156 || tempx < 0 || tempy > 88 || tempy < 0)) {
      /*
       * The blip's brightness comes from abs(tempx + tempy) >> 8 and its
       * position from tempx >> 5 and tempy >> 2, relative to fixed points
       * of the viewport. This is how the original did it; left alone.
       */
      int temp = (abs(tempx + tempy)) >> 8;

      if (temp >= 8)
        temp = 7;

      tempx = tempx >> 5;
      tempy = tempy >> 2;

      radar[player_tsides[l2]][temp]->blit(
          x1_raja[l], y_muutos[l] + 44 + tempy, x1_raja[l],
          y1_raja[l] + in_closing[l], x2_raja[l], y2_raja[l]);
      radar[player_tsides[l2]][temp]->blit(
          x_muutos[l] + 78 + tempx, y1_raja[l], x1_raja[l],
          y1_raja[l] + in_closing[l], x2_raja[l], y2_raja[l]);
    }
  } else if (v.kind == View::Kind::Solo) {
    int temp = abs(player_x_8[l] - player_x_8[l2]);

    if (temp > 160) {
      temp = temp >> 8;

      if (temp >= 8)
        temp = 7;

      if ((player_x_8[l] - player_x_8[l2]) > 0)
        radar[player_tsides[l2]][temp]->blit(0, player_y_8[l2] - 1);
      else
        radar[player_tsides[l2]][temp]->blit(317, player_y_8[l2] - 1);
    }
  }
}

void draw_planes(const View &v) {
  for (int l2 = 0; l2 < 16; l2++) {
    if (plane_flying(l2)) {
      if (l2 < 4 && hangarmenu_active[l2])
        continue;

      draw_radar_blip(v, l2);
      draw(plane_sprite(l2), v.air(player_x_8[l2] - 10, player_y_8[l2] - 10));
    } else if (plane_crashing(l2)) {
      draw(plane_crash[(in_closing[l2] >> 1) - 1],
           v.air(player_x_8[l2] - 10, player_y_8[l2] - 10));
    }
  }

  if (v.kind != View::Kind::Svga)
    return;

  /*
   * 800x600: a plane straddling a row boundary is drawn once more at the
   * start of the next row, after all the planes above. Unlike above, a
   * plane whose player is in the hangar menu is not skipped here.
   */
  for (int l2 = 0; l2 < 16; l2++) {
    if (!v.wraps(player_x_8[l2] - 10, 20))
      continue;

    if (plane_flying(l2))
      draw(plane_sprite(l2),
           v.air(player_x_8[l2] - 10, player_y_8[l2] - 10, 1));
    else if (plane_crashing(l2))
      draw(plane_crash[(in_closing[l2] >> 1) - 1],
           v.air(player_x_8[l2] - 10, player_y_8[l2] - 10, 1));
  }
}

void draw_shots(const View &v) {
  if (config.shots_visible)
    for (int l2 = 0; l2 < MAX_SHOTS; l2++) {
      if (shots_flying_x[l2]) {
        const Placement p =
            v.ground(shots_flying_x[l2] >> 8, shots_flying_y[l2] >> 8);
        putpix(p.x, p.y, SHOTS_COLOR, p.clip.x1, p.clip.y1, p.clip.x2,
               p.clip.y2);
      }
    }

  if (config.it_shots_visible)
    for (int l2 = 0; l2 < MAX_ITGUN_SHOTS; l2++) {
      if (itgun_shot_x[l2]) {
        const Placement p =
            v.ground(itgun_shot_x[l2] >> 8, itgun_shot_y[l2] >> 8);
        putpix(p.x, p.y, ITGUN_SHOT_COLOR, p.clip.x1, p.clip.y1, p.clip.x2,
               p.clip.y2);
      }
    }
}

Bitmap *fobject_sprite(const flying_objects_data &o) {
  switch (o.type) {
  case FOBJECTS_SMOKE:
    return smoke[o.phase].get();

  case FOBJECTS_SSMOKE:
    return ssmoke[o.phase].get();

  case FOBJECTS_RIFLE:
    return rifle[o.phase].get();

  case FOBJECTS_FLAME:
    return config.flames ? flames[o.phase].get() : nullptr;

  case FOBJECTS_WAVE1:
    return wave1[o.phase].get();

  case FOBJECTS_WAVE2:
    return wave2[o.phase].get();

  case FOBJECTS_ITEXPLOSION:
    return itexplosion[o.phase].get();

  case FOBJECTS_EXPLOX:
    return o.phase < 0 ? nullptr : explox[o.subtype][o.phase].get();

  case FOBJECTS_PARTS:
    return bites[o.phase].get();

  default:
    return nullptr;
  }
}

void draw_fobjects(const View &v) {
  for (int l2 = 0; l2 < MAX_FLYING_OBJECTS; l2++) {
    const flying_objects_data &o = fobjects[l2];
    if (!o.x)
      continue;

    if (Bitmap *sprite = fobject_sprite(o))
      draw(sprite,
           v.air((o.x >> 8) - (o.width >> 1), (o.y >> 8) - (o.height >> 1)));
  }
}

/*
 * The three sliders of player l's hangar menu, whose top-left corner is at
 * (x, y): bombs, ammo and gas, the selected one highlighted.
 */
void draw_hangar_selector(int l, int x, int y, const Clip &clip) {
  if (hangarmenu_position[l] < 0 || hangarmenu_position[l] > 2)
    return;

  const int slider_x[3] = {35, 69, 103};
  const int slider_y[3] = {
      74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l],
      74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l],
      74 - (46 * hangarmenu_gas[l]) / plane_gas[l],
  };

  for (int i = 0; i < 3; i++) {
    const std::unique_ptr<Bitmap> &knob =
        hangarmenu_position[l] == i ? hangaractive : hangarinactive;
    knob->blit(x + slider_x[i], y + slider_y[i], clip.x1, clip.y1, clip.x2,
               clip.y2);
  }
}

/*
 * The bomb, ammo and gas icons of player l's HUD. (ox, oy) is the top-left
 * corner of the player's board. All icons are clipped to `clip`; the
 * moving gas icon additionally to the rows oy + 2 .. gas_bottom.
 */
void draw_icon_strip(int l, int ox, int oy, const Clip &clip,
                     int gas_bottom) {
  for (int l2 = 5; l2 >= 0; l2--) {
    if ((player_bombs[l] - 1) >= l2)
      break;

    bomb_icon->blit(ox + 57 + l2 * 6, oy + 1, clip.x1, clip.y1, clip.x2,
                    clip.y2);
  }

  for (int l2 = 7; l2 >= 0; l2--) {
    if (((player_ammo[l] >> 4) - 1) >= l2)
      break;

    big_ammo_icon->blit(ox + 101 + l2 * 4, oy + 1, clip.x1, clip.y1, clip.x2,
                        clip.y2);
  }

  for (int l2 = 15; l2 >= 0; l2--) {
    if ((player_ammo[l] - ((player_ammo[l] >> 4) << 4) - 1) >= l2)
      break;

    small_ammo_icon->blit(ox + 101 + l2 * 2, oy + 7, clip.x1, clip.y1,
                          clip.x2, clip.y2);
  }

  for (int l2 = 7; l2 >= 0; l2--) {
    if (((player_gas[l]) >> 8) == l2)
      break;

    gas_icon->blit(ox + 1 + l2 * 3, oy + 2, clip.x1, clip.y1, clip.x2,
                   clip.y2);
  }

  gas_icon->blit(ox + 1 + (player_gas[l] >> 8) * 3,
                 oy + 2 - ((player_gas[l] - ((player_gas[l] >> 8) << 8))) / 32,
                 clip.x1, oy + 2, clip.x2, gas_bottom);
}

void draw_points(int x, int y, int points) {
  fontti->printf(x, y, "%3d", abs(points));
  if (points < 0)
    fontti->printf(x, y, "-");
}

/*
 * The solo game's extra HUD: planes left, power switch and the blinking
 * mission status. oy is the top of the board. This also advances the
 * status blink, so it runs once per frame.
 */
void draw_solo_status(int oy) {
  for (int l2 = 0; l2 <= number_of_planes[solo_country]; l2++)
    picons[solo_country]->blit(181 + l2 * 10, oy + 1);

  if (controls_power[solo_country])
    pwon->blit(163, oy + 2);
  else
    pwoff->blit(163, oy + 6);

  if (++status_frames > STATUS_SPEED) {
    status_frames = 0;
    if (status_state)
      status_state = 0;
    else
      status_state = 1;
  }

  if (solo_failed) {
    status_icons[0][status_state]->blit(285, oy + 1);

  } else {
    if (solo_success) {
      status_icons[1][status_state]->blit(285, oy + 1);
    }
  }
}

/* The hangar menu slides down over a closing player's viewport. */
void advance_in_closing() {
  for (int l = 0; l < 16; l++) {
    if (!player_exists[l])
      continue;

    if ((in_closing[l] > 0) && (in_closing[l] <= 86))
      in_closing[l] += 2;
  }
}

/* Players whose hangar menu has come all the way down start over. */
void reset_closed_players() {
  for (int l = 0; l < 16; l++)
    if (in_closing[l] >= 88) {
      init_player(l, 1);
    }
}

#ifdef COMPUDEBUG
void compudebug_report(int x, int y) {
  for (int l = 0; l < 4; l++) {
    frost->printf(x, y + 12 * l,
                  "Plane %d: Mission:%d Phase:%d Target:%d Up:%d Down:%d "
                  "Power:%d Speed:%d Angle:%d Dst:%d A:%d",
                  l, current_mission[l], mission_phase[l], mission_target[l],
                  controls_up[l], controls_down[l], controls_power[l],
                  player_speed[l], player_angle[l] >> 8, distances[l],
                  angles[l]);
  }
}
#endif

} // namespace

void tboxi(int x1, int y1, int x2, int y2, unsigned char vari) {
  if (x2 < 5 || y2 < 8 || x1 > 315 || y1 > 178)
    return;

  if (x2 > 315)
    x2 = 315;

  if (y2 > 178)
    y2 = 178;

  if (x1 < 5)
    x1 = 5;

  if (y1 < 8)
    y1 = 8;

  draw_line(x1, y1, x2, y1, vari);
  draw_line(x1, y1, x1, y2, vari);
  draw_line(x2, y2, x2, y1, vari);
  draw_line(x2, y2, x1, y2, vari);
}

/* The four-player 320x200 split screen. */
void terrain_to_screen() {
  int l;

  advance_in_closing();

  for (l = 0; l < 4; l++) {
    if (!player_exists[l])
      continue;

    if (!hangarmenu_active[l]) {
      const View v{View::Kind::Split, l};

      player_shown_x[l] = 80 - (player_x_speed[l] / 5500);
      player_shown_y[l] = 45 + (player_y_speed[l] / 10000);

      if (((player_x_8[l]) - player_shown_x[l] + 160) > NUMBER_OF_SCENES * 160)
        player_shown_x[l] -= NUMBER_OF_SCENES * 160 -
                             ((player_x_8[l]) - player_shown_x[l] + 160);

      if (((player_y_8[l]) - player_shown_y[l] + 90) > 200)
        player_shown_y[l] -= 200 - ((player_y_8[l]) - player_shown_y[l] + 90);

      if (((player_x_8[l]) - player_shown_x[l]) < 0)
        player_shown_x[l] += ((player_x_8[l]) - player_shown_x[l]);

      if (((player_y_8[l]) - player_shown_y[l]) < 0)
        player_shown_y[l] += ((player_y_8[l]) - player_shown_y[l]);

      /* The landscape itself, clipped like the airborne sprites. */
      draw(maisema, v.air(0, 0));

      draw_structures(v);
      draw_hangar_doors(v);
      draw_flags(v);
      draw_aa_guns(v);
      draw_infantry(v);
      draw_mechanics(v);
      draw_bombs(v);
      draw_planes(v);
      draw_shots(v);
      draw_fobjects(v);
    }

    const int board_x = 2 + x_muutos[l];
    const int board_y = 90 + (l / 2) * 98;

    boards[l]->blit(board_x, board_y);
    draw_icon_strip(l, board_x, board_y, vga_screen, board_y + 9);

    if (computer_active[l]) {
      closed->blit(2 + x_muutos[l], 2 + y_muutos[l] + in_closing[l] - 88,
                   x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]);
    } else {
      if (in_closing[l]) {
        const Clip clip{x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]};
        const int top = y1_raja[l] + in_closing[l] - 88;

        hangarmenu->blit(x1_raja[l], top, clip.x1, clip.y1, clip.x2, clip.y2);
        flags[l][7]->blit(x1_raja[l] + 14, top + 67, clip.x1, clip.y1,
                          clip.x2, clip.y2);
        draw_hangar_selector(l, x1_raja[l], top, clip);

      } else if (hangarmenu_active[l]) {

        hangarmenu->blit(x1_raja[l], y1_raja[l], x1_raja[l], y1_raja[l],
                         x2_raja[l], y2_raja[l]);
        flags[l][7]->blit(x1_raja[l] + 14, y1_raja[l] + 67);
        draw_hangar_selector(l, x1_raja[l], y1_raja[l], vga_screen);
      }
    }

    draw_points(144 + x_muutos[l], 93 + (l / 2) * 98, get_player_points(l));
  }

  reset_closed_players();
}

/* The single-player 320x200 screen of the solo game. */
void solo_terrain_to_screen() {
  int l;

  advance_in_closing();

  l = solo_mode;
  const View v{View::Kind::Solo, l};

  player_shown_x[l] = 160;
  player_shown_y[l] = player_y[l] >> 8;

  if (((player_x_8[l]) - player_shown_x[l] + 320) > NUMBER_OF_SCENES * 160)
    player_shown_x[l] -=
        NUMBER_OF_SCENES * 160 - ((player_x_8[l]) - player_shown_x[l] + 320);

  if (((player_x_8[l]) - player_shown_x[l]) < 0)
    player_shown_x[l] += ((player_x_8[l]) - player_shown_x[l]);

  boards[l]->blit(0, 0);
  board2->blit(158, 0);
  draw_solo_status(0);
  draw_icon_strip(l, 0, 0, vga_screen, 9);
  draw_points(142, 3, player_points[l]);

  draw_structures(v);
  draw_hangar_doors(v);
  draw_flags(v);
  draw_aa_guns(v);

  if (hangarmenu_active[l]) {

    hangarmenu->blit(0, 15);
    flags[solo_country][7]->blit(14, 15 + 67);

    if (!hangarmenu_max_bombs[solo_country])
      hruks->blit(31, 8 + 15);

    if (!hangarmenu_max_ammo[solo_country])
      hruks->blit(65, 8 + 15);

    draw_hangar_selector(l, 0, 15, vga_screen);
  }

  draw_infantry(v);
  draw_mechanics(v);
  draw_bombs(v);
  draw_planes(v);
  draw_shots(v);
  draw_fobjects(v);

  reset_closed_players();
#ifdef COMPUDEBUG
  compudebug_report(5, 10);
#endif
}

/* The level overview of the mission menu, scrolled to left_x. */
void kangas_terrain_to_screen(int left_x) {
  int l2, l, l3;
  static int color_frame = 0;

  if (color_frame++ >= 3)
    color_frame = 0;

  do_flags();

  maisema->blit(left_x, -PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1, PROJECTOR_Y1,
                PROJECTOR_X2, PROJECTOR_Y2);

  for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {

    if ((structures[l2][struct_state[l2]] != nullptr) &&
        (!leveldata.struct_hit[l2])) {
      structures[l2][0]->blit((leveldata.struct_x[l2]) + left_x,
                              leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT,
                              PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2,
                              PROJECTOR_Y2);
    }
  }

  for (l2 = 0; l2 < MAX_FLAGS; l2++) {
    if (flags_x[l2]) {

      flags[flags_owner[l2]][flags_frame[l2]]->blit(
          flags_x[l2] + left_x, flags_y[l2] - PROJECTOR_Y_ADJUSTMENT,
          PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);
    }
  }

  for (l2 = 0; l2 < MAX_AA_GUNS; l2++) {
    if (!kkbase_x[l2])
      continue;

    if (kkbase_status[l2] != 2)
      kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2]]->blit(
          kkbase_x[l2] + left_x, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT,
          PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);
    else
      kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2] >> 1]->blit(
          kkbase_x[l2] + left_x, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT,
          PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);
  }

  /*
   * Mission targets get three nested frames, each one pixel further out
   * (l3 = 2, 3, 4), in the three colours base + 0..2. The colours rotate
   * one step every call: the innermost frame has colour
   * base + color_frame % 3, the next one base + (color_frame + 1) % 3, and
   * so on. (color_frame runs 0..3; 3 is the same as 0 here.)
   */
  for (l2 = 0; l2 < MAX_AA_GUNS; l2++) {
    if (!kkbase_x[l2])
      continue;

    if (kkbase_mission[l2]) {
      l3 = 2;

      for (int i = 0; i < 3; i++) {
        l = (color_frame + i) % 3;
        world::terrain::tboxi((kkbase_x[l2]) + left_x - l3,
                              kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3,
                              (kkbase_x[l2]) + left_x + 25 + l3,
                              kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT + 21 + l3,
                              (kkbase_mission[l2] - 1) * 7 + 240 + l);
        l3++;
      }
    }
  }

  for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {

    if ((structures[l2][struct_state[l2]] != nullptr) &&
        (!leveldata.struct_hit[l2])) {

      if (leveldata.struct_type[l2]) {
        l3 = 2;

        for (int i = 0; i < 3; i++) {
          l = (color_frame + i) % 3;
          world::terrain::tboxi(
              (leveldata.struct_x[l2]) + left_x - l3,
              leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3,
              (leveldata.struct_x[l2]) + left_x + struct_width[l2] + l3 - 1,
              leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT +
                  struct_heigth[l2] + l3,
              (leveldata.struct_type[l2] - 1) * 7 + 240 + l);
          l3++;
        }
      }
    }
  }
}

/*
 * The 800x600 screen, where the whole world is visible as rows of
 * screen_width pixels. The landscape and the structures are already in the
 * background; only the moving parts are drawn here.
 */
void vesa_terrain_to_screen() {
  const View v{View::Kind::Svga, 0};
  int l;

  advance_in_closing();

  draw_hangar_doors(v);
  draw_flags(v);
  draw_aa_guns(v);

  for (l = 0; l < 4; l++)
    if (hangarmenu_active[l]) {
      hangarmenu->blit(200 * l + 20, 0);
      flags[l][7]->blit(200 * l + 20 + 14, 67);
      draw_hangar_selector(l, 200 * l + 20, 0, vga_screen);
    }

  draw_infantry(v);
  draw_mechanics(v);
  draw_planes(v);
  draw_shots(v);
  draw_bombs(v);
  draw_fobjects(v);

  // Icons
  if (playing_solo) {
    world::terrain::vesa_solo_icons(solo_mode);
  } else
    for (l = 0; l < 4; l++) {
      world::terrain::vesa_icons(l);
    }

  reset_closed_players();
#ifdef COMPUDEBUG
  compudebug_report(50, 20);
#endif
}

/* Player l's HUD at the bottom of the 800x600 screen (four-player game). */
void vesa_icons(int l) {
  const Clip screen{0, 0, screen_width_less, screen_height_less};
  const int board_x = l * 160;
  const int board_y = screen_height - 12;

  boards[l]->blit(board_x, board_y, screen.x1, screen.y1, screen.x2,
                  screen.y2);
  draw_icon_strip(l, board_x, board_y, screen, screen_height_less);
  draw_points(board_x + 142, board_y + 3, get_player_points(l));
}

/* The solo player's HUD at the bottom of the 800x600 screen. */
void vesa_solo_icons(int l) {
  const Clip screen{0, 0, screen_width_less, screen_height_less};
  const int board_y = screen_height - 12;

  boards[l]->blit(0, board_y, screen.x1, screen.y1, screen.x2, screen.y2);
  board2->blit(158, board_y, screen.x1, screen.y1, screen.x2, screen.y2);

  draw_icon_strip(l, 0, board_y, screen, screen_height_less);
  draw_points(142, board_y + 3, player_points[l]);
  draw_solo_status(board_y);
}

} // namespace world::terrain
