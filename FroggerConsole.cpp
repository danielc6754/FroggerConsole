using namespace std;

#include "ConsoleEngine.h"
#include <string>

class Frogger : public ConsoleTemplateEngine {
public:
	Frogger() {
		m_sAppName = L"Frogger";
	}
private:
	// Store Level Data  // 64 elements per lane
	vector<pair<float, wstring>> vecLanes = {
		{0.0f,  L"wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // Goal
		{-3.0f, L",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"}, // L
		{3.0f,  L",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"}, // O
		{2.0f,  L",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"}, // G
		{0.0f,  L"rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"}, // Safe Zone
		{-3.0f, L"....asdf.......asdf....asdf..........asdf........asdf....asdf..."}, // Bus
		{ 3.0f, L".....cv..cv....cv....cv.....cv........cv..cv.cv......cv.......cv"}, // Car1
		{-4.0f, L"..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."}, // Car2
		{2.0f,  L"..cv.....cv.......cv.....cv......cv..cv.cv.......cv....cv......."}, // Car1
		{0.0f,  L"rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"}  // Safe Zone
	};

	float fTimeSinceStart = 0.0f;
	// 1 game unit is 8 character size
	int nCellSize = 8;

	float fFrogX = 8.0f;
	float fFrogY = 9.0f;

	// Separate buffer safe and unsafe
	bool* bufDanger = nullptr;

	// Sprites
	TemplateSprite* spriteBus = nullptr;
	TemplateSprite* spriteLog = nullptr;
	TemplateSprite* spriteWater = nullptr;
	TemplateSprite* spriteFrog = nullptr;
	TemplateSprite* spritePavement = nullptr;
	TemplateSprite* spriteCar1 = nullptr;
	TemplateSprite* spriteCar2 = nullptr;
	TemplateSprite* spriteWall = nullptr;
	TemplateSprite* spriteHome = nullptr;

protected:
	virtual bool OnUserCreate() {
		spriteBus = new TemplateSprite(L"Assets/bus.spr");
		spriteLog = new TemplateSprite(L"Assets/log.spr");
		spriteWater = new TemplateSprite(L"Assets/water.spr");
		spriteFrog = new TemplateSprite(L"Assets/frog.spr");
		spritePavement = new TemplateSprite(L"Assets/pavement.spr");
		spriteCar1 = new TemplateSprite(L"Assets/car1.spr");
		spriteCar2 = new TemplateSprite(L"Assets/car2.spr");
		spriteWall = new TemplateSprite(L"Assets/wall.spr");
		spriteHome = new TemplateSprite(L"Assets/home.spr");

		bufDanger = new bool[ScreenWidth() * ScreenHeight()];
		memset(bufDanger, 0, ScreenWidth() * ScreenHeight() * sizeof(bool));

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		fTimeSinceStart += fElapsedTime;

		// Handle Input
		if (m_keys[VK_UP].bReleased) fFrogY -= 1.0f;
		if (m_keys[VK_DOWN].bReleased) fFrogY += 1.0f;
		if (m_keys[VK_LEFT].bReleased) fFrogX -= 1.0f;
		if (m_keys[VK_RIGHT].bReleased) fFrogX += 1.0f;

		// Move frog by platforsm
		if (fFrogY <= 3)
			fFrogX -= fElapsedTime * vecLanes[(int)fFrogY].first;

		// Collision Detection - check four corners fo frog to see if out of danger buffer
		bool tl = bufDanger[(int)(fFrogY * nCellSize + 1) * ScreenWidth() + (int)(fFrogX * nCellSize + 1)];
		bool tr = bufDanger[(int)(fFrogY * nCellSize + 1) * ScreenWidth() + (int)((fFrogX + 1) * nCellSize - 1)];
		bool bl = bufDanger[(int)((fFrogY + 1) * nCellSize - 1) * ScreenWidth() + (int)(fFrogX * nCellSize + 1)];
		bool br = bufDanger[(int)((fFrogY + 1) * nCellSize - 1) * ScreenWidth() + (int)((fFrogX + 1) * nCellSize - 1)];

		if (tl || tr || bl || br) {
			// Frog died
			fFrogX = 8.0f;
			fFrogY = 9.0f;
		}
		
		// Draw Lanes
		int x = -1, y = 0;

		for (auto lane : vecLanes) {
			// Find offset
			int nStartPos = (int)(fTimeSinceStart * lane.first) % 64;
			// Need to manually check for negative velocity of objects
			if (nStartPos < 0)
				nStartPos = 64 - (abs(nStartPos) % 64);

			// Using fixed point math to use fraction values to fix offset fix blocky movement
			// Scale remainders and modulate it to fit in cell size
			int nCellOffset = (int)((float)nCellSize * fTimeSinceStart * lane.first) % nCellSize;

			for (int i = 0; i < (ScreenWidth() / nCellSize) + 2; i++) {
				wchar_t graphic = lane.second[(nStartPos + i) % 64];
				//Fill((x + i) * nCellSize - nCellOffset, y * nCellSize, (x + i + 1) * nCellSize - nCellOffset, (y + 1) * nCellSize, graphic);
			
				switch (graphic) {
				case L'a': // Bus 1
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteBus, 0, 0, 8, 8);
					break;
				case L's': // Bus 2
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteBus, 8, 0, 8, 8);
					break;
				case L'd': // Bus 3
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteBus, 16, 0, 8, 8);
					break;
				case L'f': // Bus 4
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteBus, 24, 0, 8, 8);
					break;
				
				case L'j': // Log 1
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteLog, 0, 0, 8, 8);
					break;
				case L'l': // Log 2
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteLog, 8, 0, 8, 8);
					break;
				case L'k': // Log 3
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteLog, 16, 0, 8, 8);
					break;
				
				case L'z': // Car1 Front
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar1, 0, 0, 8, 8);
					break;
				case L'x': // Car1 Back
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar1, 8, 0, 8, 8);
					break;
				case L'c': // Car2 Front
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar2, 0, 0, 8, 8);
					break;
				case L'v': // Car2 Back
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar2, 8, 0, 8, 8);
					break;

				case L'w': // Wall
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteWall, 0, 0, 8, 8);
					break;
				case L'h': // Home
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteHome, 0, 0, 8, 8);
					break;
				case L',': // Water
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteWater, 0, 0, 8, 8);
					break;
				case L'r': // Pavement
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spritePavement, 0, 0, 8, 8);
					break;

				case L'.': // Road
					Fill((x + i) * nCellSize - nCellOffset, y * nCellSize, (x + i + 1) * nCellSize - nCellOffset, (y + 1) * nCellSize, L' ');
					break;
				/*case L'p': // Player Frogger
					DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteFrog, 0, 0, 8, 8);
					break;*/
				}

				// FIll Danger buffer
				for (int j = (x + i) * nCellSize - nCellOffset; j < (x + i + 1) * nCellSize - nCellOffset; j++)
					for (int k = y * nCellSize; k < (y + 1) * nCellSize; k++)
						if (j >= 0 && j < ScreenWidth() && k >= 0 && k < ScreenHeight())
							bufDanger[k * ScreenWidth() + j] = !(graphic == L'.' || graphic == L'j' || graphic == L'k' || graphic == L'l' || graphic == L'r' || graphic == L'h');
			}

			y++;
		}

		// Draw Frog
		DrawSprite(fFrogX * nCellSize, fFrogY * nCellSize, spriteFrog);
		
		return true;
	}
};

int main() {
	Frogger game;
	game.ConstructConsole(128, 80);
	game.Start();

	return 0;
}