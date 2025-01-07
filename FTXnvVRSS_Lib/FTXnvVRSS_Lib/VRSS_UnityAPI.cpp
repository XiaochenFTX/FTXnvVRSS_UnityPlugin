#include "pch.h"

#include "FTX_UnityLog.h"

#include <format>


#define EVENT_ENABLE_VRS 1
#define EVENT_DISABLE_VRS 2



static IUnityInterfaces* s_UnityInterfaces = nullptr;
static IUnityGraphics* s_UnityGraphics = nullptr;


static ID3D11Device* s_Device = nullptr;

static ID3DNvVRSHelper* s_VRSHelper = nullptr;
static ID3DNvGazeHandler* s_GazeHandler = nullptr;

static NV_VRS_HELPER_ENABLE_PARAMS s_VRSEnableParams;
static NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS s_FoveatedGazeParams;
unsigned long long      s_GazeTimestamp;

static bool s_VRSInited = false;


//
static void init_nv_vrs_params()
{
	ZeroMemory(&s_VRSEnableParams, sizeof(NV_VRS_HELPER_ENABLE_PARAMS));

	s_VRSEnableParams.version = NV_VRS_HELPER_ENABLE_PARAMS_VER;
	s_VRSEnableParams.ContentType = NV_VRS_CONTENT_TYPE_FOVEATED_RENDERING;

	s_VRSEnableParams.sFoveatedRenderingDesc.version = NV_FOVEATED_RENDERING_DESC_VER;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.version = NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_VER;
	s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.version = NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_VER;


	// Init Settings
	//
	s_VRSEnableParams.RenderMode = NV_VRS_RENDER_MODE_MONO;

	// 
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternPreset = NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_BALANCED;

	//
	s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRatePreset = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_PERFORMANCE;

	// 
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[0] = 0.15f;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[1] = 0.1f;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[0] = 0.25f;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[1] = 0.2f;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[0] = 0.35f;
	s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[1] = 0.3f;

	//
	s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.InnerMostRegionShadingRate = NV_PIXEL_X1_PER_RASTER_PIXEL;
	s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.MiddleRegionShadingRate = NV_PIXEL_X1_PER_2X2_RASTER_PIXELS;
	s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.PeripheralRegionShadingRate = NV_PIXEL_X1_PER_4X4_RASTER_PIXELS;

}

static void init_nv_gaze_params()
{
	ZeroMemory(&s_FoveatedGazeParams, sizeof(NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS));
	s_FoveatedGazeParams.version = NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_VER;
}

static void init_nv_vrs(ID3D11Device* dxDevices)
{
	if (s_VRSInited)
	{
		return;
	}

	//
	NvAPI_Status NvStatus = NVAPI_OK;

	//
	NvAPI_Initialize();

	// Register the D3D device with NVAPI in order to safely call various NvAPI_D3D11_Create... functions 
	// from a separate shader compiling thread.
	NvAPI_D3D_RegisterDevice(dxDevices);

	//
	NV_D3D1x_GRAPHICS_CAPS capabilities = {};
	NvStatus = NvAPI_D3D1x_GetGraphicsCapabilities(dxDevices, NV_D3D1x_GRAPHICS_CAPS_VER, &capabilities);
	if (NvStatus != NvAPI_Status::NVAPI_OK) 
	{
		UnityLog_Error(std::format("FTX VRSS: NvAPI check device capabilities fail! Status: {}", (int)NvStatus));
		return;
	}

	//
	NV_VRS_HELPER_INIT_PARAMS vrsHelperInitParams;
	ZeroMemory(&vrsHelperInitParams, sizeof(NV_VRS_HELPER_INIT_PARAMS));
	vrsHelperInitParams.version = NV_VRS_HELPER_INIT_PARAMS_VER;
	vrsHelperInitParams.ppVRSHelper = &s_VRSHelper;
	NvStatus = NvAPI_D3D_InitializeVRSHelper(dxDevices, &vrsHelperInitParams);

	if (NvStatus != NVAPI_OK)
	{
		UnityLog_Error(std::format("FTX VRSS: [VRS Helper] is not supported on this setup! Status: {}", (int)NvStatus));
		return;
	}

	s_GazeTimestamp = 0;

	NV_GAZE_HANDLER_INIT_PARAMS gazeHandlerInitParams;
	ZeroMemory(&gazeHandlerInitParams, sizeof(NV_GAZE_HANDLER_INIT_PARAMS));
	gazeHandlerInitParams.version = NV_GAZE_HANDLER_INIT_PARAMS_VER;

	gazeHandlerInitParams.GazeDataDeviceId = 0;
	gazeHandlerInitParams.GazeDataType = NV_GAZE_DATA_MONO;
	gazeHandlerInitParams.fHorizontalFOV = 110.0;
	gazeHandlerInitParams.fVericalFOV = 110.0;

	gazeHandlerInitParams.ppNvGazeHandler = &s_GazeHandler;

	NvStatus = NvAPI_D3D_InitializeNvGazeHandler(dxDevices, &gazeHandlerInitParams);
	if (NvStatus != NVAPI_OK)
	{
		UnityLog_Error(std::format("FTX VRSS: [Gaze Helper] is not supported on this setup. Status: {}", (int)NvStatus));
		return;
	}

	init_nv_vrs_params();
	init_nv_gaze_params();
	s_VRSInited = true;

	DebugLog("FTX VRSS: Helper init finished.");
}

