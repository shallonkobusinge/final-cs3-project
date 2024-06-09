#ifndef __END_PAGE_H__
#define __END_PAGE_H__

#include "state.h"

/**
 * Display and renders all the assets on the landing page.
 * @param state state of the game
 */
void show_end_page(state_t *state);

/**
 * Initialize end page images, buttons and texts.
 * @return state of game.
 */
end_page_state_t *end_page_init();

#endif // #ifndef __END_PAGE_H__