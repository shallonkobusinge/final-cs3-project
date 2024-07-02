#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "maze.h"
#include "stack.h"
#include "state.h"
#include "sdl_wrapper.h"
#include "landing_page.h"
#include "maze.h"
#include "sound_effect.h"
#include "asset.h"
#include "forces.h"
#include "maze_body.h"

const size_t GRID_WIDTH = 22;
const size_t GRID_HEIGHT = 11;
const size_t NUM_CELLS = GRID_WIDTH * GRID_HEIGHT;
const size_t GRID_CELL_SIZE = 45;

const vector_t SDL_SCREEN_CENTER = {500, 250};

const size_t MAZE_WINDOW_WIDTH = (GRID_WIDTH * GRID_CELL_SIZE) + 1;
const size_t MAZE_WINDOW_HEIGHT = (GRID_HEIGHT * GRID_CELL_SIZE) + 1;

const size_t NUM_BUILDINGS = 5;
const size_t TOTAL_GAME_TIME = 120; // IN SECONDS

const size_t MISSION_PAGE_TEXT_ELEMENTS = 1;
const size_t MISSION_PAGE_BTN_ELEMENTS = 1;
const size_t MISSION_PAGE_IMG_ELEMENTS = 1;

const char *building_paths[] = {
    "assets/images/scenery/caltech-hall.png",
    "assets/images/scenery/cahill.jpeg",
    "assets/images/scenery/chen.jpeg",
    "assets/images/scenery/annenberg.jpg",
    "assets/images/scenery/beckman-auditorium.png"};

typedef struct maze
{
    size_t width, height, cell_size;
    cell_t **cells;
    node_t *stack;
} maze_t;

typedef struct building
{
    vector_t position;
    const char *img_path;
    rgb_color_t color;
} building_t;

typedef struct maze_state
{
    maze_t *maze;
    double time_elapsed;
    body_t *random_building;
    list_t *imgs;
    list_t *texts;
    list_t *btns;
    building_t buildings[];
} maze_state_t;

typedef struct state
{
    scene_t *scene;
    size_t page;
    maze_state_t *maze_state;
    landing_page_state_t *landing_page_state;
    end_page_state_t *end_game_state;
    sound_effect_t *sound_effect;
    list_t *body_assets;
} state_t;

static text_element_t mission_text_elements[] = {
    {
        .text = "Your Target building: ",
        .font_path = "assets/fonts/Inter-Regular.ttf",
        .color = (rgb_color_t){0, 0, 0},
        .frame = (SDL_Rect){SDL_SCREEN_CENTER.x - 80, SDL_SCREEN_CENTER.y - 200, 150, 28},
    },
};

static void start_game_screen(state_t *state)
{
    state->page = 3;
}

static btn_element_t mission_btn_elements[] = {
    {
        .text.frame = (SDL_Rect){SDL_SCREEN_CENTER.x - 40, SDL_SCREEN_CENTER.y + 145, 90, 48},
        .text.font_path = "assets/fonts/Inter-Regular.ttf",
        .text.color = (rgb_color_t){255, 255, 255},
        .text.text = "START",
        .img.file_path = "assets/images/landing-page/fill_btn.png",
        .img.frame = (SDL_Rect){SDL_SCREEN_CENTER.x - 70, SDL_SCREEN_CENTER.y + 130, 200, 80},
        .handler = (void *)start_game_screen,
    },
};

/**
 * Create memory for the maze.
 * @return empty maze
 */
maze_t *create_maze()
{
    maze_t *maze = malloc(sizeof(maze_t));
    maze->cells = malloc(GRID_HEIGHT * sizeof(cell_t *));
    for (size_t i = 0; i < GRID_HEIGHT; i++)
    {
        maze->cells[i] = malloc(GRID_WIDTH * sizeof(cell_t));
    }
    return maze;
}

/**
 * Initializes and draws the grid, draws buildings and hider.
 * @param state state struct.
 */
static void init_grid(state_t *state)
{
    maze_state_t *maze_state = state->maze_state;

    render_color((rgb_color_t){230, 230, 230});

    for (size_t x = 0; x < MAZE_WINDOW_WIDTH; x += GRID_CELL_SIZE)
    {
        render_line(x, 0, x, MAZE_WINDOW_HEIGHT);
    }
    for (size_t y = 0; y < MAZE_WINDOW_HEIGHT; y += GRID_CELL_SIZE)
    {
        render_line(0, y, MAZE_WINDOW_WIDTH, y);
    }
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        
        add_to_scene(state,
                     &(maze_body_t){.color = (rgb_color_t){200, 200, 200},
                                  .img_path = building_paths[i],
                                  .position = (vector_t){.x = maze_state->buildings[i].position.x,
                                                         .y = maze_state->buildings[i].position.y}});
    }
}

