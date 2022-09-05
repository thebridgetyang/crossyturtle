#include "body.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "star.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// window constants
const vector_t WINDOW = (vector_t){.x = 1000, .y = 500};
const vector_t CENTER = (vector_t){.x = 500, .y = 250};
const double MAX_TIME_OFFLINE = 0.1;
const double MIN_TIME_CLICK = 0.1;
#define MAX_SND_CHANNELS 8

const rgb_color_t NULL_COLOR = (rgb_color_t){0, 0, 0};
const size_t ROAD_COUNT = 6;
const size_t ROAD_WIDTH = 11;
const double PAN_SPEED = 30;

// car constants
const double CAR_HEIGHT = 60;
const double CAR_WIDTH = 80;
const double CAR_SPEED = 160;
const double CAR_WEIGHT = 3;
const size_t MAX_INIT_CAR = 3;
const double CAR_FREQ = 0.08;

// coin constants
const double COIN_HEIGHT = 20;
const double COIN_WIDTH = 20;
const size_t MAX_INIT_COIN = 1;

// heart constants
const double HEART_HEIGHT = 40;
const double HEART_WIDTH = 40;
const size_t MAX_INIT_HEART = 1;
const double HEART_FREQ = 0.06;

// ghost constants
const double GHOST_HEIGHT = 40;
const double GHOST_WIDTH = 50;
const size_t MAX_INIT_GHOST = 1;
const double GHOST_FREQ = 0.04;
const double INVINCIBILITY_TIME = 5.0;

// rock constants
const double ROCK_WIDTH = 60;
const double ROCK_HEIGHT = 60;
const size_t MAX_INIT_ROCK = 3;

// turtle constants
const size_t DEFAULT_TURTLE_INDEX = 4;
const double TURTLE_WIDTH = 50;
const double TURTLE_HEIGHT = 50;
const double TURTLE_WEIGHT = 1;
const double STANDARD_SPEED = 100;
const double RESPAWN_INVS_TIME = 2.0;
const double MAX_ANG_VEL = 1;

// window constants
const double FONT_SIZE = 24;
const vector_t COIN_COUNTER_CENTROID = {900, 20};
const vector_t LIVES_COUNTER_CENTROID = {100, 20};
const vector_t LIVES_COUNTER_NUM_CENTROID = {170, 20};
const vector_t COINS_COUNTER_NUM_CENTROID = {940, 20};
const double COUNTER_HEIGHT = 40;
const double COIN_COUNT_WIDTH = 50;
const double LIVES_COUNT_WIDTH = 110;
const double LIVES_COUNT_NUM_WIDTH = 30;
const double COINS_COUNT_NUM_WIDTH = 30;

// end screen constants
const vector_t FINAL_POINTS = {460, 270};
const double END_SCREEN_MULTIPLIER = 2.2;
const vector_t END_COIN_POSITION = {530, 267};

typedef enum { TITLE, INSTRUCTIONS, GAME, GAME_OVER } game_screen_t;

// Body types define order in which body is drawn
typedef enum {
  ROAD, // ROAD TYPES
  GRASS,
  CAR,          // CAR TYPES
  COIN,         // COIN TYPES
  HEART,        // HEART TYPES
  GHOST,        // INVINCIBILITY POWER UP
  ROCK,         // OBSTACLE TYPES
  TURTLE,       // USER
  TURTLE_CLONE, // USER
  COUNTER,      // COIN AND LIVES
  SCREEN,       // START/END SCREEN
} body_type_t;

body_type_t *make_type_info(body_type_t type) {
  body_type_t *info = malloc(sizeof(*info));
  *info = type;
  return info;
}

body_type_t get_type(body_t *body) {
  return *(body_type_t *)body_get_info(body);
}

typedef struct state {
  scene_t *scene;
  double total_time;
  double time_since_last_click;
  double time_since_last_inv;
  game_screen_t game_screen;
  Mix_Music *music;
  Mix_Chunk **sounds;
  int count_lives;
  int count_coins;
  TTF_Font *font;
} state_t;

