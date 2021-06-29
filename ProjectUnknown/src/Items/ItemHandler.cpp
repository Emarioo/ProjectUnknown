#include "ItemHandler.h"

engine::BufferContainer itemContainer;
void InitItemHandler() {
	engine::AddTextureAsset("items/resources0");
	
	unsigned int ind[]{ 0,1,2,2,3,0 };
	
	itemContainer.Setup(true,nullptr,16,ind,6);
	itemContainer.SetAttrib(0, 4, 4, 0);
}
void FillItemWithData(Item* item) {
	item->type = (ItemType) (1+item->name/64);

	// What about meta data?
}
int imageSize=512;
int itemWidth=8;
float size=1.f/itemWidth;
void DrawItem(Item* item, float x, float y,float w, float h) {
	if (item == nullptr)
		return;

	short index = item->name;
	if (item->type == ResourceType) {
		engine::BindTexture(0, "items/resources0");
	}
	float u = (index%itemWidth)*size;
	float v = ((int)((itemWidth-1)-index/itemWidth))*size;
	
	y -= h; // offsetting
	/*
	float vertices[16]{
		x,y,u,v,
		x + w,y,u+size,v,
		x + w,y + h,u+size,v+size,
		x,y + h,u,v+size
	};

	engine::GuiColor(1, 1, 1, 1);
	engine::GuiSize(1, 1);
	engine::GuiTransform(0, 0);
	
	itemContainer.SubVB(0,16,vertices);
	itemContainer.Draw();
	*/
	engine::DrawUVRect(x,y,w,h,u,v,size,size);

	if (item->count != 1) {
		//engine::GuiColor(1, 1, 1, 1);
		engine::GuiTransform(x+w/6,y+h/4);
		engine::DrawString(engine::GetFont(), std::to_string(item->count), true, h*0.6, -1);
	}
}