static void release_nv_vrs()
{
	s_VRSHelper->Release();
	s_VRSHelper = nullptr;

	s_GazeHandler->Release();
	s_GazeHandler = nullptr;

	s_VRSInited = false;

	DebugLog("FTX VRSS: Helper released.");
}

static void update_nv_gaze()
{
	if (s_VRSHelper == nullptr)
		return;

	// Timestamp at which the gaze data has been captured
	// Needs to be greater than previous timestamp so as to update the shading rate pattern.
	s_GazeTimestamp++;
	s_FoveatedGazeParams.Timestamp = s_GazeTimestamp;


	ID3D11DeviceContext* ctx;
	s_Device->GetImmediateContext(&ctx);

	NvAPI_Status NvStatus = s_GazeHandler->UpdateGazeData(ctx, &s_FoveatedGazeParams);
	if (NvStatus != NVAPI_OK)
	{
		UnityLog_Error(std::format("FTX VRSS: update_nv_gaze Failed! Status: {}", (int)NvStatus));
	}
}

static void update_nv_gaze_stereo(float leftEyeX, float leftEyeY, float rightEyeX, float rightEyeY)
{
	s_FoveatedGazeParams.sStereoData.sLeftEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
	s_FoveatedGazeParams.sStereoData.sLeftEye.fGazeNormalizedLocation[0] = leftEyeX;
	s_FoveatedGazeParams.sStereoData.sLeftEye.fGazeNormalizedLocation[1] = leftEyeY;
	s_FoveatedGazeParams.sStereoData.sLeftEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;

	s_FoveatedGazeParams.sStereoData.sRightEye.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
	s_FoveatedGazeParams.sStereoData.sRightEye.fGazeNormalizedLocation[0] = rightEyeX;
	s_FoveatedGazeParams.sStereoData.sRightEye.fGazeNormalizedLocation[1] = rightEyeY;
	s_FoveatedGazeParams.sStereoData.sRightEye.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;
}

static void update_nv_gaze_mono(float eyeX, float eyeY)
{
	s_FoveatedGazeParams.sMonoData.version = NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER;
	s_FoveatedGazeParams.sMonoData.fGazeNormalizedLocation[0] = eyeX;
	s_FoveatedGazeParams.sMonoData.fGazeNormalizedLocation[1] = eyeY;
	s_FoveatedGazeParams.sMonoData.GazeDataValidityFlags = NV_GAZE_LOCATION_VALID;
}

static void latch_nv_gaze()
{
	if (s_VRSHelper == nullptr)
		return;

	NV_VRS_HELPER_LATCH_GAZE_PARAMS latchGazeParams;
	ZeroMemory(&latchGazeParams, sizeof(NV_VRS_HELPER_LATCH_GAZE_PARAMS));
	latchGazeParams.version = NV_VRS_HELPER_LATCH_GAZE_PARAMS_VER;

	//
	ID3D11DeviceContext* ctx;
	s_Device->GetImmediateContext(&ctx);

	NvAPI_Status NvStatus = s_VRSHelper->LatchGaze(ctx, &latchGazeParams);
	if (NvStatus != NVAPI_OK) 
	{
		UnityLog_Error(std::format("FTX VRSS: latch_nv_gaze Error! Status: {}", (int)NvStatus));
	}
}

static void enable_nv_vrs()
{
	if (s_VRSHelper == nullptr)
		return;

	//
	ID3D11DeviceContext* ctx;
	s_Device->GetImmediateContext(&ctx);

	NvAPI_Status NvStatus = s_VRSHelper->Enable(ctx, &s_VRSEnableParams);
	if (NvStatus != NVAPI_OK) 
	{
		UnityLog_Error(std::format("FTX VRSS: enable_nv_vrs Failed! Status: {}", (int)NvStatus));
	}
}

static void disable_nv_vrs()
{
	ID3D11DeviceContext* ctx;
	s_Device->GetImmediateContext(&ctx);

	NV_VRS_HELPER_DISABLE_PARAMS disableParams;
	ZeroMemory(&disableParams, sizeof(NV_VRS_HELPER_DISABLE_PARAMS));
	disableParams.version = NV_VRS_HELPER_DISABLE_PARAMS_VER;

	NvAPI_Status NvStatus = s_VRSHelper->Disable(ctx, &disableParams);
}

