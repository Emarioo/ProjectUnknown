#pragma once

#include "Engone/Profiling/ScopeTimer.h"

#include "Engone/Engone.h"

namespace engone {

	Profiler::Profiler() : Execution("Profiler") {

	}
	void Profiler::reset() {
		for (int i = 0; i < m_timedSections.size(); i++) {
			TimedSection& s0 = m_timedSections[i];
			s0.loopSeconds = s0.accSeconds;
			s0.loopCount = s0.accCount;
			s0.accSeconds = 0;
			s0.accCount = 0;
		}
	}
	void Profiler::updateSection(const std::string& name, double seconds) {
		auto find = m_timedSectionsMap.find(name);
		if (find == m_timedSectionsMap.end()) {
			uint32 index = m_timedSections.size();
			m_timedSections.push_back({});
			m_timedSections.back().name = name;
			m_timedSections.back().accSeconds += seconds;
			m_timedSections.back().accCount += seconds;
			m_timedSections.back().totalSeconds += seconds;
			m_timedSectionsMap[name] = index;
		} else {
			m_timedSections[find->second].accSeconds += seconds;
			m_timedSections[find->second].accCount += seconds;
			m_timedSections[find->second].totalSeconds += seconds;
		}
	}
	void Profiler::Sampler::increment() {
		totalSamples++;
		countSamples[sampleIndex]++;
	}
	void Profiler::Sampler::next(double delta) {
		deltaSamples[sampleIndex] = delta;

		constexpr double AVG_RATE = 0.1;

		avgDelay += delta;
		if (avgDelay>=AVG_RATE) {
			avgDelay -= AVG_RATE;
			average = 0;
			double sumDelta = 0;
			for (int i = 0; i < MAX_SAMPLES; i++) {
				average += countSamples[i];
				sumDelta += deltaSamples[i];
			}
			average /= sumDelta;
		}

		sampleIndex = (sampleIndex+1)%MAX_SAMPLES;
		countSamples[sampleIndex] = 0;
		deltaSamples[sampleIndex] = 0;
	}
	double Profiler::Sampler::getAverage() {
		return average;
	}
	Profiler::Sampler& Profiler::getSampler(void* key) {
		return m_samples[key];
	}
	//void Profiler::show() {
	//	auto win = engone::GetActiveWindow();
	//	auto app = win->getParent();
	//	if (app->isRenderingWindow()) {
	//		m_profilingData[win].show = true;
	//	} else {
	//		//m_profilingData[app].show = true;
	//	}
	//}
	//void Profiler::hide() {
	//	auto win = engone::GetActiveWindow();
	//	auto app = win->getParent();
	//	if (app->isRenderingWindow()) {
	//		m_profilingData[win].show = false;
	//	} else {
	//		//m_profilingData[app].show = false;
	//	}
	//}
	void Profiler::renderBasicDebugInfo(LoopInfo& info) {
		//if (IsKeyDown(GLFW_KEY_F3) && IsKeyPressed(GLFW_KEY_B)) {
		if (IsKeyPressed(GLFW_KEY_F3)) {
			m_profilingData[info.window].show = !m_profilingData[info.window].show;
		}

		if(!m_profilingData[info.window].show) return;

		float sw = GetWidth();
		float sh = GetHeight();

		const int bufferSize = 50;
		char str[bufferSize];

		float rightEdge = sw - 3;
		float upEdge = 3;

		// should be changed
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		auto& stats = info.app->getEngine()->getStats();
		float scroll = IsScrolledY();
		if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
			scroll *= 100;
		} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			scroll *= 10;
		}

		const ui::Color highlighted = { 0.5f,0.8f,1.f,1.f };
		const ui::Color normalColor = { 1.f,1.0f,1.0f,1.f };

		std::string runStr = "Runtime ";
		runStr += FormatTime(info.app->getRuntime(), true, FormatTimeH | FormatTimeM | FormatTimeS | FormatTimeMS);
		ui::TextBox runtimeBox = { runStr, 0,0,20,consolas,normalColor };
		runtimeBox.x = rightEdge - runtimeBox.getWidth();
		runtimeBox.y = upEdge;
		upEdge = runtimeBox.y + runtimeBox.h;

		//std::string sleepStr = "Sleep time ";
		//
		//snprintf(str, bufferSize, "(%.1f%%) ", 100 * -1 / info.app->getRuntime()); // <- using '-1' as sleep time because it hasn't been implemented and perhaps shouldn't exist
		//sleepStr += str;
		//sleepStr += FormatTime(stats.getSleepTime(), true, FormatTimeH | FormatTimeM | FormatTimeS | FormatTimeMS);
		//ui::TextBox sleepBox = { sleepStr, 0,0,20,consolas,normalColor };
		//sleepBox.x = rightEdge - sleepBox.getWidth();
		//sleepBox.y = upEdge;
		//upEdge = sleepBox.y + sleepBox.h;

		snprintf(str, bufferSize, "%.2f (%.2f) FPS", info.window->getRealFPS(), info.window->getFPS());
		ui::TextBox fpsBox = { str,0,0,20,consolas,normalColor };
		fpsBox.x = rightEdge - fpsBox.getWidth();
		fpsBox.y = upEdge;
		upEdge = fpsBox.y + fpsBox.h;
		if (ui::Hover(fpsBox)) {
			fpsBox.rgba = highlighted;
			if (scroll != 0) {
				info.window->setFPS(info.window->getFPS() + scroll);
			}
		}

		snprintf(str, bufferSize, "%.2f (%.2f) UPS", info.app->getRealUPS(), info.app->getUPS());
		ui::TextBox upsBox = { str,0,0,20,consolas,normalColor };
		upsBox.x = rightEdge - upsBox.getWidth();
		upsBox.y = upEdge;
		upEdge = upsBox.y + upsBox.h;
		if (ui::Hover(upsBox)) {
			upsBox.rgba = highlighted;
			if (scroll != 0) {
				info.app->setUPS(info.app->getUPS() + scroll);
			}
		}

		//snprintf(str, bufferSize, "%.2f%% Game Speed", stats.getGameSpeed() * 100);
		//ui::TextBox speedBox = { str,0,0,20,consolas,normalColor };
		//speedBox.x = rightEdge - speedBox.getWidth();
		//speedBox.y = upEdge;
		//upEdge = speedBox.y + speedBox.h;
		//if (ui::Hover(speedBox)) {
		//	speedBox.rgba = highlighted;
		//	if (scroll != 0) {
		//		stats.setGameSpeed(stats.getGameSpeed() + scroll / 100);
		//	}
		//}


		const ui::Color areaColor = { 0,0.3 };
		float stoof[] = { fpsBox.getWidth() , upsBox.getWidth(), runtimeBox.getWidth()};
		//float stoof[] = { fpsBox.getWidth() , upsBox.getWidth(), runtimeBox.getWidth(), sleepBox.getWidth(),speedBox.getWidth() };
		float areaW = 0;
		for (int i = 0; i < sizeof(stoof) / sizeof(float); i++) {
			if (stoof[i] > areaW) {
				areaW = stoof[i];
			}
		}
		float areaH = upsBox.y + upsBox.h;
		ui::Box area = { 0,0,areaW + 6,areaH + 6,areaColor };
		area.x = sw - area.w;
		area.y = 0;

		ui::Draw(area);
		ui::Draw(runtimeBox);
		//ui::Draw(sleepBox);
		ui::Draw(fpsBox);
		ui::Draw(upsBox);
		//ui::Draw(speedBox);
	}
	void Profiler::render(engone::LoopInfo& info) {
		renderBasicDebugInfo(info);
	}
	std::vector<Profiler::TimedSection>& Profiler::getSections() {
		return m_timedSections;
	}
	void Profiler::sort() {
		for (int i = 0; i < m_timedSections.size(); i++) {
			bool sorted = true;
			for (int j = 0; j < m_timedSections.size()-1-i;j++) {
				TimedSection& s0 = m_timedSections[j];
				TimedSection& s1 = m_timedSections[j+1];
				if (s0.loopSeconds < s1.loopSeconds) {
					TimedSection temp = s0;
					s0 = s1;
					s1 = temp;
					sorted = false;
				}
			}
			if (sorted)
				break;
		}
	}

	ScopeTimer::ScopeTimer(const std::string& name, Profiler* profiler) : name(name), profiler(profiler) {
		startSeconds = engone::GetSystemTime();
	}
	ScopeTimer::~ScopeTimer() {
		double endSeconds = engone::GetSystemTime();
		if (profiler) {
			profiler->updateSection(name, endSeconds - startSeconds);
		}
	}
	void test() {
		Profiler profiler;
		{
			TIME_SECTION(test, &profiler)
		
		}
	}
}