#pragma once

#include "Core.h"
#include "Renderer.h"
#include "ABCSketchManager.h"
#include "Mesh.h"
#include <map>
#include <string>
#include <conio.h>
#include <iostream>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HLU/hlu.h>
#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HDU/hduMath.h>
#include <HDU/hduVector.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduHapticDevice.h>

#define CURSOR_SIZE_PIXELS  15
#define DEVICE_UPDATE_PRIORITY          (HD_MAX_SCHEDULER_PRIORITY)
#define SYNCHRONIZE_STATE_PRIORITY      HD_DEFAULT_SCHEDULER_PRIORITY

namespace midl
{
	class ABCSketchManager;

	class HapticsEventManagerGeneric
	{
	protected:
		HapticsEventManagerGeneric() {}
		virtual ~HapticsEventManagerGeneric() {}

	public:
		static HapticsEventManagerGeneric *Initialize();
		static void Delete(HapticsEventManagerGeneric *&pInterface);

		virtual void Setup(ABCSketchManager *skManager) = 0;
		virtual void Cleanup() = 0;

		virtual void UpdateState() = 0;
		virtual void UpdateWorkspace() = 0;

		virtual void RenderStylus() = 0;
	};

	class HapticsEventManager : public HapticsEventManagerGeneric
	{
	private:
		static HDCallbackCode HDCALLBACK deviceUpdateCallback(void *pUserData);
		static HDCallbackCode HDCALLBACK setDeviceTransformCallback(void *pUserData);

		static void madeContactCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void lostContactCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void button1UpClickCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void button1DownClickCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void button2UpClickCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void button2DownClickCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		static void errorCallbackGT(IHapticDevice::EventType event,
			const IHapticDevice::IHapticDeviceState * const pState,
			void *pUserData);

		/* BEGIN: HAPTICS DEVICE VARIABLES */

		HHD m_hHD;
		HDSchedulerHandle m_hUpdateCallback;
		IHapticDevice *m_pHapticDeviceHT;
		IHapticDevice *m_pHapticDeviceGT;

		hduMatrix m_workspaceXform;
		HDdouble m_cursorScale;
		hduVector3Dd m_cameraPosWC;
		HLdouble proxyPosition[3];
		
		GLuint m_nCursorDisplayList;

		/* BEGIN: SKETCH RENDERING VARIABLES */
		ABCSketchManager *abcSketchManager;
		Mesh *meshManager;

		//-- Shaders
		Shader colorShader;
		bool isForceOn;

	public:
		HapticsEventManager();
		~HapticsEventManager();

		void Setup(ABCSketchManager *skManager);
		//void GetMappedCordinates(float *mappedC);
		void Cleanup();
		void UpdateState();
		void UpdateWorkspace();
		void RenderStylus();
	};
}
