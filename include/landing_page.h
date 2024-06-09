#ifndef __LANDING_PAGE_H__
#define __LANDING_PAGE_H__

#include "state.h"

typedef struct text_element text_element_t;

typedef struct img_element img_element_t;

typedef struct btn_element btn_element_t;

typedef struct landing_page_state landing_page_state_t;

/**
 * Display and renders all the assets on the landing page.
 * @param page_state state of the page
 */
void show_landing_page(landing_page_state_t *page_state);

void set_state(state_t *state);

/**
 * Initialize landing page images, buttons and texts.
 * @return state of the landing page.
 */
landing_page_state_t *landing_page_init(state_t *state);

#endif // #ifndef __LANDING_PAGE_H__