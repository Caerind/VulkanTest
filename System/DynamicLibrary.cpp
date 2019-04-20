#include "DynamicLibrary.hpp"

#if defined(NU_PLATFORM_WINDOWS)
	#include <Windows.h>
	#include "String.hpp"
#endif

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
	#if defined(NU_PLATFORM_WINDOWS)
		mLibraryHandle = LoadLibrary(libraryName.c_str());
	#elif defined(NU_PLATFORM_LINUX)
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
		#if defined(NU_PLATFORM_WINDOWS) 
			FreeLibrary(mLibraryHandle);
		#elif defined(NU_PLATFORM_LINUX)
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

	#if defined(NU_PLATFORM_WINDOWS)
		return GetProcAddress(mLibraryHandle, functionName.c_str());
	#elif defined(NU_PLATFORM_LINUX)
		return dlsym(mLibraryHandle, functionName.c_str());
	#endif
}

const std::string& DynamicLibrary::getLibraryName() const
{
	return mLibraryName;
}

} // namespace nu