/**
 * Initializes the maze by marking all cells as unvisited and setting the border cells as visited.
 * and initializes the adjacency matrix and sets the random seed.
 */
static void init_maze(maze_t *maze)
{
    maze->stack = NULL;

    for (size_t y = 0; y < GRID_HEIGHT; ++y)
    {
        for (size_t x = 0; x < GRID_WIDTH; ++x)
        {
            maze->cells[y][x] = (cell_t){
                .box = {x * GRID_CELL_SIZE, y * GRID_CELL_SIZE, GRID_CELL_SIZE, GRID_CELL_SIZE},
                .west = true,
                .north = true,
                .east = true,
                .south = true,
                .visited = false,
                .start = false,
                .end = false};
        }
    }
}

/**
 * Checks the four possible neighbors (north, east, south, west) of the cell located at
 * coordinates (x, y) in the maze. It adds the coordinates of the unvisited neighbors to the
 * neighbors array and returns the count of unvisited neighbors.
 *
 * @param maze A pointer to the maze structure, which contains the grid of cells.
 * @param x The x-coordinate of the current cell.
 * @param y The y-coordinate of the current cell.
 * @param neighbors A 2D array to store the coordinates of the unvisited neighboring cells. Each
 *                  element is an array of size 2, where the first element is the x-coordinate and
 *                  the second element is the y-coordinate of an unvisited neighbor.
 *
 * @return The number of unvisited neighboring cells.
 */
static size_t get_neighbors(maze_t *maze, size_t x, size_t y, size_t neighbors[][2])
{
    size_t count = 0;

    if (y > 0 && !maze->cells[y - 1][x].visited)
    {
        neighbors[count][0] = x;
        neighbors[count++][1] = y - 1;
    }
    if (x < GRID_WIDTH - 1 && !maze->cells[y][x + 1].visited)
    {
        neighbors[count][0] = x + 1;
        neighbors[count++][1] = y;
    }
    if (y < GRID_HEIGHT - 1 && !maze->cells[y + 1][x].visited)
    {
        neighbors[count][0] = x;
        neighbors[count++][1] = y + 1;
    }
    if (x > 0 && !maze->cells[y][x - 1].visited)
    {
        neighbors[count][0] = x - 1;
        neighbors[count++][1] = y;
    }
    return count;
}

/**
 * Removes the wall between the current cell and its neighbor.
 * Checks whether the neighbor is in the same row or column and draws a line to remove the wall.
 * @param cell current cell
 * @param neighbor cell neighbor
 */
static void remove_wall(cell_t *current, cell_t *next, size_t direction)
{
    switch (direction)
    {
    case 0:
        current->north = false;
        next->south = false;
        break;
    case 1:
        current->east = false;
        next->west = false;
        break;
    case 2:
        current->south = false;
        next->north = false;
        break;
    case 3:
        current->west = false;
        next->east = false;
        break;
    }
}

/**
 * Generate maze using DFS algorithm
 * @param maze maze
 */
static void generate_maze(maze_t *maze)
{
    size_t x = 0, y = 0;
    maze->cells[y][x].visited = true;
    push(&maze->stack, x, y);

    while (!is_empty(maze->stack))
    {
        size_t neighbors[4][2];
        size_t count = get_neighbors(maze, x, y, neighbors);

        if (count > 0)
        {
            size_t r = rand() % count;
            size_t nx = neighbors[r][0];
            size_t ny = neighbors[r][1];
            size_t direction = (nx == x) ? ((ny > y) ? 2 : 0) : ((nx > x) ? 1 : 3);

            remove_wall(&maze->cells[y][x], &maze->cells[ny][nx], direction);
            maze->cells[ny][nx].visited = true;
            push(&maze->stack, x, y);
            x = nx;
            y = ny;
        }
        else
        {
            pop(&maze->stack, &x, &y);
        }
    }
}

/**
 * Generates a random integer between lower and upper bounds (inclusive).
 *
 *
 * @param lower The lower bound of the range.
 * @param upper The upper bound of the range.
 * @return size_t A random integer between lower and upper bounds.
 */
