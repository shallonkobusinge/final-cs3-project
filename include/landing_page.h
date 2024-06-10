#ifndef __LANDING_PAGE_H__
#define __LANDING_PAGE_H__

#include "state.h"
#include "asset.h"

typedef struct text_element
{
    const char *text;
    const char *font_path;
    rgb_color_t color;
    SDL_Rect frame;
} text_element_t;

typedef struct img_element
{
    const char *file_path;
    SDL_Rect frame;
} img_element_t;

typedef struct btn_element
{
    text_element_t text;
    img_element_t img;
    button_handler_t handler;
} btn_element_t;

typedef struct landing_page_state landing_page_state_t;
typedef struct end_page_state end_page_state_t;

/**
 * Display and renders all the assets on the landing page.
 * @param state state of the game
 */
void show_landing_page(state_t *state);

/**
 * Display and renders all the assets on the end page.
 * @param state state of the game
 */
void show_end_page(state_t *state);

/**
 * Create button assets from a button template
 * @return button asset
 */
asset_t *create_btn(btn_element_t btn_element);

/**
 * Initialize landing page images, buttons and texts.
 * @return state of the landing page.
 */
landing_page_state_t *landing_page_init();

/**
 * Initialize end page images, buttons and texts.
 * @return state of game.
 */
end_page_state_t *end_page_init();

#endif // #ifndef __LANDING_PAGE_H__