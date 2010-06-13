 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>

#include "cdrip_info.h"
#include "dllinterface.h"

const String &BoCA::CDRipInfo::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (cdripdll != NIL && ex_CR_GetNumCDROM() >= 1)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>CDRip Ripper Device Info Component</name>	\
		    <version>1.0</version>				\
		    <id>cdrip-info</id>					\
		    <type>deviceinfo</type>				\
		  </component>						\
									\
		";
	}

	return componentSpecs;
}

static Bool	 initializedCDRip = False;

Void smooth::AttachDLL(Void *instance)
{
	LoadCDRipDLL();

	if (cdripdll == NIL) return;

	if (!ex_CR_IsInitialized())
	{
		BoCA::Config	*config = BoCA::Config::Get();

		Long		 error = CDEX_OK;
		OSVERSIONINFOA	 vInfo;

		vInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

		GetVersionExA(&vInfo);

		if (vInfo.dwPlatformId != VER_PLATFORM_WIN32_NT) config->SetIntValue("CDRip", "UseNTSCSI", False);

		error = ex_CR_Init(config->GetIntValue("CDRip", "UseNTSCSI", True));

		if (error != CDEX_OK		 && 
		    error != CDEX_ACCESSDENIED	 &&
		    vInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			config->SetIntValue("CDRip", "UseNTSCSI", !config->GetIntValue("CDRip", "UseNTSCSI", True));

			error = ex_CR_Init(config->GetIntValue("CDRip", "UseNTSCSI", True));
		}

		if	(error == CDEX_ACCESSDENIED)	BoCA::Utilities::ErrorMessage("Access to CD-ROM drives was denied by Windows.\n\nPlease contact your system administrator in order\nto be granted the right to access the CD-ROM drive.");
		else if (error != CDEX_OK &&
			 error != CDEX_NOCDROMDEVICES)	BoCA::Utilities::ErrorMessage("Unable to load ASPI drivers! CD ripping disabled!");

		if (error == CDEX_OK) config->cdrip_numdrives = ex_CR_GetNumCDROM();

		if (config->cdrip_numdrives <= config->cdrip_activedrive) config->cdrip_activedrive = 0;

		initializedCDRip = True;
	}
}

Void smooth::DetachDLL()
{
	if (cdripdll == NIL) return;

	if (initializedCDRip) ex_CR_DeInit();

	FreeCDRipDLL();
}

Bool	 BoCA::CDRipInfo::initialized = False;

BoCA::CDRipInfo::CDRipInfo()
{
	if (!initialized)
	{
		CollectDriveInfo();

		initialized = True;
	}
}

BoCA::CDRipInfo::~CDRipInfo()
{
}

Bool BoCA::CDRipInfo::OpenNthDeviceTray(Int n)
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n < nDrives)
	{
		ex_CR_SetActiveCDROM(n);
		ex_CR_EjectCD(True);

		return True;
	}

	return False;
}

Bool BoCA::CDRipInfo::CloseNthDeviceTray(Int n)
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n < nDrives)
	{
		ex_CR_SetActiveCDROM(n);
		ex_CR_EjectCD(False);

		return True;
	}

	return False;
}

const Array<String> &BoCA::CDRipInfo::GetNthDeviceTrackList(Int n)
{
	static Array<String>	 trackList;

	trackList.RemoveAll();

	const MCDI	&mcdi = GetNthDeviceMCDI(n);

	for (Int i = 0; i < mcdi.GetNumberOfEntries(); i++)
	{
		/* Find all valid audio tracks (tracks with a positive length).
		 */
		if (mcdi.GetNthEntryType(i) == ENTRY_AUDIO && mcdi.GetNthEntryOffset(i + 1) - mcdi.GetNthEntryOffset(i) > 0)
		{
			/* Add CD track to joblist using a cdda:// URI
			 */
			trackList.Add(String("cdda://")
				     .Append(String::FromInt(n))
				     .Append("/")
				     .Append(String::FromInt(mcdi.GetNthEntryTrackNumber(i))));
		}
	}

	return trackList;
}

const BoCA::MCDI &BoCA::CDRipInfo::GetNthDeviceMCDI(Int n)
{
	static MCDI	 mcdi = MCDI(Buffer<UnsignedByte>());

	/* Do not read the TOC again if the last
	 * request was less than half a second ago.
	 */
	{
		static Int	 lastDrive = -1;
		static Int	 lastAccess = 0;

		Int	 clockValue = clock();

		if (lastDrive == n && clockValue - lastAccess < 500)
		{
			lastAccess = clockValue;

			return mcdi;
		}

		lastDrive = n;
		lastAccess = clockValue;
	}

	mcdi.SetData(Buffer<UnsignedByte>());

	Int	 nDrives = ex_CR_GetNumCDROM();

	if (n < nDrives)
	{
		ex_CR_SetActiveCDROM(n);
		ex_CR_ReadToc();

		Int			 numTocEntries = ex_CR_GetNumTocEntries();
		Buffer<UnsignedByte>	 buffer(4 + 8 * numTocEntries + 8);

		((short *) (UnsignedByte *) buffer)[0] = htons(buffer.Size() - 2);

		buffer[2] = ex_CR_GetTocEntry(0).btTrackNumber;
		buffer[3] = ex_CR_GetTocEntry(numTocEntries - 1).btTrackNumber;

		for (Int i = 0; i <= numTocEntries; i++)
		{
			TOCENTRY	 entry = ex_CR_GetTocEntry(i);

			buffer[4 + 8 * i + 0] = 0;
			buffer[4 + 8 * i + 1] = entry.btFlag;
			buffer[4 + 8 * i + 2] = entry.btTrackNumber;
			buffer[4 + 8 * i + 3] = 0;

			long		 address = entry.dwStartSector;

			if (address < 0) address = ~((-address) - 1) & ((1 << 24) - 1);

			((unsigned long *) (UnsignedByte *) buffer)[1 + 2 * i + 1] = htonl(address);
		}

		mcdi.SetData(buffer);
	}

	return mcdi;
}

Void BoCA::CDRipInfo::CollectDriveInfo()
{
	Int	 nDrives = ex_CR_GetNumCDROM();

	for (Int i = 0; i < nDrives; i++)
	{
		ex_CR_SetActiveCDROM(i);

		CDROMPARAMS	 params;

		ex_CR_GetCDROMParameters(&params);

		Device	 drive;

		drive.type = DEVICE_CDROM;
		drive.name = params.lpszCDROMID;

		drive.canOpenTray = True;

		devices.Add(drive);
	}
}