static size_t generate_random(size_t lower, size_t upper)
{
    if (lower > upper)
    {
        int temp = lower;
        lower = upper;
        upper = temp;
    }

    return (rand() % (upper - lower + 1)) + lower;
}

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

/**
 * Build buttons assets from buttons templates
 * @return list of button assets
 */
static list_t *build_btn_assets(size_t NUM_BTN_ELEMENTS, btn_element_t *btn_elements)
{
    list_t *assets = list_init(NUM_BTN_ELEMENTS, (free_func_t)asset_destroy);
    for (size_t i = 0; i < NUM_BTN_ELEMENTS; i++)
    {
        asset_t *asset = create_btn(btn_elements[i]);
        list_add(assets, asset);
    }
    return assets;
}

/**
 * Initialize and randomly generate a buildings
 * @param maze_state state of the maze
 */
static void buildings_init(maze_state_t *maze_state)
{
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        size_t rand_x = (rand() % GRID_WIDTH) + 1;
        size_t rand_y = (rand() % GRID_HEIGHT) + 1;

        rgb_color_t *random_color = NULL;
        do
        {
            random_color = color_get_random();
        } while (random_color->r == 0 && random_color->g == 0 && random_color->b == 0);

        maze_state->buildings[i] = (building_t){
            .position = (vector_t){
                .x = ((GRID_WIDTH - rand_x) * GRID_CELL_SIZE) + (GRID_CELL_SIZE) / 2,
                .y = ((GRID_HEIGHT - rand_y) * GRID_CELL_SIZE) - (GRID_CELL_SIZE / 3)},
        };
        maze_state->buildings[i].img_path = building_paths[i];
    }

    size_t rand = generate_random(0, NUM_BUILDINGS - 1);

    vector_t center = (vector_t){.x = maze_state->buildings[rand].position.x, .y = maze_state->buildings[rand].position.y};
    body_t *body = make_body(center, (rgb_color_t){200, 200, 200});

    maze_state->random_building = body;

    img_element_t mission_img_elements[] = {
        {
            .file_path = maze_state->buildings[rand].img_path,
            .frame = (SDL_Rect){SDL_SCREEN_CENTER.x - 150, SDL_SCREEN_CENTER.y - 200, 300, 300},
        },
    };

    maze_state->imgs = build_img_assets(MISSION_PAGE_IMG_ELEMENTS, mission_img_elements);
    maze_state->texts = build_text_assets(MISSION_PAGE_TEXT_ELEMENTS, mission_text_elements);
    maze_state->btns = build_btn_assets(MISSION_PAGE_BTN_ELEMENTS, mission_btn_elements);
}

maze_state_t *maze_init(state_t *state)
{
    srand(time(NULL));

    maze_state_t *maze_state = malloc(sizeof(maze_state_t) + (sizeof(cell_t) * NUM_BUILDINGS));
    maze_state->maze = create_maze();
    maze_state->time_elapsed = 0;
    hider_seeker_init(state);
    buildings_init(maze_state);

    init_maze(maze_state->maze);
    generate_maze(maze_state->maze);

    return maze_state;
}

/**
 *  Draw maze
 * @param maze maze to draw
 */
static void draw_maze(maze_t *maze)
{
    render_color((rgb_color_t){0, 0, 0});

    for (size_t y = 0; y < GRID_HEIGHT; ++y)
    {
        for (size_t x = 0; x < GRID_WIDTH; ++x)
        {
            cell_t *cell = &maze->cells[y][x];
            if (cell->north)
                render_line(cell->box.x, cell->box.y, cell->box.x + cell->box.w, cell->box.y);
            if (cell->east)
                render_line(cell->box.x + cell->box.w, cell->box.y, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->south)
                render_line(cell->box.x, cell->box.y + cell->box.h, cell->box.x + cell->box.w, cell->box.y + cell->box.h);
            if (cell->west)
                render_line(cell->box.x, cell->box.y, cell->box.x, cell->box.y + cell->box.h);
        }
    }
}

