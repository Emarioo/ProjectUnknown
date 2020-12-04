#include "MetaComponent.h"

#include <iostream>

MetaStrip::MetaStrip(MetaIdentity i0) {
	types[0] = i0;
	types[1] = None;
	types[2] = None;
}
MetaStrip::MetaStrip(MetaIdentity i0, MetaIdentity i1) {
	types[0] = i0;
	types[1] = i1;
	types[2] = None;
}
MetaStrip::MetaStrip(MetaIdentity i0, MetaIdentity i1, MetaIdentity i2) {
	types[0] = i0;
	types[1] = i1;
	types[2] = i2;
}

MetaComponent::MetaComponent() {

}
int MetaComponent::AddMeta(MetaStrip m) {// Return 0 = Success, 1 = MetaStrip Exists
	MetaStrip* vel=nullptr;
	for (int i = 0; i < meta.size(); i++) {
		if (meta[i].types[0] == Velocity && meta[i].types[1] == m.types[1] && meta[i].types[2] == None) {
			vel = &meta[i];
		}
		if (meta[i].types[0] == m.types[0] && meta[i].types[1] == m.types[1] && meta[i].types[2] == m.types[2]) {
			return 1;
		}
	}

	if (m.types[0] == Polation) {
		if (vel==nullptr) {
			MetaStrip t(Velocity, m.types[1], None);
			t.floats[m.types[2]] = m.floats[2];
			AddMeta(t);
		} else {
			vel->floats[m.types[2]] = m.floats[2];
		}
	}
	meta.push_back(m);
	//std::cout << "seems fine "<<std::endl;
	return 0;
}
void MetaComponent::DelMeta(MetaIdentity i0, MetaIdentity i1,MetaIdentity i2) {
	for (int i = 0; i < meta.size(); i++) {
		if (meta[i].types[0] == i0 && meta[i].types[1]==i1&& meta[i].types[2]==i2) {
			meta.erase(meta.begin() + i);
			break;
		}
	}
}
MetaStrip* MetaComponent::GetMeta(MetaIdentity i0, MetaIdentity i1, MetaIdentity i2) {// Returns nullptr if MetaStrip doesn't exist
	for (int i = 0; i < meta.size(); i++) {
		if (meta[i].types[0] == i0 && meta[i].types[1] == i1 && meta[i].types[2] == i2) {
			return &meta[i];
		}
	}
	return nullptr;
}