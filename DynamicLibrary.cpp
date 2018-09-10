#include "DynamicLibrary.hpp"

namespace nu
{

DynamicLibrary::DynamicLibrary()
	: mLibraryHandle(nullptr)
	, mLibraryName("")
{
}

DynamicLibrary::DynamicLibrary(const std::string& libraryName)
	: mLibraryHandle(nullptr)
	, mLibraryName("")
{
	loadLibrary(libraryName);
}

DynamicLibrary::~DynamicLibrary()
{
	if (isLibraryLoaded())
	{
		unloadLibrary();
	}
}

bool DynamicLibrary::loadLibrary(const std::string& libraryName)
{
	// TODO : Use Numea Prerequisites instead
	#if defined(WIN32)
		// TODO : String to WideString from Numea
		// TODO : Improve this ?
		// https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
		// -> ws(s.begin(), s.end()) is elegant but wrong
		int len;
		int slength = (int)libraryName.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, libraryName.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, libraryName.c_str(), slength, buf, len);
		std::wstring wideLibraryName(buf);
		delete[] buf;

		mLibraryHandle = LoadLibrary(wideLibraryName.c_str());

	#elif defined(__linux)
		mLibraryHandle = dlopen(libraryName.c_str(), RTLD_NOW);
	#endif

	if (mLibraryHandle != nullptr)
	{
		mLibraryName = libraryName;
		return true;
	}
	else
	{
		mLibraryName.clear();
		return false;
	}
}

bool DynamicLibrary::isLibraryLoaded() const
{
	return mLibraryHandle != nullptr;
}

void DynamicLibrary::unloadLibrary()
{
	if (mLibraryHandle != nullptr)
	{
		// TODO : Use Numea Prerequisites instead
		#if defined(WIN32) 
			FreeLibrary(mLibraryHandle);
		#elif defined(__linux )
			dlclose(mLibraryHandle);
		#endif

		mLibraryHandle = nullptr;
		mLibraryName.clear();
	}
}

void* DynamicLibrary::loadFunction(const std::string& functionName) const
{
	if (!isLibraryLoaded())
	{
		return nullptr;
	}

	// TODO : Use Numea Prerequisites instead
	#if defined(WIN32)
		return GetProcAddress(mLibraryHandle, functionName.c_str());
	#elif defined __linux
		return dlsym(mLibraryHandle, functionName.c_str());
	#endif
}

const std::string& DynamicLibrary::getLibraryName() const
{
	return mLibraryName;
}

} // namespace nu