void render_frame(state_t *state) {
  sdl_clear();
  for (int i = ROAD; i <= SCREEN; i++) {
    for (int j = 0; j < scene_bodies(state->scene); j++) {
      if (get_type(scene_get_body(state->scene, j)) == (body_type_t)i)
        sdl_draw_sprite(scene_get_body(state->scene, j));
    }
  }
  sdl_show();
}

// Sound enum
enum { SND_WALK, SND_DEATH, SND_MAX };

void play_sound(state_t *state, int id, int channel) {
  Mix_PlayChannel(channel, state->sounds[id], 0);
}

double get_rand() { return (double)rand() / RAND_MAX; }

double get_random_column() {
  return (double)CENTER.x + WINDOW.y / ROAD_COUNT *
                                (floor((double)ROAD_WIDTH * get_rand()) -
                                 floor((double)ROAD_WIDTH / 2));
}

void init_start_screen(scene_t *scene) {
  list_t *rect = rectangle_init(WINDOW.y, WINDOW.x, CENTER);
  SDL_Surface *start_image = IMG_Load("assets/start_screen.png");
  body_t *start_screen =
      body_init_with_info(rect, 0, NULL_COLOR, start_image, WINDOW.y, WINDOW.x,
                          make_type_info(SCREEN), free);
  scene_add_body(scene, start_screen);
}

void init_directions_screen(scene_t *scene) {
  list_t *rect = rectangle_init(WINDOW.y, WINDOW.x, CENTER);
  SDL_Surface *directions_image = IMG_Load("assets/directions.png");
  body_t *directions_screen =
      body_init_with_info(rect, 0, NULL_COLOR, directions_image, WINDOW.y,
                          WINDOW.x, make_type_info(SCREEN), free);
  scene_add_body(scene, directions_screen);
}

