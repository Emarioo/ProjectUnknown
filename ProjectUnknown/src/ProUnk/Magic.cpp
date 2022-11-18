#include "ProUnk/Magic/Magic.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/Magic/Focals.h"

//#include "Engone/Tests/BasicRendering.h"

namespace prounk {
	void ParticleMagicTest(engone::LoopInfo& info, GameApp* app) {
		using namespace engone;

		Shader* shad = app->particleGroup->getShader();
		shad->bind();
		
		EngineObject* player = app->playerController.getPlayerObject();

		FocalPoint focalPoint0(ForceTypeAttractive, player->getPosition(),ToGlmVec3(player->rigidBody->getLinearVelocity()), 5, 0, 999);
		focalPoint0.bind(shad,0);
		//FocalPoint focalPoint1(ForceTypeAttractive, player->getPosition(),-2,0,3);
		//focalPoint1.bind(shad,1);

		if (!IsKeyDown(GLFW_KEY_F)) {
			//FocalPoint focalPoint2(ForceTypeField, player->getPosition(),ToGlmVec3(player->rigidBody->getLinearVelocity()), 1, 2, 4);
			//focalPoint2.bind(shad, 0);
			//FocalPoint focalPoint4(ForceTypeField, player->getPosition(), ToGlmVec3(player->rigidBody->getLinearVelocity()), 1, 4, 2);
			//focalPoint4.bind(shad, 3);
			FocalPlane focalPlane0(ForceTypeField, player->getPosition(), {0,1,0}, ToGlmVec3(player->rigidBody->getLinearVelocity()), 1, 1);
			focalPlane0.bind(shad, 0);
			//FocalPlane focalPlane1(ForceTypeField, player->getPosition() + glm::vec3(0, 5, 0), { 0,-1,0 }, ToGlmVec3(player->rigidBody->getLinearVelocity()), 1, 0, 1);
			//focalPlane1.bind(shad, 1);
		}
		else {
			FocalPoint focalP;
			//focalP.bind(shad, 0);
			//focalP.bind(shad, 1);
			FocalPlane focalE;
			focalE.bind(shad, 0);
			focalE.bind(shad, 1);
		}
		//FocalPoint focalPoint3(ForceTypeFriction, app->player->getPosition(),0.7f,0.f,999.f);

		//FocalPlane focalPlane1(ForceTypeAttractive, app->player->getPosition(), {1,1,0},1, 0, 999);
		//FocalPlane focalPlane2(ForceTypeField, app->player->getPosition(), {1,0,0},1, 0, 1);
		//FocalPoint focalPlane1(ForceTypeFriction, app->player->getPosition(),1,0,999);

		//focalPlane1.bind(shad, 0);

		//focalPoint3.bind(shad,2);
		shad->setInt("uClipping", IsKeyDown(GLFW_KEY_J));

		//part->setFloat("delta", 0);
		//part->setVec3("playerPos", {0,0,0});
		//buffer.drawPoints(1000,0);

		//group.render(info);
		//test::TestParticles(info);
	}
}