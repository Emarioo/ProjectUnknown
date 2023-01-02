
#include "ProUnk/UI/UIMenus.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void DebugInfo(engone::LoopInfo& info, GameApp* app) {
		using namespace engone;
		
		float sw = GetWidth();
		float sh = GetHeight();

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		// do extra nice format with kilobytes and megabytes
		const int BUFFER_SIZE = 50;
		char str[BUFFER_SIZE];
		int bufferSize = BUFFER_SIZE;
		float sps = app->getActiveSession()->getNetworkStats().getBytesSentPerSecond();
		int at = 0;
		at += snprintf(str, bufferSize, "sending ");
		//if (sps<1000) {
			at += snprintf(str+at, bufferSize-at, "%.2f ", sps);
		//} else if(sps<1000000) {
		//	sps /= 1000;
		//	at += snprintf(str + at, bufferSize-at, "%.2f K", sps);
		//} else {
		//	sps /= 1000000;
		//	at += snprintf(str + at, bufferSize-at, "%.2f M", sps);
		//}
		snprintf(str + at, bufferSize-at, "B/S");
		
		ui::TextBox sent = { str,0,0,20,consolas,{1.f,1.f,1.f,1.f} };
		sent.x = sw-10-consolas->getWidth(str, sent.h);
		sent.y = 10;
		ui::Draw(sent);
		
		float rps = app->getActiveSession()->getNetworkStats().getBytesReceivedPerSecond();
		at = 0;
		bufferSize = BUFFER_SIZE;
		at += snprintf(str, bufferSize, "receiving ");
		//if (rps < 1000) {
			at += snprintf(str + at, bufferSize - at, "%.2f ", rps);
		//}
		//else if (rps < 1000000) {
		//	rps /= 1000;
		//	at += snprintf(str + at, bufferSize - at, "%.2f K", rps);
		//}
		//else {
		//	rps /= 1000000;
		//	at += snprintf(str + at, bufferSize - at, "%.2f M", rps);
		//}
		snprintf(str + at, bufferSize - at, "B/S");

		ui::TextBox recv = { str,0,0,20,consolas,{1.f,1.f,1.f,1.f} };
		recv.x = sw - 10 - consolas->getWidth(str, recv.h);
		recv.y = 10+sent.y+sent.h;
		ui::Draw(recv);


		//-- FPS / UPS
		auto& stats = info.app->getEngine()->getStats();
		float scroll = IsScrolledY();

		const ui::Color highlighted = { 1.f,1.f,1.f,1.f };
		const ui::Color normalColor = { 0.9f,0.9f,0.9f,0.9f };

		snprintf(str, bufferSize, "%.2f (%.2f) FPS",stats.getFPS(), stats.getFPSLimit());

		ui::TextBox fpsBox = { str,0,0,20,consolas,normalColor };
		fpsBox.x = sw - 10 - fpsBox.getWidth();
		fpsBox.y = 10 + recv.y + sent.h;

		snprintf(str, bufferSize, "%.2f (%.2f) UPS",stats.getUPS(),stats.getUPSLimit());

		ui::TextBox upsBox = { str,0,0,20,consolas,normalColor };
		upsBox.x = sw - 10 - upsBox.getWidth();
		upsBox.y = 10 + fpsBox.y + fpsBox.h;
		
		//log::out << scroll << "\n";

		if (ui::Hover(fpsBox)) {
			fpsBox.rgba = highlighted;
			//log::out << "fpsbox " << scroll << "\n";
			if (scroll != 0) {
				stats.setFPSLimit(info.app->getEngine()->getStats().getFPSLimit() + scroll);
			}
		}
		if (ui::Hover(upsBox)) {
			upsBox.rgba = highlighted;
			//log::out << "upsbox " << scroll << "\n";
			if (scroll != 0) {
				stats.setUPSLimit(info.app->getEngine()->getStats().getUPSLimit() + scroll);
			}
		}

		ui::Draw(fpsBox);
		ui::Draw(upsBox);
	}
}