#include "landing_page.h"
#include "asset.h"
#include "state.h"
#include "asset_cache.h"
#include "sound_effect.h"
#include "seeker.h"
#include "maze.h"

const vector_t SCREEN_MAX = {1000, 500};
const vector_t SCREEN_CENTER = {500, 250};

const size_t LANDING_PAGE_IMG_ELEMENTS = 4;
const size_t LANDING_PAGE_TEXT_ELEMENTS = 7;
const size_t LANDING_PAGE_BTN_ELEMENTS = 1;

const size_t END_PAGE_TEXT_ELEMENTS = 1;
const size_t END_PAGE_BTN_ELEMENTS = 2;

typedef struct end_page_state
{
    list_t *imgs;
    list_t *btns;
    list_t *texts;
} end_page_state_t;

typedef struct landing_page_state
{
    list_t *imgs;
    list_t *btns;
    list_t *texts;
} landing_page_state_t;

typedef struct state
{
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
    end_page_state_t *end_game_state;
    sound_effect_t *sound_effect;
    seeker_t *seeker;
    list_t *body_assets;
} state_t;

static text_element_t landing_text_elements[] = {
    {
        .text = "Welcome to The Caltech Hideout",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){241, 108, 45},
        .frame = (SDL_Rect){(SCREEN_CENTER.x - (280 / 2)), 15, 280, 48},
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
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){60, 460, 60, 28},
    },
    {
        .text = "By Shallon & Divin.",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){255, 255, 255},
        .frame = (SDL_Rect){SCREEN_MAX.x - 180, 459, 150, 28},
    },
};

static text_element_t end_text_elements[] = {
    {
        .text = "GAME OVER",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){0, 0, 0},
        .frame = (SDL_Rect){SCREEN_CENTER.x - 80, SCREEN_CENTER.y - 200, 150, 28},
    },
};

static img_element_t landing_img_elements[] = {
    {
        .file_path = "assets/images/common/nav_line.png",
        .frame = (SDL_Rect){0, 80, SCREEN_MAX.x, 1},
    },
    {
        .file_path = "assets/images/landing-page/how_to_bg.png",
        .frame = (SDL_Rect){23, 100, SCREEN_MAX.x - 50, 150},
    },
    {
        .file_path = "assets/images/common/footer_bg.png",
        .frame = (SDL_Rect){0, 450, SCREEN_MAX.x, 70},
    },
    {
        .file_path = "assets/images/common/copyright.png",
        .frame = (SDL_Rect){23, 460, 30, 30},
    },
};

static void load_game_screen(state_t *state)
{
    state->page = 2;
}

static btn_element_t landing_btn_elements[] = {
    {
        .text.frame = (SDL_Rect){SCREEN_CENTER.x - 20, SCREEN_CENTER.y + 45, 90, 48},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){0, 0, 0},
        .text.text = "PLAY",
        .img.file_path = "assets/images/landing-page/play_btn.png",
        .img.frame = (SDL_Rect){SCREEN_CENTER.x - 50, SCREEN_CENTER.y + 30, 200, 80},
        .handler = (void *)load_game_screen,
    },
};

static btn_element_t end_btn_elements[] = {
    {
        .text.frame = (SDL_Rect){SCREEN_CENTER.x - 65, SCREEN_CENTER.y - 51, 120, 40},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){255, 255, 255},
        .text.text = "RESTART GAME",
        .img.file_path = "assets/images/landing-page/fill_btn.png",
        .img.frame = (SDL_Rect){SCREEN_CENTER.x - 80, SCREEN_CENTER.y - 70, 200, 80},
        .handler = (void *)load_game_screen,
    },
    {
        .text.frame = (SDL_Rect){SCREEN_CENTER.x - 65, SCREEN_CENTER.y + 55, 150, 40},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){230, 91, 47},
        .text.text = "Go to Home",
        .img.file_path = "assets/images/landing-page/white_btn.png",
        .img.frame = (SDL_Rect){SCREEN_CENTER.x - 80, SCREEN_CENTER.y + 40, 200, 80},
        .handler = (void *)load_game_screen,
    },
};

