#ifndef NU_DYNAMIC_LIBRARY_HPP
#define NU_DYNAMIC_LIBRARY_HPP

// TODO : Use Numea Prerequisites instead
#if defined(WIN32)
	#include <Windows.h>
#endif

#include <string>

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
		#if defined(WIN32)
		HMODULE mLibraryHandle;
		#elif defined(__linux)
		void* mLibraryHandle;
		#endif

		std::string mLibraryName;
};

} // namespace nu

#endif // NU_DYNAMIC_LIBRARY_HPP

