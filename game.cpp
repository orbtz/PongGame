#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player1_p, player1_dp;
float player2_p, player2_dp;

float player_half_size_x = 2.5f;
float player_half_size_y = 12.f;

float arena_half_size_x = 85;
float arena_half_size_y = 45;

float ball_p_x;
float ball_p_y;
float ball_dp_x = 180;
float ball_dp_y;
float ball_half_size = 1;

int player01_score;
int player02_score;

internal bool aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {

	return (
		p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y + hs1y < p2y + hs2y
		);
}

internal void simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp *= -1;
	}
	else if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp *= -1;
	}
}

enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
};
int hot_button;

Gamemode current_gamemode;
bool enemy_is_ai;

internal void simulate_game(Input* input, float dt) {
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xffaa33);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xff5500);

	if (current_gamemode == GM_GAMEPLAY) {
		float player1_ddp = 0.f;
		float player2_ddp = 0.f;

		if (enemy_is_ai) {
			if (is_down(BUTTON_UP)) {
				player1_ddp += 2000;
			}
			if (is_down(BUTTON_DOWN)) {
				player1_ddp -= 2000;
			}
		}
		else {
			/*if ( ball_p_y > player1_p ) {
				player1_ddp += 1000;
			}
			if ( ball_p_y < player1_p ) {
				player1_ddp -= 1000;
			}*/

			player1_ddp = (ball_p_y - player1_p) * 90;

			if (player1_ddp > 1200) {
				player1_ddp = 1200;
			}
			if (player1_ddp < -1200) {
				player1_ddp = -1200;
			}

		}

		if (is_down(BUTTON_W)) {
			player2_ddp += 2000;
		}
		if (is_down(BUTTON_S)) {
			player2_ddp -= 2000;
		}

		simulate_player(&player1_p, &player1_dp, player1_ddp, dt);
		simulate_player(&player2_p, &player2_dp, player2_ddp, dt);

		// Simulate Ball
		{

			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;

			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 80, player1_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = 80 - player_half_size_x - ball_half_size;
				ball_dp_x *= -1;

				ball_dp_y = ((ball_p_y - player1_p) * 6.5f) + (player1_dp * .2f);

			}
			else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -80, player2_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = -80 + player_half_size_x + ball_half_size;
				ball_dp_x *= -1;

				ball_dp_y = ((ball_p_y - player2_p) * 6.5f) + (player2_dp * .2f);

			}

			if (ball_p_y + ball_half_size > arena_half_size_y) {
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;
			}
			else if (ball_p_y - ball_half_size < -arena_half_size_y) {
				ball_p_y = -arena_half_size_y + ball_half_size;
				ball_dp_y *= -1;
			}

			if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;

				player01_score++;
			}
			else if (ball_p_x - ball_half_size < -arena_half_size_x) {
				ball_dp_x *= -1;
				ball_dp_y = 0;
				ball_p_x = 0;
				ball_p_y = 0;

				player02_score++;
			}
		}

		draw_number(player01_score, -10, 40, 1.f, 0xbbffbb);
		draw_number(player02_score, 10, 40, 1.f, 0xbbffbb);

		draw_rect(ball_p_x, ball_p_y, 1, 1, 0xffffff);

		draw_rect(-80, player2_p, player_half_size_x, player_half_size_y, 0xff0000);
		draw_rect(80, player1_p, player_half_size_x, player_half_size_y, 0xff0000);

		//draw_rect(60, 40, 5, 25, 0x772271);

	}
	else {

		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {
			hot_button = !hot_button;
		}

		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}

		if (hot_button == 0) {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xff0000);
		}
		else {
			draw_text("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			draw_text("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
		}

		draw_text("PONG TUTORIAL", -73, 40, 2, 0xffffff);

	}
}