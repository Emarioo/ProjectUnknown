#include "CraftingList.h"


#include "Items/ItemHandler.h"
#include "GLFW/glfw3.h"
#include "InterfaceManager.h"

#include "KeyBinding.h"

CraftingList::CraftingList(const std::string& name) : engine::IBase(name) {

}
bool CraftingList::IsActive() {
	return active;
}
bool CraftingList::ClickEvent(int mx, int my, int button, int action) {
	if (IsActive()) {
		CalcConstraints();

		float mouseX = engine::ToFloatScreenX(mx);
		float mouseY = engine::ToFloatScreenY(my);

		if (action != 1)
			return false;

		// Check if mouse is inside inventory
		if (mouseX > x && mouseX<x + w &&
			mouseY>y && mouseY < y + h) {

			float iw = w * 64 / 512.f;

			float catX = w * 4 / 512.f;
			float catY = -h * 4 / 850.f;

			float sideX = w * 72 / 512.f;
			float bottomY = -h * 671 / 850.f;

			//bug::outs < mouseY < (y+h/2+bottomY) < bug::end;

			if (mouseX < x + sideX) {
				float catGap = h * 68 / 850.f;
				float row = (y + h + catY - mouseY) / catGap;
				int size = GetCategoriesSize();
				if (0 <= row && row <= size) {
					selectedCategory = row;
					//bug::out < "Category " < row < bug::end;
				}
			}
			else if (mouseY > y + h + bottomY) {
				CraftingCategory* cat = GetCategory(selectedCategory);
				//bug::out < "cat" < bug::end;
				if (cat != nullptr) {
					float recipeGap = h * 66 / 850.f;
					float recipeY = -h * 7 / 850.f;
					float row = (y + h + recipeY + scrolling - mouseY) / recipeGap;
					//bug::out < row < bug::end;
					if (0 <= row && row <= cat->recipes.size()) {
						selectedRecipe = row;
						//bug::out < "Recipe " <row < bug::end;
					}
				}
			}
			else {
				float makeX = w * 126 / 512.f;
				float makeY = h * 766 / 850.f;
				float makeW = w * 132 / 512.f;
				float makeH = h * 55 / 850.f;

				//bug::outs < "mx " < mouseX < (x + makeX) < (x + makeX + makeW) < bug::end;
				//bug::outs < "my " < mouseY < (y + h - makeY - makeH) < (y + h - makeY) < bug::end;

				if (x + makeX < mouseX && mouseX < x + makeX + makeW &&
					y + h - makeY - makeH < mouseY && mouseY < y + h - makeY) {
					CraftingCategory* cat = GetCategory(selectedCategory);
					if (cat != nullptr) {
						CraftingRecipe* rec = cat->GetRecipe(selectedRecipe);
						if (rec != nullptr) {
							Inventory* inv = UI::GetInventory();
							if (inv != nullptr) {
								Container* con = inv->container;
								if (con != nullptr && craftCount > 0) {
									rec->AttemptCraft(con, craftCount);
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
bool CraftingList::KeyEvent(int key, int action) {
	if (TestKeyAction(KeyCrafting,key)) {
		if (action == 1) {
			active = !active;
			engine::SetCursorMode(active);
		}
	}
	return false;
}
bool CraftingList::ScrollEvent(double scroll) {
	if (IsActive()) {
		float mouseX = engine::GetFloatMouseX();
		float mouseY = engine::GetFloatMouseY();

		float recipeX = w * 72 / 512.f;
		float recipeW = w * 440 / 512.f;
		float recipeH = h * 671 / 850.f;

		float countX = w * 326 / 512.f;
		float countY = h * 766 / 850.f;
		float countW = w * 132 / 512.f;
		float countH = h * 55 / 850.f;

		if (x + recipeX < mouseX && mouseX < x + recipeX + recipeW &&
			y + h - recipeH < mouseY && mouseY < y + h) {
			scrolling += scroll * scrollingSensitivity;
		}

		if (x + countX < mouseX && mouseX < x + countX + countW &&
			y + h - countY - countH < mouseY && mouseY < y + h - countY) {
			craftCount += scroll;
			if (craftCount < 1)
				craftCount = 1;
			else if (scroll > 0) {
				CraftingCategory* cat = GetCategory(selectedCategory);
				if (cat != nullptr) {
					CraftingRecipe* rec = cat->GetRecipe(selectedRecipe);
					if (rec != nullptr) {
						Inventory* inv = UI::GetInventory();
						if (inv != nullptr) {
							Container* con = inv->container;
							if (con != nullptr) {
								int attempts = rec->GetPossibleCraftingAttempts(con);
								if (craftCount > attempts) {
									craftCount = attempts;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}
void CraftingList::Update(float delta) {
	if (IsActive()) {
		CraftingCategory* cat = GetCategory(selectedCategory);
		if (cat != nullptr) {
			CraftingRecipe* rec = cat->GetRecipe(selectedRecipe);
			if (rec != nullptr) {
				Inventory* inv = UI::GetInventory();
				if (inv != nullptr) {
					Container* con = inv->container;
					if (con != nullptr) {
						int attempts = rec->GetPossibleCraftingAttempts(con);
						if (craftCount > attempts) {
							craftCount = attempts;
						}
						else if(attempts>0&&craftCount==0) {
							craftCount = 1;
						}
					}
				}
			}
		}
	}
}
void CraftingList::Render() {
	if (IsActive()) {

		CalcConstraints();

		// Crafting background
		engine::BindTexture(0, "containers/craftinglist");
		engine::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);

		float iw = w * (64 / 512.f);
		float ih = h * (64 / 850.f);

		float bottomH = h * (179 / 850.f);

		float listGap = h * (66 / 850.f);

		float recipeW = w * (440/512.f);

		float listX = w * 79 / 512.f;
		float listY = -h * 7 / 850.f;

		float craftX = w * 90 / 512.f;
		float craftY = -h * 685 / 850.f;
		float craftGap = w * 68 / 512.f;

		CraftingCategory* category = GetCategory(selectedCategory);
		if (category != nullptr) {

			float catX = w * 4 / 512.f;
			float catY = h * 4 / 850.f;
			float catGap = h * 68 / 850.f;

			// Category Marking
			engine::BindTexture(0, "blank");
			engine::DrawRect(x + catX, y + h - catY - catGap * selectedCategory + scrolling -ih, iw, ih, 0.5, 0.5, 1, 0.5);

			// Crafting list
			engine::GuiArea(x,y+bottomH,w,h-bottomH);

			for (int i = 0; i < category->recipes.size(); i++) {
				CraftingRecipe* recipe = &category->recipes[i];
				ItemType type = GetItemType(recipe->output.name);
				if (selectedRecipe == i) {
					engine::BindTexture(0, "blank");
					engine::DrawRect(x+listX,y+h+listY-listGap*i+scrolling-ih,recipeW,ih,0.5,0.5,1,0.5);
				}
				engine::GuiTransform(0, 0);
				DrawItem(type,x+listX,y+h+listY-listGap*i+scrolling,iw,ih,1,1,1,1,"");
				engine::GuiTransform(x+listX+iw,y+h+listY-listGap*i+scrolling-ih*0.85);
				
				engine::DrawString(engine::GetFont(),type.name,false,ih*0.7f,-1);
			}
			engine::GuiArea(-1, -1, 2, 2);
			// Ingredients
			if (-1<selectedRecipe&&selectedRecipe<category->recipes.size()) {
				CraftingRecipe* recipe = &category->recipes[selectedRecipe];
				Container* inventory = UI::GetInventory()->container;
				for (int i = 0; i< recipe->inputs.size(); i++) {
					engine::GuiTransform(0, 0);
					ItemType type = GetItemType(recipe->inputs[i].name);

					int count = inventory->GetItemCount(type.name);

					if (count >= recipe->inputs[i].count) {
						DrawItem(type, x + craftX + craftGap * i, y +h+ craftY, iw, ih, 0.2, 1, 0.2, 1, std::to_string(count) + "/" + std::to_string(recipe->inputs[i].count));
					} else {
						DrawItem(type, x + craftX + craftGap * i, y +h+ craftY, iw, ih, 1, 0.2, 0.2, 1, std::to_string(count) + "/" + std::to_string(recipe->inputs[i].count));
					}
				}
				float countX = w * 326 / 512.f;
				float countY = h * 766 / 850.f;
				float countW = w * 132 / 512.f;
				float countH = h * 55 / 850.f;

				engine::GuiColor(1, 1, 1, 1);
				engine::GuiTransform(x+countX+countW/2,y+h-countY-countH/2);
				engine::GuiSize(1, 1);
				engine::DrawString(engine::GetFont(),std::to_string(craftCount),true,countH*0.8,-1);
			}
		}
	}
}