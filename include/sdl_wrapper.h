#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum
{
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE_BAR = 5,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum
{
  KEY_PRESSED,
  KEY_RELEASED
} key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *state);

/**
 * A mouse handler.
 *
 * @param xpos
 * @param ypos
 */
typedef void (*mouse_handler_t)(int xpos, int ypos, void *state);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle inputs.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param poly a struct representing the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(polygon_t *poly, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 * @param aux an additional body to draw (can be NULL if no additional bodies)
 */
void sdl_render_scene(scene_t *scene, void *aux);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

/**
 * Return SDL texture for a given path.
 * @param IMG_PATH the image path
 * @return image SDL texture
 */
SDL_Texture *get_img(const char IMG_PATH[]);

/**
 * Return SDL texture for text.
 * @param text the text
 * @param font the font of the text
 * @param rgb_color the color of the text
 * @return text SDL texture
 */
SDL_Texture *get_text(const char text[], TTF_Font *font, rgb_color_t rgb_color);

/**
 * Render text.
 * @param text text to render
 * @param frame frame of the text
 */
void render_text(SDL_Texture *text, SDL_Rect *frame);

/**
 * Render image.
 * @param Img image to render
 * @param frame frame of the image
 */
void render_image(SDL_Texture *Img, SDL_Rect *frame);

/**
 * SDl on click
 */
void sdl_on_click(mouse_handler_t handler);

/*
 * Return the bounding box of a body
 */
SDL_Rect get_bounding_box(body_t *body);

/**
 * Render rect.
 * @param frame frame of the rect
 */
void render_rect(SDL_Rect *frame);

/**
 * Render line.
 * @param x x coordinate of the line
 * @param y y coordinate of the line
 * @param w width of the line
 * @param h height of the line
 */
void render_line(int x, int y, int w, int h);

/**
 * Render color.
 * @param color rgb color
 */
void render_color(rgb_color_t color);

void render_show();

#endif // #ifndef __SDL_WRAPPER_H__