void init_counters(state_t *state) {
  list_t *shape = rectangle_init(50, 50, (vector_t){100, 100});
  SDL_Color color = {0, 0, 0};
  SDL_Surface *coin = TTF_RenderText_Solid(state->font, "Coins:", color);
  body_t *coin_counter =
      body_init_with_info(shape, 0, NULL_COLOR, coin, COUNTER_HEIGHT,
                          COIN_COUNT_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(coin_counter, COIN_COUNTER_CENTROID);
  scene_add_body(state->scene, coin_counter);

  SDL_Surface *lives = TTF_RenderText_Solid(state->font, "Remaining Lives:", color);
  body_t *lives_counter = body_init_with_info(
      shape, 0, (rgb_color_t){0, 1, 0}, lives, COUNTER_HEIGHT,
      LIVES_COUNT_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(lives_counter, LIVES_COUNTER_CENTROID);
  scene_add_body(state->scene, lives_counter);

  int num_lives = state->count_lives;
  char str[10];
  sprintf(str, "%8d", num_lives);
  SDL_Surface *lives_num = TTF_RenderText_Solid(state->font, str, color);
  body_t *lives_counter_num = body_init_with_info(
      shape, 0, (rgb_color_t){0, 1, 0}, lives_num, COUNTER_HEIGHT,
      LIVES_COUNT_NUM_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(lives_counter_num, LIVES_COUNTER_NUM_CENTROID);
  scene_add_body(state->scene, lives_counter_num);

  int num_coins = state->count_coins;
  char str_coin[10];
  sprintf(str_coin, "%8d", num_coins);
  SDL_Surface *coins_num = TTF_RenderText_Solid(state->font, str_coin, color);
  body_t *coins_counter_num = body_init_with_info(
      shape, 0, (rgb_color_t){0, 1, 0}, coins_num, COUNTER_HEIGHT,
      COINS_COUNT_NUM_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(coins_counter_num, COINS_COUNTER_NUM_CENTROID);
  scene_add_body(state->scene, coins_counter_num);
}

void update_lives_display(state_t *state) {
  list_t *shape = rectangle_init(50, 50, (vector_t){100, 100});
  SDL_Color color = {0, 0, 0};
  int num_lives = state->count_lives;
  char str[10];
  sprintf(str, "%8d", num_lives);

  SDL_Surface *lives_num = TTF_RenderText_Solid(state->font, str, color);
  body_t *lives_counter_num = body_init_with_info(
      shape, 0, (rgb_color_t){0, 1, 0}, lives_num, COUNTER_HEIGHT,
      LIVES_COUNT_NUM_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(lives_counter_num, LIVES_COUNTER_NUM_CENTROID);
  scene_set_body(state->scene, lives_counter_num, 2);
}

void update_coins_display(state_t *state) {
  list_t *shape = rectangle_init(50, 50, (vector_t){100, 100});
  SDL_Color color = {0, 0, 0};
  int num_coins = state->count_coins;
  char str[10];
  sprintf(str, "%8d", num_coins);

  SDL_Surface *coins_num = TTF_RenderText_Solid(state->font, str, color);
  body_t *coins_counter_num = body_init_with_info(
      shape, 0, (rgb_color_t){0, 1, 0}, coins_num, COUNTER_HEIGHT,
      COINS_COUNT_NUM_WIDTH, make_type_info(COUNTER), free);
  body_set_centroid(coins_counter_num, COINS_COUNTER_NUM_CENTROID);
  scene_set_body(state->scene, coins_counter_num, 3);
}

void set_user_direction(body_t *user, double angle) {
  body_set_rotation(user, angle + M_PI / 2);
}

void move_user(body_t *user, double angle, scene_t *scene) {
  body_set_centroid(user,
                    vec_add(body_get_centroid(user),
                            (vector_t){cos(angle) * WINDOW.y / ROAD_COUNT,
                                       sin(angle) * WINDOW.y / ROAD_COUNT}));

  // Checks if moving was allowed
  if (body_get_centroid(user).x - TURTLE_WIDTH < 0 ||
      body_get_centroid(user).x + TURTLE_WIDTH > WINDOW.x ||
      body_get_centroid(user).y - TURTLE_HEIGHT < 0 ||
      body_get_centroid(user).y + TURTLE_HEIGHT > WINDOW.y) {
    body_set_centroid(user,
                      vec_add(body_get_centroid(user),
                              (vector_t){-cos(angle) * WINDOW.y / ROAD_COUNT,
                                         -sin(angle) * WINDOW.y / ROAD_COUNT}));
    return;
  }
  list_t *shape = body_get_shape(user);
  list_t *shape_2;
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    if (scene_get_body(scene, i) == user)
      continue;
    switch (get_type(scene_get_body(scene, i))) {
    case ROCK:
      shape_2 = body_get_shape(scene_get_body(scene, i));
      if (find_collision(shape, shape_2).collided)
        body_set_centroid(
            user, vec_add(body_get_centroid(user),
                          (vector_t){-cos(angle) * WINDOW.y / ROAD_COUNT,
                                     -sin(angle) * WINDOW.y / ROAD_COUNT}));
      list_free(shape_2);
      break;
    default:
      break;
    }
  }
  list_free(shape);
}

void turtle_key_handler(char key, key_event_type_t type, double held_time,
                        state_t *state) {
  if (state->time_since_last_click < MIN_TIME_CLICK)
    return;
  state->time_since_last_click = 0;
  if (scene_bodies(state->scene) <= DEFAULT_TURTLE_INDEX ||
      get_type(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX)) != TURTLE ||
      type != KEY_PRESSED)
    return;

  switch (key) {
  case RIGHT_ARROW:
    set_user_direction(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), 0);
    move_user(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), 0,
              state->scene);
    play_sound(state, SND_WALK, SND_WALK);
    break;
  case LEFT_ARROW:
    set_user_direction(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                       M_PI);
    move_user(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), M_PI,
              state->scene);
    play_sound(state, SND_WALK, SND_WALK);
    break;
  case DOWN_ARROW:
    set_user_direction(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                       M_PI / 2);
    move_user(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), M_PI / 2,
              state->scene);
    play_sound(state, SND_WALK, SND_WALK);
    break;
  case UP_ARROW:
    set_user_direction(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                       3 * M_PI / 2);
    move_user(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), 3 * M_PI / 2,
              state->scene);
    double v_dist_from_center =
        CENTER.y -
        body_get_centroid(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX)).y;
    if (v_dist_from_center > 0) {
      for (size_t i = DEFAULT_TURTLE_INDEX; i < scene_bodies(state->scene);
           i++) {
        body_set_centroid(
            scene_get_body(state->scene, i),
            vec_add(body_get_centroid(scene_get_body(state->scene, i)),
                    (vector_t){0, v_dist_from_center}));
      }
    }
    play_sound(state, SND_WALK, SND_WALK);
    break;
  default:
    break;
  }
}

