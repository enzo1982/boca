 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/componentspecs.h>
#include <boca/common/config.h>

BoCA::AS::ComponentSpecs::ComponentSpecs()
{
	library	   = NIL;

	type	   = COMPONENT_TYPE_UNKNOWN;
	mode	   = COMPONENT_MODE_INTERNAL;

	threadSafe = True;
	debug	   = False;

	external_ignoreExitCode = False;

	/* Init component function pointers.
	 */
	func_GetComponentSpecs		= NIL;

	func_Create			= NIL;
	func_Delete			= NIL;

	func_GetConfigurationLayer	= NIL;

	func_GetErrorState		= NIL;
	func_GetErrorString		= NIL;

	func_GetConfiguration		= NIL;
	func_SetConfiguration		= NIL;

	func_CanOpenStream		= NIL;
	func_CanVerifyTrack		= NIL;

	func_GetStreamInfo		= NIL;
	func_GetFormatInfo		= NIL;

	func_SetAudioTrackInfo		= NIL;
	func_SetVendorString		= NIL;

	func_ParseBuffer		= NIL;
	func_ParseStreamInfo		= NIL;

	func_RenderBuffer		= NIL;
	func_RenderStreamInfo		= NIL;

	func_UpdateStreamInfo		= NIL;

	func_GetPackageSize		= NIL;

	func_SetDriver			= NIL;

	func_GetInBytes			= NIL;

	func_CanWrite			= NIL;

	func_SetPause			= NIL;
	func_IsPlaying			= NIL;

	func_GetOutputFileExtension	= NIL;
	func_GetNumberOfPasses		= NIL;

	func_IsLossless			= NIL;

	func_Activate			= NIL;
	func_Deactivate			= NIL;

	func_Seek			= NIL;
	func_NextPass			= NIL;

	func_ReadData			= NIL;
	func_WriteData			= NIL;
	func_TransformData		= NIL;
	func_ProcessData		= NIL;

	func_Flush			= NIL;

	func_Verify			= NIL;

	func_GetMainTabLayer		= NIL;
	func_GetStatusBarLayer		= NIL;

	func_GetNumberOfDevices		= NIL;
	func_GetNthDeviceInfo		= NIL;

	func_OpenNthDeviceTray		= NIL;
	func_CloseNthDeviceTray		= NIL;

	func_GetNthDeviceTrackList	= NIL;
	func_GetNthDeviceMCDI		= NIL;

	func_SetTrackList		= NIL;

	func_CanOpenFile		= NIL;

	func_ReadPlaylist		= NIL;
	func_WritePlaylist		= NIL;
}

BoCA::AS::ComponentSpecs::~ComponentSpecs()
{
	if (library != NIL) delete library;

	foreach (FileFormat *format, formats) delete format;
	foreach (TagSpec *spec, tag_specs) delete spec;

	foreach (Parameter *parameter, external_parameters)
	{
		foreach (Option *option, parameter->GetOptions()) delete option;

		delete parameter;
	}
}