//
static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType) {
	switch (eventType)
	{
		case kUnityGfxDeviceEventInitialize:
		{
			// 
			if (s_UnityInterfaces) {
				s_Device = s_UnityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
			}

			init_nv_vrs(s_Device);

			break;
		}

		case kUnityGfxDeviceEventShutdown:
		{
			release_nv_vrs();

			break;
		}
		default:
			break;
	}
}

static void UNITY_INTERFACE_API
OnRenderEvent(int eventId)
{
	if (!s_VRSInited)
	{
		return;
	}

	switch (eventId)
	{
		case EVENT_ENABLE_VRS:
			update_nv_gaze();
			latch_nv_gaze();
			// enable vrs
			enable_nv_vrs();
			break;
		case EVENT_DISABLE_VRS:
			// disable vrs
			disable_nv_vrs();
			break;
		default:
			break;
	}
}

extern "C" {

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		s_UnityInterfaces = unityInterfaces;
		s_UnityGraphics = unityInterfaces->Get<IUnityGraphics>();

		IUnityLog* pLog = unityInterfaces->Get<IUnityLog>();
		SetUnityLog(pLog);
		
		s_UnityGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
		OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);

		DebugLog("FTX VRSS: UnityPluginLoad");
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	UnityPluginUnload()
	{
		s_Device->Release();
		s_Device = nullptr;

		SetUnityLog(nullptr);

		s_UnityGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
		s_UnityGraphics = nullptr;

		s_UnityInterfaces = nullptr;

		DebugLog("FTX VRSS: UnityPluginUnload");
	}

	UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	IsInited()
	{
		return s_VRSInited;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	ResetGazeInitData(int type, float fov_h, float fov_v)
	{
		s_GazeHandler->Release();
		s_GazeHandler = nullptr;

		NV_GAZE_HANDLER_INIT_PARAMS gazeHandlerInitParams;
		ZeroMemory(&gazeHandlerInitParams, sizeof(NV_GAZE_HANDLER_INIT_PARAMS));
		gazeHandlerInitParams.version = NV_GAZE_HANDLER_INIT_PARAMS_VER;

		gazeHandlerInitParams.GazeDataDeviceId = 0;
		gazeHandlerInitParams.GazeDataType = (NV_GAZE_DATA_TYPE)type;
		gazeHandlerInitParams.fHorizontalFOV = fov_h;
		gazeHandlerInitParams.fVericalFOV = fov_v;

		gazeHandlerInitParams.ppNvGazeHandler = &s_GazeHandler;

		NvAPI_Status NvStatus = NvAPI_D3D_InitializeNvGazeHandler(s_Device, &gazeHandlerInitParams);
		if (NvStatus != NVAPI_OK)
		{
			UnityLog_Error(std::format("FTX VRSS: ResetGazeInitData Failed! Status: {}", (int)NvStatus));
		}
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	UpdateGazeLocationStereo(float leftEyeX, float leftEyeY, float rightEyeX, float rightEyeY)
	{
		if (s_VRSHelper == nullptr)
			return;

		update_nv_gaze_stereo(leftEyeX, leftEyeY, rightEyeX, rightEyeY);
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	UpdateGazeLocationMono(float eyeX, float eyeY)
	{
		if (s_VRSHelper == nullptr)
			return;

		update_nv_gaze_mono(eyeX, eyeY);
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	SetVRSRenderMode(int mode)
	{
		s_VRSEnableParams.RenderMode = (NV_VRS_RENDER_MODE)mode;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	SetVRSFoveationPattern(int pattern)
	{
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternPreset = (NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET)pattern;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	SetVRSShadingRatePreset(int rate)
	{
		s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRatePreset = (NV_FOVEATED_RENDERING_SHADING_RATE_PRESET)rate;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	SetVRSRange(float innerRadiusX, float innerRadiusY, float middleRadiusX, float middleRadiusY, float peripheralRadiusX, float peripheralRadiusY)
	{
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[0] = innerRadiusX;
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fInnermostRadii[1] = innerRadiusY;
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[0] = middleRadiusX;
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fMiddleRadii[1] = middleRadiusY;
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[0] = peripheralRadiusX;
		s_VRSEnableParams.sFoveatedRenderingDesc.FoveationPatternCustomPresetDesc.fPeripheralRadii[1] = peripheralRadiusY;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
	SetVRSShadingRateCustom(int inner, int middle, int peripheral)
	{
		s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.InnerMostRegionShadingRate = (NV_PIXEL_SHADING_RATE)inner;
		s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.MiddleRegionShadingRate = (NV_PIXEL_SHADING_RATE)middle;
		s_VRSEnableParams.sFoveatedRenderingDesc.ShadingRateCustomPresetDesc.PeripheralRegionShadingRate = (NV_PIXEL_SHADING_RATE)peripheral;
	}
}
