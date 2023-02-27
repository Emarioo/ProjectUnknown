
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
		//auto find = m_samples.find(key);
		//if (find == m_samples.end()) {
		//	m_samples.insert(std::pair<void* const,Sampler>(key, {}));
		//	return m_samples[key];
		//}
		return m_samples[key];
	}
	Profiler::Graph& Profiler::getGraph(const std::string& name) {
		return m_graphs[name];
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
		//if (IsKeyPressed(GLFW_KEY_F3)) {
		//	m_profilingData[info.window].show = !m_profilingData[info.window].show;
		//}

		//if(!m_profilingData[info.window].show) return;

		float sw = GetWidth();
		float sh = GetHeight();

		const int bufferSize = 50;
		char str[bufferSize];

		float rightEdge = sw - 3;
		float upEdge = 3;

		// should be changed
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

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

		snprintf(str, bufferSize, "%.2f (%.2fms, %.2f) FPS", info.window->getRealFPS(), 1000./info.window->getRealFPS(),info.window->getFPS());
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

		snprintf(str, bufferSize, "%.2f (%.2fms, %.2f) UPS", info.app->getRealUPS(), 1000./info.app->getRealUPS(), info.app->getUPS());
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


		const ui::Color areaColor = { 0.f,0.3 };
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
	void Profiler::render(LoopInfo& info) {
		if (IsKeyPressed(GLFW_KEY_F3)) {
			m_profilingData[info.window].show = !m_profilingData[info.window].show;
		}

		if (!m_profilingData[info.window].show) return;

		renderBasicDebugInfo(info);
		renderGraphs(info);
		//updateGraph.renderBack(info);
		//updateGraph.render(info);
		//renderGraph.render(info);
	}
	void Profiler::renderGraphs(LoopInfo& info) {
		//-- Base graph timeline
		const ui::Color timelineColor = { 1,1,1,1 };
		const ui::Color backColor = { 0,0,0,0.1 };

		ui::Box back = { 0,0,GetWidth(),40,backColor };
		ui::Box timeline = { 0,0,GetWidth(),2 ,timelineColor };
		timeline.y = GetHeight() - timeline.h - 40;
		back.y = timeline.y - back.h / 2;
		ui::Draw(back);
		ui::Draw(timeline);
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		//ui::TextBox zoomText{ std::to_string(zoom),0,0,20,consolas,{1}};
		//zoomText.x = 10;
		//zoomText.y = GetHeight()-150;
		//ui::Draw(zoomText);

		//-- Input
		float speedX = 300;
		float zoomPower = 4;
		if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
			speedX *= 16;
			zoomPower *= 1.04;
		} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			speedX *= 4;
			zoomPower *= 1.02;
		}

		if (IsKeyDown(GLFW_KEY_LEFT)) {
			offsetX += speedX / zoom * info.timeStep;
		}
		if (IsKeyDown(GLFW_KEY_RIGHT)) {
			offsetX -= speedX / zoom * info.timeStep;
		}
		if (IsKeyDown(GLFW_KEY_UP)) {
			float prevZoom = zoom;
			zoom *= pow(zoomPower, info.timeStep);
			//offsetX += GetWidth() / 2 * (zoom-prevZoom);
		}
		if (IsKeyDown(GLFW_KEY_DOWN)) {
			float prevZoom = zoom;
			zoom /= pow(zoomPower, info.timeStep);
			//offsetX += GetWidth() / 2 * (zoom - prevZoom);
			//offsetX = offsetX * prevZoom / zoom;
		}

		//-- Graphs
		for (auto& pair : m_graphs) {
			renderGraph(info, pair.second);
		}
	}
	Profiler::Graph::~Graph() {
		if(points)
			Free(points,sizeof(float)*MAX_POINTS);
	}
	void Profiler::Graph::plot(double time) {
		if (!points) {
			points = (float*)Allocate(sizeof(float) * MAX_POINTS);
			for (int i = 0; i < MAX_POINTS; i++) {
				points[i] = 0;
			}
		}
		points[pushIndex] = time - engone::GetActiveWindow()->getParent()->getEngine()->getUpdateTimer().startTime;
		pushIndex = (pushIndex + 1) % MAX_POINTS;
	}
	void Profiler::renderGraph(LoopInfo& info, Graph& graph) {
		//CommonRenderer* renderer = GET_COMMON_RENDERER();
		//renderer->
		
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		ui::Box timeline = { 0,0,GetWidth(),2};
		timeline.y = GetHeight()-timeline.h-40;

		//-- time points
		
		// This stuff is complicated and annoying. 2 hours gone
#ifdef gone
		//float timeX = 0;
		//float pixelX = GetWidth() / 2 + (timeX - GetWidth() / 2 + offsetX) * zoom;

		//float minX = offsetX * zoom;
		//float maxX = minX + GetWidth();
		//float pixelX = GetWidth();
		//float timeX = (pixelX - GetWidth() / 2) / zoom - offsetX + GetWidth()/2;
		char buffer[100];
		//snprintf(buffer,sizeof(buffer),"%.1f",timeX);

		//float pixelSpacing = 200;
		float timeSpacing = GetWidth()/zoom/5;

		float hm = 5*(zoom*zoom);
		float tsd = floor(zoom*zoom) * hm;
		if(tsd==0||hm==0)
			timeSpacing = GetWidth() / 5;
		else
			timeSpacing = GetWidth()/ tsd / 5;
		log::out << "timeSpacing " << timeSpacing << "\n";

		float pixelView = GetWidth();
		//float pixelView = GetWidth()+timeSpacing;
		int amount = ceil((pixelView / timeSpacing) / zoom); // do ceil?
		log::out << "Amount: " << amount << "\n";
		for (int i = 0; i < amount;i++) {
			float timePoint = i * timeSpacing;
			
			float pixelX = GetWidth() / 2 + (timePoint - GetWidth() / 2 + offsetX) * zoom;

			float view = pixelView * zoom;
			
			//if (pixelX < 0) {
				//int hops = floor((pixelX) / pixelView);
				////int nhops = floor((pixelX + timeSpacing / 2) / view);
				//if (hops != 0) {
				//	float pv= pixelX;
				//	pixelX -= pixelView * hops;
				//	timePoint -= hops * pixelView;
				//	log::out << timePoint << " " << hops << " "<<pv<<" "<<pixelX << "\n";
				//}
			//} 
			//log::out << timePoint << " " << hops << " "<< pv<<" "<<pixelX << "\n";
			//log::out << timePoint << " " << pixelX<<"\n";

			snprintf(buffer, sizeof(buffer), "%.2f", timePoint);

			ui::TextBox time{ buffer,0,0,16,consolas,{1} };
			
			ui::Box stripe = { 0,0,2,20,{1} };
			stripe.x = pixelX;
			stripe.y = timeline.y - stripe.h;

			time.x = stripe.x+4;
			time.y = stripe.y;
			ui::Draw(stripe);
			ui::Draw(time);
		}
#endif
		//-- points
		if (!graph.points)
			return;

		//log::out << "st " << info.app->getEngine()->getUpdateTimer().startTime << "\n";
		for (int i = 0; i < Graph::MAX_POINTS;i++) {
			float x = graph.points[i];
			if (x == 0)
				continue;
			//if(i == graph.pushIndex-1)
			//	log::out << " " <<x<< "\n";
			ui::Box box = { 0,0,2,20,graph.color };
			box.y = timeline.y + graph.offsetY;
			box.x = GetWidth()/2+(x-GetWidth()/2 + offsetX) * zoom;
			//+GetWidth() / 2;
			ui::Draw(box);
		}
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
	// void test() {
	// 	Profiler profiler;
	// 	{
	// 		TIME_SECTION(test, &profiler)
		
	// 	}
	// }
}