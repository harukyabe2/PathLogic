#include "CameraController.hpp"

CameraController::CameraController(const Size& sceneSize)
: mCursorPos(Scene::Center())
, mVerticalFOV(20_deg)
, mEyePosition({ 64, 8, 64 })
, mFocusPosition({ 0, 0, 0 })
, mPhi(-20_deg)
, mTheta(60_deg)
, mCamera(sceneSize, mVerticalFOV, mEyePosition, mFocusPosition)
, mIsGrabbed(false)
{

};

void CameraController::Update(const bool isMouseRUp, const bool isMouseRDown)
{
	if (isMouseRUp) mIsGrabbed = false;

	if (mIsGrabbed)
	{
		const double before = (mCursorPos - Scene::Center()).getAngle();
		const double after = (Cursor::Pos() - Scene::Center()).getAngle();
		mPhi -= (after - before);
		mCursorPos = Cursor::Pos();
	}

	if ((!mIsGrabbed) && isMouseRDown)
	{
		mIsGrabbed = true;
		mCursorPos = Cursor::Pos();
	}

	mEyePosition = Spherical{ 40, mTheta, (270_deg - mPhi) };
	mCamera.setView(mEyePosition, mFocusPosition);
	Graphics3D::SetCameraTransform(mCamera);
}
