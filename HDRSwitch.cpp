// HDRSwitch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "nvapi.h"
#include "NvApiDriverSettings.h"
#include <stdlib.h>
#include <stdio.h>
/*
 This function is used to print to the command line a text message
 describing the nvapi error and quits
*/
void PrintError(NvAPI_Status status)
{
	NvAPI_ShortString szDesc = { 0 };
	NvAPI_GetErrorMessage(status, szDesc);
	printf(" NVAPI error: %s\n", szDesc);
	exit(-1);
}
int main(int argc, char** argv)
{
	NvAPI_Status status;
	// (0) Initialize NVAPI. This must be done first of all
	status = NvAPI_Initialize();
	if (status != NVAPI_OK)
		PrintError(status);

	NvU32 dispIdCount = 0;

	NvPhysicalGpuHandle gpuHandles[64];
	NvU32 numOfGPUs;
	NvAPI_EnumPhysicalGPUs(gpuHandles, &numOfGPUs);

	NvU32 connected_displays = 0;
	NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], NULL, &dispIdCount, NULL);

	NV_GPU_DISPLAYIDS* dispIds = NULL;
	dispIds = new NV_GPU_DISPLAYIDS[dispIdCount];
	dispIds->version = NV_GPU_DISPLAYIDS_VER;

	if ((status != NVAPI_OK) || (dispIdCount == 0))
		PrintError(status);

	status = NvAPI_GPU_GetConnectedDisplayIds(gpuHandles[0], dispIds, &dispIdCount, 0);
	if (status != NVAPI_OK)
		PrintError(status);

	NV_HDR_COLOR_DATA hdrData = { 0 };
	hdrData.cmd = NV_HDR_CMD_SET;
	hdrData.version = NV_HDR_COLOR_DATA_VER;
	hdrData.hdrBpc = NV_BPC_10;
	hdrData.hdrColorFormat = NV_COLOR_FORMAT_RGB;
	hdrData.hdrDynamicRange = NV_DYNAMIC_RANGE_VESA;
	hdrData.hdrMode = NV_HDR_MODE_UHDA;
	// hdrData.mastering_display_data = NV_HDR_COLOR_DATA

	status = NvAPI_Disp_HdrColorControl(dispIds[0].displayId, &hdrData);
	if (status != NVAPI_OK)
		PrintError(status);
	
	int r;
	std::cin >> r;
	
	delete[] dispIds;

	NvAPI_Unload();

	return 0;
}