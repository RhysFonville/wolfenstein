#include <Windows.h>
#include <vector>
#include <string>
#include <numbers>
#include <chrono>

#define SCREEN_AREA (screen_width * screen_height)
#define MAP_AREA (map_width * map_height)

#define WALL '#'

constexpr int screen_width = 120u;
constexpr int screen_height = 40u;

float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

constexpr int map_width = 16u;
constexpr int map_height = 16u;

std::string map;

float fov = (float)(std::numbers::pi * 4.0);
float max_view_depth = MAP_AREA;

int main() {
	std::string screen;
	screen.resize(screen_width * screen_height);
	HANDLE console  = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD chars_written = 0ul;

	map += "################";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "#              #";
	map += "################";

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = tp1;

	while (true) {
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed_time = tp2 - tp1;
		tp1 = tp2;
		float dt = elapsed_time.count();

		if (GetAsyncKeyState((USHORT)'W') & 0x8000) {
			playerX += sinf(playerA) * 5.0f * dt;
			playerY += cosf(playerA) * 5.0f * dt;
		}
		if (GetAsyncKeyState((USHORT)'A') & 0x8000) {
			playerA -= 0.5f * dt;
		}
		if (GetAsyncKeyState((USHORT)'S') & 0x8000) {
			playerX -= sinf(playerA) * 5.0f * dt;
			playerY -= cosf(playerA) * 5.0f * dt;
		}
		
		if (GetAsyncKeyState((USHORT)'D') & 0x8000) {
			playerA += 0.5f * dt;
		}

		for (int x = 0; x < screen_width; x++) {
			float ray_angle = (playerA - fov / 2.0f) + ((float)x / (float)screen_width) * fov;
			float distance_to_wall = 0.0f;

			float eyeX = sinf(ray_angle);
			float eyeY = cosf(ray_angle);

			bool ray_hit_wall = false;
			while (!ray_hit_wall && distance_to_wall < max_view_depth) {
				distance_to_wall += 0.1f;

				int testX = (int)(playerX + eyeX * distance_to_wall);
				int testY = (int)(playerY + eyeY * distance_to_wall);

				if (testX < 0 || testX >= map_width || testY < 0 || testY >= map_height) {
					ray_hit_wall = true;
					distance_to_wall = max_view_depth;
				} else if (map[testX * map_width + testY] == WALL) {
					ray_hit_wall = true;
				}
			}

			int ceiling = (float)(screen_height / 2.0f) - screen_height / distance_to_wall;
			int floor = screen_height - ceiling;

			for (int y = 0; y < screen_height; y++) {
				if (y <= ceiling)
					screen[y * screen_width + x] = ' ';
				else if (y > ceiling && y <= floor)
					screen[y * screen_width + x] = '#';
				else
					screen[y * screen_width + x] = ' ';
			}
		}

		screen[screen_width * screen_height - 1] = '\0';
		WriteConsoleOutputCharacterA(console, &screen[0], screen_width * screen_height, { 0, 0 }, &chars_written);
	}
}
