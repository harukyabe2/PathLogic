#pragma once
#include <Siv3D.hpp>

class CameraController
{
	public:
		explicit CameraController(const Size& sceneSize);

		void Update(const bool isMouseRUp, const bool isMouseRDown);

		Ray GetMouseRay() const { return mCamera.screenToRay(Cursor::PosF()); }
		Ray GetUIRay(const Vec2& pos) const { return mCamera.screenToRay(pos); }

		void ApplyTransform() const;

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
