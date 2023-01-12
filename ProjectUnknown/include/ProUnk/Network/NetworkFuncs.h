#pragma once

#include "Engone/EngineObject.h"
#include "Engone/Networking/NetworkModule.h"

#include "Engone/Logger.h"

namespace prounk {
	enum NetCommand : int {
		MoveObject,
		//AddObject,
		DELETE_OBJECT,
		AnimateObject,
		EditObject,
		DamageObject,
		NET_ADD_TERRAIN,
		NET_ADD_ITEM,
		NET_ADD_WEAPON,
		NET_ADD_CREATURE,
	};
	const char* to_string(NetCommand value);
	engone::Logger& operator<<(engone::Logger& log, NetCommand value);

	class Dimension;
	class Session;
	class NetworkFuncs {
	public:
		NetworkFuncs() = default;

		// change to move objects, how to provide multiple objects?
		void netMoveObject(engone::EngineObject* object);

		void netAddGeneral(engone::EngineObject* object);
		void netAddItem(engone::EngineObject* object);
		void netAddWeapon(engone::EngineObject* object);
		void netAddCreature(engone::EngineObject* object);
		void netAddTerrain(engone::EngineObject* object);

		void netDeleteObject(engone::EngineObject* object);

		void netAnimateObject(engone::EngineObject* object, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame);
		// separate edit commands into multilpe commands. not sub types of edit commands.
		void netEditObject(engone::EngineObject* object, int type, uint64_t data);
		void netEditCombatData(engone::EngineObject* object, engone::UUID from, bool yes);

		void netDamageObject(engone::EngineObject* object, float damage);

		//-- Receive
		void recAddTerrain(engone::MessageBuffer& msg, engone::UUID clientUUID);
		void recAddItem(engone::MessageBuffer& msg, engone::UUID clientUUID);
		void recAddWeapon(engone::MessageBuffer& msg, engone::UUID clientUUID);
		void recAddCreature(engone::MessageBuffer& msg, engone::UUID clientUUID);

		virtual engone::Client& getClient() = 0;
		virtual engone::Server& getServer() = 0;

		//-- Helpful functions
		bool networkRunning();
		void networkSend(engone::MessageBuffer& msg);
		// uuid and dimension
		void pushObject(engone::MessageBuffer& msg, engone::EngineObject* obj);
		struct UUID_DIM {
			engone::UUID uuid=0;
			Dimension* dim=nullptr;
		};
		void pullObject(engone::MessageBuffer& msg, UUID_DIM& data);

		Session* getSession();

	private:

		//Session* m_session = nullptr;

		friend class Session;
	};
}