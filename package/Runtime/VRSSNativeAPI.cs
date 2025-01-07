using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class VRSSNativeAPI
{
    private const string PluginName = "FTXnvVRSS_Lib";

    [DllImport(PluginName)]
    public static extern IntPtr GetRenderEventFunc();

    [DllImport(PluginName)]
    public static extern bool IsInited();

    [DllImport(PluginName)]
    public static extern void ResetGazeInitData(int type, float fov_h, float fov_v);

    [DllImport(PluginName)]
    public static extern void UpdateGazeLocationStereo(float leftEyeX, float leftEyeY, float rightEyeX, float rightEyeY);

    [DllImport(PluginName)]
    public static extern void UpdateGazeLocationMono(float eyeX, float eyeY);

    [DllImport(PluginName)]
    public static extern void SetVRSRenderMode(int mode);

    [DllImport(PluginName)]
    public static extern void SetVRSFoveationPattern(int pattern);

    [DllImport(PluginName)]
    public static extern void SetVRSShadingRatePreset(int rate);

    [DllImport(PluginName)]
    public static extern void SetVRSRange(float innerRadiusX, float innerRadiusY, float middleRadiusX, float middleRadiusY, float peripheralRadiusX, float peripheralRadiusY);

    [DllImport(PluginName)]
    public static extern void SetVRSShadingRateCustom(int inner, int middle, int peripheral);

    //
    public const int EVENT_ENABLE_VRS = 1;
    public const int EVENT_DISABLE_VRS = 2;

    //
    public const int NV_GAZE_DATA_MONO = 1;
    public const int NV_GAZE_DATA_STEREO = 2;

    //
    public const int NV_VRS_RENDER_MODE_INVALID = 0;      // Do not use !!!
    public const int NV_VRS_RENDER_MODE_MONO = 1;         // States mono rendering on the entire render target
    public const int NV_VRS_RENDER_MODE_LEFT_EYE = 2;     // States Left eye rendering of a stereo pair on the entire render target
    public const int NV_VRS_RENDER_MODE_RIGHT_EYE = 3;    // States Right eye rendering of a stereo pair on the entire render target
    public const int NV_VRS_RENDER_MODE_STEREO = 4;       // States side-by-side stereo rendering on the render target

    //
    public const int NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_INVALID = 0; // Do not use !!!
    public const int NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_WIDE = 1;
    public const int NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_BALANCED = 2;
    public const int NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_NARROW = 3;
    public const int NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_CUSTOM = 4;

    //
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_INVALID = 0;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_PERFORMANCE = 1;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_PERFORMANCE = 2;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_BALANCED = 3;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_QUALITY = 4;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_QUALITY = 5;
    public const int NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM = 6;

    //
    public const int NV_PIXEL_X0_CULL_RASTER_PIXELS = 0;         // No shading, tiles are culled
    public const int NV_PIXEL_X16_PER_RASTER_PIXEL = 1;          // 16 shading passes per 1 raster pixel
    public const int NV_PIXEL_X8_PER_RASTER_PIXEL = 2;           //  8 shading passes per 1 raster pixel
    public const int NV_PIXEL_X4_PER_RASTER_PIXEL = 3;           //  4 shading passes per 1 raster pixel
    public const int NV_PIXEL_X2_PER_RASTER_PIXEL = 4;           //  2 shading passes per 1 raster pixel
    public const int NV_PIXEL_X1_PER_RASTER_PIXEL = 5;           //  Per-pixel shading
    public const int NV_PIXEL_X1_PER_2X1_RASTER_PIXELS = 6;      //  1 shading pass per  2 raster pixels
    public const int NV_PIXEL_X1_PER_1X2_RASTER_PIXELS = 7;      //  1 shading pass per  2 raster pixels
    public const int NV_PIXEL_X1_PER_2X2_RASTER_PIXELS = 8;      //  1 shading pass per  4 raster pixels
    public const int NV_PIXEL_X1_PER_4X2_RASTER_PIXELS = 9;      //  1 shading pass per  8 raster pixels
    public const int NV_PIXEL_X1_PER_2X4_RASTER_PIXELS = 10;     //  1 shading pass per  8 raster pixels
    public const int NV_PIXEL_X1_PER_4X4_RASTER_PIXELS = 11;     //  1 shading pass per 16 raster pixels

    //

}
