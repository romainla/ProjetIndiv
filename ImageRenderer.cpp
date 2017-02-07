//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageRenderer.h"

static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 30.0f;


/// <summary>
/// Constructor
/// </summary>
ImageRenderer::ImageRenderer() : 
    m_hWnd(0),
    m_sourceWidth(0),
    m_sourceHeight(0),
    m_sourceStride(0),
    m_pD2DFactory(NULL), 
    m_pRenderTarget(NULL),
	m_pBrushJointTracked(NULL),
	m_pBrushJointInferred(NULL),
	m_pBrushBoneTracked{ NULL, NULL, NULL },
	m_pBrushBoneInferred{ NULL, NULL, NULL },
	m_pBrushHandClosed(NULL),
	m_pBrushHandOpen(NULL),
	m_pBrushHandLasso(NULL),
    m_pBitmap(0)
{
	// create heap storage for composite image pixel data in RGBX format
	m_pOutputRGBX = new RGBQUAD[cColorWidth * cColorHeight];
}

/// <summary>
/// Destructor
/// </summary>
ImageRenderer::~ImageRenderer()
{
    DiscardResources();
    SafeRelease(m_pD2DFactory);

	if (m_pOutputRGBX)
	{
		delete[] m_pOutputRGBX;
		m_pOutputRGBX = NULL;
	}
}

/// <summary>
/// Ensure necessary Direct2d resources are created
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT ImageRenderer::EnsureResources()
{
    HRESULT hr = S_OK;

    if (NULL == m_pRenderTarget)
    {
        D2D1_SIZE_U size = D2D1::SizeU(m_sourceWidth, m_sourceHeight);

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a hWnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(m_hWnd, size),
            &m_pRenderTarget
        );

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a bitmap that we can copy image data into and then render to the target
        hr = m_pRenderTarget->CreateBitmap(
            size, 
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
            &m_pBitmap 
        );


        if (FAILED(hr))
        {
            SafeRelease(m_pRenderTarget);
            return hr;
        }

		// light green
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);

		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);

		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen, 1.0f), &(m_pBrushBoneTracked[(int)phaseExercise::INITIALIZATION]));
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &(m_pBrushBoneTracked[(int)phaseExercise::ACTIVE]));
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGreen, 1.0f), &(m_pBrushBoneTracked[(int)phaseExercise::POST_EXERCISE]));

		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray, 1.0f), &(m_pBrushBoneInferred[(int)phaseExercise::INITIALIZATION]));
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &(m_pBrushBoneInferred[(int)phaseExercise::ACTIVE]));
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray, 1.0f), &(m_pBrushBoneInferred[(int)phaseExercise::POST_EXERCISE]));

		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);
    }

    return hr;
}

/// <summary>
/// Dispose of Direct2d resources 
/// </summary>
void ImageRenderer::DiscardResources()
{
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pBitmap);

	SafeRelease(m_pBrushJointTracked);
	SafeRelease(m_pBrushJointInferred);
	SafeRelease(m_pBrushBoneTracked[0]);
	SafeRelease(m_pBrushBoneTracked[1]);
	SafeRelease(m_pBrushBoneTracked[2]);
	SafeRelease(m_pBrushBoneInferred[0]);
	SafeRelease(m_pBrushBoneInferred[1]);
	SafeRelease(m_pBrushBoneInferred[2]);

	SafeRelease(m_pBrushHandClosed);
	SafeRelease(m_pBrushHandOpen);
	SafeRelease(m_pBrushHandLasso);
}

/// <summary>
/// Set the window to draw to as well as the video format
/// Implied bits per pixel is 32
/// </summary>
/// <param name="hWnd">window to draw to</param>
/// <param name="pD2DFactory">already created D2D factory object</param>
/// <param name="sourceWidth">width (in pixels) of image data to be drawn</param>
/// <param name="sourceHeight">height (in pixels) of image data to be drawn</param>
/// <param name="sourceStride">length (in bytes) of a single scanline</param>
/// <returns>indicates success or failure</returns>
HRESULT ImageRenderer::Initialize(HWND hWnd, int sourceWidth, int sourceHeight, int sourceStride)
{
    m_hWnd = hWnd;

	// Init Direct2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    m_pD2DFactory->AddRef();

    // Get the frame size
    m_sourceWidth  = sourceWidth;
    m_sourceHeight = sourceHeight;
    m_sourceStride = sourceStride;

    return S_OK;
}