void display_endscreen(state_t *state) {
  // end screen background
  state->scene = scene_init();
  list_t *rect = rectangle_init(WINDOW.y, WINDOW.x, CENTER);
  SDL_Surface *end_image = IMG_Load("assets/endscreen.png");
  body_t *end_screen =
      body_init_with_info(rect, 0, NULL_COLOR, end_image, WINDOW.y, WINDOW.x,
                          make_type_info(SCREEN), free);
  scene_add_body(state->scene, end_screen);

  // display number of points
  SDL_Color color = {0, 0, 0};
  int num_coins = state->count_coins;
  char str[10];
  sprintf(str, "%8d", num_coins);
  SDL_Surface *num_points = TTF_RenderText_Solid(state->font, str, color);
  body_t *points = body_init_with_info(
      rect, 0, NULL_COLOR, num_points, END_SCREEN_MULTIPLIER *
      COUNTER_HEIGHT, END_SCREEN_MULTIPLIER * COINS_COUNT_NUM_WIDTH,
      make_type_info(SCREEN), free);
  body_set_centroid(points, FINAL_POINTS);
  scene_add_body(state->scene, points);

  SDL_Surface *coin_img = IMG_Load("assets/coin.png");
  body_t *coin = body_init_with_info(
      rect, 1, NULL_COLOR, coin_img, END_SCREEN_MULTIPLIER * COIN_WIDTH,
      END_SCREEN_MULTIPLIER * COIN_HEIGHT, make_type_info(SCREEN), free);
  body_set_centroid(coin, END_COIN_POSITION);
  scene_add_body(state->scene, coin);
}

void make_inv_turtle(state_t *state) {
  body_set_image(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                 IMG_Load("assets/turtle_inv.png"));
  state->time_since_last_inv = 0;
}

void reset_turtle(state_t *state) {
  body_set_image(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                 IMG_Load("assets/turtle.png"));
}

void check_user_offscreen(state_t *state) {
  if (body_get_centroid(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX)).y +
          TURTLE_HEIGHT >
      WINDOW.y) {
    vector_t new_center = {
        body_get_centroid(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX)).x,
        body_get_centroid(
            scene_get_body(state->scene, scene_bodies(state->scene) - 1))
                .y +
            ceil((double)ROAD_COUNT / 2) * WINDOW.y / ROAD_COUNT};
    body_set_centroid(scene_get_body(state->scene, DEFAULT_TURTLE_INDEX),
                      new_center);
    make_inv_turtle(state);
    state->count_lives -= 1;
    play_sound(state, SND_DEATH, SND_DEATH);
    update_lives_display(state);
  }
}

void get_coin(body_t *user, body_t *coin, vector_t axis, void *aux) {
  body_remove(coin);
  state_t *state = aux;
  state->count_coins += 1;
  update_coins_display(state);
}

void get_heart(body_t *user, body_t *heart, vector_t axis, void *aux) {
  body_remove(heart);
  state_t *state = aux;
  state->count_lives += 1;
  update_lives_display(state);
}

// invincibility powerup
void get_ghost(body_t *user, body_t *ghost, vector_t axis, void *aux) {
  body_remove(ghost);
  state_t *state = aux;
  make_inv_turtle(state);
  if (state->time_since_last_inv > INVINCIBILITY_TIME)
    reset_turtle(state);
}

