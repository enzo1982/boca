 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureSpeex::ConfigureSpeex()
{
	Config	*config = Config::Get();

	quality = config->GetIntValue("Speex", "Quality", 8);
	bitrate = config->GetIntValue("Speex", "Bitrate", -16);

	if (quality > 0) cbrmode = 0;
	else		 cbrmode = 1;

	quality = Math::Abs(quality);
	bitrate = Math::Abs(bitrate);

	vbrmode = config->GetIntValue("Speex", "VBR", 0);
	vbrq = config->GetIntValue("Speex", "VBRQuality", 80);
	vbrmax = config->GetIntValue("Speex", "VBRMaxBitrate", -48);

	if (vbrmax > 0) use_vbrmax = True;
	else		use_vbrmax = False;

	vbrmax = Math::Abs(vbrmax);

	abr = config->GetIntValue("Speex", "ABR", -16);

	if (abr > 0) vbrmode = 2;

	abr = Math::Abs(abr);

	complexity = config->GetIntValue("Speex", "Complexity", 3);

	use_vad = config->GetIntValue("Speex", "VAD", 0);
	use_dtx = config->GetIntValue("Speex", "DTX", 0);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::Speex");

	group_profile		= new GroupBox(i18n->TranslateString("Profile"), Point(7, 11), Size(366, 43));

	text_profile		= new Text(i18n->TranslateString("Select encoding profile:"), Point(10, 16));

	combo_profile		= new ComboBox(Point(18 + text_profile->textSize.cx, 13), Size(338 - text_profile->textSize.cx, 0));
	combo_profile->AddEntry(i18n->TranslateString("Auto"));
	combo_profile->AddEntry(i18n->TranslateString("Narrowband (8 kHz)"));
	combo_profile->AddEntry(i18n->TranslateString("Wideband (16 kHz)"));
	combo_profile->AddEntry(i18n->TranslateString("Ultra-Wideband (32 kHz)"));
	combo_profile->SelectNthEntry(config->GetIntValue("Speex", "Mode", -1) + 1);

	group_profile->Add(text_profile);
	group_profile->Add(combo_profile);

	group_vbr_mode		= new GroupBox(i18n->TranslateString("VBR mode"), Point(7, 66), Size(128, 91));

	option_cbr		= new OptionBox(i18n->TranslateString("CBR (no VBR)"), Point(10, 14), Size(108, 0), &vbrmode, 0);
	option_cbr->onAction.Connect(&ConfigureSpeex::SetVBRMode, this);

	option_vbr		= new OptionBox(i18n->TranslateString("VBR"), Point(10, 39), Size(108, 0), &vbrmode, 1);
	option_vbr->onAction.Connect(&ConfigureSpeex::SetVBRMode, this);

	option_abr		= new OptionBox(i18n->TranslateString("ABR"), Point(10, 64), Size(108, 0), &vbrmode, 2);
	option_abr->onAction.Connect(&ConfigureSpeex::SetVBRMode, this);

	group_vbr_mode->Add(option_cbr);
	group_vbr_mode->Add(option_vbr);
	group_vbr_mode->Add(option_abr);

	group_cbr_quality	= new GroupBox(i18n->TranslateString("CBR quality"), Point(143, 66), Size(230, 66));

	option_cbr_quality	= new OptionBox(i18n->TranslateString("Quality:"), Point(10, 14), Size(55, 0), &cbrmode, 0);
	option_cbr_quality->onAction.Connect(&ConfigureSpeex::SetCBRMode, this);

	slider_cbr_quality	= new Slider(Point(74, 14), Size(101, 0), OR_HORZ, &quality, 0, 10);
	slider_cbr_quality->onValueChange.Connect(&ConfigureSpeex::SetQuality, this);

	text_cbr_quality_value	= new Text(NIL, Point(182, 16));

	option_cbr_bitrate	= new OptionBox(i18n->TranslateString("Bitrate:"), Point(10, 39), Size(55, 0), &cbrmode, 1);
	option_cbr_bitrate->onAction.Connect(&ConfigureSpeex::SetCBRMode, this);

	slider_cbr_bitrate	= new Slider(Point(74, 39), Size(101, 0), OR_HORZ, &bitrate, 4, 64);
	slider_cbr_bitrate->onValueChange.Connect(&ConfigureSpeex::SetBitrate, this);

	text_cbr_bitrate_value	= new Text(NIL, Point(182, 41));

	group_cbr_quality->Add(option_cbr_quality);
	group_cbr_quality->Add(slider_cbr_quality);
	group_cbr_quality->Add(text_cbr_quality_value);
	group_cbr_quality->Add(option_cbr_bitrate);
	group_cbr_quality->Add(slider_cbr_bitrate);
	group_cbr_quality->Add(text_cbr_bitrate_value);

	group_vbr_quality	= new GroupBox(i18n->TranslateString("VBR quality"), Point(143, 66), Size(230, 66));

	text_vbr_quality	= new Text(i18n->TranslateString("Quality:"), Point(10, 16));

	slider_vbr_quality	= new Slider(Point(text_vbr_quality->textSize.cx + 18, 14), Size(157 - text_vbr_quality->textSize.cx, 0), OR_HORZ, &vbrq, 0, 100);
	slider_vbr_quality->onValueChange.Connect(&ConfigureSpeex::SetVBRQuality, this);

	text_vbr_quality_value	= new Text(NIL, Point(182, 16));

	check_vbr_bitrate	= new CheckBox(i18n->TranslateString("Max. bitrate:"), Point(10, 39), Size(80, 0), &use_vbrmax);
	check_vbr_bitrate->onAction.Connect(&ConfigureSpeex::ToggleVBRBitrate, this);

	slider_vbr_bitrate	= new Slider(Point(93, 39), Size(82, 0), OR_HORZ, &vbrmax, 4, 64);
	slider_vbr_bitrate->onValueChange.Connect(&ConfigureSpeex::SetVBRBitrate, this);

	text_vbr_bitrate_value	= new Text(NIL, Point(182, 41));

	group_vbr_quality->Add(text_vbr_quality);
	group_vbr_quality->Add(slider_vbr_quality);
	group_vbr_quality->Add(text_vbr_quality_value);
	group_vbr_quality->Add(check_vbr_bitrate);
	group_vbr_quality->Add(slider_vbr_bitrate);
	group_vbr_quality->Add(text_vbr_bitrate_value);

	group_abr_bitrate	= new GroupBox(i18n->TranslateString("ABR target bitrate"), Point(143, 66), Size(230, 41));

	text_abr_bitrate	= new Text(i18n->TranslateString("Bitrate:"), Point(10, 16));

	slider_abr_bitrate	= new Slider(Point(text_abr_bitrate->textSize.cx + 18, 14), Size(157 - text_abr_bitrate->textSize.cx, 0), OR_HORZ, &abr, 4, 64);
	slider_abr_bitrate->onValueChange.Connect(&ConfigureSpeex::SetABRBitrate, this);

	text_abr_bitrate_value	= new Text(NIL, Point(182, 16));

	group_abr_bitrate->Add(text_abr_bitrate);
	group_abr_bitrate->Add(slider_abr_bitrate);
	group_abr_bitrate->Add(text_abr_bitrate_value);

	group_options		= new GroupBox(i18n->TranslateString("Options"), Point(7, 169), Size(167, 66));

	check_vad		= new CheckBox(i18n->TranslateString("Voice Activity Detection"), Point(10, 14), Size(147, 0), &use_vad);
	check_vad->onAction.Connect(&ConfigureSpeex::SetVAD, this);

	check_dtx		= new CheckBox(i18n->TranslateString("Discontinued Transmission"), Point(10, 39), Size(147, 0), &use_dtx);

	group_options->Add(check_vad);
	group_options->Add(check_dtx);

	group_complexity	= new GroupBox(i18n->TranslateString("Algorithm complexity"), Point(182, 169), Size(191, 42));

	text_complexity		= new Text(i18n->TranslateString("Complexity:"), Point(10, 16));

	slider_complexity	= new Slider(Point(text_complexity->textSize.cx + 18, 14), Size(144 - text_complexity->textSize.cx, 0), OR_HORZ, &complexity, 1, 10);
	slider_complexity->onValueChange.Connect(&ConfigureSpeex::SetComplexity, this);

	text_complexity_value	= new Text(NIL, Point(169, 16));

	group_complexity->Add(text_complexity);
	group_complexity->Add(slider_complexity);
	group_complexity->Add(text_complexity_value);

	SetVBRMode();
	SetVBRQuality();

	ToggleVBRBitrate();
	SetVBRBitrate();

	SetCBRMode();
	SetQuality();
	SetBitrate();

	SetABRBitrate();

	SetComplexity();

	Add(group_profile);
	Add(group_vbr_mode);
	Add(group_cbr_quality);
	Add(group_vbr_quality);
	Add(group_abr_bitrate);
	Add(group_options);
	Add(group_complexity);

	SetSize(Size(380, 242));
}

