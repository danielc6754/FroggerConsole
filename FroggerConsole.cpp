using namespace std;

#include "ConsoleEngine.h"
#include <string>

class Frogger : public ConsoleTemplateEngine {
public:
	Frogger() {
		m_sAppName = L"Frogger";
	}
private:
	// Store Level Data
	vector<pair<float, wstring>> vecLanes = {
		{ 0.0f, L"xxx..xxx..xxx..xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxXxxxx" }, // Goal
		{-3.0f, L"...xxxxx....xxxx............xxxx............xx.......xxxx......." }, // L
		{ 3.0f, L"....xxxx.....xxxxx.......xx....xxxx.....xxx.....xxxxx.....xx...." }, // O
		{ 2.0f, L".....xx......xxx.......xxxx......xx....xxx......xxx......xxx...." }, // G
		{ 0.0f, L"................................................................" }, // Safe Zone
		{-3.0f, L"....xx....xx....xx.......xx...xx....xx...xx..xx......xx........." }, // Cars
		{ 3.0f, L"......xxxx..........xxxx............xxxx..........xxxx.........." }, // Buses
		{-4.0f, L"...xx...xx.......xx......xx..........xx.......xx..xx............" }, // Cars
		{ 2.0f, L"....xx....xx....xx..xx....xx.....xx......xx..xx.xx......xx...xx." }, // Cars
		{ 0.0f, L"................................................................" } // Safe Zone
	};

	float fTimeSinceStart = 0.0f;
	// 1 game unit is 8 character size
	int nCellSize = 8;

	TemplateSprite* spriteCar2 = nullptr;

protected:
	virtual bool OnUserCreate() {
		spriteCar2 = new TemplateSprite(L"Assets/car2.spr");

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		fTimeSinceStart += fElapsedTime;

		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

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
				Fill((x + i) * nCellSize - nCellOffset, y * nCellSize, (x + i + 1) * nCellSize - nCellOffset, (y + 1) * nCellSize, graphic);
			}

			y++;
		}
		
		return true;
	}
};

int main() {
	Frogger game;
	game.ConstructConsole(128, 80);
	game.Start();

	return 0;
}