void car_crash(body_t *user, body_t *car, vector_t axis, void *aux) {
  state_t *state = aux;
  if (state->time_since_last_inv < RESPAWN_INVS_TIME)
    return;
  state->count_lives -= 1;
  play_sound(state, SND_DEATH, SND_DEATH);
  update_lives_display(state);
  make_inv_turtle(state);

  list_t *rect =
      rectangle_init(TURTLE_WIDTH, TURTLE_HEIGHT, body_get_centroid(user));
  SDL_Surface *turtle_image = IMG_Load("assets/turtle.png");
  body_t *turtle_clone = body_init_with_info(
      rect, TURTLE_WEIGHT, NULL_COLOR, turtle_image, TURTLE_WIDTH,
      TURTLE_HEIGHT, make_type_info(TURTLE_CLONE), free);
  double rand_angle = get_rand() * 2 * M_PI;
  body_set_velocity(turtle_clone,
                    vec_multiply(CAR_WEIGHT * CAR_SPEED / TURTLE_WEIGHT,
                                 (vector_t){cos(rand_angle), sin(rand_angle)}));
  body_set_angular_velocity(turtle_clone,
                            2 * MAX_ANG_VEL * rand() - MAX_ANG_VEL);
  scene_add_body(state->scene, turtle_clone);
}

void add_turtle(scene_t *scene) {
  list_t *rect = rectangle_init(TURTLE_WIDTH, TURTLE_HEIGHT, CENTER);
  SDL_Surface *turtle_image = IMG_Load("assets/turtle.png");
  body_t *turtle = body_init_with_info(
      rect, TURTLE_WEIGHT, NULL_COLOR, turtle_image, TURTLE_WIDTH,
      TURTLE_HEIGHT, make_type_info(TURTLE), free);
  body_set_velocity(turtle, (vector_t){0, PAN_SPEED});
  scene_add_body(scene, turtle);
}

void add_obstacle(state_t *state, vector_t center, body_type_t *body_type) {
  SDL_Surface *surface = NULL;
  list_t *rect;
  body_t *body;
  switch (*body_type) {
  case CAR:
    if (get_rand() > 0.5) {
      surface = IMG_Load("assets/car.png");
    } else {
      surface = IMG_Load("assets/car1.png");
    }
    rect = rectangle_init(CAR_HEIGHT, CAR_WIDTH, center);
    body = body_init_with_info(rect, 250, NULL_COLOR, surface, CAR_HEIGHT,
                               CAR_WIDTH, make_type_info(CAR), free);
    body_set_velocity(body, (vector_t){CAR_SPEED, PAN_SPEED});
    break;
  case COIN:
    surface = IMG_Load("assets/coin.png");
    rect = rectangle_init(COIN_WIDTH, COIN_HEIGHT, center);
    body = body_init_with_info(rect, 1, NULL_COLOR, surface, COIN_WIDTH,
                               COIN_HEIGHT, make_type_info(COIN), free);
    body_set_velocity(body, (vector_t){0, PAN_SPEED});
    break;
  case HEART:
    surface = IMG_Load("assets/heart.png");
    rect = rectangle_init(HEART_HEIGHT, HEART_WIDTH, center);
    body = body_init_with_info(rect, 1, NULL_COLOR, surface, HEART_HEIGHT,
                               HEART_WIDTH, make_type_info(HEART), free);
    body_set_velocity(body, (vector_t){0, PAN_SPEED});
    break;
  case GHOST:
    surface = IMG_Load("assets/ghost.png");
    rect = rectangle_init(GHOST_HEIGHT, GHOST_WIDTH, center);
    body = body_init_with_info(rect, 1, NULL_COLOR, surface, GHOST_HEIGHT,
                               GHOST_WIDTH, make_type_info(GHOST), free);
    body_set_velocity(body, (vector_t){0, PAN_SPEED});
    break;
  case ROCK:
    surface = IMG_Load("assets/rock.png");
    rect = rectangle_init(ROCK_WIDTH, ROCK_HEIGHT, center);
    body = body_init_with_info(rect, 1, NULL_COLOR, surface, ROCK_WIDTH,
                               ROCK_HEIGHT, make_type_info(ROCK), free);
    body_set_velocity(body, (vector_t){0, PAN_SPEED});
    break;
  default:
    return;
  }

  // Add object if not colliding with exiting onstacle type object
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    if (get_type(scene_get_body(state->scene, i)) < CAR ||
        get_type(scene_get_body(state->scene, i)) > TURTLE)
      continue;
    list_t *shape = body_get_shape(scene_get_body(state->scene, i));
    if (find_collision(rect, shape).collided) {
      free(shape);
      body_free(body);
      return;
    }
    free(shape);
  }
  scene_add_body(state->scene, body);
  switch (*body_type) {
  case CAR:
    create_collision(state->scene,
                     scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), body,
                     car_crash, state, NULL);
    break;
  case COIN:
    create_collision(state->scene,
                     scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), body,
                     get_coin, state, NULL);
    break;
  case HEART:
    create_collision(state->scene,
                     scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), body,
                     get_heart, state, NULL);
    break;
  case GHOST:
    create_collision(state->scene,
                     scene_get_body(state->scene, DEFAULT_TURTLE_INDEX), body,
                     get_ghost, state, NULL);
    break;
  default:
    return;
  }
}