BoCA::ConfigureSpeex::~ConfigureSpeex()
{
	DeleteObject(group_profile);
	DeleteObject(text_profile);
	DeleteObject(combo_profile);

	DeleteObject(group_vbr_mode);
	DeleteObject(option_cbr);
	DeleteObject(option_vbr);
	DeleteObject(option_abr);

	DeleteObject(group_cbr_quality);
	DeleteObject(option_cbr_quality);
	DeleteObject(slider_cbr_quality);
	DeleteObject(text_cbr_quality_value);
	DeleteObject(option_cbr_bitrate);
	DeleteObject(slider_cbr_bitrate);
	DeleteObject(text_cbr_bitrate_value);

	DeleteObject(group_vbr_quality);
	DeleteObject(text_vbr_quality);
	DeleteObject(slider_vbr_quality);
	DeleteObject(text_vbr_quality_value);
	DeleteObject(check_vbr_bitrate);
	DeleteObject(slider_vbr_bitrate);
	DeleteObject(text_vbr_bitrate_value);

	DeleteObject(group_abr_bitrate);
	DeleteObject(text_abr_bitrate);
	DeleteObject(slider_abr_bitrate);
	DeleteObject(text_abr_bitrate_value);

	DeleteObject(group_options);
	DeleteObject(check_vad);
	DeleteObject(check_dtx);

	DeleteObject(group_complexity);
	DeleteObject(text_complexity);
	DeleteObject(slider_complexity);
	DeleteObject(text_complexity_value);
}

