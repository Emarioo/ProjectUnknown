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
/*
void Cube::MakeCube(float x, float y, float z, float w, float h, float d) {
	glm::vec3 O = glm::vec3(x, y, z);

	collision.originPoints.push_back(O+glm::vec3(-w, -h, -d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(w, -h, -d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(w, -h, d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(-w, -h, d) / 2.f);

	collision.originPoints.push_back(O+glm::vec3(-w, h, -d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(w, h, -d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(w, h, d) / 2.f);
	collision.originPoints.push_back(O+glm::vec3(-w, h, d) / 2.f);

	collision.quadIndex.push_back(0);
	collision.quadIndex.push_back(1);
	collision.quadIndex.push_back(2);
	collision.quadIndex.push_back(3);

	collision.quadIndex.push_back(0);
	collision.quadIndex.push_back(1);
	collision.quadIndex.push_back(5);
	collision.quadIndex.push_back(4);

	collision.quadIndex.push_back(1);
	collision.quadIndex.push_back(2);
	collision.quadIndex.push_back(6);
	collision.quadIndex.push_back(5);

	collision.quadIndex.push_back(2);
	collision.quadIndex.push_back(3);
	collision.quadIndex.push_back(7);
	collision.quadIndex.push_back(6);

	collision.quadIndex.push_back(3);
	collision.quadIndex.push_back(0);
	collision.quadIndex.push_back(4);
	collision.quadIndex.push_back(7);

	collision.quadIndex.push_back(4);
	collision.quadIndex.push_back(5);
	collision.quadIndex.push_back(6);
	collision.quadIndex.push_back(7);

	float fl[]{
		collision.originPoints.at(0).x,collision.originPoints.at(0).y,collision.originPoints.at(0).z,
		collision.originPoints.at(1).x,collision.originPoints.at(1).y,collision.originPoints.at(1).z,
		collision.originPoints.at(2).x,collision.originPoints.at(2).y,collision.originPoints.at(2).z,
		collision.originPoints.at(3).x,collision.originPoints.at(3).y,collision.originPoints.at(3).z,
		collision.originPoints.at(4).x,collision.originPoints.at(4).y,collision.originPoints.at(4).z,
		collision.originPoints.at(5).x,collision.originPoints.at(5).y,collision.originPoints.at(5).z,
		collision.originPoints.at(6).x,collision.originPoints.at(6).y,collision.originPoints.at(6).z,
		collision.originPoints.at(7).x,collision.originPoints.at(7).y,collision.originPoints.at(7).z
		//,0,0,0
	};
	unsigned int in[]{
		0,1,
		1,2,
		2,3,
		3,0,
		0,4,
		1,5,
		2,6,
		3,7,
		4,5,
		5,6,
		6,7,
		7,4

		/*,0,8,
		1,8,
		2,8,
		3,8,
		4,8,
		5,8,
		6,8,
		7,8/*
	};
	collision.outline.type = 1;
	collision.outline.Setup(true, fl, 3 * (8/*+1/*), in, 2 * (12/*+8/*));
	collision.outline.SetAttrib(0, 3, 3, 0);
	//points = originPoints;
	collision.UpdatePoints(glm::vec3(0, 0, 0));
}*/
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