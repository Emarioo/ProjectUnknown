#include "ProUnk/UI/PanelFuncs.h"

#include "ProUnk/Game.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {
    
    void PlayerBarUpdate(engone::LoopInfo* info, Panel* panel){
        using namespace engone;
        
        UIModule& ui = info->window->uiModule;
        GameInstance* game = (GameInstance*)panel->extraData;
        Assert(game);
        
        // Todo: assuming dimension 0 which will not always be the case.
        EngineObject* plr = game->session->getDimension("0")->getWorld()->getObject(game->playerController.playerId);
		CombatData* combatData = GetCombatData(plr);
		
		float health = 0;
		float maxHealth = 0;
		if (combatData) {
			health = combatData->getHealth();
			maxHealth = combatData->getMaxHealth();
		}
		// Todo: displaying atk of weapon here is temporary and should be removed.
		//		It is useful for debugging so display it in some debug menu.

        
        // Todo: Use UIModule instead of UIRenderer
        {
            auto area = ui.makeBox();
            auto healthBack = ui.makeBox();
            auto healthFront = ui.makeBox();
            auto healthText = ui.makeText();
            
            char buffer[30];
            
            UIColor areaColor = { 0.4,0.5,0.6,0.5 };
            UIColor healthBackColor = {.5,0.05,0.08,1};
		    UIColor healthFrontColor = {.95,0.05,0.08,1};
            UIColor textColor = {1,1,1,1};
            
            float barHeight = 27;
		    float barGap = 8;
            
            snprintf(buffer, 20, "%.2f/%.2f", health, maxHealth);
            healthText->text = buffer;
            
            panel->retrieveXYWH(&area->x); // Note: area's xywh will be set to the panel's xywh
            
            healthBack->x = area->x;
            healthBack->y = area->y;
            healthBack->w = area->w;
            healthBack->h = barHeight;
            
            healthFront->x = area->x;
            healthFront->y = area->y;
            healthFront->w = health/maxHealth*healthBack->w;
            healthFront->h = barHeight;
            
            healthText->h = healthBack->h;
            float textWidth = ui.getWidthOfText(healthText);
            healthText->x = healthBack->x + healthBack->w/2 - textWidth/2;
            healthText->y = healthBack->y;
            
            area->color = areaColor;
            healthBack->color = healthBackColor;
            healthFront->color = healthFrontColor;
            healthText->color = textColor;
            
            float widths[]{textWidth};
            float minWidth = 0;
            for (float w : widths) if(w > minWidth) minWidth = w;

            minWidth += 20; // extra padding
            panel->setMinWidth(minWidth);
		    panel->setMinHeight(healthBack->h + barGap);
            
            // DrawToolTip(area.x + area.w, area.y, 20, 20, "Hello, random tooltip here.");
        } 
    }
}