vector_t traverse_maze(state_t *state, vector_t new_vec, size_t movement_direction)
{

    vector_t valid_move = VEC_ZERO;
    maze_t *maze = state->maze_state->maze;

    size_t x = (size_t)new_vec.x / GRID_CELL_SIZE;
    size_t y = GRID_HEIGHT - 1 - ((size_t)new_vec.y / GRID_CELL_SIZE);
    cell_t *cell = &maze->cells[y][x];
    vector_t directions[] = {
        {.x = 0.0, .y = valid_move.y + GRID_CELL_SIZE}, // north
        {.x = valid_move.x + GRID_CELL_SIZE, .y = 0},   // east
        {.x = 0.0, .y = valid_move.y - GRID_CELL_SIZE}, // south
        {.x = valid_move.x - GRID_CELL_SIZE, .y = 0},   // west
    };
    bool walls[] = {
        cell->north,
        cell->east,
        cell->south,
        cell->west,
    };

    if (movement_direction == -1)
    {
        vector_t possible_move[4];
        size_t move_counts = 0;

        for (size_t i = 0; i < 4; i++)
        {
            if (!walls[i])
            {
                possible_move[move_counts++] = directions[i];
            }
        }
        valid_move = possible_move[rand() % move_counts];
    }

    else if (movement_direction >= 0 && movement_direction < 4 && !walls[movement_direction])
    {
        valid_move = directions[movement_direction];
    }
    else
    {
        valid_move = VEC_ZERO;
    }
    return valid_move;
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state)
{
    body_t *beaver = scene_get_body(state->scene, 0);
    vector_t centroid = body_get_centroid(beaver);
    vector_t translation = VEC_ZERO;

    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case LEFT_ARROW:
        {
            translation = traverse_maze(state, centroid, 3);
            break;
        }
        case RIGHT_ARROW:
        {
            translation = traverse_maze(state, centroid, 1);
            break;
        }
        case UP_ARROW:
        {
            translation = traverse_maze(state, centroid, 0);
            break;
        }
        case DOWN_ARROW:
        {
            translation = traverse_maze(state, centroid, 2);
            break;
        }
        }
    }
    move_body(beaver, translation);
}

/**
 * Display the time elapsed in a human-readable format.
 *
 * This function takes the remaining time in seconds and calculates the
 * time elapsed from a predefined total game time. It then formats and
 * prints the remaining time in minutes and seconds if at least one minute
 * has passed, or just seconds if less than a minute has passed.
 *
 * @param remaining_seconds The remaining time in seconds.
 */
static void display_time_elapsed(int32_t remaining_seconds)
{
    int32_t time_elapsed = TOTAL_GAME_TIME - remaining_seconds;

    int32_t minutes = time_elapsed / 60;
    int32_t seconds = time_elapsed % 60;

    if (minutes < 2)
    {
        if (minutes == 1)
        {
            printf("TIME REMAINING: %d min %d sec\n", minutes, seconds);
        }
        else
        {
            printf("TIME REMAINING: %d sec\n", seconds);
        }
    }
}

void show_mission(state_t *state)
{

    list_t *imgs = state->maze_state->imgs;
    for (size_t i = 0; i < list_size(imgs); i++)
    {

        asset_render(list_get(imgs, i));
    }

    list_t *texts = state->maze_state->texts;
    for (size_t i = 0; i < list_size(texts); i++)
    {

        asset_render(list_get(texts, i));
    }

    list_t *btns = state->maze_state->btns;
    for (size_t i = 0; i < list_size(btns); i++)
    {
        asset_render(list_get(btns, i));
    }
}
/**
 * Function handler when game is won
 * @param body1 the first body.
 * @param body2 the second body.
 * @param axis unit vector pointing from body1 toward body2 that
 *    that defines the direction the two bodies are colliding in
 * @param aux the auxiliary value passed to create_collision.
 * @param force_const the force constant passed to create_collision()
 */
static void win_game(body_t *body1, body_t *body2, vector_t axis, void *aux,
                     double force_const)
{
    size_t *page_ptr = (size_t *)aux;
    *page_ptr = 4;
}
void hider_building_collision(state_t *state)
{
    maze_state_t *maze_state = state->maze_state;
    create_collision(state->scene, scene_get_body(state->scene, 0), maze_state->random_building, win_game, &state->page, 0.0);
}
void show_maze(state_t *state, double dt)
{
    sdl_on_key((key_handler_t)on_key);
    state->maze_state->time_elapsed += dt;

    display_time_elapsed((int32_t)state->maze_state->time_elapsed);

    init_grid(state);
    draw_maze(state->maze_state->maze);
    // render_seeker(state, dt);
    render_bodies(state->body_assets);
    // seekers_random_movement(state);
    // hider_seeker_collision(state);
    // hider_building_collision(state);
}
