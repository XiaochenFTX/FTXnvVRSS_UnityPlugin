using UnityEngine;

public class GazeSimulate : MonoBehaviour
{
    public float InnerRadiusX = 0.35f;
    public float InnerRadiusY = 0.25f;

    public float MiddleRadiusX = 0.85f;
    public float MiddleRadiusY = 0.75f;

    public float PeripheralRadiusX = 1.05f;
    public float PeripheralRadiusY = 0.95f;

    void Awake()
    {
        InitVRSTest();
    }

    // Update is called once per frame
    void LateUpdate()
    {
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
        var pos = Camera.main.ScreenToViewportPoint(Input.mousePosition);
        VRSSNativeAPI.UpdateGazeLocationMono(pos.x - 0.5f, pos.y - 0.5f);
#endif
    }

    void InitVRSTest()
    {
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
        VRSSNativeAPI.ResetGazeInitData(VRSSNativeAPI.NV_GAZE_DATA_MONO, 100, 100);

        VRSSNativeAPI.SetVRSRenderMode(VRSSNativeAPI.NV_VRS_RENDER_MODE_MONO);
        VRSSNativeAPI.SetVRSFoveationPattern(VRSSNativeAPI.NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_CUSTOM);
        VRSSNativeAPI.SetVRSShadingRatePreset(VRSSNativeAPI.NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM);

        //
        VRSSNativeAPI.SetVRSShadingRateCustom(
            VRSSNativeAPI.NV_PIXEL_X1_PER_RASTER_PIXEL,
            VRSSNativeAPI.NV_PIXEL_X1_PER_2X2_RASTER_PIXELS,
            VRSSNativeAPI.NV_PIXEL_X1_PER_4X4_RASTER_PIXELS);

        //
        VRSSNativeAPI.SetVRSRange(
            InnerRadiusX, InnerRadiusY,
            MiddleRadiusX, MiddleRadiusY,
            PeripheralRadiusX, PeripheralRadiusY);
#endif
    }
}
