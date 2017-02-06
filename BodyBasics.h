//------------------------------------------------------------------------------
// <copyright file="BodyBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "resource.h"
#include "myBVH.h"
#include "myAngleFile.h"
#include "common.h"
#include "ImageRenderer.h"

class CBodyBasics
{
    
public:
    /// <summary>
    /// Constructor
    /// </summary>
    CBodyBasics();

    /// <summary>
    /// Destructor
    /// </summary>
    ~CBodyBasics();

    /// <summary>
    /// Handles window messages, passes most to the class instance to handle
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Handle windows messages for a class instance
    /// </summary>
    /// <param name="hWnd">window message is for</param>
    /// <param name="uMsg">message</param>
    /// <param name="wParam">message data</param>
    /// <param name="lParam">additional message data</param>
    /// <returns>result of message processing</returns>
    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /// <summary>
    /// Creates the main window and begins processing
    /// </summary>
    /// <param name="hInstance"></param>
    /// <param name="nCmdShow"></param>
    int                     Run(HINSTANCE hInstance, int nCmdShow);

private:
    HWND                    m_hWnd;
    INT64                   m_nStartTime;
    INT64                   m_nLastCounter;
    double                  m_fFreq;
    INT64                   m_nNextStatusTime;
    DWORD                   m_nFramesSinceUpdate;

    // Current Kinect
    IKinectSensor*          m_pKinectSensor;
    ICoordinateMapper*      m_pCoordinateMapper;
	DepthSpacePoint*        m_pDepthCoordinates;

	// Frame reader
	IMultiSourceFrameReader*m_pMultiSourceFrameReader;

    // Body reader
    IBodyFrameReader*       m_pBodyFrameReader;

	
    // Direct2D
	ImageRenderer*          m_pDrawCoordinateMapping;

	myBVH m_myBVH;
	myAngleFile m_myAngleFile;
    /// <summary>
    /// Main processing function
    /// </summary>
    void                    Update();

	///<summary>
	/// Do the mesh rendering
	/// </summary>
	void drawMeshRendering();

	
    /// <summary>
    /// Initializes the default Kinect sensor
    /// </summary>
    /// <returns>S_OK on success, otherwise failure code</returns>
    HRESULT                 InitializeDefaultSensor();
    
    /// <summary>
    /// Handle new body data
    /// <param name="nTime">timestamp of frame</param>
    /// <param name="nBodyCount">body data count</param>
    /// <param name="ppBodies">body data in frame</param>
    /// </summary>
    void                    ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);

    /// <summary>
    /// Set the status bar message
    /// </summary>
    /// <param name="szMessage">message to display</param>
    /// <param name="nShowTimeMsec">time in milliseconds for which to ignore future status messages</param>
    /// <param name="bForce">force status update</param>
    bool                    SetStatusMessage(_In_z_ WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);

	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	D2D1_POINT_2F           BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);


};

