// adapted from https://social.msdn.microsoft.com/Forums/en-US/850b61ce-a1f4-4e05-a0c9-b0c208276bec/joint-smoothing-code-for-c?forum=kinectv2sdk

#pragma once
#include "common.h"

class KinectJointFilter
{
public:
	//struct TRANSFORM_SMOOTH_PARAMETERS
	//{
	//	float   fSmoothing;             // [0..1], lower values closer to raw data
	//	float   fCorrection;            // [0..1], lower values slower to correct towards the raw data
	//	float   fPrediction;            // [0..n], the number of frames to predict into the future
	//	float   fJitterRadius;          // The radius in meters for jitter reduction
	//	float   fMaxDeviationRadius;    // The maximum radius in meters that filtered positions are allowed to deviate from raw data
	//};

	class FilterDoubleExponentialData
	{
	public:
		Vector4 m_vRawPosition;
		Vector4 m_vFilteredPosition;
		//Vector4 m_vTrend;
		int  m_dwFrameCount;
		bool wasFiltered;
	};

	// Holt Double Exponential Smoothing filter
	Vector4 m_pFilteredJoints[JointType_Count];
	FilterDoubleExponentialData m_pHistory[JointType_Count];
	//float m_fSmoothing;
	/*float m_fCorrection;
	float m_fPrediction;
	float m_fJitterRadius;
	float m_fMaxDeviationRadius;*/

	KinectJointFilter();

	~KinectJointFilter();

	void Init();
	//void Init(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f);

	void Shutdown();

	//void Reset(float fSmoothing = 0.25f, float fCorrection = 0.25f, float fPrediction = 0.25f, float fJitterRadius = 0.03f, float fMaxDeviationRadius = 0.05f);
	void Reset();
	//--------------------------------------------------------------------------------------
	// Implementation of a Holt Double Exponential Smoothing filter. The double exponential
	// smooths the curve and predicts.  There is also noise jitter removal. And maximum
	// prediction bounds.  The paramaters are commented in the init function.
	//--------------------------------------------------------------------------------------
	void UpdateFilter(IBody* pBody);

	//--------------------------------------------------------------------------------------
	// if joint is 0 it is not valid.
	//--------------------------------------------------------------------------------------
	bool JointPositionIsValid(Vector4 vJointPosition);

	Vector4* GetFilteredJoints();

	Vector4 CSVectorSet(float w, float x, float y, float z);

	Vector4 CSVectorZero();

	Vector4 CSVectorAdd(Vector4 p1, Vector4 p2);

	Vector4 CSVectorScale(Vector4 p, float scale);

	Vector4 CSVectorSubtract(Vector4 p1, Vector4 p2);

	double CSVectorScalarProduct(Vector4 p1, Vector4 p2); 

	float CSVectorLength(Vector4 p);

	//void UpdateJoint(IBody* pBody, JointType jt, TRANSFORM_SMOOTH_PARAMETERS smoothingParams);
	void UpdateJoint(IBody* pBody, JointType jt);
};

