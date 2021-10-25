#pragma once

namespace engone {
	enum PolationType : unsigned char {
		Constant, Linear, Bezier
	};
	class Keyframe {
	public:
		PolationType polation;
		unsigned short frame;
		float value;
		/*
		"polation" is the type of polation between keyframes.
		*/
		Keyframe(PolationType polation, unsigned short frame, float value);
	};
	class FCurve {
	public:
		FCurve();
		Keyframe& Get(int index);
		void Add(Keyframe keyframe);

		std::vector<Keyframe> frames;
	};
	enum ChannelType : unsigned char {
		PosX=0, PosY, PosZ, 
		RotX, RotY, RotZ, // Not used. Deprecated
		ScaX, ScaY, ScaZ, 
		QuaX, QuaY, QuaZ, QuaW
	};
	class Channels {
	public:
		Channels() {}
		FCurve& Get(ChannelType channel);
		/*
		"channel" is what type of value the keyframes will be changing. Example PosX.
		Create a "FCurve" object and see the "Add" function for more details.
		*/
		void Add(ChannelType channel, FCurve fcurve);
		/*
		Add values to the references given in the argument
		*/
		void GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater);
		std::unordered_map<ChannelType, FCurve> fcurves;
	};
	class Animation {
	public:
		Animation() {}

		std::string name;

		Channels& Get(unsigned short i);
		/*
		objectIndex is the index of the bone. Also known as the vertex group in blender.
		Create a "Channels" object and see the "Add" function for more details.
		*/
		void AddObjectChannels(int objectIndex, Channels channels);
		/*
		Use this to create an animation by code.
		See the "AddObjectChannels" function for more details.
		*/
		void Modify(unsigned short startFrame, unsigned short endFrame);
		/*
		Use this to create an animation by code.
		"speed" is 24 by default. Speed can also be changed in the animator.
		See the "AddObjectChannels" function for more details.
		*/
		void Modify(unsigned short startFrame, unsigned short endFrame, float speed);
		/*
		Change to vector for better performance. This is done by ordering the objects.
		Shouldn't be to hard.
		*/
		std::unordered_map<unsigned short, Channels> objects;

		unsigned short frameStart = 0;
		unsigned short frameEnd = 0;
		float defaultSpeed = 24;// frames per second. 24 is default from blender.

		bool hasError = false;

	};
}