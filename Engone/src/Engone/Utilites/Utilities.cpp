
#include "Engone/Utilities/Utilities.h"

#include "Engone/vendor/stb_image/stb_image.h"

namespace engone {
	TrackerId ItemVector::trackerId = "ItemVector";

	std::vector<std::string> SplitString(std::string text, std::string delim) {
		std::vector<std::string> out;
		unsigned int at = 0;
		while ((at = text.find(delim)) != std::string::npos) {
			std::string push = text.substr(0, at);
			//if (push.size() > 0) {
			out.push_back(push);
			//}
			text.erase(0, at + delim.length());
		}
		if (text.size() != 0)
			out.push_back(text);
		return out;
	}

	/*
	void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}*/
	float lerp(float a, float b, float c) {
		return (1 - c) * a + c * b;
	}
	float inverseLerp(float min, float max, float x) {
		return (x - min) / (max - min);
	}
	float distance(float x, float y, float x1, float y1) {
		return (float)sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
	}
	float AngleDifference(float a, float b) {
		float d = a - b;
		if (d > glm::pi<float>()) {
			d -= glm::pi<float>() * 2;
		}
		if (d < -glm::pi<float>()) {
			d += glm::pi<float>() * 2;
		}
		return d;
	}
	/*
	void insert(float* ar, int off, int len, float* data) { // carefull with going overboard
		//std::cout << "INSERT ";
		for (int i = 0; i < len; i++) {
			ar[off + i] = data[i];
			//std::cout << off + i << "_" << ar[off + i] << "   ";
		}
		//std::cout << std::endl;
	}*/
	// returned value goes from 0 to 1. x goes from xStart to xEnd. Function does not handle errors like 1/0
	float bezier(float x, float xStart, float xEnd) {
		float t = (x - xStart) / (xEnd - xStart);
		float va = /*(pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) +*/(3 * (1 - t) * (float)pow(t, 2) + (float)pow(t, 3));
		return va;
	}
	/*
	std::string Crypt(const std::string& word, const std::string& key, bool encrypt) {
		std::string out = "";
		for (int i = 0; i < word.length();i++) {
			int at = word[i];
			for (int j = 0; j < key.length();j++) {
				int val = (i - j) * key[j];
				if (encrypt) at += val;
				else at -= val;
			}
			out += (at % 256);
		}
		return out;
	}*/

	ItemVector::ItemVector(int size) {
		if (size != 0) {
			resize(size); // may fail but it's fine
		}
	}
	ItemVector::~ItemVector() {
		cleanup();
	}
	char ItemVector::readType() {
		if (m_writeIndex < m_readIndex + sizeof(char)) {
			return 0;
		}

		char type = *(m_data + m_readIndex);
		m_readIndex += sizeof(char);
		return type;
	}
	bool ItemVector::empty() const {
		return m_writeIndex == 0;
	}
	void ItemVector::clear() {
		m_writeIndex = 0;
		m_readIndex = 0;
	}
	void ItemVector::cleanup() {
		resize(0); // <- this will also set write and read to 0
	}
	bool ItemVector::resize(uint32_t size) {
		if (size == 0) {
			if (m_data) {
				alloc::free(m_data, m_maxSize);
				GetTracker().subMemory<ItemVector>(m_maxSize);
				m_data = nullptr;
				m_maxSize = 0;
				m_writeIndex = 0;
				m_readIndex = 0;
			}
		}
		else {
			if (!m_data) {
				char* newData = (char*)alloc::malloc(size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed allocation memory\n";
					return false;
				}
				GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
			}
			else {
				char* newData = (char*)alloc::realloc(m_data, m_maxSize, size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed reallocating memory\n";
					return false;
				}
				GetTracker().subMemory<ItemVector>(m_maxSize);
				GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
				if (m_writeIndex > m_maxSize)
					m_writeIndex = m_maxSize;
				if (m_readIndex > m_maxSize)
					m_readIndex = m_maxSize;
			}
		}
		return true;
	}
	/*
	void CountingTest(int times, int numElements, std::function<int()> func) {
		std::vector<int> occurrences(numElements);

		double bef = GetSystemTime();
		for (int i = 0; i < times; i++) {
			int index = func();
			if(index>-1&&index<numElements)
				occurrences[func()]++;
		}
		double aft = GetSystemTime();
		for (int i = 0; i < numElements; i++) {
			std::cout << i << " " << occurrences[i] << std::endl;
		}
		std::cout << "Time: " << (aft - bef) << std::endl;
	}*/

