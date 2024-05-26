#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "landing_page.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const vector_t TEXT_FRAME = {100, 48};
const size_t TEXT_SIZE = 18;

struct state
{
    scene_t *scene;
};

const size_t LANDING_PAGE_IMG_ELEMENTS = 5;
const size_t LANDING_PAGE_TEXT_ELEMENTS = 7;
const size_t LANDING_PAGE_BTN_ELEMENTS = 1;

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

/**
 * Load game screen
 */
static void
load_game_screen()
{
    printf("New screen");
    // destroy the screen and build landing_a new one.
}

btn_element_t btn_elements[] = {
    {
        .text.frame = (SDL_Rect){CENTER.x - 20, CENTER.y + 40, 90, 48},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){0, 0, 0},
        .text.text = "PLAY",
        .img.file_path = "assets/images/landing-page/play_btn.png",
        .img.frame = (SDL_Rect){CENTER.x - 50, CENTER.y + 30, 200, 80},

        .handler = (void *)load_game_screen,
    },
};

text_element_t text_elements[] = {
    {
        .text = "Welcome to The Caltech Hideout",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){241, 108, 45},
        .frame = (SDL_Rect){(CENTER.x - (280 / 2)), 15, 280, 48},
    },
    {
        .text = "HOW TO PLAY",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){43, 110, 180, 30},
    },
    {
        .text = "Recycling one aluminum can saves enough energy to power a television for three hours.",
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){48, 150, 680, 30},
        .font_path = "assets/fonts/Inter-Regular.ttf",
    },
    {
        .text = "Recycling one aluminum can saves enough energy to power a television for three hours.",
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){48, 175, 680, 30},
        .font_path = "assets/fonts/Inter-Regular.ttf",
    },

    {
        .text = "Recycling one aluminum can saves enough energy to power a television for three hours.",
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){48, 200, 680, 30},
        .font_path = "assets/fonts/Inter-Regular.ttf",
    },
    {
        .text = "2024",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){0, 0, 1},
        .frame = (SDL_Rect){0, 400, TEXT_FRAME.x, TEXT_FRAME.y},
    },
    {
        .text = "By Shallon & Divin.",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){0, 0, 1},
        .frame = (SDL_Rect){0, 400, TEXT_FRAME.x, TEXT_FRAME.y},
    },
};

img_element_t img_elements[] = {
    {
        .file_path = "assets/images/common/nav_line.png",
        .frame = (SDL_Rect){0, 80, MAX.x, 1},
    },
    {
        .file_path = "assets/images/landing-page/how_to_bg.png",
        .frame = (SDL_Rect){23, 100, MAX.x - 50, 150},
    },
    {
        .file_path = "assets/images/common/footer_bg.png",
        .frame = (SDL_Rect){0, 500, MAX.x, 70},
    },
    {
        .file_path = "assets/images/common/copyright.png",
        .frame = (SDL_Rect){23, 400, 50, 50},
    },
};

/**
 * Build text assets from text templates
 * @return list of text assets
 */
static list_t *
build_landing_text_assets()
{
    list_t *assets = list_init(LANDING_PAGE_TEXT_ELEMENTS, free);
    for (size_t i = 0; i < LANDING_PAGE_TEXT_ELEMENTS; i++)
    {
        asset_t *asset =
            asset_make_text(text_elements[i].font_path, text_elements[i].frame,
                            text_elements[i].text, text_elements[i].color);
        list_add(assets, asset);
    }
    return assets;
}

/**
 * Build image assets from image templates
 * @return list of image assets
 */
static list_t *build_landing_img_assets()
{
    list_t *assets = list_init(LANDING_PAGE_IMG_ELEMENTS, free);
    for (size_t i = 0; i < LANDING_PAGE_IMG_ELEMENTS; i++)
    {
        asset_t *asset =
            asset_make_image(img_elements[i].file_path, img_elements[i].frame);
        list_add(assets, asset);
    }
    return assets;
}

/**
 * Create button assets from a button template
 * @return button asset
 */
static asset_t *create_btn(btn_element_t btn_element)
{
    asset_t *img_asset = NULL;
    asset_t *text_asset = NULL;

    if (btn_element.text.font_path != NULL)
    {
        text_asset = asset_make_text(btn_element.text.font_path, btn_element.text.frame, btn_element.text.text,
                                     btn_element.text.color);
    }

    if (btn_element.img.file_path != NULL)
    {
        printf("%s\n", btn_element.img.file_path);
        img_asset = asset_make_image(btn_element.img.file_path, btn_element.img.frame);
    }

    asset_t *asset =
        asset_make_button(btn_element.img.frame, img_asset, text_asset, btn_element.handler);
    asset_cache_register_button(asset);

    return asset;
}

/**
 * Build buttons assets from buttons templates
 * @return list of button assets
 */
static list_t *build_landing_btn_assets()
{
    list_t *assets = list_init(LANDING_PAGE_BTN_ELEMENTS, (free_func_t)asset_destroy);
    for (size_t i = 0; i < LANDING_PAGE_BTN_ELEMENTS; i++)
    {
        asset_t *asset = create_btn(btn_elements[i]);
        list_add(assets, asset);
    }
    return assets;
}

void build_landing_page()
{

    list_t *imgs = build_landing_img_assets();
    for (size_t i = 0; i < list_size(imgs); i++)
    {
        asset_render(list_get(imgs, i));
    }

    list_t *texts = build_landing_text_assets();
    for (size_t i = 0; i < list_size(texts); i++)
    {
        asset_render(list_get(texts, i));
    }

    list_t *btns = build_landing_btn_assets();
    for (size_t i = 0; i < list_size(btns); i++)
    {
        asset_render(list_get(btns, i));
    }
}

state_t *emscripten_init()
{
    asset_cache_init();
    sdl_init(MIN, MAX);
    state_t *state = malloc(sizeof(state_t));

    return state;
}

bool emscripten_main(state_t *state)
{
    sdl_clear();
    build_landing_page();

    sdl_show();
    return false;
}

void emscripten_free(state_t *state)
{
    scene_free(state->scene);
    asset_cache_destroy();
    free(state);
}
