#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

typedef struct
{
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry)
{
  // TODO: first, copy and paste your implementation from last week.
  // You'll need to add a case for if the entry has type `ASSET_BUTTON`.
  assert(entry != NULL);

  switch (entry->type)
  {
  case ASSET_FONT:
  {
    break;
  }
  case ASSET_IMAGE:
  {
    SDL_DestroyTexture(entry->obj);
    break;
  }
  case ASSET_BUTTON:
  {
    // free image
    // assume nothing needs to be done for button deletion
    asset_destroy(entry->obj);
    break;
  }
  default:
    printf("Type not found\n");
  }
  free(entry->obj);
  free(entry);
}

void asset_cache_init()
{
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

entry_t *find_entry_by_filepath(const char *filepath)
{
  assert(filepath != NULL);
  size_t asset_size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < asset_size; i++)
  {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (entry->type != ASSET_BUTTON)
    {
      if (strcmp(entry->filepath, filepath) == 0)
      {
        return entry;
      }
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath)
{
  assert(filepath != NULL);
  entry_t *entry = find_entry_by_filepath(filepath);
  if (entry != NULL)
  {
    return entry->obj;
  }

  void *obj = NULL;
  switch (ty)
  {
  case ASSET_FONT:
  {
    obj = TTF_OpenFont(filepath, FONT_SIZE);
    break;
  }
  case ASSET_IMAGE:
  {
    obj = get_img(filepath);
    break;
  }
  default:
  {
    printf("Invalid entry type");
  }
  }

  if (obj != NULL)
  {
    entry = malloc(sizeof(entry_t));
    entry->type = ty;
    entry->filepath = strdup(filepath);
    entry->obj = obj;
    list_add(ASSET_CACHE, entry);
  }

  return obj;
}

void asset_cache_register_button(asset_t *button)
{
  // Assert statement
  assert(button != NULL && asset_get_type(button) == ASSET_BUTTON);
  entry_t *entry = malloc(sizeof(entry_t));
  entry->type = ASSET_BUTTON;
  entry->filepath = NULL; // Buttons don't have filepaths
  entry->obj = (entry_t *)button;
  list_add(ASSET_CACHE, entry);
}

void asset_cache_handle_buttons(state_t *state, double x, double y)
{
  size_t asset_size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < asset_size; i++)
  {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (entry->type == ASSET_BUTTON)
    {
      asset_on_button_click(entry->obj, state, x, y);
    }
  }
}
