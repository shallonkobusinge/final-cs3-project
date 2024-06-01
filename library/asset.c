#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

typedef struct asset
{
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct text_asset
{
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
} text_asset_t;

typedef struct image_asset
{
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
} image_asset_t;

typedef struct button_asset
{
  asset_t base;
  image_asset_t *image_asset;
  text_asset_t *text_asset;
  button_handler_t handler;
  bool is_rendered;
} button_asset_t;

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box)
{
  asset_t *new;
  switch (ty)
  {
  case ASSET_IMAGE:
  {
    new = malloc(sizeof(image_asset_t));
    break;
  }
  case ASSET_FONT:
  {
    new = malloc(sizeof(text_asset_t));
    break;
  }
  case ASSET_BUTTON:
  {
    new = malloc(sizeof(button_asset_t));
    break;
  }
  default:
  {
    assert(false && "Unknown asset type");
  }
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_type_t asset_get_type(asset_t *asset) { return asset->type; }

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box)
{
  image_asset_t *asset = (image_asset_t *)asset_init(ASSET_IMAGE, bounding_box);
  asset->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  asset->body = NULL;
  return (asset_t *)asset;
}

asset_t *asset_make_image_with_body(const char *filepath, body_t *body)
{
  SDL_Rect bounding_box = get_bounding_box(body);
  image_asset_t *asset = (image_asset_t *)asset_init(ASSET_IMAGE, bounding_box);
  asset->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  asset->body = body;
  return (asset_t *)asset;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color)
{
  text_asset_t *asset = (text_asset_t *)asset_init(ASSET_FONT, bounding_box);
  asset->font = (TTF_Font *)asset_cache_obj_get_or_create(ASSET_FONT, filepath);
  asset->text = strdup(text);
  asset->color = color;
  return (asset_t *)asset;
}

asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler)
{
  // TODO: implement this!
  // Remember to assert the types of image_asset and text_asset (if not NULL)
  assert(image_asset != NULL && asset_get_type(image_asset) == ASSET_IMAGE);
  button_asset_t *asset =
      (button_asset_t *)asset_init(ASSET_BUTTON, bounding_box);
  asset->image_asset = (image_asset_t *)image_asset;
  asset->text_asset = (text_asset_t *)text_asset;
  asset->handler = handler;
  asset->is_rendered = true;
  return (asset_t *)asset;
}

/**
 * Helper function to check if a point (x, y) is within the bounding box.
 *
 * @param box the bounding box
 * @param x the x-coordinate of the point
 * @param y the y-coordinate of the point
 * @return true if the point is within the bounding box, false otherwise
 */
static bool point_in_rect(SDL_Rect *box, double x, double y)
{
  return x >= box->x && x <= box->x + box->w && y >= box->y &&
         y <= box->y + box->h;
}

void asset_on_button_click(asset_t *button, state_t *state, double x,
                           double y)
{

  button_asset_t *button_asset = (button_asset_t *)button;
  if (!button_asset->is_rendered)
  {
    return;
  }

  if (point_in_rect(&button->bounding_box, x, y))
  {
    // Execute the button handler if it exists
    if (button_asset->handler != NULL)
    {
      button_asset->handler(state);
      button_asset->is_rendered = false;
    }
  }
}

void asset_render(asset_t *asset)
{
  // TODO: copy and paste your implementation from last week, and then add a
  // case for button assets.
  assert(asset);
  switch (asset->type)
  {
  case ASSET_IMAGE:
  {
    image_asset_t *img_asset = (image_asset_t *)asset;
    if (img_asset->body != NULL)
    {
      SDL_Rect bounding_box = get_bounding_box(img_asset->body);
      render_image(img_asset->texture, &bounding_box);
    }
    else
    {
      render_image(img_asset->texture, &img_asset->base.bounding_box);
    }
  }
  break;
  case ASSET_FONT:
  {
    text_asset_t *text_asset = (text_asset_t *)asset;
    SDL_Texture *texture =
        get_text(text_asset->text, text_asset->font, text_asset->color);
    render_text(texture, &text_asset->base.bounding_box);
  }
  break;

  case ASSET_BUTTON:
  {
    button_asset_t *button_asset = (button_asset_t *)asset;
    // image and then generate text
    if (button_asset->image_asset != NULL)
    {
      render_image(button_asset->image_asset->texture,
                   &button_asset->base.bounding_box);
    }
    if (button_asset->text_asset != NULL)
    {
      SDL_Texture *texture = get_text(button_asset->text_asset->text,
                                      button_asset->text_asset->font,
                                      button_asset->text_asset->color);
      render_text(texture, &button_asset->text_asset->base.bounding_box);
    }
    button_asset->is_rendered = true;
  }
  break;

  default:
  {
    printf("No type found\n");
  }
  }
}

void asset_destroy(asset_t *asset) { free(asset); }
