#pragma once
#include "../util/config.h"
#include "../util/debug.h"

struct character{
	uint8_t char_val;
	int x;
	int y;
};
struct shellstate_t{
	int num_keys;
	uint8_t shell_state;
	struct character in_buf[256];
	int pnt_buf;
	struct character curr_cmd[50];		//current command
	int pnt_cmd;
	uint64_t output;
	int cursor_x;
	int cursor_y;
};

struct renderstate_t{
	int num_keys;
	uint8_t shell_state;
	struct character out_buf[256];
	uint64_t output;
};

// struct shellstate_t{
// 	int num_keys;
// 	uint8_t shell_state;
// 	uint8_t input[256];
// 	uint64_t output;
// 	string errorMsg;
// 	int cursor_x;
// 	int cursor_y;

// };

// struct renderstate_t{
// 	int num_keys;
// 	uint8_t shell_state;
// 	// uint8_t input[256];
// 	uint64_t output;
// 	string errorMsg;
// 	int cursor_x;
// 	int cursor_y;

// };

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