	//static std::unordered_map<int, int> timerCounting;
	//Timer::Timer() : time(GetAppTime()) { }
	//Timer::Timer(const std::string& str) : time(GetAppTime()), name(str) { }
	//Timer::Timer(const std::string& str, int id) : time(GetAppTime()), name(str), id(id) { }
	//Timer::~Timer() {
	//	if (time != 0) end();
	//}
	//void Timer::end() {
	//	if (id != 0) {
	//		if (timerCounting.count(id)>0) {
	//			timerCounting[id]++;
	//			if (timerCounting[id] < 60) {
	//				return;
	//			}else{
	//				timerCounting[id] = 0;
	//			}
	//		} else {
	//			timerCounting[id] = 0;
	//			return;
	//		}
	//	}

	//	log::out << name << " : " << (GetAppTime() - time) << "\n";
	//	time = 0;
	//}
	static wchar_t buffer[256]{};// Will this hardcoded size be an issue?
	std::wstring convert(const std::string& in) {
		ZeroMemory(buffer, 256);
		for (int i = 0; i < in.length(); i++) {
			buffer[i] = in[i];
		}
		return buffer;
	}
	bool StartProgram(const std::string& path) {
		if (!FindFile(path)) {
			return false;
		}

		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		int slashIndex = path.find_last_of("\\");

		std::string workingDir = path.substr(0, slashIndex);

#ifdef NDEBUG
		std::wstring exeFile = convert(path);
		std::wstring workDir = convert(workingDir);
#else
		const std::string& exeFile = path;
		std::string& workDir = workingDir;
#endif
		CreateProcess(exeFile.c_str(),   // the path
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			workDir.c_str(),   // starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}


	template<>
	ICO* Image::Convert(PNG* img) {
		return nullptr;
	}
	template<>
	RawImage* Image::Convert(BMP* img) {
		if (!img) return nullptr;
		if (!img->data) return new RawImage();

		RawImage* out = new RawImage();

		char* imgData = img->data + sizeof(BITMAPINFOHEADER);

		out->data = (char*)alloc::malloc(img->header()->biSizeImage);
		if (!img->data) {
			log::out << log::RED << "BMP::toRawImage - failed allocation\n";
			return {};
		}
		out->size = img->header()->biSizeImage;
		out->width = img->header()->biWidth;
		out->height = img->header()->biHeight / 2; // bmp has double size for and mask
		out->channels = img->header()->biBitCount / 8;
		for (int i = 0; i < out->size / 4; i++) {
			int p = out->size / 4-1 - i;
			//bgra -> rgba
			out->data[p * 4 + 2] = imgData[i * 4 + 0];
			out->data[p * 4 + 1] = imgData[i * 4 + 1];
			out->data[p * 4 + 0] = imgData[i * 4 + 2];
			out->data[p * 4 + 3] = imgData[i * 4 + 3];
		}

		out->flags = Image::OwnerSelf;
		return out;
	}
	template<>
	BMP* Image::Convert(RawImage* img) {
		if (!img) return nullptr;
		if (!img->data) return new BMP();
	
		uint32_t andMaskSize = img->width * img->height / 8;
		//uint32_t andMaskSize = 0;
		uint32_t total = sizeof(BITMAPINFOHEADER) + img->size + andMaskSize;
		char* data = (char*)alloc::malloc(total);
		if (!data) {
			log::out << log::RED << "BMP image failed allocating memory\n";
			return {};
		}
		BMP* out = new BMP();
		out->data = data;
		out->flags = OwnerSelf;
		out->size = total;
		memset(out->data, 0, sizeof(BITMAPINFOHEADER));
		if (andMaskSize != 0)
			//memset(out->data + total - andMaskSize, 255, andMaskSize);
			memset(out->data + total - andMaskSize, 0, andMaskSize);

		out->header()->biSize = sizeof(BITMAPINFOHEADER);
		out->header()->biSizeImage = size;
		out->header()->biClrUsed = 0;
		out->header()->biClrImportant = 0;
		out->header()->biCompression = 0;
		out->header()->biPlanes = 1;
		out->header()->biXPelsPerMeter = 0;
		out->header()->biYPelsPerMeter = 0;

		out->header()->biWidth = img->width;
		out->header()->biHeight = img->height*2; // double because of and mask
		out->header()->biBitCount = img->channels * 8;

		//memcpy_s(out->getData(), size, data, size);
		char* imgData = data;
		char* dstData = out->data + sizeof(BITMAPINFOHEADER);

		for (int i = 0; i < size / 4; i++) {
			int p = size / 4 - 1 - i;
			//rgba -> bgra
			dstData[p * 4 + 2] = imgData[i * 4 + 0];
			dstData[p * 4 + 1] = imgData[i * 4 + 1];
			dstData[p * 4 + 0] = imgData[i * 4 + 2];
			dstData[p * 4 + 3] = imgData[i * 4 + 3];
		}

		return out;
	}

	void Image::writeFile(const char* path) {
		if (!data) return;

		std::ofstream file(path, std::ios::binary);

		file.write(data, size);

		file.close();
	}
	Image::Flags Image::StripInternal(Image::Flags flags) {
		return flags & (~(OwnerSelf | OwnerStbi));
	}
	void Image::cleanup() {
		if (data) {
			if (flags & OwnerSelf) {
				alloc::free(data, size);
			}
			else if (flags & OwnerStbi) {
				stbi_image_free(data);
			}
		}
		data = nullptr;
		size = 0;
		flags = 0;
	}
	RawImage* RawImage::ReadFromPNG(const char* path, Flags flags) {
		int channels = 0;
		if (flags & RGBA)
			channels = 4;
		int realChannels;
		int width, height;
		stbi_set_flip_vertically_on_load(flags&FlipOnLoad);
		char* data = (char*)stbi_load(path, &width, &height, &realChannels, channels);
		if (!data) {
			return nullptr;
		}
		RawImage* img = new RawImage();
		img->data = data;
		img->width = width;
		img->height = height;
		img->channels = channels;
		img->size = img->width * img->height * img->channels;
		img->flags = StripInternal(flags)|OwnerStbi;
		return img;
	}
	RawImage* RawImage::LoadFromPNG(int id, Flags flags) {
		HRSRC hs = FindResource(NULL, MAKEINTRESOURCE(id), "PNG");
		HGLOBAL hg = LoadResource(NULL, hs);
		void* ptr = LockResource(hg);
		DWORD size = SizeofResource(NULL, hs);

		int channels = 0;
		if (flags & RGBA)
			channels = 4;
		int realChannels;
		int width, height;
		stbi_set_flip_vertically_on_load(flags & FlipOnLoad);
		char* data = (char*)stbi_load_from_memory((uint8_t*)ptr, size, &width, &height, &realChannels, channels);
		if (!data) {
			return nullptr;
		}
		RawImage* img = new RawImage();
		img->data = data;
		img->width = width;
		img->height = height;
		img->channels = channels;
		img->size = img->width * img->height * img->channels;
		img->flags = StripInternal(flags) | OwnerStbi;
		return img;
	}
	//template<>
	//BMP* RawImage::toBMP<BMP>() {
	//	//BMP img = BMP::CreateEmpty(size,width,height*2);
	//
	//	BMP img;
	//	uint32_t andMaskSize = width * height / 8;
	//	//uint32_t andMaskSize = 0;
	//	uint32_t total = sizeof(BITMAPINFOHEADER) + size + andMaskSize;
	//	img.data = (char*)alloc::malloc(total);
	//	if (!img.data) {
	//		log::out << log::RED << "BMP image failed allocating memory\n";
	//		return {};
	//	}
	//	img.flags = SelfOwned;
	//	img.size = total;
	//	memset(img.data, 0, sizeof(BITMAPINFOHEADER));
	//	if (andMaskSize != 0)
	//		//memset(img.data + total - andMaskSize, 255, andMaskSize);
	//		memset(img.data + total - andMaskSize, 0, andMaskSize);

	//	img.header()->biSize = sizeof(BITMAPINFOHEADER);
	//	img.header()->biSizeImage = size;
	//	img.header()->biClrUsed = 0;
	//	img.header()->biClrImportant = 0;
	//	img.header()->biCompression = 0;
	//	img.header()->biPlanes = 1;
	//	img.header()->biXPelsPerMeter = 0;
	//	img.header()->biYPelsPerMeter = 0;

	//	img.header()->biWidth = width;
	//	img.header()->biHeight = height*2; // double because of and mask
	//	img.header()->biBitCount = channels * 8;

	//	//memcpy_s(img.getData(), size, data, size);
	//	char* imgData = data;
	//	char* dstData = img.data + sizeof(BITMAPINFOHEADER);
	//	for (int i = 0; i < size / 4; i++) {
	//		int p = size / 4 - 1 - i;
	//		//rgba -> bgra
	//		dstData[p * 4 + 2] = imgData[i * 4 + 0];
	//		dstData[p * 4 + 1] = imgData[i * 4 + 1];
	//		dstData[p * 4 + 0] = imgData[i * 4 + 2];
	//		dstData[p * 4 + 3] = imgData[i * 4 + 3];
	//	}

	//	return img;
	//}
	PNG* PNG::Load(int id) {
		HRSRC hs = FindResource(NULL, MAKEINTRESOURCE(id), "PNG");
		HGLOBAL hg = LoadResource(NULL, hs);
		void* ptr = LockResource(hg);
		DWORD size = SizeofResource(NULL, hs);
		
		PNG* img = new PNG();
		img->data = (char*)ptr;
		img->size = size;
		img->flags = 0;

		return img;
	}
	PNG* PNG::ReadFile(const char* path) {
		PNG* img = new PNG();
		Image::ReadFile(path, img);
		return img;
	}
	//RawImage* PNG::toRaw(int channels) {
	//	RawImage img;
	//	int realChannels;
	//	img.data = (char*)stbi_load_from_memory((uint8_t*)data, size, &img.width, &img.height, &realChannels, channels);
	//	img.channels = channels;
	//	return img;
	//}
	BMP* BMP::ReadFile(const char* path) {
		BMP* img = new BMP();
		Image::ReadFile(path, img);
		return img;
	}
	//template<>
	//ICO BMP::toICO() {
	//	ICO img = ICO::CreateEmpty(1,size);

	//	ICO::ICONDIRENTRY* e = img.entry(0);
	//	e->bitsPerPixel = header()->biBitCount;
	//	e->colorPalette=header()->biClrUsed;
	//	e->colorPlanes=0;
	//	//e->colorPlanes=header()->biPlanes;
	//	e->width = header()->biWidth;
	//	e->height = header()->biHeight/2;
	//	e->imageSize = size;

	//	img.autoSetOffsets();

	//	memcpy_s(img.getData(0),e->imageSize,data,size);

	//	return img;
	//}
	//RawImage* BMP::toRawImage() {
	//	RawImage img;

	//	char* imgData = data + sizeof(BITMAPINFOHEADER);

	//	img.data = (char*)alloc::malloc(header()->biSizeImage);
	//	if (!img.data) {
	//		log::out << log::RED << "BMP::toRawImage - failed allocation\n";
	//		return {};
	//	}
	//	img.size = header()->biSizeImage;
	//	img.width = header()->biWidth;
	//	img.height = header()->biHeight / 2; // bmp has double size for and mask
	//	img.channels = header()->biBitCount / 8;
	//	for (int i = 0; i < img.size / 4; i++) {
	//		int p = img.size / 4-1 - i;
	//		//bgra -> rgba
	//		img.data[p * 4 + 2] = imgData[i * 4 + 0];
	//		img.data[p * 4 + 1] = imgData[i * 4 + 1];
	//		img.data[p * 4 + 0] = imgData[i * 4 + 2];
	//		img.data[p * 4 + 3] = imgData[i * 4 + 3];
	//	}

	//	img.flags = SelfOwned;
	//	return img;
	//}
	ICO* ICO::Load(int id) {
		HRSRC hs = FindResource(NULL, MAKEINTRESOURCE(id), "ICO");
		HGLOBAL hg = LoadResource(NULL, hs);
		void* ptr = LockResource(hg);
		DWORD size = SizeofResource(NULL, hs);

		ICO* img = new ICO();
		img->data = (char*)ptr;
		img->size = size;
		img->flags = 0;

		return img;
	}
	ICO* ICO::ReadFile(const char* path) {
		ICO* img = new ICO();
		Image::ReadFile(path, img);
		return img;
	}
	ICO* ICO::CreateEmpty(uint32_t imageCount, uint32_t size) {
		uint32_t total = sizeof(ICONDIR) + imageCount * sizeof(ICONDIRENTRY) + size;
		char* data = (char*)alloc::malloc(total);
		if (!data) {
			log::out << log::RED << "ICO::CreateEmpty - image failed allocating memory\n";
			return {};
		}
		ICO* img=new ICO();
		img->data = data;
		img->flags = OwnerSelf;
		img->size = total;
		memset(img->data, 0, sizeof(ICONDIR)+imageCount*sizeof(ICONDIRENTRY));
		img->header()->numImages = imageCount;
		img->header()->type = 1;
		return img;
	}
	void ICO::autoSetOffsets() {
		if (!data) return;
		uint32_t offset=sizeof(ICONDIR)+header()->numImages*sizeof(ICONDIRENTRY);
		for (int i = 0; i < header()->numImages;i++) {
			entry(i)->offset = offset;
			offset += entry(i)->imageSize;
		}
	}
}