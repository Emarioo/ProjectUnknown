#pragma once

namespace engine {

	class IBase;
	class IConstraintX {
	public:
		IConstraintX(IBase* e);
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
		void Add(int pos);
		void Add(float pos);
		float Value();

		float raw = 0;
		bool isPixel = false;
	private:
		IBase* _this = nullptr;
		IBase* parent = nullptr;
		bool center = false;
		bool sticky;

	};
	class IConstraintY {
	public:
		IConstraintY(IBase* e);
		IBase* Center(int pos);
		IBase* Center(float pos);
		IBase* Center(IBase* base, int pos);
		IBase* Center(IBase* base, float pos);
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
		float Value();

		float raw = 0;
		bool isPixel = false;
	private:
		IBase* _this = nullptr;
		IBase* parent = nullptr;
		bool center = false;
		bool sticky;

	};
	class IConstraintW {
	public:
		IConstraintW(IBase* e);
		IBase* Center(int pos);
		IBase* Center(float pos);
		IBase* Center(IBase* base, int pos);
		IBase* Center(IBase* base, float pos);
		void Add(int pos);
		void Add(float pos);
		float Value();

		float raw = 0;
		bool isPixel = false;
	private:
		IBase* _this = nullptr;
		IBase* parent = nullptr;

	};
	class IConstraintH {
	public:
		IConstraintH(IBase* e);
		IBase* Center(int pos);
		IBase* Center(float pos);
		IBase* Center(IBase* base, int pos);
		IBase* Center(IBase* base, float pos);
		void Add(int pos);
		void Add(float pos);
		float Value();

		float raw = 0;
		bool isPixel = false;
	private:
		IBase* _this = nullptr;
		IBase* parent = nullptr;

	};
}