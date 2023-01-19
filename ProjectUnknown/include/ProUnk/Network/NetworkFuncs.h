#pragma once

#include "Engone/EngineObject.h"
#include "Engone/Networking/NetworkModule.h"

#include "Engone/Logger.h"

namespace prounk {
	enum NetCommand : int {
		NET_ADD_TERRAIN,
		NET_ADD_ITEM,
		NET_ADD_WEAPON,
		NET_ADD_CREATURE,
		NET_MOVE,
		NET_DELETE,

		NET_ANIMATE,
		NET_TRIGGER,
		NET_DAMAGE,
		NET_SET_HEALTH,

		NET_EDIT,
		NET_EDIT_BODY,
	};
	const char* to_string(NetCommand value);
	engone::Logger& operator<<(engone::Logger& log, NetCommand value);

	class Dimension;
	class Session;
	class NetworkFuncs {
	public:
		NetworkFuncs() = default;


		void netAddGeneral(engone::EngineObject* object);
		void netAddItem(engone::EngineObject* object);
		void netAddWeapon(engone::EngineObject* object);
		void netAddCreature(engone::EngineObject* object);
		void netAddTerrain(engone::EngineObject* object);

		// change to move objects, how to provide multiple objects?
		void netMoveObject(engone::EngineObject* object);
		
		void netDeleteObject(engone::EngineObject* object);

		// separate edit commands into multilpe commands. not sub types of edit commands.
		void netEditObject(engone::EngineObject* object, int type, uint64_t data);
		//void netEditCombatData(engone::EngineObject* object, engone::UUID from, bool yes);

		void netDamageObject(engone::EngineObject* object, float damage);
		void netSetHealth(engone::EngineObject* object, float health);
		void netAnimateObject(engone::EngineObject* object, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame);
		void netSetTrigger(engone::EngineObject* object, bool yes);

		//-- Receive
		void recAddTerrain(engone::MessageBuffer* msg, engone::UUID clientUUID);
		void recAddItem(engone::MessageBuffer* msg, engone::UUID clientUUID);
		void recAddWeapon(engone::MessageBuffer* msg, engone::UUID clientUUID);
		void recAddCreature(engone::MessageBuffer* msg, engone::UUID clientUUID);

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
		void pullObject(engone::MessageBuffer* msg, UUID_DIM& data);

		Session* getSession();

	private:

		//Session* m_session = nullptr;

		friend class Session;
	};
}