void add_rand_car(state_t *state) {

  if (get_rand() < CAR_FREQ) {
    size_t r = floor(get_rand() * scene_bodies(state->scene));
    if (get_type(scene_get_body(state->scene, r)) == (body_type_t)(ROAD)) {
      vector_t road_center = (vector_t){
          -5.0, (body_get_centroid(scene_get_body(state->scene, r))).y};
      add_obstacle(state, road_center, make_type_info(CAR));
    }
  }
}

void add_road(state_t *state, vector_t center, body_type_t *body_type) {
  list_t *rect = rectangle_init(WINDOW.y / ROAD_COUNT, WINDOW.x, center);
  SDL_Surface *road_surface;
  switch (*body_type) {
  case ROAD:
    road_surface = IMG_Load("assets/road.png");
    for (size_t i = 0; i < MAX_INIT_CAR; i++)
      add_obstacle(state, (vector_t){get_random_column(), center.y},
                   make_type_info(CAR));
    break;
  case GRASS:
    road_surface = IMG_Load("assets/grass.png");
    for (size_t i = 0; i < MAX_INIT_ROCK; i++)
      add_obstacle(state, (vector_t){get_random_column(), center.y},
                   make_type_info(ROCK));
    break;
  default:
    road_surface = NULL;
    break;
  }
  for (size_t i = 0; i < MAX_INIT_COIN; i++)
    add_obstacle(state, (vector_t){get_random_column(), center.y},
                 make_type_info(COIN));
  for (size_t i = 0; i < MAX_INIT_HEART; i++)
    if (get_rand() < HEART_FREQ) {
      add_obstacle(state, (vector_t){get_random_column(), center.y},
                   make_type_info(HEART));
    }
  for (size_t i = 0; i < MAX_INIT_GHOST; i++)
    if (get_rand() < GHOST_FREQ) {
      add_obstacle(state, (vector_t){get_random_column(), center.y},
                   make_type_info(GHOST));
    }
  body_t *new_road =
      body_init_with_info(rect, INFINITY, NULL_COLOR, road_surface,
                          WINDOW.y / ROAD_COUNT + 2, WINDOW.x, body_type, free);
  body_set_velocity(new_road, (vector_t){0, PAN_SPEED});
  scene_add_body(state->scene, new_road);
}

void add_rand_road(state_t *state, vector_t center) {
  body_type_t *body_type =
      make_type_info(ROAD + (GRASS - ROAD + 1) * get_rand());
  add_road(state, center, body_type);
}

