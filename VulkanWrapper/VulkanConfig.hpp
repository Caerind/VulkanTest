#pragma once

// A good idea is to include your own build/platform/option/... here
// And use your own define to define those for the vulkan wrapper

///////////////////////////////////////////////
// VULKAN_NAMESPACE_NAME
//
// Namespace name, if not defined there will be no namespace
//
///////////////////////////////////////////////
//#define VULKAN_NAMESPACE_NAME myNamespaceName

///////////////////////////////////////////////
// VULKAN_LINKING
//
// If both or none, DYNAMIC will be used
//
///////////////////////////////////////////////
#define VULKAN_LINKING_DYNAMIC
//#define VULKAN_LINKING_STATIC

///////////////////////////////////////////////
// VULKAN_KHR
//
// Define if you are using a KHR
//
///////////////////////////////////////////////
#define VULKAN_KHR
// TODO : More options

///////////////////////////////////////////////
// VULKAN_MONO_DEVICE
//
// Define this if you will use only one Device
// This will optimize memory if so
//
///////////////////////////////////////////////
#define VULKAN_MONO_DEVICE

///////////////////////////////////////////////
// VULKAN_BUILD
//
// If not defined, VULKAN_BUILD_DEBUG will be defined
//
///////////////////////////////////////////////
#define VULKAN_BUILD_DEBUG
//#define VULKAN_BUILD_RELEASE
//#define VULKAN_BUILD_PROFILE
//#define VULKAN_BUILD_FINAL

///////////////////////////////////////////////
// VULKAN_OPTION
//
// Defined by the VULKAN_BUILD if not defined
// You can change the default behavior by define what you need (exclusion not supported yet)
//
// OptionName             | DEBUG | RELEASE | PROFILE | FINAL | Description
// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// OBJECTNAMES       |      X       |       X        |                 |             | Objects can have a name to easily identify them
// OBJECTTRACKER   |      X       |       X        |                 |             | Object amounts will be tracked to identify leaks and know amount used for each type
// ASSERT                    |      X       |       X        |                 |             | Assertion functions will be used
// LOG                           |      X       |       X        |                 |             | Logging functions will be used 
// PROFILE                   |               |                 |       X        |              | Profile functions will be used
//
// The _NO_ version can be used to force the disabling of an option
// The _NO_ priority is higher, meaning if you have both, the option won't be enabled
//
///////////////////////////////////////////////
#define VULKAN_OPTION_OBJECTNAMES
//#define VULKAN_OPTION_NO_OBJECTNAMES
#define VULKAN_OPTION_OBJECTTRACKER
//#define VULKAN_OPTION_NO_OBJECTTRACKER
#define VULKAN_OPTION_ASSERT
//#define VULKAN_OPTION_NO_ASSERT
#define VULKAN_OPTION_LOG
//#define VULKAN_OPTION_NO_LOG
#define VULKAN_OPTION_PROFILE
//#define VULKAN_OPTION_NO_PROFILE

//#define VULKAN_ASSERT(x) assert((x));
//#define VULKAN_LOG_INFO(x) info((x));
//#define VULKAN_LOG_WARNING(x) warning((x));
//#define VULKAN_LOG_ERROR(x) error((x));

///////////////////////////////////////////////
// ADVANCED
//
// Should not really be public, but still provided for maximum control
// Those defines detection is done by the VulkanWrapper if no define is provided
//
///////////////////////////////////////////
// TODO : Platform
// TODO : KHR ?
// TODO : Endianness ?