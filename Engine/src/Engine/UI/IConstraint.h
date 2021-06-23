#pragma once

namespace engine {

	class IBase;

	class IConstraint {
	public:
		// More constraint options for width and height. Fixed width and height for example
		IConstraint(IBase* e, bool use_xAxis);
		IBase* Center(int pos);
		IBase* Center(float pos);
		IBase* Center(IBase* base, int pos);
		IBase* Center(IBase* base, float pos);
		IBase* Left(int pos);
		IBase* Left(float pos);
		IBase* Left(IBase* base, int pos);
		IBase* Left(IBase* base, float pos);
		IBase* Right(int pos);
		IBase* Right(float pos);
		IBase* Right(IBase* base, int pos);
		IBase* Right(IBase* base, float pos);
		IBase* Bottom(int pos);
		IBase* Bottom(float pos);
		IBase* Bottom(IBase* base, int pos);
		IBase* Bottom(IBase* base, float pos);
		IBase* Top(int pos);
		IBase* Top(float pos);
		IBase* Top(IBase* base, int pos);
		IBase* Top(IBase* base, float pos);
		void Add(int pos);
		void Add(float pos);
		float Value(float w);

		float raw = 0;
	private:
		IBase* _this = nullptr;

		bool center = false;
		bool xAxis = false;
		bool side = false; // positive or negative axis
		IBase* parent = nullptr;

		bool isPixel = false;
	};
}