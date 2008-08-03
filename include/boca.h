 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_BOCA
#define H_BOCA_BOCA

#include <smooth.h>

using namespace smooth;

#if defined __BOCA_COMPONENT_BUILD__
# define BOCA_EXPORT __declspec (dllexport)

# include "boca/component/component.h"
# include "boca/component/decodercomponent.h"
# include "boca/component/encodercomponent.h"
# include "boca/component/extensioncomponent.h"
# include "boca/component/outputcomponent.h"

#else
# define BOCA_EXPORT __declspec (dllimport)

# include "boca/application/component.h"
# include "boca/application/componentspecs.h"
# include "boca/application/decodercomponent.h"
# include "boca/application/encodercomponent.h"
# include "boca/application/extensioncomponent.h"
# include "boca/application/outputcomponent.h"
# include "boca/application/registry.h"
#endif

#include "boca/common/config.h"
#include "boca/common/configlayer.h"
#include "boca/common/i18n.h"
#include "boca/common/protocol.h"
#include "boca/common/track.h"
#include "boca/common/utilities.h"

#include "boca/common/communication/joblist.h"
#include "boca/common/communication/menu.h"

#define BoCA_BEGIN_COMPONENT(componentName)																							\
	extern "C" {																										\
		BOCA_EXPORT const char *BoCA_GetComponentName()								{ return (const char *) #componentName; }								\
	}

#define BoCA_DEFINE_DECODER_COMPONENT(componentName)										 												\
	extern "C" {																										\
		BOCA_EXPORT bool BoCA_##componentName##_CanOpenStream(void *component, const wchar_t *file)		{ return ((BoCA::componentName *) component)->CanOpenStream(file); }					\
		BOCA_EXPORT int BoCA_##componentName##_GetStreamInfo(void *component, const wchar_t *file, void *track)	{ return ((BoCA::componentName *) component)->GetStreamInfo(file, *((BoCA::Track *) track)); }		\
		BOCA_EXPORT void BoCA_##componentName##_SetInputFormat(void *component, const void *track)		{ ((BoCA::componentName *) component)->SetInputFormat(*((BoCA::Track *) track)); }			\
		BOCA_EXPORT __int64 BoCA_##componentName##_GetInBytes(void *component)					{ return ((BoCA::componentName *) component)->GetInBytes(); }						\
																												\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)					{ return ((BoCA::componentName *) component)->Activate(); }						\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)					{ return ((BoCA::componentName *) component)->Deactivate(); }						\
		BOCA_EXPORT int BoCA_##componentName##_ReadData(void *component, void *buffer, int size)		{ return ((BoCA::componentName *) component)->ReadData(*((Buffer<UnsignedByte> *) buffer), size); }	\
																												\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)					{ return ((BoCA::componentName *) component)->GetPackageSize(); }					\
		BOCA_EXPORT int BoCA_##componentName##_SetDriver(void *component, void *driver)				{ return ((BoCA::componentName *) component)->SetDriver((IO::Driver *) driver); }			\
	}

#define BoCA_DEFINE_ENCODER_COMPONENT(componentName)										 												\
	extern "C" {																										\
		BOCA_EXPORT int BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)		{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }	\
		BOCA_EXPORT char *BoCA_##componentName##_GetOutputFileExtension(void *component)			{ return ((BoCA::componentName *) component)->GetOutputFileExtension(); }				\
																												\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)					{ return ((BoCA::componentName *) component)->Activate(); }						\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)					{ return ((BoCA::componentName *) component)->Deactivate(); }						\
		BOCA_EXPORT int BoCA_##componentName##_WriteData(void *component, void *buffer, int size)		{ return ((BoCA::componentName *) component)->WriteData(*((Buffer<UnsignedByte> *) buffer), size); }	\
																												\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)					{ return ((BoCA::componentName *) component)->GetPackageSize(); }					\
		BOCA_EXPORT int BoCA_##componentName##_SetDriver(void *component, void *driver)				{ return ((BoCA::componentName *) component)->SetDriver((IO::Driver *) driver); }			\
	}

#define BoCA_DEFINE_EXTENSION_COMPONENT(componentName)										 												\
	extern "C" {																										\
		BOCA_EXPORT void *BoCA_##componentName##_GetMainTabLayer(void *component)				{ return ((BoCA::componentName *) component)->getMainTabLayer.Emit(); }						\
	}

#define BoCA_DEFINE_OUTPUT_COMPONENT(componentName)										 												\
	extern "C" {																										\
		BOCA_EXPORT int BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)		{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }	\
																												\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)					{ return ((BoCA::componentName *) component)->Activate(); }						\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)					{ return ((BoCA::componentName *) component)->Deactivate(); }						\
		BOCA_EXPORT int BoCA_##componentName##_WriteData(void *component, void *buffer, int size)		{ return ((BoCA::componentName *) component)->WriteData(*((Buffer<UnsignedByte> *) buffer), size); }	\
																												\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)					{ return ((BoCA::componentName *) component)->GetPackageSize(); }					\
																												\
		BOCA_EXPORT int BoCA_##componentName##_CanWrite(void *component)					{ return ((BoCA::componentName *) component)->CanWrite(); }						\
																												\
		BOCA_EXPORT int BoCA_##componentName##_SetPause(void *component, bool pause)				{ return ((BoCA::componentName *) component)->SetPause(pause); }					\
		BOCA_EXPORT bool BoCA_##componentName##_IsPlaying(void *component)					{ return ((BoCA::componentName *) component)->IsPlaying(); }						\
	}

#define BoCA_END_COMPONENT(componentName)										 													\
	extern "C" {																										\
		BOCA_EXPORT const char *BoCA_##componentName##_GetComponentSpecs()					{ return BoCA::componentName::GetComponentSpecs(); }							\
		BOCA_EXPORT void *BoCA_##componentName##_Create()							{ return new BoCA::componentName(); }									\
		BOCA_EXPORT bool BoCA_##componentName##_Delete(void *component)						{ delete ((BoCA::componentName *) component); return True; }						\
		BOCA_EXPORT void *BoCA_##componentName##_GetConfigurationLayer(void *component)				{ return ((BoCA::componentName *) component)->GetConfigurationLayer(); }				\
		BOCA_EXPORT void BoCA_##componentName##_FreeConfigurationLayer(void *component)				{ ((BoCA::componentName *) component)->FreeConfigurationLayer(); }					\
		BOCA_EXPORT bool BoCA_##componentName##_GetErrorState(void *component)					{ return ((BoCA::componentName *) component)->GetErrorState(); }					\
		BOCA_EXPORT const void *BoCA_##componentName##_GetErrorString(void *component)				{ return &((BoCA::componentName *) component)->GetErrorString(); }					\
	}

#endif
