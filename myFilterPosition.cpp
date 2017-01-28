#include "myFilterPosition.h"

myFilterPosition::myFilterPosition() {
	
	for (int i = 0; i < JointType_Count; i++)
	{
		m_pHistory[i] = FilterDoubleExponentialData();
	}

	Init();
}

myFilterPosition::~myFilterPosition()
{
	Shutdown();
}

void myFilterPosition::Init(float fSmoothing, float fCorrection, float fPrediction, float fJitterRadius, float fMaxDeviationRadius)
{
	Reset(fSmoothing, fCorrection, fPrediction, fJitterRadius, fMaxDeviationRadius);
}

void myFilterPosition::Shutdown()
{
}

void myFilterPosition::Reset(float fSmoothing, float fCorrection, float fPrediction, float fJitterRadius, float fMaxDeviationRadius)
{
	
	m_fMaxDeviationRadius = fMaxDeviationRadius; // Size of the max prediction radius Can snap back to noisy data when too high
	m_fSmoothing = fSmoothing;                   // How much smothing will occur.  Will lag when too high
	m_fCorrection = fCorrection;                 // How much to correct back from prediction.  Can make things springy
	m_fPrediction = fPrediction;                 // Amount of prediction into the future to use. Can over shoot when too high
	m_fJitterRadius = fJitterRadius;             // Size of the radius where jitter is removed. Can do too much smoothing when too high

	for (int i = 0; i < JointType_Count; i++)
	{
		m_pFilteredJoints[i].X = 0.0f;
		m_pFilteredJoints[i].Y = 0.0f;
		m_pFilteredJoints[i].Z = 0.0f;

		m_pHistory[i].m_vFilteredPosition.X = 0.0f;
		m_pHistory[i].m_vFilteredPosition.Y = 0.0f;
		m_pHistory[i].m_vFilteredPosition.Z = 0.0f;

		m_pHistory[i].m_vRawPosition.X = 0.0f;
		m_pHistory[i].m_vRawPosition.Y = 0.0f;
		m_pHistory[i].m_vRawPosition.Z = 0.0f;

		m_pHistory[i].m_vTrend.X = 0.0f;
		m_pHistory[i].m_vTrend.Y = 0.0f;
		m_pHistory[i].m_vTrend.Z = 0.0f;

		m_pHistory[i].m_dwFrameCount = 0;
	}
}

void myFilterPosition::UpdateFilter(IBody* pBody)
{
	if (pBody == NULL)
	{
		return;
	}

	// Check for divide by zero. Use an epsilon of a 10th of a millimeter
	m_fJitterRadius = max(0.0001f, m_fJitterRadius);

	TRANSFORM_SMOOTH_PARAMETERS SmoothingParams;

	Joint joints[JointType_Count];

	HRESULT hr = pBody->GetJoints(_countof(joints), joints);

	for (JointType jt = JointType_SpineBase; jt <= JointType_ThumbRight; jt=(JointType)(jt+1))
	{
		SmoothingParams.fSmoothing = m_fSmoothing;
		SmoothingParams.fCorrection = m_fCorrection;
		SmoothingParams.fPrediction = m_fPrediction;
		SmoothingParams.fJitterRadius = m_fJitterRadius;
		SmoothingParams.fMaxDeviationRadius = m_fMaxDeviationRadius;

		// If inferred, we smooth a bit more by using a bigger jitter radius
		Joint joint = joints[(int)jt];
		if (joint.TrackingState == TrackingState_Inferred)
		{
			SmoothingParams.fJitterRadius *= 2.0f;
			SmoothingParams.fMaxDeviationRadius *= 2.0f;
		}

		UpdateJoint(joints, jt, SmoothingParams);
	}
}

bool myFilterPosition::JointPositionIsValid(CameraSpacePoint vJointPosition)
{
	return (vJointPosition.X != 0.0f ||
		vJointPosition.Y != 0.0f ||
		vJointPosition.Z != 0.0f);
}

CameraSpacePoint* myFilterPosition::GetFilteredJoints()
{
	return m_pFilteredJoints;
}

CameraSpacePoint myFilterPosition::CSVectorSet(float x, float y, float z)
{
	CameraSpacePoint point = CameraSpacePoint();

	point.X = x;
	point.Y = y;
	point.Z = z;

	return point;
}

CameraSpacePoint myFilterPosition::CSVectorZero()
{
	CameraSpacePoint point = CameraSpacePoint();

	point.X = 0.0f;
	point.Y = 0.0f;
	point.Z = 0.0f;

	return point;
}

CameraSpacePoint myFilterPosition::CSVectorAdd(CameraSpacePoint p1, CameraSpacePoint p2)
{
	CameraSpacePoint sum = CameraSpacePoint();

	sum.X = p1.X + p2.X;
	sum.Y = p1.Y + p2.Y;
	sum.Z = p1.Z + p2.Z;

	return sum;
}

