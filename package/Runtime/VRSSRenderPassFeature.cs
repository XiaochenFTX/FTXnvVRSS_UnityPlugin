using UnityEngine;
using UnityEngine.Rendering.Universal;

public class VRSSRenderPassFeature : ScriptableRendererFeature
{
    private EnableVRSSRenderPass m_EnableVRSSRenderPass;
    private DisableVRSSRenderPass m_DisableVRSSRenderPass;


    public override void Create()
    {
        m_EnableVRSSRenderPass = new EnableVRSSRenderPass();
        m_DisableVRSSRenderPass = new DisableVRSSRenderPass();

        m_EnableVRSSRenderPass.renderPassEvent = RenderPassEvent.BeforeRenderingOpaques;
        m_DisableVRSSRenderPass.renderPassEvent = RenderPassEvent.AfterRenderingTransparents;
    }

    public override void AddRenderPasses(ScriptableRenderer renderer, ref RenderingData renderingData)
    {
        if (renderingData.cameraData.cameraType != CameraType.Game)
            return;

        renderer.EnqueuePass(m_EnableVRSSRenderPass);
        renderer.EnqueuePass(m_DisableVRSSRenderPass);
    }
}