void delete_offscreen_body(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (body_get_centroid(body).y < 0)
      continue;
    list_t *shape = body_get_shape(body);
    list_t *screen = rectangle_init(WINDOW.y, WINDOW.x, CENTER);
    if (!find_collision(shape, screen).collided) {
      switch (get_type(body)) {
      case TURTLE:
        break;
      case ROAD:
        body_remove(body);
        add_rand_road(state,
                      vec_subtract(body_get_centroid(body),
                                   (vector_t){0, WINDOW.y * (ROAD_COUNT + 1) /
                                                     ROAD_COUNT}));
        break;
      case GRASS:
        body_remove(body);
        add_rand_road(state,
                      vec_subtract(body_get_centroid(body),
                                   (vector_t){0, WINDOW.y * (ROAD_COUNT + 1) /
                                                     ROAD_COUNT}));
        break;
      default:
        body_remove(body);
        break;
      }
    }
    list_free(shape);
    list_free(screen);
  }
}

// initialize the state
state_t *emscripten_init() {
  vector_t min = VEC_ZERO;
  vector_t max = WINDOW;
  sdl_init(min, max);

  state_t *state = malloc(sizeof(state_t));
  state->game_screen = TITLE;
  state->count_lives = 3;
  state->count_coins = 0;
  state->time_since_last_click = 0;
  state->time_since_last_inv = 0;
  state->scene = scene_init();
  state->font = TTF_OpenFont("assets/font.ttf", FONT_SIZE);;
  init_start_screen(state->scene);

  sdl_show();
  sdl_on_key((key_handler_t)turtle_key_handler);

  // Sound init
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Couldn't initialize SDL Mixer\n");
    exit(1);
  }
  Mix_AllocateChannels(MAX_SND_CHANNELS);
  state->music = Mix_LoadMUS("assets/background.ogg");
  Mix_PlayMusic(state->music, -1); // -1 for loop
  state->sounds = malloc(sizeof(Mix_Chunk *) * SND_MAX);
  state->sounds[SND_WALK] = Mix_LoadWAV("assets/step.ogg");
  state->sounds[SND_DEATH] = Mix_LoadWAV("assets/death.ogg");

  return state;
}

void game_init(state_t *state) {
  scene_free(state->scene);
  state->scene = scene_init();
  state->count_lives = 3;
  state->count_coins = 0;
  state->time_since_last_inv = 0;
  init_counters(state);
  add_turtle(state->scene);
  for (int i = 0; i <= ROAD_COUNT; i++) {
    add_road(state, (vector_t){CENTER.x, i * WINDOW.y / ROAD_COUNT},
             make_type_info(GRASS));
  }
  state->game_screen = GAME;
}

// run the state
void emscripten_main(state_t *state) {
  if (state->game_screen == GAME) {
    if (state->count_lives <= 0) {
      display_endscreen(state);
      state->game_screen = GAME_OVER;
      return;
    }
    double time_elapsed = time_since_last_tick();
    if (time_elapsed > MAX_TIME_OFFLINE)
      return;
    state->total_time += time_elapsed;
    state->time_since_last_click += time_elapsed;
    state->time_since_last_inv += time_elapsed;
    add_rand_car(state);
    if (state->time_since_last_inv > RESPAWN_INVS_TIME)
      reset_turtle(state);
    scene_tick(state->scene, time_elapsed);
    check_user_offscreen(state);
    delete_offscreen_body(state);
  } else {
    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN) {
      switch (state->game_screen) {
      case TITLE:
        init_directions_screen(state->scene);
        state->game_screen = INSTRUCTIONS;
        break;
      case INSTRUCTIONS:
        game_init(state);
        break;
      case GAME_OVER:
        game_init(state);
        break;
      default:
        break;
      }
    }
  }
  // Render new frame
  render_frame(state);
}

// frees the memory associated with everything
void emscripten_free(state_t *state) {
  scene_free(state->scene);
  Mix_HaltMusic();
  Mix_FreeMusic(state->music);
  for (int i = 0; i < SND_MAX; i++) {
    Mix_FreeChunk(state->sounds[i]);
  }
  free(state->sounds);
  TTF_CloseFont((TTF_Font *)"assets/font.ttf");
  TTF_Quit();
  free(state);
}
