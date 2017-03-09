#pragma once
#include "common.h"

class myFilterPosition {
public:
	struct TRANSFORM_SMOOTH_PARAMETERS
	{
		float   fSmoothing;             // [0..1], lower values closer to raw data
		float   fCorrection;            // [0..1], lower values slower to correct towards the raw data
		float   fPrediction;            // [0..n], the number of frames to predict into the future
		float   fJitterRadius;          // The radius in meters for jitter reduction
		float   fMaxDeviationRadius;    // The maximum radius in meters that filtered positions are allowed to deviate from raw data
	};

	class FilterDoubleExponentialData
	{
	public:
		CameraSpacePoint m_vRawPosition;
		CameraSpacePoint m_vFilteredPosition;
		CameraSpacePoint m_vTrend;
		int    m_dwFrameCount;
	};

	// Holt Double Exponential Smoothing filter
	CameraSpacePoint m_pFilteredJoints[JointType_Count];
	FilterDoubleExponentialData m_pHistory[JointType_Count];
	float m_fSmoothing;
	float m_fCorrection;
	float m_fPrediction;
	float m_fJitterRadius;
	float m_fMaxDeviationRadius;

	myFilterPosition();

	~myFilterPosition();

	void Init(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f);

	void Shutdown();

	void Reset(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f);

	//--------------------------------------------------------------------------------------
	// Implementation of a Holt Double Exponential Smoothing filter. The double exponential
	// smooths the curve and predicts.  There is also noise jitter removal. And maximum
	// prediction bounds.  The paramaters are commented in the init function.
	//--------------------------------------------------------------------------------------
	void UpdateFilter(IBody* pBody);

	//--------------------------------------------------------------------------------------
	// if joint is 0 it is not valid.
	//--------------------------------------------------------------------------------------
	bool JointPositionIsValid(CameraSpacePoint vJointPosition);

	CameraSpacePoint* GetFilteredJoints();

	CameraSpacePoint CSVectorSet(float x, float y, float z);

	CameraSpacePoint CSVectorZero();

	CameraSpacePoint CSVectorAdd(CameraSpacePoint p1, CameraSpacePoint p2);

	CameraSpacePoint CSVectorScale(CameraSpacePoint p, float scale);

	CameraSpacePoint CSVectorSubtract(CameraSpacePoint p1, CameraSpacePoint p2);

	float CSVectorLength(CameraSpacePoint p);

	void UpdateJoint(Joint* joints, JointType jt, TRANSFORM_SMOOTH_PARAMETERS smoothingParams);

};