Int BoCA::ConfigureSpeex::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("Speex", "Mode", combo_profile->GetSelectedEntryNumber() - 1);
	config->SetIntValue("Speex", "VBR", vbrmode == 1);
	config->SetIntValue("Speex", "VBRQuality", vbrq);
	config->SetIntValue("Speex", "VBRMaxBitrate", use_vbrmax ? vbrmax : -vbrmax);
	config->SetIntValue("Speex", "ABR", vbrmode == 2 ? abr : -abr);
	config->SetIntValue("Speex", "Quality", cbrmode == 0 ? quality : -quality);
	config->SetIntValue("Speex", "Bitrate", cbrmode == 1 ? bitrate : -bitrate);
	config->SetIntValue("Speex", "Complexity", complexity);
	config->SetIntValue("Speex", "VAD", use_vad);
	config->SetIntValue("Speex", "DTX", use_dtx);

	return Success();
}

Void BoCA::ConfigureSpeex::SetVBRMode()
{
	group_cbr_quality->Hide();
	group_vbr_quality->Hide();
	group_abr_bitrate->Hide();

	switch (vbrmode)
	{
		case 0:
			group_cbr_quality->Show();

			check_vad->Activate();

			SetVAD();

			break;
		case 1:
			group_vbr_quality->Show();

			check_vad->Deactivate();
			check_dtx->Activate();

			break;
		case 2:
			group_abr_bitrate->Show();

			check_vad->Deactivate();
			check_dtx->Activate();

			break;
	}
}

Void BoCA::ConfigureSpeex::SetVBRQuality()
{
	String	 txt = String::FromFloat(double(vbrq) / 10);

	if (vbrq % 10 == 0) txt.Append(".0");

	text_vbr_quality_value->SetText(txt);
}

Void BoCA::ConfigureSpeex::ToggleVBRBitrate()
{
	if (use_vbrmax)
	{
		slider_vbr_bitrate->Activate();
		text_vbr_bitrate_value->Activate();
	}
	else
	{
		slider_vbr_bitrate->Deactivate();
		text_vbr_bitrate_value->Deactivate();
	}
}

Void BoCA::ConfigureSpeex::SetVBRBitrate()
{
	text_vbr_bitrate_value->SetText(String::FromInt(vbrmax).Append(" kbps"));
}

Void BoCA::ConfigureSpeex::SetCBRMode()
{
	switch (cbrmode)
	{
		case 0:
			slider_cbr_quality->Activate();
			text_cbr_quality_value->Activate();

			slider_cbr_bitrate->Deactivate();
			text_cbr_bitrate_value->Deactivate();

			break;
		case 1:
			slider_cbr_bitrate->Activate();
			text_cbr_bitrate_value->Activate();

			slider_cbr_quality->Deactivate();
			text_cbr_quality_value->Deactivate();

			break;
	}
}

Void BoCA::ConfigureSpeex::SetQuality()
{
	text_cbr_quality_value->SetText(String::FromInt(quality));
}

Void BoCA::ConfigureSpeex::SetBitrate()
{
	text_cbr_bitrate_value->SetText(String::FromInt(bitrate).Append(" kbps"));
}

Void BoCA::ConfigureSpeex::SetABRBitrate()
{
	text_abr_bitrate_value->SetText(String::FromInt(abr).Append(" kbps"));
}

Void BoCA::ConfigureSpeex::SetComplexity()
{
	text_complexity_value->SetText(String::FromInt(complexity));
}

Void BoCA::ConfigureSpeex::SetVAD()
{
	if (use_vad) check_dtx->Activate();
	else	     check_dtx->Deactivate();
}
