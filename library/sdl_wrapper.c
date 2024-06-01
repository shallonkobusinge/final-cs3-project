#include "sdl_wrapper.h"
#include "asset_cache.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;

mouse_handler_t mouse_handler = NULL;

// Declare global variables to track mouse click position
int mouse_click_x = -1;
int mouse_click_y = -1;

/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void)
{
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center)
{
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center)
{
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key)
{
  switch (key)
  {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_SPACE:
    return SPACE_BAR;
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

void sdl_init(vector_t min, vector_t max)
{
  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  TTF_Init();
}

bool sdl_is_done(void *state)
{
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event))
  {
    switch (event->type)
    {
    case SDL_QUIT:
      free(event);
      return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed
      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat)
      {
        key_start_timestamp = timestamp;
      }
      key_event_type_t type =
          event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, state);
      break;

    case SDL_MOUSEBUTTONDOWN:
      // SDL_GetMouseState(&(event->button.x), &(event->button.y));
      // mouse_handler(event->button.x, event->button.y, state);
      asset_cache_handle_buttons(state, event->button.x, event->button.y);
    }
  }
  free(event);
  return false;
}

void sdl_clear(void)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void sdl_draw_polygon(polygon_t *poly, rgb_color_t color)
{
  list_t *points = polygon_get_points(poly);
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++)
  {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, 255);
  free(x_points);
  free(y_points);
}

void sdl_show(void)
{
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(renderer, boundary);
  free(boundary);

  SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene, void *aux)
{
  sdl_clear();
  size_t body_count = scene_bodies(scene);
  for (size_t i = 0; i < body_count; i++)
  {
    body_t *body = scene_get_body(scene, i);
    list_t *shape = body_get_shape(body);
    polygon_t *poly = polygon_init(shape, (vector_t){0, 0}, 0, 0, 0, 0);
    sdl_draw_polygon(poly, *body_get_color(body));
    list_free(shape);
  }
  if (aux != NULL)
  {
    body_t *body = aux;
    sdl_draw_polygon(body_get_polygon(body), *body_get_color(body));
  }
  sdl_show();
}

void sdl_on_key(key_handler_t handler) { key_handler = handler; }

double time_since_last_tick(void)
{
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}

SDL_Texture *get_img(const char IMG_PATH[])
{
  return IMG_LoadTexture(renderer, IMG_PATH);
}

SDL_Texture *get_text(const char text[], TTF_Font *font,
                      rgb_color_t rgb_color)
{
  SDL_Color sdl_color = {rgb_color.r, rgb_color.g, rgb_color.b};
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, sdl_color);
  return SDL_CreateTextureFromSurface(renderer, surface);
}

void render_text(SDL_Texture *Text, SDL_Rect *frame)
{
  SDL_RenderCopy(renderer, Text, NULL, frame);
}

void render_image(SDL_Texture *Img, SDL_Rect *frame)
{
  SDL_RenderCopy(renderer, Img, NULL, frame);
}

void sdl_on_click(mouse_handler_t handler) { mouse_handler = handler; }

SDL_Rect get_bounding_box(body_t *body)
{

  list_t *shape = body_get_shape(body);

  vector_t min = {.x = __DBL_MAX__, .y = __DBL_MAX__};
  vector_t max = {.x = -__DBL_MAX__, .y = -__DBL_MAX__};
  vector_t dimensions_w_h = {.x = 0.0, .y = 0.0};
  SDL_Rect box_rect_fields;
  vector_t window_center = get_window_center();

  for (size_t i = 1; i < list_size(shape); i++)
  {
    vector_t *point = list_get(shape, i);

    if (point->x < min.x)
    {
      min.x = point->x;
    }
    if (point->x > max.x)
    {
      max.x = point->x;
    }
    if (point->y < min.y)
    {
      min.y = point->y;
    }
    if (point->y > max.y)
    {
      max.y = point->y;
    }
  }
  list_free(shape);

  vector_t window_top_left_sdl =
      get_window_position((vector_t){.x = min.x, .y = max.y}, window_center);
  vector_t window_botton_right_sdl =
      get_window_position((vector_t){.x = max.x, .y = min.y}, window_center);

  dimensions_w_h.x = window_botton_right_sdl.x - window_top_left_sdl.x;
  dimensions_w_h.y = window_botton_right_sdl.y - window_top_left_sdl.y;

  box_rect_fields.h = (int)dimensions_w_h.y;
  box_rect_fields.w = (int)dimensions_w_h.x;
  box_rect_fields.x = (int)window_top_left_sdl.x;
  box_rect_fields.y = (int)window_top_left_sdl.y;

  return box_rect_fields;
}

void render_rect(SDL_Rect *frame)
{
  SDL_RenderFillRect(renderer, frame);
  SDL_RenderDrawRect(renderer, frame);
}

void render_line(int x, int y, int w, int h)
{
  SDL_RenderDrawLine(renderer, x, y, w, h);
}

void draw_color(rgb_color_t color)
{
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
}

void render_present()
{
  SDL_RenderPresent(renderer);
}
