#include "Cube.h"

Cube::Cube(float x,float y,float z,float w,float h,float d) {
	name = "Cube";
	weight = 10;
	//collision.SetHitbox(0, 0, 0, w, h, d);
	collision.radius = sqrt(pow(w, 2) + pow(h, 2) + pow(d, 2));
	collision.MakeCube(0, 0, 0, w, h, d);
	position = glm::vec3(x,y,z);
	this->w = w;
	this->h = h;
	this->d = d;
	GenMesh(1, 1, 1, 1);
}
Cube::Cube(float x, float y, float z, float w, float h, float d,float r,float g,float b,float a) {
	name = "Cube";
	weight = 10;
	//collision.SetHitbox(0, 0, 0, w, h, d);
	collision.radius = sqrt(pow(w,2)+pow(h,2)+pow(d,2));
	collision.MakeCube(0, 0, 0, w, h, d);
	

	//collision.MakeFace(glm::vec3(-w / 2, -h / 2, d / 2), glm::vec3(w / 2, -h / 2, d / 2), glm::vec3(w / 2, h / 2, d / 2), glm::vec3(-w / 2, h / 2, d / 2));
	//collision.MakeFace(glm::vec3(-w/2,-h/2,d/4),glm::vec3(w/2,-h/2,d/4),glm::vec3(w/2,h/2,-d/4),glm::vec3(-w/2,h/2,-d/4));
	//collision.MakeFace(glm::vec3(-w/2,-h/2,0),glm::vec3(w/2,-h/2,0),glm::vec3(w/2,h/2,0),glm::vec3(-w/2,h/2,0));
	SetPosition(x, y, z);
	this->w = w;
	this->h = h;
	this->d = d;
	GenMesh(r, g, b, a);
}
void Cube::GenMesh(float r,float g,float b,float a) {
	float v[]{// vec3, color, normal - Dark Bottom
		-w / 2,-h / 2,-d / 2,r*0.5,g*0.5,b*0.5,a,0,-1,0,
		w / 2,-h / 2,-d / 2,r*0.5,g*0.5,b*0.5,a,1,0,0,
		w / 2,-h / 2,d / 2,r*0.5,g*0.5,b*0.5,a,0,0,1,
		-w / 2,-h / 2,d / 2,r*0.5,g*0.5,b*0.5,a,-1,0,0,
		-w / 2,h / 2,-d / 2,r,g,b,a,0,1,0,
		w / 2,h / 2,-d / 2,r,g,b,a,0,0,-1,
		w / 2,h / 2,d / 2,r,g,b,a,0,1,0,
		-w / 2,h / 2,d / 2,r,g,b,a,0,1,0
	};/**/
	/*
	float v[]{// vec3, color, normal - Filled
		-w / 2,-h / 2,-d / 2,r,g,b,a,0,-1,0,
		w / 2,-h / 2,-d / 2,r,g,b,a,1,0,0,
		w / 2,-h / 2,d / 2,r,g,b,a,0,0,1,
		-w / 2,-h / 2,d / 2,r,g,b,a,-1,0,0,
		-w / 2,h / 2,-d / 2,r,g,b,a,0,1,0,
		w / 2,h / 2,-d / 2,r,g,b,a,0,0,-1,
		w / 2,h / 2,d / 2,r,g,b,a,0,1,0,
		-w / 2,h / 2,d / 2,r,g,b,a,0,1,0
	};/**/
	unsigned int i[]{
		1,2,0,
		2,3,0,
		7,3,2,
		6,7,2,
		6,2,1,
		5,6,1,
		1,0,5,
		0,4,5,
		4,0,3,
		7,4,3,
		7,6,4,
		6,5,4
	};/*
	float v[]{// vec3, color, normal - Filled
		0,0,0,r,g,b,a,0,-1,0,
		5,0,0,r,g,b,a,1,0,0,
		0,0,5,r,g,b,a,0,0,1
	};
	unsigned int i[]{
		0,1,2,
		2,1,0
	};*/
	container.Setup(false,v,10*8,i,6*6);
	container.SetAttrib(0,3,10,0);
	container.SetAttrib(1,4,10,3);
	container.SetAttrib(2,3,10,7);
}
void Cube::Draw() {
	Location body;
	body.Translate(position);
	body.Rotate(rotation);
	ObjectTransform(body.mat());
	BindLights("current", position);
	container.Draw();
}
void Cube::Update(float delta) {
	
}