/// <summary>
/// Draws a 32 bit per pixel image of previously specified width, height, and stride to the associated hwnd
/// </summary>
/// <param name="pImage">image data in RGBX format</param>
/// <param name="cbImage">size of image data in bytes</param>
/// <returns>indicates success or failure</returns>
HRESULT ImageRenderer::Draw(BYTE* pImage, unsigned long cbImage)
{
    // incorrectly sized image data passed in
    if (cbImage < ((m_sourceHeight - 1) * m_sourceStride) + (m_sourceWidth * 4))
    {
        return E_INVALIDARG;
    }

    // create the resources for this draw device
    // they will be recreated if previously lost
    HRESULT hr = EnsureResources();

    if (FAILED(hr))
    {
        return hr;
    }
    
    // Copy the image that was passed in into the direct2d bitmap
    hr = m_pBitmap->CopyFromMemory(NULL, pImage, m_sourceStride);

    if (FAILED(hr))
    {
        return hr;
    }
       
    m_pRenderTarget->BeginDraw();

    // Draw the bitmap stretched to the size of the window
    m_pRenderTarget->DrawBitmap(m_pBitmap);
            
    hr = m_pRenderTarget->EndDraw();

    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardResources();
    }

    return hr;
}

/// <summary>


/// <summary>
/// Draws a body 
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
void ImageRenderer::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints)
{
	// Draw the bones

	// Torso
	DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck);
	DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid);
	DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft);

	// Right Arm    
	DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight);
	DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight);
	DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight);

	// Left Arm
	DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft);
	DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft);
	DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft);

	// Right Leg
	DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight);
	DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight);
	DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight);

	// Left Leg
	DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft);
	DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft);
	DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft);

	// Draw the joints
	for (int i = 0; i < JointType_Count; ++i)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], c_JointThickness, c_JointThickness);

		if (pJoints[i].TrackingState == TrackingState_Inferred)
		{
			m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
		}
		else if (pJoints[i].TrackingState == TrackingState_Tracked)
		{
			m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
		}
	}
}

/// <summary>
/// Draws one bone of a body (joint to joint)
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="joint0">one joint of the bone to draw</param>
/// <param name="joint1">other joint of the bone to draw</param>
void ImageRenderer::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneTracked[(int)currentPhase], c_TrackedBoneThickness);
	}
	else
	{
		m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred[(int)currentPhase], c_InferredBoneThickness);
	}
}

/// <summary>
/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
/// </summary>
/// <param name="handState">state of the hand</param>
/// <param name="handPosition">position of the hand</param>
void ImageRenderer::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)
{
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);

	switch (handState)
	{
	case HandState_Closed:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);
		break;

	case HandState_Open:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
		break;

	case HandState_Lasso:
		m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
		break;
	}
}


void ImageRenderer::ProcessFrame(INT64 nTime, const UINT16 * pDepthBuffer, int nDepthHeight, int nDepthWidth, int nColorWidth, int nColorHeight, const BYTE * pBodyIndexBuffer, int nBodyIndexWidth, int nBodyIndexHeight, DepthSpacePoint* depthPoints)
{
	const RGBQUAD c_black = { 0, 0, 0 };
	const RGBQUAD c_blue = { 255, 0, 0 };
	// Make sure we've received valid data
	if (m_pOutputRGBX &&
		pDepthBuffer && (nDepthWidth == cDepthWidth) && (nDepthHeight == cDepthHeight) &&
		(nColorWidth == cColorWidth) && (nColorHeight == cColorHeight) &&
		pBodyIndexBuffer && (nBodyIndexWidth == cDepthWidth) && (nBodyIndexHeight == cDepthHeight))
	{


		// loop over output pixels
		for (int colorIndex = 0; colorIndex < (nColorWidth*nColorHeight); ++colorIndex)
		{
			// default setting source to copy from the background pixel
			RGBQUAD pSrc = c_black;

			DepthSpacePoint p = depthPoints[colorIndex];

			// Values that are negative infinity means it is an invalid color to depth mapping so we
			// skip processing for this pixel
			if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
			{
				int depthX = static_cast<int>(p.X + 0.5f);
				int depthY = static_cast<int>(p.Y + 0.5f);

				if ((depthX >= 0 && depthX < nDepthWidth) && (depthY >= 0 && depthY < nDepthHeight))
				{
					BYTE player = pBodyIndexBuffer[depthX + (depthY * cDepthWidth)];

					// if we're tracking a player for the current pixel, draw from the color camera
					if (player != 0xff)
					{
						// set source for copy to the color pixel
						pSrc = c_blue;
					}
				}
			}

			// write output
			m_pOutputRGBX[colorIndex] = pSrc;
		}

		// Draw the data with Direct2D
		Draw(reinterpret_cast<BYTE*>(m_pOutputRGBX), cColorWidth * cColorHeight * sizeof(RGBQUAD));

	}

}