Bool BoCA::AS::ComponentSpecs::LoadFromDLL(const String &file)
{
	/* Try to load the DLL.
	 */
	library = new DynamicLoader(file);

	/* Bail out if the library could not be loaded.
	 */
	if (library->GetSystemModuleHandle() == NIL)
	{
		Object::DeleteObject(library);

		library = NIL;

		return False;
	}

	const char *(*BoCA_GetComponentName)() = (const char *(*)()) library->GetFunctionAddress("BoCA_GetComponentName");

	if (BoCA_GetComponentName == NIL) return False;

	componentName = BoCA_GetComponentName();

	/* Get component function pointers.
	 */
	func_GetComponentSpecs		= (const char *(*)())					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetComponentSpecs"));

	func_Create			= (void *(*)())						library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Create"));
	func_Delete			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Delete"));

	func_GetConfigurationLayer	= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfigurationLayer"));

	func_GetErrorState		= (bool (*)(const void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorState"));
	func_GetErrorString		= (const void *(*)(const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorString"));

	func_GetConfiguration		= (const void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfiguration"));
	func_SetConfiguration		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetConfiguration"));

	func_CanOpenStream		= (bool (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenStream"));
	func_CanVerifyTrack		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanVerifyTrack"));

	func_GetStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStreamInfo"));
	func_GetFormatInfo		= (void (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetFormatInfo"));

	func_SetAudioTrackInfo		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetAudioTrackInfo"));
	func_SetVendorString		= (void (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetVendorString"));

	func_ParseBuffer		= (int (*)(void *, const void *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseBuffer"));
	func_ParseStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseStreamInfo"));

	func_RenderBuffer		= (int (*)(void *, void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderBuffer"));
	func_RenderStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderStreamInfo"));

	func_UpdateStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_UpdateStreamInfo"));

	func_GetPackageSize		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetPackageSize"));

	func_SetDriver			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetDriver"));

	func_GetInBytes			= (__int64 (*)(const void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetInBytes"));

	func_CanWrite			= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanWrite"));

	func_SetPause			= (int (*)(void *, bool))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetPause"));
	func_IsPlaying			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsPlaying"));

	func_GetOutputFileExtension	= (char *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetOutputFileExtension"));
	func_GetNumberOfPasses		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNumberOfPasses"));

	func_IsLossless			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsLossless"));

	func_Activate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Activate"));
	func_Deactivate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Deactivate"));

	func_Seek			= (bool (*)(void *, __int64))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Seek"));
	func_NextPass			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_NextPass"));

	func_ReadData			= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadData"));
	func_WriteData			= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WriteData"));
	func_TransformData		= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_TransformData"));
	func_ProcessData		= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ProcessData"));

	func_Flush			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Flush"));

	func_Verify			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Verify"));

	func_GetMainTabLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetMainTabLayer"));
	func_GetStatusBarLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStatusBarLayer"));

	func_GetNumberOfDevices		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNumberOfDevices"));
	func_GetNthDeviceInfo		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceInfo"));

	func_OpenNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_OpenNthDeviceTray"));
	func_CloseNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CloseNthDeviceTray"));

	func_GetNthDeviceTrackList	= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceTrackList"));
	func_GetNthDeviceMCDI		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceMCDI"));

	func_SetTrackList		= (void (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetTrackList"));

	func_CanOpenFile		= (bool (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenFile"));

	func_ReadPlaylist		= (const void *(*)(void *, const wchar_t *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadPlaylist"));
	func_WritePlaylist		= (int (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WritePlaylist"));

	return ParseXMLSpec(String(func_GetComponentSpecs()).Trim());
}

Bool BoCA::AS::ComponentSpecs::LoadFromXML(const String &file)
{
	IO::InStream	 in(IO::STREAM_FILE, file, IO::IS_READ);
	String		 xml = in.InputString(in.Size());

	in.Close();

	return ParseXMLSpec(xml);
}

String BoCA::AS::ComponentSpecs::GetExternalArgumentsString()
{
	Config	*config = Config::Get();
	String	 arguments;

	foreach (Parameter *param, external_parameters)
	{
		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				if (!config->GetIntValue(id, param->GetName(), param->GetEnabled())) continue;

				arguments.Append(param->GetArgument()).Append(" ");

				break;
			case PARAMETER_TYPE_SELECTION:
				if (!config->GetIntValue(id, String("Set ").Append(param->GetName()), param->GetEnabled())) continue;

				arguments.Append(param->GetArgument().Replace("%VALUE", config->GetStringValue(id, param->GetName(), param->GetDefault()))).Append(" ");

				break;
			case PARAMETER_TYPE_RANGE:
				if (!config->GetIntValue(id, String("Set ").Append(param->GetName()), param->GetEnabled())) continue;

				arguments.Append(param->GetArgument().Replace("%VALUE", String::FromFloat(config->GetIntValue(id, param->GetName(), Math::Round(param->GetDefault().ToFloat() / param->GetStepSize())) * param->GetStepSize()))).Append(" ");

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	return arguments;
}

Bool BoCA::AS::ComponentSpecs::ParseXMLSpec(const String &xml)
{
	if (xml == NIL) return False;

	/* Places for external commands.
	 */
#if defined __WIN32__
	static const char	*places[] = { "%APPDIR\\codecs\\cmdline\\%COMMAND", "%APPDIR\\codecs\\cmdline\\%COMMAND.exe", NIL };
#elif defined __APPLE__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/opt/local/bin/%COMMAND", "/sw/bin/%COMMAND", NIL };
#elif defined __HAIKU__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "/boot/common/bin/%COMMAND", NIL };
#elif defined __NetBSD__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/usr/pkg/bin/%COMMAND", NIL };
#else
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", NIL };
#endif

	XML::Document	*document = new XML::Document();

	document->ParseMemory((void *) (char *) xml, xml.Length());

	XML::Node	*root = document->GetRootNode();

	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if (node->GetName() == "name")
		{
			name = node->GetContent();
		}
		else if (node->GetName() == "id")
		{
			id = node->GetContent();
		}
		else if (node->GetName() == "version")
		{
			version = node->GetContent();

			if (node->GetAttributeByName("debug") != NIL)
			{
				debug = (node->GetAttributeByName("debug")->GetContent() == "true");
			}
		}
		else if (node->GetName() == "type")
		{
			if	(node->GetContent() == "decoder")	type = COMPONENT_TYPE_DECODER;
			else if (node->GetContent() == "encoder")	type = COMPONENT_TYPE_ENCODER;
			else if (node->GetContent() == "output")	type = COMPONENT_TYPE_OUTPUT;
			else if (node->GetContent() == "deviceinfo")	type = COMPONENT_TYPE_DEVICEINFO;
			else if (node->GetContent() == "dsp")		type = COMPONENT_TYPE_DSP;
			else if (node->GetContent() == "extension")	type = COMPONENT_TYPE_EXTENSION;
			else if (node->GetContent() == "playlist")	type = COMPONENT_TYPE_PLAYLIST;
			else if (node->GetContent() == "tagger")	type = COMPONENT_TYPE_TAGGER;
			else if (node->GetContent() == "verifier")	type = COMPONENT_TYPE_VERIFIER;
			else						type = COMPONENT_TYPE_UNKNOWN;

			if (node->GetAttributeByName("threadSafe") != NIL)
			{
				threadSafe = (node->GetAttributeByName("threadSafe")->GetContent() == "true");
			}
		}
		else if (node->GetName() == "require")
		{
			requireComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "replace")
		{
			replaceComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "conflict")
		{
			conflictComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "precede")
		{
			precedeComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "succeed")
		{
			succeedComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "format")
		{
			FileFormat	*format = new FileFormat();

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "name")	  format->SetName(node2->GetContent());
				else if (node2->GetName() == "lossless")  format->SetLossless(node2->GetContent() == "true");
				else if (node2->GetName() == "extension") format->AddExtension(node2->GetContent());
				else if (node2->GetName() == "tag")
				{
					TagFormat	 tagFormat;

					tagFormat.SetName(node2->GetContent());

					if (node2->GetAttributeByName("id") != NIL) tagFormat.SetTagger(node2->GetAttributeByName("id")->GetContent());

					if (node2->GetAttributeByName("mode") != NIL)
					{
						if	(node2->GetAttributeByName("mode")->GetContent() == "prepend")	tagFormat.SetMode(TAG_MODE_PREPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "append")	tagFormat.SetMode(TAG_MODE_APPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "other")	tagFormat.SetMode(TAG_MODE_OTHER);
					}

					format->AddTagFormat(tagFormat);
				}
			}

			formats.Add(format);
		}
		else if (node->GetName() == "tagspec")
		{
			TagSpec	*spec = new TagSpec();

			spec->SetDefault(True);

			spec->SetCoverArtSupported(False);
			spec->SetCoverArtDefault(True);

			spec->SetFreeEncodingSupported(False);

			if (node->GetAttributeByName("default") != NIL) spec->SetDefault(node->GetAttributeByName("default")->GetContent() == "true");

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "name")	 spec->SetName(node2->GetContent());
				else if	(node2->GetName() == "coverart")
				{
					if (node2->GetAttributeByName("supported") != NIL) spec->SetCoverArtSupported(node2->GetAttributeByName("supported")->GetContent() == "true");
					if (node2->GetAttributeByName("default")   != NIL) spec->SetCoverArtDefault(node2->GetAttributeByName("default")->GetContent() == "true");
				}
				else if (node2->GetName() == "encodings")
				{
					if (node2->GetAttributeByName("free") != NIL) spec->SetFreeEncodingSupported(node2->GetAttributeByName("free")->GetContent() == "true");

					for (Int k = 0; k < node2->GetNOfNodes(); k++)
					{
						XML::Node	*node3 = node2->GetNthNode(k);

						if (node3->GetName() == "encoding")
						{
							spec->AddEncoding(node3->GetContent());

							if (spec->GetEncodings().Length() == 1) spec->SetDefaultEncoding(node3->GetContent());

							if (node3->GetAttributeByName("default") != NIL)
							{
								if (node3->GetAttributeByName("default")->GetContent() == "true") spec->SetDefaultEncoding(node3->GetContent());
							}
						}
					}
				}
			}

			tag_specs.Add(spec);
		}
		else if (node->GetName() == "external" && external_command == NIL)
		{
			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if (node2->GetName() == "command")
				{
					external_command = node2->GetContent();

					if (node2->GetAttributeByName("ignoreExitCode") != NIL) external_ignoreExitCode = (node2->GetAttributeByName("ignoreExitCode")->GetContent() == "true");
				}
				else if (node2->GetName() == "md5")
				{
					external_md5_arguments = node2->GetContent();

					if (node2->GetAttributeByName("require") != NIL) external_md5_require = node2->GetAttributeByName("require")->GetContent();
					if (node2->GetAttributeByName("prefix")	 != NIL) external_md5_prefix  = node2->GetAttributeByName("prefix")->GetContent();
				}
				else if (node2->GetName() == "arguments")  external_arguments	= node2->GetContent();
				else if (node2->GetName() == "informat")   external_informat	= node2->GetContent();
				else if (node2->GetName() == "outformat")  external_outformat	= node2->GetContent();
				else if (node2->GetName() == "mode")	   mode			= node2->GetContent() == "file" ? COMPONENT_MODE_EXTERNAL_FILE : COMPONENT_MODE_EXTERNAL_STDIO;
				else if (node2->GetName() == "parameters") ParseExternalParameters(node2);
			}

			/* Check if external command actually exists.
			 */
			if (external_command[0] != '/' && external_command[1] != ':')
			{
				for (Int i = 0; places[i] != NIL; i++)
				{
					String	 delimiter = Directory::GetDirectoryDelimiter();
					String	 file	   = String(places[i]).Replace("%APPDIR", GUI::Application::GetApplicationDirectory()).Replace("%COMMAND", external_command).Replace(String(delimiter).Append(delimiter), delimiter);

					if (File(file).Exists()) { external_command = file; break; }
				}

				if (external_command[0] != '/' && external_command[1] != ':') external_command = NIL;
			}

			if (!File(external_command).Exists()) external_command = NIL;
		}
	}

	delete document;

	/* Report an error if no external command could be found in external mode.
	 */
	if (mode != COMPONENT_MODE_INTERNAL && external_command == NIL) return False;

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseExternalParameters(XML::Node *root)
{
	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if (node->GetName() == "switch" || node->GetName() == "selection" || node->GetName() == "range")
		{
			Parameter	*parameter = new Parameter();

			parameter->SetEnabled(False);

			if (node->GetAttributeByName("name")	 != NIL) parameter->SetName(node->GetAttributeByName("name")->GetContent());
			if (node->GetAttributeByName("argument") != NIL) parameter->SetArgument(node->GetAttributeByName("argument")->GetContent());
			if (node->GetAttributeByName("enabled")	 != NIL) parameter->SetEnabled(node->GetAttributeByName("enabled")->GetContent() == "true" ? True : False);

			if (node->GetName() == "switch")
			{
				parameter->SetType(PARAMETER_TYPE_SWITCH);
			}
			else if (node->GetName() == "selection")
			{
				parameter->SetType(PARAMETER_TYPE_SELECTION);

				if (node->GetAttributeByName("default") != NIL) parameter->SetDefault(node->GetAttributeByName("default")->GetContent());

				for (Int j = 0; j < node->GetNOfNodes(); j++)
				{
					XML::Node	*node2 = node->GetNthNode(j);

					if (node2->GetName() == "option")
					{
						Option	 *option = new Option();

						option->SetValue(node2->GetContent());

						if (node2->GetAttributeByName("alias") != NIL)	option->SetAlias(node2->GetAttributeByName("alias")->GetContent());
						else						option->SetAlias(option->GetValue());

						option->SetType(OPTION_TYPE_OPTION);

						parameter->AddOption(option);
					}
				}
			}
			else if (node->GetName() == "range")
			{
				parameter->SetType(PARAMETER_TYPE_RANGE);

				if (node->GetAttributeByName("default") != NIL) parameter->SetDefault(node->GetAttributeByName("default")->GetContent());

				if (node->GetAttributeByName("step") != NIL) parameter->SetStepSize(node->GetAttributeByName("step")->GetContent().ToFloat());
				else					     parameter->SetStepSize(1.0);

				for (Int j = 0; j < node->GetNOfNodes(); j++)
				{
					XML::Node	*node2 = node->GetNthNode(j);

					if (node2->GetName() == "min" || node2->GetName() == "max")
					{
						Option	 *option = new Option();

						option->SetValue(node2->GetContent());

						if (node2->GetAttributeByName("alias") != NIL)	option->SetAlias(node2->GetAttributeByName("alias")->GetContent());
						else						option->SetAlias(option->GetValue());

						if	(node2->GetName() == "min") option->SetType(OPTION_TYPE_MIN);
						else if (node2->GetName() == "max") option->SetType(OPTION_TYPE_MAX);

						parameter->AddOption(option);
					}
				}
			}

			external_parameters.Add(parameter);
		}
	}

	return True;
}
