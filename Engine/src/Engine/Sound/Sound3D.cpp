#include "SoundHandler.h"

#include <stdio.h>
#include <iostream>
#include <vector>

int main5(int argc, char** argv) {
	using namespace engine;
	InitSound();
	/*
	SoundBuffer buffer;
	buffer.Init("helloworld.wav");
	
	SoundSource source;
	source.Init();
	source.Loop(true);
	source.Gain(0.5);
	
	source.Bind(buffer);
	source.Play();*/
	
	SoundStream stream;
	stream.Init("longsound.wav");

	stream.source.Gain(1.5);
	stream.source.Play();

	ALint state = AL_PLAYING;

	double rot=0;
	int print = 0;
	while (state == AL_PLAYING) {
		stream.UpdateStream();
		state = stream.source.GetState();
		
		//stream.source.Position(sin(rot) * 5, 0, cos(rot) * 5);
		print++;
		if (10000 == print) {
			rot += 0.001;
			print = 0;
			if(rot>10)
				break;
		}
	}

	/* exit context */
	
	stream.Delete();
	UninitSound();

	std::cin.get();

	return 0;
}