/**
 * Build text assets from text templates
 * @return list of text assets
 */
static list_t *build_text_assets(size_t NUM_TEXT_ELEMENTS, text_element_t *text_elements)
{
    list_t *assets = list_init(NUM_TEXT_ELEMENTS, free);
    for (size_t i = 0; i < NUM_TEXT_ELEMENTS; i++)
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
static list_t *build_img_assets(size_t NUM_IMG_ELEMENTS, img_element_t *img_elements)
{
    list_t *assets = list_init(NUM_IMG_ELEMENTS, free);
    for (size_t i = 0; i < NUM_IMG_ELEMENTS; i++)
    {
        asset_t *asset =
            asset_make_image(img_elements[i].file_path, img_elements[i].frame);
        list_add(assets, asset);
    }
    return assets;
}

asset_t *create_btn(btn_element_t btn_element)
{
    asset_t *img_asset = NULL;
    asset_t *text_asset = NULL;

    if (btn_element.img.file_path != NULL)
    {
        img_asset = asset_make_image(btn_element.img.file_path, btn_element.img.frame);
    }
    if (btn_element.text.font_path != NULL)
    {
        text_asset = asset_make_text(btn_element.text.font_path, btn_element.text.frame, btn_element.text.text,
                                     btn_element.text.color);
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
static list_t *build_btn_assets(size_t NUM_BTN_ELEMENTS, btn_element_t *landing_btn_elements)
{
    list_t *assets = list_init(NUM_BTN_ELEMENTS, (free_func_t)asset_destroy);
    for (size_t i = 0; i < NUM_BTN_ELEMENTS; i++)
    {
        asset_t *asset = create_btn(landing_btn_elements[i]);
        list_add(assets, asset);
    }
    return assets;
}

void show_landing_page(state_t *state)
{
    list_t *imgs = state->landing_page_state->imgs;
    for (size_t i = 0; i < list_size(imgs); i++)
    {
        asset_render(list_get(imgs, i));
    }

    list_t *texts = state->landing_page_state->texts;
    for (size_t i = 0; i < list_size(texts); i++)
    {

        asset_render(list_get(texts, i));
    }

    list_t *btns = state->landing_page_state->btns;
    for (size_t i = 0; i < list_size(btns); i++)
    {
        asset_render(list_get(btns, i));
    }
}

void show_end_page(state_t *state)
{
    // list_t *imgs = state->end_game_state->imgs;
    // for (size_t i = 0; i < list_size(imgs); i++)
    // {
    //     asset_render(list_get(imgs, i));
    // }

    list_t *texts = state->end_game_state->texts;
    for (size_t i = 0; i < list_size(texts); i++)
    {

        asset_render(list_get(texts, i));
    }

    list_t *btns = state->end_game_state->btns;
    for (size_t i = 0; i < list_size(btns); i++)
    {
        asset_render(list_get(btns, i));
    }
}

landing_page_state_t *landing_page_init()
{
    landing_page_state_t *page_state = malloc(sizeof(landing_page_state_t));

    page_state->imgs = build_img_assets(LANDING_PAGE_IMG_ELEMENTS, landing_img_elements);
    page_state->texts = build_text_assets(LANDING_PAGE_TEXT_ELEMENTS, landing_text_elements);
    page_state->btns = build_btn_assets(LANDING_PAGE_BTN_ELEMENTS, landing_btn_elements);

    return page_state;
}

end_page_state_t *end_page_init()
{
    end_page_state_t *page_state = malloc(sizeof(end_page_state_t));

    page_state->texts = build_text_assets(END_PAGE_TEXT_ELEMENTS, end_text_elements);
    page_state->btns = build_btn_assets(END_PAGE_BTN_ELEMENTS, end_btn_elements);

    return page_state;
}