//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the drawing of image data

#pragma once

#include <d2d1.h>
#include "common.h"

class ImageRenderer
{
private:
	HWND                     m_hWnd;

	// Format information
	UINT                     m_sourceHeight;
	UINT                     m_sourceWidth;
	LONG                     m_sourceStride;

	// Direct2D 
	ID2D1Factory*            m_pD2DFactory;
	ID2D1Bitmap*             m_pBitmap;
	RGBQUAD*                m_pOutputRGBX;
	ID2D1SolidColorBrush*   m_pBrushJointTracked;
	ID2D1SolidColorBrush*   m_pBrushJointInferred;
	ID2D1SolidColorBrush*   m_pBrushBoneTracked[3];
	ID2D1SolidColorBrush*   m_pBrushBoneInferred[3];
	ID2D1SolidColorBrush*   m_pBrushHandClosed;
	ID2D1SolidColorBrush*   m_pBrushHandOpen;
	ID2D1SolidColorBrush*   m_pBrushHandLasso;

public:
	ID2D1HwndRenderTarget*   m_pRenderTarget;
    /// <summary>
    /// Constructor
    /// </summary>
    ImageRenderer();

    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~ImageRenderer();

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
    HRESULT Initialize(HWND hwnd, int sourceWidth, int sourceHeight, int sourceStride);

    /// <summary>
    /// Draws a 32 bit per pixel image of previously specified width, height, and stride to the associated hwnd
    /// </summary>
    /// <param name="pImage">image data in RGBX format</param>
    /// <param name="cbImage">size of image data in bytes</param>
    /// <returns>indicates success or failure</returns>
    HRESULT Draw(BYTE* pImage, unsigned long cbImage);

    /// <summary>
    /// Ensure necessary Direct2d resources are created
    /// </summary>
    /// <returns>indicates success or failure</returns>
    HRESULT EnsureResources();

    /// <summary>
    /// Dispose of Direct2d resources 
    /// </summary>
    void DiscardResources();


	
	/// <summary>
	/// Draws a body 
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	void                    DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);

	/// <summary>
	/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
	/// </summary>
	/// <param name="handState">state of the hand</param>
	/// <param name="handPosition">position of the hand</param>
	void                    DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);

	/// <summary>
	/// Draws one bone of a body (joint to joint)
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="joint0">one joint of the bone to draw</param>
	/// <param name="joint1">other joint of the bone to draw</param>
	void                    DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

	/// <summary>
	/// Handle new depth and color data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="pDepthBuffer">pointer to depth frame data</param>
	/// <param name="nDepthWidth">width (in pixels) of input depth image data</param>
	/// <param name="nDepthHeight">height (in pixels) of input depth image data</param>
	/// <param name="pColorBuffer">pointer to color frame data</param>
	/// <param name="nColorWidth">width (in pixels) of input color image data</param>
	/// <param name="nColorHeight">height (in pixels) of input color image data</param>
	/// <param name="pBodyIndexBuffer">pointer to body index frame data</param>
	/// <param name="nBodyIndexWidth">width (in pixels) of input body index data</param>
	/// <param name="nBodyIndexHeight">height (in pixels) of input body index data</param>
	/// </summary>
	void                    ProcessFrame(INT64 nTime,
		const UINT16* pDepthBuffer, int nDepthHeight, int nDepthWidth,
		int nColorWidth, int nColorHeight,
		const BYTE* pBodyIndexBuffer, int nBodyIndexWidth, int nBodyIndexHeight,DepthSpacePoint* depthPoints);

};