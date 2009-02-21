 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "winamp/out.h"

using namespace smooth;
using namespace smooth::System;

extern Array<DynamicLoader *>	 winamp_out_plugins;
extern Array<Out_Module *>	 winamp_out_modules;

Bool	 LoadWinampDLLs();
Void	 FreeWinampDLLs();