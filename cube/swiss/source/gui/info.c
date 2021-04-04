#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <gccore.h>
#include <ogc/exi.h>
#include <ogc/machine/processor.h>
#include "deviceHandler.h"
#include "FrameBufferMagic.h"
#include "IPLFontWrite.h"
#include "swiss.h"
#include "settings.h"
#include "main.h"
#include "ata.h"
#include "exi.h"
#include "bba.h"
#include "gcloader.h"
#include "wkf.h"

char topStr[256];

uiDrawObj_t * info_draw_page(int page_num) {
	uiDrawObj_t *container = DrawEmptyBox(20,60, getVideoMode()->fbWidth-20, 420);
	
	// System Info (Page 1/3)
	if(!page_num) {
		DrawAddChild(container, DrawLabel(30, 55, "System Info (1/3):"));
		// Model
		DrawAddChild(container, DrawStyledLabel(640/2, 90, (char*)"MODEL", 0.65f, true, defaultColor));
		if(mfpvr() == GC_CPU_VERSION01) {
			if(*(u32*)&driveVersion[0] == 0x20010831) {
				sprintf(topStr, "Panasonic Q SL-GC10-S");
			}
			else if(!strncmp(&IPLInfo[0x55], "MPAL Revision 1.1", 17)) {
				sprintf(topStr, "Nintendo GameCube DOL-002 (BRA)");
			}
			else {
				sprintf(topStr, "Nintendo GameCube DOL-001");
			}
		}
		else if(mfpvr() == GC_CPU_VERSION02) {
			sprintf(topStr, "Nintendo GameCube DOL-101");
		}
		else {
			sprintf(topStr, "Nintendo Wii");
		}
		DrawAddChild(container, DrawStyledLabel(640/2, 106, topStr, 0.75f, true, defaultColor));
		// IPL version string
		DrawAddChild(container, DrawStyledLabel(640/2, 130, (char*)"IPL VERSION", 0.65f, true, defaultColor));
		if(is_gamecube()) {
			if(!IPLInfo[0x55]) {
				sprintf(topStr, "NTSC Revision 1.0");
			}
			else {
				sprintf(topStr, "%s", &IPLInfo[0x55]);
			}
		}
		else {
			sprintf(topStr, "Wii");
		}
		DrawAddChild(container, DrawStyledLabel(640/2, 146, topStr, 0.75f, true, defaultColor));
		
		DrawAddChild(container, DrawStyledLabel(640/2, 170, (char*)"VIDEO MODE", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 186, getVideoModeString(), 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 210, (char*)"AUDIO", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 226, (char*)(swissSettings.sramStereo ? "Stereo" : "Mono"), 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 250, (char*)"LANGUAGE", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 266, (char*)(swissSettings.sramLanguage > SRAM_LANG_MAX ? "Unknown" : sramLang[swissSettings.sramLanguage]), 0.75f, true, defaultColor));

		// GC 00083214, 00083410
		DrawAddChild(container, DrawStyledLabel(640/2, 290, (char*)"CPU PVR", 0.65f, true, defaultColor));
		sprintf(topStr,"%08X",mfpvr());
		DrawAddChild(container, DrawStyledLabel(640/2, 306, topStr, 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 330, (char*)"CPU ECID", 0.65f, true, defaultColor));
		sprintf(topStr,"%08X:%08X:%08X:%08X",mfspr(0x39C),mfspr(0x39D),mfspr(0x39E),mfspr(0x39F));
		DrawAddChild(container, DrawStyledLabel(640/2, 346, topStr, 0.75f, true, defaultColor));
	}
	else if(page_num == 1) {
		DrawAddChild(container, DrawLabel(30, 55, "Device Info (2/3):"));
		
		DEVICEHANDLER_INTERFACE* dev = getDeviceByLocation(LOC_MEMCARD_SLOT_A);
		DrawAddChild(container, DrawStyledLabel(640/2, 90, (char*)"SLOT-A", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 106, (char*)(dev == NULL ? "Empty" : dev->hwName), 0.75f, true, defaultColor));
		dev = getDeviceByLocation(LOC_MEMCARD_SLOT_B);
		DrawAddChild(container, DrawStyledLabel(640/2, 130, (char*)"SLOT-B", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 146, (char*)(dev == NULL ? "Empty" : dev->hwName), 0.75f, true, defaultColor));
		dev = getDeviceByLocation(LOC_SERIAL_PORT_1);
		DrawAddChild(container, DrawStyledLabel(640/2, 170, (char*)"SERIAL PORT 1", 0.65f, true, defaultColor));
		if(dev != NULL) {
			sprintf(topStr, "%s (%s)",dev->hwName, net_initialized ? bba_ip : "Not initialized");
		}
		DrawAddChild(container, DrawStyledLabel(640/2, 186, (char*)(dev == NULL ? "Empty" : topStr), 0.75f, true, defaultColor));
		dev = getDeviceByLocation(LOC_SERIAL_PORT_2);
		DrawAddChild(container, DrawStyledLabel(640/2, 210, (char*)"SERIAL PORT 2", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 226, (char*)(dev == NULL ? "Empty" : dev->hwName), 0.75f, true, defaultColor));
		dev = getDeviceByLocation(LOC_DVD_CONNECTOR);
		DrawAddChild(container, DrawStyledLabel(640/2, 250, (char*)"DRIVE INTERFACE", 0.65f, true, defaultColor));
		if(dev == &__device_dvd) {
			sprintf(topStr, "%s %02X %02X%02X/%02X (%02X)",dev->hwName,driveVersion[2],driveVersion[0],driveVersion[1],driveVersion[3],driveVersion[4]);
		}
		else if(dev == &__device_gcloader) {
			char* gcloaderVersionStr = gcloaderGetVersion();
			sprintf(topStr, "%s %s",dev->hwName,gcloaderVersionStr);
			free(gcloaderVersionStr);
		}
		else if(dev == &__device_wkf) {
			sprintf(topStr, "%s (%s)",dev->hwName,wkfGetSerial());
		}
		else {
			strcpy(topStr, (dev == NULL ? "Empty" : dev->hwName));
		}
		DrawAddChild(container, DrawStyledLabel(640/2, 266, topStr, 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 290, (char*)"PROGRESSIVE VIDEO", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 306, (char*)(getDTVStatus() ? "Yes" : "No"), 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 330, (char*)"CURRENT MEDIUM", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 346, (char*)(devices[DEVICE_CUR] != NULL ? devices[DEVICE_CUR]->deviceName : "None"), 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 370, (char*)"CONFIG MEDIUM", 0.65f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 386, (char*)(devices[DEVICE_CONFIG] != NULL ? devices[DEVICE_CONFIG]->deviceName : "None"), 0.75f, true, defaultColor));
	}
	else if(page_num == 2) {
		DrawAddChild(container, DrawLabel(30, 55, "Credits (3/3):"));
		DrawAddChild(container, DrawStyledLabel(640/2, 115, "Swiss version 0.5", 1.0f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 140, "by emu_kidid & Extrems, 2021", 0.75f, true, defaultColor));
		sprintf(txtbuffer, "Commit %s Revision %s", GITREVISION, GITVERSION);
		DrawAddChild(container, DrawStyledLabel(640/2, 165, txtbuffer, 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 208, "Patreon supporters", 0.75f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 228, "meneerbeer, Dan Kunz, Heather Kent, Joshua Witt, Filyx20, SubElement, KirovAir", 0.60f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 246, "Cristofer Cruz, LemonMeringueTy, badsector, Fernando Avelino, RamblingOkie", 0.60f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 264, "Kory, Lindh0lm154, Alex Mitchell, Haymose", 0.60f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 292, "Extra Greetz: FIX94, megalomaniac, sepp256, novenary", 0.60f, true, defaultColor));
		DrawAddChild(container, DrawStyledLabel(110, 318, "Web/Support", 0.64f, false, defaultColor));
		DrawAddChild(container, DrawStyledLabel(410, 318, "Source/Updates", 0.64f, false, defaultColor));
		DrawAddChild(container, DrawStyledLabel(85, 334, "www.gc-forever.com", 0.64f, false, defaultColor));
		DrawAddChild(container, DrawStyledLabel(355, 334, "github.com/emukidid/swiss-gc", 0.64f, false, defaultColor));
		DrawAddChild(container, DrawStyledLabel(640/2, 378, "Visit us on IRC at EFNet/#gc-forever", 0.75f, true, defaultColor));
	}
	if(page_num != 2) {
		DrawAddChild(container, DrawLabel(520, 400, "->"));
	}
	if(page_num != 0) {
		DrawAddChild(container, DrawLabel(100, 400, "<-"));
	}
	DrawAddChild(container, DrawStyledLabel(640/2, 410, "Press A or B to return", 1.0f, true, defaultColor));
	return container;
}

