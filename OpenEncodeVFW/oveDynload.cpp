#include "stdafx.h"
#include "OVEncodeDyn.h"

static HMODULE hMod = NULL, hModCL = NULL;
static int64 refCount = 0;
#define FUNDEF(x) f_##x x = nullptr
#define FUNDEFDECL(x) decltype(&x) f_##x = nullptr

#define LOADPROC(x) \
do {\
if ((##x = (f_##x)GetProcAddress(hMod, #x)) == NULL){\
	goto error; }\
} while (0)

#define LOADPROC2(x) \
do {\
if ((f_##x = (decltype(&##x))GetProcAddress(hModCL, #x)) == NULL){\
	goto error; }\
} while (0)

FUNDEF(OVEncodeGetDeviceInfo);
FUNDEF(OVEncodeGetDeviceCap);
FUNDEF(OVCreateOVEHandleFromOPHandle);
FUNDEF(OVReleaseOVEHandle);
FUNDEF(OVEncodeAcquireObject);
FUNDEF(OVEncodeReleaseObject);
FUNDEF(OVCreateOVEEventFromOPEventHandle);
FUNDEF(OVEncodeReleaseOVEEventHandle);
FUNDEF(OVEncodeCreateSession);
FUNDEF(OVEncodeDestroySession);
FUNDEF(OVEncodeGetPictureControlConfig);
FUNDEF(OVEncodeGetRateControlConfig);
FUNDEF(OVEncodeGetMotionEstimationConfig);
FUNDEF(OVEncodeGetRDOControlConfig);
FUNDEF(OVEncodeSendConfig);
FUNDEF(OVEncodeTask);
FUNDEF(OVEncodeQueryTaskDescription);
FUNDEF(OVEncodeReleaseTask);

FUNDEFDECL(clGetPlatformIDs);
FUNDEFDECL(clGetPlatformInfo);
FUNDEFDECL(clCreateContext);
FUNDEFDECL(clReleaseContext);
FUNDEFDECL(clCreateContextFromType);
FUNDEFDECL(clCreateCommandQueue);
FUNDEFDECL(clReleaseCommandQueue);
FUNDEFDECL(clGetEventInfo);
FUNDEFDECL(clCreateBuffer);
FUNDEFDECL(clCreateKernel);
FUNDEFDECL(clReleaseMemObject);
FUNDEFDECL(clReleaseKernel);
FUNDEFDECL(clReleaseProgram);
FUNDEFDECL(clEnqueueMapBuffer);
FUNDEFDECL(clEnqueueNDRangeKernel);
FUNDEFDECL(clEnqueueUnmapMemObject);
FUNDEFDECL(clReleaseEvent);
FUNDEFDECL(clCreateProgramWithSource);
FUNDEFDECL(clBuildProgram);
FUNDEFDECL(clSetKernelArg);
FUNDEFDECL(clGetDeviceInfo);
FUNDEFDECL(clFlush);
FUNDEFDECL(clFinish);
FUNDEFDECL(clWaitForEvents);
FUNDEFDECL(clGetProgramBuildInfo);
FUNDEFDECL(clGetEventProfilingInfo);
FUNDEFDECL(clEnqueueWriteBuffer);
FUNDEFDECL(clGetDeviceIDs);
FUNDEFDECL(clGetKernelWorkGroupInfo);

void deinitOVE()
{
	InterlockedDecrement64(&refCount);
	if (refCount > 0)
		return;

	//TODO a bit suspect
	if (refCount < 0)
		refCount = 0;

	if (hMod)
	{
		FreeLibrary(hMod);
		hMod = NULL;
	}

	if (hModCL)
	{
		FreeLibrary(hModCL);
		hModCL = NULL;
	}
}

bool initOVE()
{
	InterlockedIncrement64(&refCount);
	if (hMod && hModCL && f_clGetKernelWorkGroupInfo)
		return true;

#ifdef _WIN64
	hMod = LoadLibrary(TEXT("OpenVideo64.dll"));
#else
	hMod = LoadLibrary(TEXT("OpenVideo.dll"));
#endif
	hModCL = LoadLibrary(TEXT("OpenCL.dll"));

	if (!hMod || !hModCL)
	{
		deinitOVE();
		return false;
	}

	LOADPROC(OVEncodeGetDeviceInfo);
	LOADPROC(OVEncodeGetDeviceCap);
	LOADPROC(OVCreateOVEHandleFromOPHandle);
	LOADPROC(OVReleaseOVEHandle);
	LOADPROC(OVEncodeAcquireObject);
	LOADPROC(OVEncodeReleaseObject);
	LOADPROC(OVCreateOVEEventFromOPEventHandle);
	LOADPROC(OVEncodeReleaseOVEEventHandle);
	LOADPROC(OVEncodeCreateSession);
	LOADPROC(OVEncodeDestroySession);
	LOADPROC(OVEncodeGetPictureControlConfig);
	LOADPROC(OVEncodeGetRateControlConfig);
	LOADPROC(OVEncodeGetMotionEstimationConfig);
	LOADPROC(OVEncodeGetRDOControlConfig);
	LOADPROC(OVEncodeSendConfig);
	LOADPROC(OVEncodeTask);
	LOADPROC(OVEncodeQueryTaskDescription);
	LOADPROC(OVEncodeReleaseTask);

	LOADPROC2(clGetPlatformIDs);
	LOADPROC2(clGetPlatformInfo);
	LOADPROC2(clCreateContext);
	LOADPROC2(clReleaseContext);
	LOADPROC2(clCreateContextFromType);
	LOADPROC2(clCreateCommandQueue);
	LOADPROC2(clReleaseCommandQueue);
	LOADPROC2(clGetEventInfo);
	LOADPROC2(clCreateBuffer);
	LOADPROC2(clCreateKernel);
	LOADPROC2(clReleaseMemObject);
	LOADPROC2(clReleaseKernel);
	LOADPROC2(clReleaseProgram);
	LOADPROC2(clEnqueueMapBuffer);
	LOADPROC2(clEnqueueNDRangeKernel);
	LOADPROC2(clEnqueueUnmapMemObject);
	LOADPROC2(clReleaseEvent);
	LOADPROC2(clCreateProgramWithSource);
	LOADPROC2(clBuildProgram);
	LOADPROC2(clSetKernelArg);
	LOADPROC2(clGetDeviceInfo);
	LOADPROC2(clFlush);
	LOADPROC2(clFinish);
	LOADPROC2(clWaitForEvents);
	LOADPROC2(clGetProgramBuildInfo);
	LOADPROC2(clGetEventProfilingInfo);
	LOADPROC2(clEnqueueWriteBuffer);
	LOADPROC2(clGetDeviceIDs);
	LOADPROC2(clGetKernelWorkGroupInfo);
	return true;

error:
	deinitOVE();
	return false;
}