#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "forces.h"
#include "scene.h"

const size_t INITIAL_CAPACITY = 10;

struct scene
{
  size_t num_bodies;
  size_t num_creators;
  list_t *bodies;
  list_t *force_creators;
  // void *aux;
};

// typedef struct force_creator_data {
//   force_creator_t force_creator;
//   void *aux;
//   free_func_t freer;
//   list_t *bodies;
// } force_creator_data_t;

// void force_creator_data_free(force_creator_data_t *forcer) {

//   list_free(forcer->bodies);
//   free(forcer);
// }

// edit
scene_t *scene_init(void)
{
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);
  scene->bodies = list_init(INITIAL_CAPACITY, (free_func_t)body_free);
  scene->force_creators =
      list_init(INITIAL_CAPACITY, (free_func_t)force_creator_data_free);
  scene->num_bodies = 0;
  scene->num_creators = 0;
  assert(scene->bodies != NULL);
  return scene;
}

// edit
void scene_free(scene_t *scene)
{

  // list_free(scene->aux);
  // for (size_t i = 0; i < list_size(scene->force_creators); i++) {
  //   force_creator_data_t *forcer = list_get(scene->force_creators, i);
  //   if (forcer->freer != NULL) {
  //     forcer->freer(forcer->aux);
  //   }
  // }
  list_free(scene->force_creators);
  list_free(scene->bodies);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return scene->num_bodies; }

body_t *scene_get_body(scene_t *scene, size_t index)
{
  assert(index < scene_bodies(scene));
  return (body_t *)list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body)
{
  list_add(scene->bodies, body);
  scene->num_bodies++;
}

void scene_remove_body(scene_t *scene, size_t index)
{
  assert(index < scene_bodies(scene));
  // list_remove(scene->bodies, index);
  body_remove(scene_get_body(scene, index));
  // scene->num_bodies--;
}

void scene_tick(scene_t *scene, double dt)
{
  for (size_t i = 0; i < list_size(scene->force_creators); i++)
  {
    force_creator_data_t *data =
        (force_creator_data_t *)list_get(scene->force_creators, i);
    data->force_creator(data->aux);
  }
  for (ssize_t i = 0; i < (ssize_t)scene->num_bodies; i++)
  {
    body_t *body = scene_get_body(scene, i);
    if (body_is_removed(body))
    {
      for (ssize_t j = 0; j < (ssize_t)list_size(scene->force_creators); j++)
      {
        force_creator_data_t *forcer = list_get(scene->force_creators, j);
        for (ssize_t k = 0; k < (ssize_t)list_size(forcer->bodies); k++)
        {
          body_t *current_body = list_get(forcer->bodies, k);
          if (current_body == body)
          {
            list_remove(scene->force_creators, j);
            force_creator_data_free(forcer);
            j--;
            break;
          }
        }
      }
      body_free(list_remove(scene->bodies, i));
      scene->num_bodies--;
      i--;
    }
    else
    {
      if (dt != 0)
      {
        body_tick(body, dt);
      }
    }
  }
}

force_creator_data_t *force_creator_init(force_creator_t forcer, void *aux,
                                         list_t *bodies)
{
  force_creator_data_t *forcer_data = malloc(sizeof(force_creator_data_t));
  forcer_data->aux = aux;
  forcer_data->bodies = bodies;
  forcer_data->force_creator = forcer;
  return forcer_data;
}

void scene_add_force_creator(scene_t *scene, force_creator_t force_creator,
                             void *aux)
{
  scene_add_bodies_force_creator(scene, force_creator, aux, list_init(0, NULL));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies)
{
  assert(scene != NULL);
  force_creator_data_t *force_data = force_creator_init(forcer, aux, bodies);
  // Add the force creator data to the list
  list_add(scene->force_creators, force_data);

  // list_add(scene->aux, aux);
  // scene->aux = aux;
  scene->num_creators++;
}