void show_info() {
	int page = 0;
	uiDrawObj_t* pagePanel = NULL;
	while (PAD_ButtonsHeld(0) & PAD_BUTTON_A){ VIDEO_WaitVSync (); }
	while(1) {
		uiDrawObj_t* newPanel = info_draw_page(page);
		if(pagePanel != NULL) {
			DrawDispose(pagePanel);
		}
		pagePanel = newPanel;
		DrawPublish(pagePanel);
		while (!((PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT) 
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT) 
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_B)
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_A)
			|| (PAD_ButtonsHeld(0) & PAD_TRIGGER_R)
			|| (PAD_ButtonsHeld(0) & PAD_TRIGGER_L)))
			{ VIDEO_WaitVSync (); }
		u16 btns = PAD_ButtonsHeld(0);
		if(((btns & PAD_BUTTON_RIGHT) || (PAD_ButtonsHeld(0) & PAD_TRIGGER_R)) && page < 2)
			page++;
		if(((btns & PAD_BUTTON_LEFT) || (PAD_ButtonsHeld(0) & PAD_TRIGGER_L)) && page > 0)
			page--;
		if((btns & PAD_BUTTON_A) || (btns & PAD_BUTTON_B))
			break;
		while ((PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT) 
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT) 
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_B)
			|| (PAD_ButtonsHeld(0) & PAD_BUTTON_A)
			|| (PAD_ButtonsHeld(0) & PAD_TRIGGER_R)
			|| (PAD_ButtonsHeld(0) & PAD_TRIGGER_L))
			{ VIDEO_WaitVSync (); }
	}
	DrawDispose(pagePanel);
	while (PAD_ButtonsHeld(0) & PAD_BUTTON_A){ VIDEO_WaitVSync (); }
}
