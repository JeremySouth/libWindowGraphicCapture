#pragma once

// Unreal native plugin API
// Compatible with C99

#if defined(__CYGWIN32__)
#define UNREAL_INTERFACE_API __stdcall
#define UNREAL_INTERFACE_EXPORT __declspec(dllexport)
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
#define UNREAL_INTERFACE_API __stdcall
#define UNREAL_INTERFACE_EXPORT __declspec(dllexport)
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__) || defined(__QNX__)
#define UNREAL_INTERFACE_API
#define UNREAL_INTERFACE_EXPORT
#else
#define UNREAL_INTERFACE_API
#define UNREAL_INTERFACE_EXPORT
#endif

// Unreal Interface GUID
// Ensures cross plugin uniqueness.
//
// Template specialization is used to produce a means of looking up a GUID from it's payload type at compile time.
// The net result should compile down to passing around the GUID.
//
// UNITY_REGISTER_INTERFACE_GUID should be placed in the header file of any payload definition outside of all namespaces.
// The payload structure and the registration GUID are all that is required to expose the interface to other systems.
struct UnrealInterfaceGUID
{
#ifdef __cplusplus
	UnrealInterfaceGUID(unsigned long long high, unsigned long long low)
	: m_GUIDHigh(high)
	, m_GUIDLow(low)
	{
	}

	UnrealInterfaceGUID(const UnityInterfaceGUID& other)
	{
		m_GUIDHigh = other.m_GUIDHigh;
		m_GUIDLow  = other.m_GUIDLow;
	}

	UnrealInterfaceGUID& operator=(const UnrealInterfaceGUID& other)
	{
		m_GUIDHigh = other.m_GUIDHigh;
		m_GUIDLow  = other.m_GUIDLow;
		return *this;
	}

	bool Equals(const UnrealInterfaceGUID& other)   const { return m_GUIDHigh == other.m_GUIDHigh && m_GUIDLow == other.m_GUIDLow; }
	bool LessThan(const UnrealInterfaceGUID& other) const { return m_GUIDHigh < other.m_GUIDHigh || (m_GUIDHigh == other.m_GUIDHigh && m_GUIDLow < other.m_GUIDLow); }
#endif
	unsigned long long m_GUIDHigh;
	unsigned long long m_GUIDLow;
};
#ifdef __cplusplus
inline bool operator==(const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return left.Equals(right); }
inline bool operator!=(const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return !left.Equals(right); }
inline bool operator< (const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return left.LessThan(right); }
inline bool operator> (const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return right.LessThan(left); }
inline bool operator>=(const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return !operator< (left,right); }
inline bool operator<=(const UnrealInterfaceGUID& left, const UnrealInterfaceGUID& right) { return !operator> (left,right); }
#else
typedef struct UnrealInterfaceGUID UnrealInterfaceGUID;
#endif



#define UNREAL_GET_INTERFACE_GUID(TYPE) TYPE##_GUID
#define UNREAL_GET_INTERFACE(INTERFACES, TYPE) (TYPE*)INTERFACES->GetInterface(UNREAL_GET_INTERFACE_GUID(TYPE));

#ifdef __cplusplus
	#define UNREAL_DECLARE_INTERFACE(NAME) \
	struct NAME : IUnrealInterface

	template<typename TYPE>                                        \
	inline const UnrealInterfaceGUID GetUnrealInterfaceGUID();       \

	#define UNREAL_REGISTER_INTERFACE_GUID(HASHH, HASHL, TYPE)      \
	const UnrealInterfaceGUID TYPE##_GUID(HASHH, HASHL);            \
	template<>                                                     \
	inline const UnrealInterfaceGUID GetUnrealInterfaceGUID<TYPE>()  \
	{                                                              \
	    return UNREAL_GET_INTERFACE_GUID(TYPE);                     \
	}
#else
	#define UNREAL_DECLARE_INTERFACE(NAME) \
	typedef struct NAME NAME;             \
	struct NAME

	#define UNREAL_REGISTER_INTERFACE_GUID(HASHH, HASHL, TYPE) \
	const UnrealInterfaceGUID TYPE##_GUID = {HASHH, HASHL};
#endif



#ifdef __cplusplus
struct IUnrealInterface
{
};
#else
typedef void IUnrealInterface;
#endif

typedef struct IUnrealInterfaces
{
	// Returns an interface matching the guid.
	// Returns nullptr if the given interface is unavailable in the active Unity runtime.
	IUnrealInterface* (UNREAL_INTERFACE_API* GetInterface)(UnrealInterfaceGUID guid);

	// Registers a new interface.
	void (UNREAL_INTERFACE_API* RegisterInterface)(UnrealInterfaceGUID guid, IUnrealInterface* ptr);

#ifdef __cplusplus
	// Helper for GetInterface.
	template <typename INTERFACE>
	INTERFACE* Get()
	{
		return static_cast<INTERFACE*>(GetInterface(GetUnrealInterfaceGUID<INTERFACE>()));
	}

	// Helper for RegisterInterface.
	template <typename INTERFACE>
	void Register(IUnrealInterface* ptr)
	{
		RegisterInterface(GetUnrealInterfaceGUID<INTERFACE>(), ptr);
	}
#endif
} IUnrealInterfaces;



#ifdef __cplusplus
extern "C" {
#endif

// If exported by a plugin, this function will be called when the plugin is loaded.
void UNREAL_INTERFACE_EXPORT UNITY_INTERFACE_API UnrealPluginLoad(IUnrealInterfaces* unrealInterfaces);
// If exported by a plugin, this function will be called when the plugin is about to be unloaded.
void UNREAL_INTERFACE_EXPORT UNITY_INTERFACE_API UnrealPluginUnload();

#ifdef __cplusplus
}
#endif
