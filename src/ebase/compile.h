#ifndef _COMPILER_H_
#define _COMPILER_H_

#define COMPILER(FEATURE) (defined _COMPILER_##FEATURE  && _COMPILER_##FEATURE)

#if defined(__clang__)
#define _COMPILER_CLANG 1
#endif

#if defined(__GNUC__)
#define _COMPILER_GCC 1
#endif


#if defined(__MINGW32__)
#define _COMPILER_MINGW 1
#endif

#if COMPILER(MINGW) && defined(__MINGW64_VERSION_MAJOR) /* best way to check for mingw-w64 vs mingw.org */
#define _COMPILER_MINGW64 1
#endif

#if defined(_MSC_VER)
#define _COMPILER_MSVC 1
#endif

#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#define _COMPILER_SUNCC 1
#endif

#if defined(__ARM_EABI__) || defined(__EABI__)
#define _COMPILER_SUPPORTS_EABI 1
#endif

#if !defined(ALWAYS_INLINE) && COMPILER(GCC) && defined(NDEBUG) && !COMPILER(MINGW)
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif

#if !defined(ALWAYS_INLINE) && COMPILER(MSVC) && defined(NDEBUG)
#define ALWAYS_INLINE __forceinline
#endif

#if !defined(ALWAYS_INLINE)
#define ALWAYS_INLINE inline
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif


///////////////////////////////////////cpu/////////////////////////////

#define CPU(FEATURE) (defined _CPU_##FEATURE  && _CPU_##FEATURE)
/* CPU(ALPHA) - DEC Alpha */
#if defined(__alpha__)
#define _CPU_ALPHA 1
#endif

#if defined(__hppa__) || defined(__hppa64__)
#define _CPU_HPPA 1
#define _CPU_BIG_ENDIAN 1
#endif


/* CPU(IA64) - Itanium / IA-64 */
#if defined(__ia64__)
#define _CPU_IA64 1
/* 32-bit mode on Itanium */
#if !defined(__LP64__)
#define _CPU_IA64_32 1
#endif
#endif


/* CPU(MIPS) - MIPS 32-bit */
/* Note: Only O32 ABI is tested, so we enable it for O32 ABI for now.  */
#if (defined(mips) || defined(__mips__) || defined(MIPS) || defined(_MIPS_)) && defined(_ABIO32)
#define _CPU_MIPS 1
#if defined(__MIPSEB__)
#define _CPU_BIG_ENDIAN 1
#endif
#endif /* MIPS */


/* CPU(PPC) - PowerPC 32-bit */
#if (  defined(__ppc__)        \
	|| defined(__PPC__)        \
	|| defined(__powerpc__)    \
	|| defined(__powerpc)      \
	|| defined(__POWERPC__)    \
	|| defined(_M_PPC)         \
	|| defined(__PPC))         \
	&& defined(__BYTE_ORDER__) \
	&& (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define _CPU_PPC 1
#define _CPU_BIG_ENDIAN 1
#endif

/* CPU(PPC64) - PowerPC 64-bit Big Endian */
#if (  defined(__ppc64__)      \
	|| defined(__PPC64__))     \
	&& defined(__BYTE_ORDER__) \
	&& (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define _CPU_PPC64 1
#define _CPU_BIG_ENDIAN 1
#endif

/* CPU(PPC64) - PowerPC 64-bit Little Endian */
#if (   defined(__ppc64__)     \
	|| defined(__PPC64__)      \
	|| defined(__ppc64le__)    \
	|| defined(__PPC64LE__))   \
	&& defined(__BYTE_ORDER__) \
	&& (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define _CPU_PPC64LE 1
#endif



/* CPU(SH4) - SuperH SH-4 */
#if defined(__SH4__)
#define _CPU_SH4 1
#endif

/* CPU(S390X) - S390 64-bit */
#if defined(__s390x__)
#define _CPU_S390X 1
#define _CPU_BIG_ENDIAN 1
#endif

/* CPU(S390) - S390 32-bit */
#if defined(__s390__)
#define _CPU_S390 1
#define _CPU_BIG_ENDIAN 1
#endif

/* CPU(X86) - i386 / x86 32-bit */
#if   defined(__i386__) \
	|| defined(i386)     \
	|| defined(_M_IX86)  \
	|| defined(_X86_)    \
	|| defined(__THW_INTEL)
#define _CPU_X86 1
#endif

/* CPU(X86_64) - AMD64 / Intel64 / x86_64 64-bit */
#if   defined(__x86_64__) \
	|| defined(_M_X64)
#define _CPU_X86_64 1
#endif

/* CPU(ARM64) - Apple */
#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
#define _CPU_ARM64 1
#endif

/* CPU(ARM) - ARM, any version*/
#if   defined(arm) \
	|| defined(__arm__) \
	|| defined(ARM) \
	|| defined(_ARM_)
#define _CPU_ARM 1

#if defined(__ARM_PCS_VFP)
#define _CPU_ARM_HARDFP 1
#endif

#if defined(__ARMEB__)
#define _CPU_BIG_ENDIAN 1

#elif !defined(__ARM_EABI__) \
	&& !defined(__EABI__) \
	&& !defined(__VFP_FP__) \
	&& !defined(_WIN32_WCE)
#define _CPU_MIDDLE_ENDIAN 1

#endif
#endif

///////////////////////os///////////////////////////
#define OS(FEATURE) (defined _OS_##FEATURE  && _OS_##FEATURE)

#ifdef _AIX
#define _OS_AIX 1
#endif

/* OS(DARWIN) - Any Darwin-based OS, including Mac OS X and iPhone OS */
#ifdef __APPLE__
#define _OS_DARWIN 1
#endif


/* OS(IOS) - iOS */
/* OS(MAC_OS_X) - Mac OS X (not including iOS) */
#if OS(DARWIN) && ((defined(TARGET_OS_EMBEDDED) && TARGET_OS_EMBEDDED) \
	|| (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)                 \
	|| (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR))
#define _OS_IOS 1
#elif OS(DARWIN) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define _OS_MAC_OS_X 1
#endif

/* OS(FREEBSD) - FreeBSD */
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#define _OS_FREEBSD 1
#endif

/* OS(HURD) - GNU/Hurd */
#ifdef __GNU__
#define _OS_HURD 1
#endif

/* OS(LINUX) - Linux */
#ifdef __linux__
#define _OS_LINUX 1
#endif

/* OS(NETBSD) - NetBSD */
#if defined(__NetBSD__)
#define _OS_NETBSD 1
#endif

/* OS(OPENBSD) - OpenBSD */
#ifdef __OpenBSD__
#define _OS_OPENBSD 1
#endif

/* OS(SOLARIS) - Solaris */
#if defined(sun) || defined(__sun)
#define _OS_SOLARIS 1
#endif

/* OS(WINDOWS) - Any version of Windows */
#if defined(WIN32) || defined(_WIN32)
#define _OS_WINDOWS 1
#endif

#define _OS_WIN ERROR "USE WINDOWS WITH OS NOT WIN"
#define _OS_MAC ERROR "USE MAC_OS_X WITH OS NOT MAC"

/* OS(UNIX) - Any Unix-like system */
#if    OS(AIX)              \
	|| OS(DARWIN)           \
	|| OS(FREEBSD)          \
	|| OS(HURD)             \
	|| OS(LINUX)            \
	|| OS(NETBSD)           \
	|| OS(OPENBSD)          \
	|| OS(SOLARIS)          \
	|| defined(unix)        \
	|| defined(__unix)      \
	|| defined(__unix__)
#define _OS_UNIX 1
#endif







#endif