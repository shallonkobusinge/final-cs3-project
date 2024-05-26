#ifndef __LANDING_PAGE_H__
#define __LANDING_PAGE_H__

typedef struct text_element text_element_t;

typedef struct img_element img_element_t;

typedef struct btn_element btn_element_t;

typedef struct state_t state;

/**
 * Build all assets of the landing page
 */
void build_landing_page();

void load_game_screen(state_t *state);

#endif // #ifndef __LANDING_PAGE_H__