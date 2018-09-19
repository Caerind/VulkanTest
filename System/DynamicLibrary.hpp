#ifndef NU_DYNAMIC_LIBRARY_HPP
#define NU_DYNAMIC_LIBRARY_HPP

#include "Prerequisites.hpp"

#if defined(NU_PLATFORM_WINDOWS)
	#include <Windows.h> // TODO : Don't include if already included
#endif

namespace nu
{

class DynamicLibrary
{
	public:
		DynamicLibrary();
		DynamicLibrary(const std::string& libraryName);
		~DynamicLibrary();

		bool loadLibrary(const std::string& libraryName);
		bool isLibraryLoaded() const;
		void unloadLibrary();

		void* loadFunction(const std::string& functionName) const;

		const std::string& getLibraryName() const;

	private:
		// TODO : Use Numea Prerequisites instead
		#if defined(NU_PLATFORM_WINDOWS)
			HMODULE mLibraryHandle;
		#elif defined(NU_PLATFORM_LINUX)
			void* mLibraryHandle;
		#endif

		std::string mLibraryName;
};

} // namespace nu

#endif // NU_DYNAMIC_LIBRARY_HPP

