using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.RenderGraphModule;
using UnityEngine.Rendering.Universal;

public class EnableVRSSRenderPass: ScriptableRenderPass
{
    private class PassData { }

    public override void RecordRenderGraph(RenderGraph renderGraph, ContextContainer frameData)
    {
        const string passName = "Enable VRSS";

        using (var builder = renderGraph.AddRasterRenderPass<PassData>(passName, out var passData))
        {
            UniversalResourceData resourceData = frameData.Get<UniversalResourceData>();

            builder.SetRenderAttachment(resourceData.activeColorTexture, 0);
            builder.SetRenderFunc((PassData data, RasterGraphContext context) =>
            {
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
                context.cmd.IssuePluginEvent(VRSSNativeAPI.GetRenderEventFunc(), VRSSNativeAPI.EVENT_ENABLE_VRS);
#endif
            });
        }
    }
}

public class DisableVRSSRenderPass: ScriptableRenderPass
{
    private class PassData { }

    public override void RecordRenderGraph(RenderGraph renderGraph, ContextContainer frameData)
    {
        const string passName = "Disable VRSS";

        using (var builder = renderGraph.AddRasterRenderPass<PassData>(passName, out var passData))
        {
            UniversalResourceData resourceData = frameData.Get<UniversalResourceData>();

            builder.SetRenderAttachment(resourceData.activeColorTexture, 0);
            builder.SetRenderFunc((PassData data, RasterGraphContext context) =>
            {
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
                context.cmd.IssuePluginEvent(VRSSNativeAPI.GetRenderEventFunc(), VRSSNativeAPI.EVENT_DISABLE_VRS);
#endif
            });
        }
    }
}
