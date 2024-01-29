#include <SDL2/SDL.h>
#include "glad_egl.h"

#include "platform.h"
#include "so_util.h"
#include "thunk_gen.h"
#include "thunk_gen_dyn.h"

#include "egl_funcs.hpp"
extern DynLibFunction symtable_egl[];
extern DynLibFunction symtable_gles2[];
static int symtable_egl_index = 0;
DynLibFunction symtable_egl[256] = {
    NULL
};

#define PTR_RESOLVE(x) resolve_thunked<&glad_##x>(#x, symtable_egl_index, symtable_egl, SDL_GL_GetProcAddress)
ABI_ATTR __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress_impl (const char *procname)
{
    for (int i = 0; symtable_gles2[i].symbol; i++) {
        if (strcmp(symtable_gles2[i].symbol, procname) == 0) {
            return (__eglMustCastToProperFunctionPointerType)symtable_gles2[i].func;
        }
    }

    return NULL;
}

void load_egl_funcs()
{
    glad_eglSetBlobCacheFuncsANDROID = (PFNEGLSETBLOBCACHEFUNCSANDROIDPROC)PTR_RESOLVE(eglSetBlobCacheFuncsANDROID);
    glad_eglCreateNativeClientBufferANDROID = (PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC)PTR_RESOLVE(eglCreateNativeClientBufferANDROID);
    glad_eglGetCompositorTimingSupportedANDROID = (PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC)PTR_RESOLVE(eglGetCompositorTimingSupportedANDROID);
    glad_eglGetCompositorTimingANDROID = (PFNEGLGETCOMPOSITORTIMINGANDROIDPROC)PTR_RESOLVE(eglGetCompositorTimingANDROID);
    glad_eglGetNextFrameIdANDROID = (PFNEGLGETNEXTFRAMEIDANDROIDPROC)PTR_RESOLVE(eglGetNextFrameIdANDROID);
    glad_eglGetFrameTimestampSupportedANDROID = (PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC)PTR_RESOLVE(eglGetFrameTimestampSupportedANDROID);
    glad_eglGetFrameTimestampsANDROID = (PFNEGLGETFRAMETIMESTAMPSANDROIDPROC)PTR_RESOLVE(eglGetFrameTimestampsANDROID);
    glad_eglGetNativeClientBufferANDROID = (PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC)PTR_RESOLVE(eglGetNativeClientBufferANDROID);
    glad_eglDupNativeFenceFDANDROID = (PFNEGLDUPNATIVEFENCEFDANDROIDPROC)PTR_RESOLVE(eglDupNativeFenceFDANDROID);
    glad_eglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC)PTR_RESOLVE(eglPresentationTimeANDROID);
    glad_eglQuerySurfacePointerANGLE = (PFNEGLQUERYSURFACEPOINTERANGLEPROC)PTR_RESOLVE(eglQuerySurfacePointerANGLE);
    glad_eglGetMscRateANGLE = (PFNEGLGETMSCRATEANGLEPROC)PTR_RESOLVE(eglGetMscRateANGLE);
    glad_eglClientSignalSyncEXT = (PFNEGLCLIENTSIGNALSYNCEXTPROC)PTR_RESOLVE(eglClientSignalSyncEXT);
    glad_eglCompositorSetContextListEXT = (PFNEGLCOMPOSITORSETCONTEXTLISTEXTPROC)PTR_RESOLVE(eglCompositorSetContextListEXT);
    glad_eglCompositorSetContextAttributesEXT = (PFNEGLCOMPOSITORSETCONTEXTATTRIBUTESEXTPROC)PTR_RESOLVE(eglCompositorSetContextAttributesEXT);
    glad_eglCompositorSetWindowListEXT = (PFNEGLCOMPOSITORSETWINDOWLISTEXTPROC)PTR_RESOLVE(eglCompositorSetWindowListEXT);
    glad_eglCompositorSetWindowAttributesEXT = (PFNEGLCOMPOSITORSETWINDOWATTRIBUTESEXTPROC)PTR_RESOLVE(eglCompositorSetWindowAttributesEXT);
    glad_eglCompositorBindTexWindowEXT = (PFNEGLCOMPOSITORBINDTEXWINDOWEXTPROC)PTR_RESOLVE(eglCompositorBindTexWindowEXT);
    glad_eglCompositorSetSizeEXT = (PFNEGLCOMPOSITORSETSIZEEXTPROC)PTR_RESOLVE(eglCompositorSetSizeEXT);
    glad_eglCompositorSwapPolicyEXT = (PFNEGLCOMPOSITORSWAPPOLICYEXTPROC)PTR_RESOLVE(eglCompositorSwapPolicyEXT);
    glad_eglQueryDeviceAttribEXT = (PFNEGLQUERYDEVICEATTRIBEXTPROC)PTR_RESOLVE(eglQueryDeviceAttribEXT);
    glad_eglQueryDeviceStringEXT = (PFNEGLQUERYDEVICESTRINGEXTPROC)PTR_RESOLVE(eglQueryDeviceStringEXT);
    glad_eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)PTR_RESOLVE(eglQueryDevicesEXT);
    glad_eglQueryDisplayAttribEXT = (PFNEGLQUERYDISPLAYATTRIBEXTPROC)PTR_RESOLVE(eglQueryDisplayAttribEXT);
    glad_eglQueryDeviceBinaryEXT = (PFNEGLQUERYDEVICEBINARYEXTPROC)PTR_RESOLVE(eglQueryDeviceBinaryEXT);
    glad_eglQueryDmaBufFormatsEXT = (PFNEGLQUERYDMABUFFORMATSEXTPROC)PTR_RESOLVE(eglQueryDmaBufFormatsEXT);
    glad_eglQueryDmaBufModifiersEXT = (PFNEGLQUERYDMABUFMODIFIERSEXTPROC)PTR_RESOLVE(eglQueryDmaBufModifiersEXT);
    glad_eglGetOutputLayersEXT = (PFNEGLGETOUTPUTLAYERSEXTPROC)PTR_RESOLVE(eglGetOutputLayersEXT);
    glad_eglGetOutputPortsEXT = (PFNEGLGETOUTPUTPORTSEXTPROC)PTR_RESOLVE(eglGetOutputPortsEXT);
    glad_eglOutputLayerAttribEXT = (PFNEGLOUTPUTLAYERATTRIBEXTPROC)PTR_RESOLVE(eglOutputLayerAttribEXT);
    glad_eglQueryOutputLayerAttribEXT = (PFNEGLQUERYOUTPUTLAYERATTRIBEXTPROC)PTR_RESOLVE(eglQueryOutputLayerAttribEXT);
    glad_eglQueryOutputLayerStringEXT = (PFNEGLQUERYOUTPUTLAYERSTRINGEXTPROC)PTR_RESOLVE(eglQueryOutputLayerStringEXT);
    glad_eglOutputPortAttribEXT = (PFNEGLOUTPUTPORTATTRIBEXTPROC)PTR_RESOLVE(eglOutputPortAttribEXT);
    glad_eglQueryOutputPortAttribEXT = (PFNEGLQUERYOUTPUTPORTATTRIBEXTPROC)PTR_RESOLVE(eglQueryOutputPortAttribEXT);
    glad_eglQueryOutputPortStringEXT = (PFNEGLQUERYOUTPUTPORTSTRINGEXTPROC)PTR_RESOLVE(eglQueryOutputPortStringEXT);
    glad_eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)PTR_RESOLVE(eglGetPlatformDisplayEXT);
    glad_eglCreatePlatformWindowSurfaceEXT = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)PTR_RESOLVE(eglCreatePlatformWindowSurfaceEXT);
    glad_eglCreatePlatformPixmapSurfaceEXT = (PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC)PTR_RESOLVE(eglCreatePlatformPixmapSurfaceEXT);
    glad_eglStreamConsumerOutputEXT = (PFNEGLSTREAMCONSUMEROUTPUTEXTPROC)PTR_RESOLVE(eglStreamConsumerOutputEXT);
    glad_eglQuerySupportedCompressionRatesEXT = (PFNEGLQUERYSUPPORTEDCOMPRESSIONRATESEXTPROC)PTR_RESOLVE(eglQuerySupportedCompressionRatesEXT);
    glad_eglSwapBuffersWithDamageEXT = (PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)PTR_RESOLVE(eglSwapBuffersWithDamageEXT);
    glad_eglUnsignalSyncEXT = (PFNEGLUNSIGNALSYNCEXTPROC)PTR_RESOLVE(eglUnsignalSyncEXT);
    glad_eglCreatePixmapSurfaceHI = (PFNEGLCREATEPIXMAPSURFACEHIPROC)PTR_RESOLVE(eglCreatePixmapSurfaceHI);
    glad_eglCreateSync64KHR = (PFNEGLCREATESYNC64KHRPROC)PTR_RESOLVE(eglCreateSync64KHR);
    glad_eglDebugMessageControlKHR = (PFNEGLDEBUGMESSAGECONTROLKHRPROC)PTR_RESOLVE(eglDebugMessageControlKHR);
    glad_eglQueryDebugKHR = (PFNEGLQUERYDEBUGKHRPROC)PTR_RESOLVE(eglQueryDebugKHR);
    glad_eglLabelObjectKHR = (PFNEGLLABELOBJECTKHRPROC)PTR_RESOLVE(eglLabelObjectKHR);
    glad_eglQueryDisplayAttribKHR = (PFNEGLQUERYDISPLAYATTRIBKHRPROC)PTR_RESOLVE(eglQueryDisplayAttribKHR);
    glad_eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)PTR_RESOLVE(eglCreateSyncKHR);
    glad_eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)PTR_RESOLVE(eglDestroySyncKHR);
    glad_eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)PTR_RESOLVE(eglClientWaitSyncKHR);
    glad_eglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)PTR_RESOLVE(eglGetSyncAttribKHR);
    glad_eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)PTR_RESOLVE(eglCreateImageKHR);
    glad_eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)PTR_RESOLVE(eglDestroyImageKHR);
    glad_eglLockSurfaceKHR = (PFNEGLLOCKSURFACEKHRPROC)PTR_RESOLVE(eglLockSurfaceKHR);
    glad_eglUnlockSurfaceKHR = (PFNEGLUNLOCKSURFACEKHRPROC)PTR_RESOLVE(eglUnlockSurfaceKHR);
    glad_eglQuerySurface64KHR = (PFNEGLQUERYSURFACE64KHRPROC)PTR_RESOLVE(eglQuerySurface64KHR);
    glad_eglSetDamageRegionKHR = (PFNEGLSETDAMAGEREGIONKHRPROC)PTR_RESOLVE(eglSetDamageRegionKHR);
    glad_eglSignalSyncKHR = (PFNEGLSIGNALSYNCKHRPROC)PTR_RESOLVE(eglSignalSyncKHR);
    glad_eglCreateStreamKHR = (PFNEGLCREATESTREAMKHRPROC)PTR_RESOLVE(eglCreateStreamKHR);
    glad_eglDestroyStreamKHR = (PFNEGLDESTROYSTREAMKHRPROC)PTR_RESOLVE(eglDestroyStreamKHR);
    glad_eglStreamAttribKHR = (PFNEGLSTREAMATTRIBKHRPROC)PTR_RESOLVE(eglStreamAttribKHR);
    glad_eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)PTR_RESOLVE(eglQueryStreamKHR);
    glad_eglQueryStreamu64KHR = (PFNEGLQUERYSTREAMU64KHRPROC)PTR_RESOLVE(eglQueryStreamu64KHR);
    glad_eglCreateStreamAttribKHR = (PFNEGLCREATESTREAMATTRIBKHRPROC)PTR_RESOLVE(eglCreateStreamAttribKHR);
    glad_eglSetStreamAttribKHR = (PFNEGLSETSTREAMATTRIBKHRPROC)PTR_RESOLVE(eglSetStreamAttribKHR);
    glad_eglQueryStreamAttribKHR = (PFNEGLQUERYSTREAMATTRIBKHRPROC)PTR_RESOLVE(eglQueryStreamAttribKHR);
    glad_eglStreamConsumerAcquireAttribKHR = (PFNEGLSTREAMCONSUMERACQUIREATTRIBKHRPROC)PTR_RESOLVE(eglStreamConsumerAcquireAttribKHR);
    glad_eglStreamConsumerReleaseAttribKHR = (PFNEGLSTREAMCONSUMERRELEASEATTRIBKHRPROC)PTR_RESOLVE(eglStreamConsumerReleaseAttribKHR);
    glad_eglStreamConsumerGLTextureExternalKHR = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC)PTR_RESOLVE(eglStreamConsumerGLTextureExternalKHR);
    glad_eglStreamConsumerAcquireKHR = (PFNEGLSTREAMCONSUMERACQUIREKHRPROC)PTR_RESOLVE(eglStreamConsumerAcquireKHR);
    glad_eglStreamConsumerReleaseKHR = (PFNEGLSTREAMCONSUMERRELEASEKHRPROC)PTR_RESOLVE(eglStreamConsumerReleaseKHR);
    glad_eglGetStreamFileDescriptorKHR = (PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC)PTR_RESOLVE(eglGetStreamFileDescriptorKHR);
    glad_eglCreateStreamFromFileDescriptorKHR = (PFNEGLCREATESTREAMFROMFILEDESCRIPTORKHRPROC)PTR_RESOLVE(eglCreateStreamFromFileDescriptorKHR);
    glad_eglQueryStreamTimeKHR = (PFNEGLQUERYSTREAMTIMEKHRPROC)PTR_RESOLVE(eglQueryStreamTimeKHR);
    glad_eglCreateStreamProducerSurfaceKHR = (PFNEGLCREATESTREAMPRODUCERSURFACEKHRPROC)PTR_RESOLVE(eglCreateStreamProducerSurfaceKHR);
    glad_eglSwapBuffersWithDamageKHR = (PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC)PTR_RESOLVE(eglSwapBuffersWithDamageKHR);
    glad_eglWaitSyncKHR = (PFNEGLWAITSYNCKHRPROC)PTR_RESOLVE(eglWaitSyncKHR);
    glad_eglCreateDRMImageMESA = (PFNEGLCREATEDRMIMAGEMESAPROC)PTR_RESOLVE(eglCreateDRMImageMESA);
    glad_eglExportDRMImageMESA = (PFNEGLEXPORTDRMIMAGEMESAPROC)PTR_RESOLVE(eglExportDRMImageMESA);
    glad_eglExportDMABUFImageQueryMESA = (PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC)PTR_RESOLVE(eglExportDMABUFImageQueryMESA);
    glad_eglExportDMABUFImageMESA = (PFNEGLEXPORTDMABUFIMAGEMESAPROC)PTR_RESOLVE(eglExportDMABUFImageMESA);
    glad_eglGetDisplayDriverConfig = (PFNEGLGETDISPLAYDRIVERCONFIGPROC)PTR_RESOLVE(eglGetDisplayDriverConfig);
    glad_eglGetDisplayDriverName = (PFNEGLGETDISPLAYDRIVERNAMEPROC)PTR_RESOLVE(eglGetDisplayDriverName);
    glad_eglSwapBuffersRegionNOK = (PFNEGLSWAPBUFFERSREGIONNOKPROC)PTR_RESOLVE(eglSwapBuffersRegionNOK);
    glad_eglSwapBuffersRegion2NOK = (PFNEGLSWAPBUFFERSREGION2NOKPROC)PTR_RESOLVE(eglSwapBuffersRegion2NOK);
    glad_eglQueryNativeDisplayNV = (PFNEGLQUERYNATIVEDISPLAYNVPROC)PTR_RESOLVE(eglQueryNativeDisplayNV);
    glad_eglQueryNativeWindowNV = (PFNEGLQUERYNATIVEWINDOWNVPROC)PTR_RESOLVE(eglQueryNativeWindowNV);
    glad_eglQueryNativePixmapNV = (PFNEGLQUERYNATIVEPIXMAPNVPROC)PTR_RESOLVE(eglQueryNativePixmapNV);
    glad_eglPostSubBufferNV = (PFNEGLPOSTSUBBUFFERNVPROC)PTR_RESOLVE(eglPostSubBufferNV);
    glad_eglStreamImageConsumerConnectNV = (PFNEGLSTREAMIMAGECONSUMERCONNECTNVPROC)PTR_RESOLVE(eglStreamImageConsumerConnectNV);
    glad_eglQueryStreamConsumerEventNV = (PFNEGLQUERYSTREAMCONSUMEREVENTNVPROC)PTR_RESOLVE(eglQueryStreamConsumerEventNV);
    glad_eglStreamAcquireImageNV = (PFNEGLSTREAMACQUIREIMAGENVPROC)PTR_RESOLVE(eglStreamAcquireImageNV);
    glad_eglStreamReleaseImageNV = (PFNEGLSTREAMRELEASEIMAGENVPROC)PTR_RESOLVE(eglStreamReleaseImageNV);
    glad_eglStreamConsumerGLTextureExternalAttribsNV = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC)PTR_RESOLVE(eglStreamConsumerGLTextureExternalAttribsNV);
    glad_eglStreamFlushNV = (PFNEGLSTREAMFLUSHNVPROC)PTR_RESOLVE(eglStreamFlushNV);
    glad_eglQueryDisplayAttribNV = (PFNEGLQUERYDISPLAYATTRIBNVPROC)PTR_RESOLVE(eglQueryDisplayAttribNV);
    glad_eglSetStreamMetadataNV = (PFNEGLSETSTREAMMETADATANVPROC)PTR_RESOLVE(eglSetStreamMetadataNV);
    glad_eglQueryStreamMetadataNV = (PFNEGLQUERYSTREAMMETADATANVPROC)PTR_RESOLVE(eglQueryStreamMetadataNV);
    glad_eglResetStreamNV = (PFNEGLRESETSTREAMNVPROC)PTR_RESOLVE(eglResetStreamNV);
    glad_eglCreateStreamSyncNV = (PFNEGLCREATESTREAMSYNCNVPROC)PTR_RESOLVE(eglCreateStreamSyncNV);
    glad_eglCreateFenceSyncNV = (PFNEGLCREATEFENCESYNCNVPROC)PTR_RESOLVE(eglCreateFenceSyncNV);
    glad_eglDestroySyncNV = (PFNEGLDESTROYSYNCNVPROC)PTR_RESOLVE(eglDestroySyncNV);
    glad_eglFenceNV = (PFNEGLFENCENVPROC)PTR_RESOLVE(eglFenceNV);
    glad_eglClientWaitSyncNV = (PFNEGLCLIENTWAITSYNCNVPROC)PTR_RESOLVE(eglClientWaitSyncNV);
    glad_eglSignalSyncNV = (PFNEGLSIGNALSYNCNVPROC)PTR_RESOLVE(eglSignalSyncNV);
    glad_eglGetSyncAttribNV = (PFNEGLGETSYNCATTRIBNVPROC)PTR_RESOLVE(eglGetSyncAttribNV);
    glad_eglGetSystemTimeFrequencyNV = (PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC)PTR_RESOLVE(eglGetSystemTimeFrequencyNV);
    glad_eglGetSystemTimeNV = (PFNEGLGETSYSTEMTIMENVPROC)PTR_RESOLVE(eglGetSystemTimeNV);
    glad_eglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWLPROC)PTR_RESOLVE(eglBindWaylandDisplayWL);
    glad_eglUnbindWaylandDisplayWL = (PFNEGLUNBINDWAYLANDDISPLAYWLPROC)PTR_RESOLVE(eglUnbindWaylandDisplayWL);
    glad_eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWLPROC)PTR_RESOLVE(eglQueryWaylandBufferWL);
    glad_eglCreateWaylandBufferFromImageWL = (PFNEGLCREATEWAYLANDBUFFERFROMIMAGEWLPROC)PTR_RESOLVE(eglCreateWaylandBufferFromImageWL);
}