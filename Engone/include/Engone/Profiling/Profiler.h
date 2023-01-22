#pragma once

#include "Engone/Core/ExecutionControl.h"

namespace engone {
	class Profiler : public Execution {
	public:
		Profiler();

		void reset(); // next round for time sections?

		void updateSection(const std::string& name, double seconds);

		void render(engone::LoopInfo& info);

		class Sampler {
		public:
			void increment();
			void next(double delta);
			double getAverage();

		private:
			static const uint32 MAX_SAMPLES = 20;
			uint32 totalSamples = 0;
			uint32 sampleIndex = 0;
			uint32 countSamples[MAX_SAMPLES];
			double deltaSamples[MAX_SAMPLES];

			double average = 0;
			double avgDelay = 0;
		};
		Sampler& getSampler(void* key);

		//void show();
		//void hide();

		struct ProfilingData {
			bool show=false;
		};
		struct TimedSection {
			// public
			std::string name;
			double loopSeconds = 0; // can be displayed
			int loopCount = 0;
			double totalSeconds = 0;

			// private
			double accSeconds = 0; // accumulation
			int accCount = 0; // accumulation
		};
		std::vector<TimedSection>& getSections();

	private:
		std::unordered_map<std::string, uint32> m_timedSectionsMap;
		
		void sort();

		void renderBasicDebugInfo(LoopInfo& info);

		std::unordered_map<void*, Sampler> m_samples;
		std::unordered_map<void*, ProfilingData> m_profilingData;

		std::vector<TimedSection> m_timedSections;
	};
}