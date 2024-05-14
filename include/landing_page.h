#ifndef __LANDING_PAGE_H__
#define __LANDING_PAGE_H__

typedef struct text_element text_element_t;

typedef struct img_element img_element_t;

typedef struct btn_element btn_element_t;

typedef struct landing_page_state landing_page_state_t;

/**
 * Display the landing page
 */
void show_landing_page();

/**
 * Initialize landing page assets
 * @return state of the landing page
 */
landing_page_state_t *landing_page_init();

#endif // #ifndef __LANDING_PAGE_H__