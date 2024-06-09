#include "end_page.h"
#include "landing_page.h"
#include "asset.h"
#include "state.h"
#include "asset_cache.h"
#include "sound_effect.h"
#include "seeker.h"
#include "maze.h"

const vector_t END_SCREEN_MAX = {1000, 500};
const vector_t END_SCREEN_CENTER = {500, 250};

const size_t END_PAGE_TEXT_ELEMENTS = 1;
const size_t END_BTN_ELEMENTS = 1;

typedef struct end_page_state
{
    list_t *btns;
    list_t *texts;
} end_page_state_t;

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

static text_element_t text_elements[] = {
    {
        .text = "Game Over",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){241, 108, 45},
        .frame = (SDL_Rect){(END_SCREEN_CENTER.x - (280 / 2)), 15, 280, 48},
    },
};

static void load_landing_page(state_t *state)
{
    state->page = 1;
}

static void load_game_screen(state_t *state)
{
    state->page = 2;
}

static btn_element_t btn_elements[] = {
    {
        .text.frame = (SDL_Rect){END_SCREEN_CENTER.x - 20, END_SCREEN_CENTER.y + 45, 90, 48},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){0, 0, 0},
        .text.text = "PLAY",
        .img.file_path = "assets/images/landing-page/play_btn.png",
        .img.frame = (SDL_Rect){END_SCREEN_CENTER.x - 50, END_SCREEN_CENTER.y + 30, 200, 80},
        .handler = (void *)load_game_screen,
    },
};
/**
 * Build text assets from text templates
 * @return list of text assets
 */
static list_t *end_build_text_assets()
{
    list_t *assets = list_init(END_PAGE_TEXT_ELEMENTS, free);
    for (size_t i = 0; i < END_PAGE_TEXT_ELEMENTS; i++)
    {
        asset_t *asset =
            asset_make_text(text_elements[i].font_path, text_elements[i].frame,
                            text_elements[i].text, text_elements[i].color);
        list_add(assets, asset);
    }
    return assets;
}

/**
 * Build buttons assets from buttons templates
 * @return list of button assets
 */
static list_t *end_build_btn_assets()
{
    list_t *assets = list_init(END_PAGE_TEXT_ELEMENTS, (free_func_t)asset_destroy);
    for (size_t i = 0; i < END_BTN_ELEMENTS; i++)
    {
        asset_t *asset = create_btn(btn_elements[i]);
        list_add(assets, asset);
    }
    return assets;
}

void show_end_page(state_t *state)
{
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

end_page_state_t *end_page_init()
{
    end_page_state_t *page_state = malloc(sizeof(end_page_state_t));

    page_state->texts = end_build_text_assets();
    page_state->btns = end_build_btn_assets();

    return page_state;
}