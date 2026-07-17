#pragma once
#include <Siv3D.hpp>

class CameraController
{
	public:
		explicit CameraController(const Size& sceneSize);

		void Update(const bool isMouseRUp, const bool isMouseRDown);

	private:
		Vec2 mCursorPos;

		double mVerticalFOV;
		Vec3 mEyePosition;
		Vec3 mFocusPosition;

		double mPhi;
		double mTheta;

		BasicCamera3D mCamera;

		bool mIsGrabbed;
};