CameraSpacePoint myFilterPosition::CSVectorScale(CameraSpacePoint p, float scale)
{
	CameraSpacePoint point = CameraSpacePoint();

	point.X = p.X * scale;
	point.Y = p.Y * scale;
	point.Z = p.Z * scale;

	return point;
}

CameraSpacePoint myFilterPosition::CSVectorSubtract(CameraSpacePoint p1, CameraSpacePoint p2)
{
	CameraSpacePoint diff = CameraSpacePoint();

	diff.X = p1.X - p2.X;
	diff.Y = p1.Y - p2.Y;
	diff.Z = p1.Z - p2.Z;

	return diff;
}

float myFilterPosition::CSVectorLength(CameraSpacePoint p)
{
	return sqrt(p.X * p.X + p.Y * p.Y + p.Z * p.Z);
}

void myFilterPosition::UpdateJoint(Joint* joints, JointType jt, TRANSFORM_SMOOTH_PARAMETERS smoothingParams)
{
	CameraSpacePoint vPrevRawPosition;
	CameraSpacePoint vPrevFilteredPosition;
	CameraSpacePoint vPrevTrend;
	CameraSpacePoint vRawPosition;
	CameraSpacePoint vFilteredPosition;
	CameraSpacePoint vPredictedPosition;
	CameraSpacePoint vDiff;
	CameraSpacePoint vTrend;
	float fDiff;
	bool bJointIsValid;

	Joint joint = joints[(int)jt];

	vRawPosition = joint.Position;
	vPrevFilteredPosition = m_pHistory[(int)jt].m_vFilteredPosition;
	vPrevTrend = m_pHistory[(int)jt].m_vTrend;
	vPrevRawPosition = m_pHistory[(int)jt].m_vRawPosition;
	bJointIsValid = JointPositionIsValid(vRawPosition);

	// If joint is invalid, reset the filter
	if (!bJointIsValid)
	{
		m_pHistory[(int)jt].m_dwFrameCount = 0;
	}

	// Initial start values
	if (m_pHistory[(int)jt].m_dwFrameCount == 0)
	{
		vFilteredPosition = vRawPosition;
		vTrend = CSVectorZero();
		m_pHistory[(int)jt].m_dwFrameCount++;
	}
	else if (m_pHistory[(int)jt].m_dwFrameCount == 1)
	{
		vFilteredPosition = CSVectorScale(CSVectorAdd(vRawPosition, vPrevRawPosition), 0.5f);
		vDiff = CSVectorSubtract(vFilteredPosition, vPrevFilteredPosition);
		vTrend = CSVectorAdd(CSVectorScale(vDiff, smoothingParams.fCorrection), CSVectorScale(vPrevTrend, 1.0f - smoothingParams.fCorrection));
		m_pHistory[(int)jt].m_dwFrameCount++;
	}
	else
	{
		// First apply jitter filter
		vDiff = CSVectorSubtract(vRawPosition, vPrevFilteredPosition);
		fDiff = CSVectorLength(vDiff);

		if (fDiff <= smoothingParams.fJitterRadius)
		{
			vFilteredPosition = CSVectorAdd(CSVectorScale(vRawPosition, fDiff / smoothingParams.fJitterRadius),
				CSVectorScale(vPrevFilteredPosition, 1.0f - fDiff / smoothingParams.fJitterRadius));
		}
		else
		{
			vFilteredPosition = vRawPosition;
		}

		// Now the double exponential smoothing filter
		vFilteredPosition = CSVectorAdd(CSVectorScale(vFilteredPosition, 1.0f - smoothingParams.fSmoothing),
			CSVectorScale(CSVectorAdd(vPrevFilteredPosition, vPrevTrend), smoothingParams.fSmoothing));


		vDiff = CSVectorSubtract(vFilteredPosition, vPrevFilteredPosition);
		vTrend = CSVectorAdd(CSVectorScale(vDiff, smoothingParams.fCorrection), CSVectorScale(vPrevTrend, 1.0f - smoothingParams.fCorrection));
	}

	// Predict into the future to reduce latency
	vPredictedPosition = CSVectorAdd(vFilteredPosition, CSVectorScale(vTrend, smoothingParams.fPrediction));

	// Check that we are not too far away from raw data
	vDiff = CSVectorSubtract(vPredictedPosition, vRawPosition);
	fDiff = CSVectorLength(vDiff);

	if (fDiff > smoothingParams.fMaxDeviationRadius)
	{
		vPredictedPosition = CSVectorAdd(CSVectorScale(vPredictedPosition, smoothingParams.fMaxDeviationRadius / fDiff),
			CSVectorScale(vRawPosition, 1.0f - smoothingParams.fMaxDeviationRadius / fDiff));
	}

	// Save the data from this frame
	m_pHistory[(int)jt].m_vRawPosition = vRawPosition;
	m_pHistory[(int)jt].m_vFilteredPosition = vFilteredPosition;
	m_pHistory[(int)jt].m_vTrend = vTrend;

	// Output the data
	m_pFilteredJoints[(int)jt] = vPredictedPosition;
}


