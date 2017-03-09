#include "myFilterOrientation.h"

KinectJointFilter::KinectJointFilter() {
	
	for (int i = 0; i < JointType_Count; i++)
	{
		m_pHistory[i] = FilterDoubleExponentialData();
	}

	Init();
}

KinectJointFilter::~KinectJointFilter()
{
	Shutdown();
}

//void KinectJointFilter::Init(float fSmoothing, float fCorrection, float fPrediction, float fJitterRadius, float fMaxDeviationRadius)
void KinectJointFilter::Init()
{
	Reset();
	//Reset(fSmoothing, fCorrection, fPrediction, fJitterRadius, fMaxDeviationRadius);
}

void KinectJointFilter::Shutdown()
{
}

//void KinectJointFilter::Reset(float fSmoothing, float fCorrection, float fPrediction, float fJitterRadius, float fMaxDeviationRadius)
void KinectJointFilter::Reset()
{
	
	//m_fMaxDeviationRadius = fMaxDeviationRadius; // Size of the max prediction radius Can snap back to noisy data when too high
	//m_fSmoothing = fSmoothing;                   // How much smothing will occur.  Will lag when too high
	//m_fCorrection = fCorrection;                 // How much to correct back from prediction.  Can make things springy
	//m_fPrediction = fPrediction;                 // Amount of prediction into the future to use. Can over shoot when too high
	//m_fJitterRadius = fJitterRadius;             // Size of the radius where jitter is removed. Can do too much smoothing when too high

	for (int i = 0; i < JointType_Count; i++)
	{
		m_pFilteredJoints[i].w = 0.0f;
		m_pFilteredJoints[i].x = 0.0f;
		m_pFilteredJoints[i].y = 0.0f;
		m_pFilteredJoints[i].z = 0.0f;

		m_pHistory[i].m_vFilteredPosition.w = 0.0f;
		m_pHistory[i].m_vFilteredPosition.x = 0.0f;
		m_pHistory[i].m_vFilteredPosition.y = 0.0f;
		m_pHistory[i].m_vFilteredPosition.z = 0.0f;

		m_pHistory[i].m_vRawPosition.w = 0.0f;
		m_pHistory[i].m_vRawPosition.x = 0.0f;
		m_pHistory[i].m_vRawPosition.y = 0.0f;
		m_pHistory[i].m_vRawPosition.z = 0.0f;

		//m_pHistory[i].m_vTrend.w = 0.0f;
		//m_pHistory[i].m_vTrend.x = 0.0f;
		//m_pHistory[i].m_vTrend.y = 0.0f;
		//m_pHistory[i].m_vTrend.z = 0.0f;

		m_pHistory[i].m_dwFrameCount = 0;
		m_pHistory[i].wasFiltered = true;
	}
}

void KinectJointFilter::UpdateFilter(IBody* pBody)
{
	if (pBody == NULL)
	{
		return;
	}

	// Check for divide by zero. Use an epsilon of a 10th of a millimeter
	//m_fJitterRadius = max(0.0001f, m_fJitterRadius);

	//TRANSFORM_SMOOTH_PARAMETERS SmoothingParams;

	Joint joints[JointType_Count];

	HRESULT hr = pBody->GetJoints(_countof(joints), joints);

	for (JointType jt = JointType_SpineBase; jt <= JointType_ThumbRight; jt=(JointType)(jt+1))
	{
		/*SmoothingParams.fSmoothing = m_fSmoothing;
		SmoothingParams.fCorrection = m_fCorrection;
		SmoothingParams.fPrediction = m_fPrediction;
		SmoothingParams.fJitterRadius = m_fJitterRadius;
		SmoothingParams.fMaxDeviationRadius = m_fMaxDeviationRadius;*/

		// If inferred, we smooth a bit more by using a bigger jitter radius
		Joint joint = joints[(int)jt];
		/*if (joint.TrackingState == TrackingState_Inferred)
		{
			SmoothingParams.fJitterRadius *= 2.0f;
			SmoothingParams.fMaxDeviationRadius *= 2.0f;
		}*/
		UpdateJoint(pBody, jt);
		//UpdateJoint(pBody, jt, SmoothingParams);
	}
}

bool KinectJointFilter::JointPositionIsValid(Vector4 vJointPosition)
{
	return (vJointPosition.w != 0.0f ||
		vJointPosition.x != 0.0f ||
		vJointPosition.y != 0.0f ||
		vJointPosition.z != 0.0f);
}

Vector4* KinectJointFilter::GetFilteredJoints()
{
	return m_pFilteredJoints;
}

Vector4 KinectJointFilter::CSVectorSet(float w, float x, float y, float z)
{
	Vector4 point = Vector4();
	point.w = w;
	point.x = x;
	point.y = y;
	point.z = z;

	return point;
}

Vector4 KinectJointFilter::CSVectorZero()
{
	Vector4 point = Vector4();
	point.w = 0.0f;
	point.x = 0.0f;
	point.y = 0.0f;
	point.z = 0.0f;

	return point;
}

Vector4 KinectJointFilter::CSVectorAdd(Vector4 p1, Vector4 p2)
{
	Vector4 sum = Vector4();
	sum.w = p1.w + p2.w;
	sum.x = p1.x + p2.x;
	sum.y = p1.y + p2.y;
	sum.z = p1.z + p2.z;

	return sum;
}

Vector4 KinectJointFilter::CSVectorScale(Vector4 p, float scale)
{
	Vector4 point = Vector4();
	point.w = p.w * scale;
	point.x = p.x * scale;
	point.y = p.y * scale;
	point.z = p.z * scale;

	return point;
}

Vector4 KinectJointFilter::CSVectorSubtract(Vector4 p1, Vector4 p2)
{
	Vector4 diff = Vector4();
	diff.w = p1.w - p2.w;
	diff.x = p1.x - p2.x;
	diff.y = p1.y - p2.y;
	diff.z = p1.z - p2.z;

	return diff;
}

float KinectJointFilter::CSVectorLength(Vector4 p)
{
	return sqrt(p.w *p.w + p.x * p.x + p.y * p.y + p.z * p.z);
}

double KinectJointFilter::CSVectorScalarProduct(Vector4 p1, Vector4 p2) {
	double result = p1.w*p2.w + p1.x*p2.x + p1.y* p2.y + p1.z * p2.z;
	return result;
}

//void KinectJointFilter::UpdateJoint(IBody* pBody, JointType jt, TRANSFORM_SMOOTH_PARAMETERS smoothingParams)
void KinectJointFilter::UpdateJoint(IBody* pBody, JointType jt)
{
	Vector4 vPrevRawPosition;
	Vector4 vPrevFilteredPosition;
	Vector4 vRawPosition;
	Vector4 vFilteredPosition;
	
	//Vector4 vPredictedPosition;
	//Vector4 vPrevTrend;
	//Vector4 vDiff;
	//Vector4 vTrend;
	//float fDiff;
	//bool bJointIsValid;
	
	Joint joints[JointType_Count];
	HRESULT hr = pBody->GetJoints(_countof(joints), joints);

	Joint joint = joints[(int)jt];

	JointOrientation jointsOrientation[JointType_Count];
	pBody->GetJointOrientations(JointType_Count, jointsOrientation);

	vRawPosition = jointsOrientation[(int)jt].Orientation;
	vPrevFilteredPosition = m_pHistory[(int)jt].m_vFilteredPosition;
	//vPrevTrend = m_pHistory[(int)jt].m_vTrend;
	vPrevRawPosition = m_pHistory[(int)jt].m_vRawPosition;
	//bJointIsValid = JointPositionIsValid(vRawPosition);

	//// If joint is invalid, reset the filter
	//if (!bJointIsValid)
	//{
	//	m_pHistory[(int)jt].m_dwFrameCount = 0;
	//}

	// Initial start values
	if (m_pHistory[(int)jt].m_dwFrameCount == 0)
	{
		vFilteredPosition = vRawPosition;
		//vTrend = CSVectorZero();
		m_pHistory[(int)jt].m_dwFrameCount++;
	}
	else //if (m_pHistory[(int)jt].m_dwFrameCount == 1)
	{
		Vector4 prevCorrectQuaternion;
		if (m_pHistory[(int)jt].wasFiltered) {
			prevCorrectQuaternion = vPrevFilteredPosition;
		}
		else {
			prevCorrectQuaternion = vPrevRawPosition;
		}
		double cosTheta = CSVectorScalarProduct(vRawPosition, prevCorrectQuaternion);
		cosTheta = cosTheta / (CSVectorLength(vRawPosition)*CSVectorLength(prevCorrectQuaternion));
		double theta = acos(cosTheta);
		double scale;
		if ((double)(theta/M_PI - (int)theta/M_PI) == 0.0) { //theta is a multiple of M_PI -> sin(theta)=0
			scale = 1 / 2;
		}
		else {
			scale = sin(theta / 2) / sin(theta);
		}
		vFilteredPosition = CSVectorAdd(CSVectorScale(vRawPosition, scale), CSVectorScale(prevCorrectQuaternion, scale));
		vFilteredPosition = CSVectorScale(vFilteredPosition, 1 / CSVectorLength(vFilteredPosition));
		//vFilteredPosition = CSVectorScale(CSVectorAdd(vRawPosition, vPrevRawPosition), 0.5f);
		//vDiff = CSVectorSubtract(vFilteredPosition, vPrevFilteredPosition);
		//vTrend = CSVectorAdd(CSVectorScale(vDiff, smoothingParams.fCorrection), CSVectorScale(vPrevTrend, 1.0f - smoothingParams.fCorrection));
		//m_pHistory[(int)jt].m_dwFrameCount++;
	}
	//else
	//{
	//	// First apply jitter filter
	//	vDiff = CSVectorSubtract(vRawPosition, vPrevFilteredPosition);
	//	fDiff = CSVectorLength(vDiff);

	//	if (fDiff <= smoothingParams.fJitterRadius)
	//	{
	//		vFilteredPosition = CSVectorAdd(CSVectorScale(vRawPosition, fDiff / smoothingParams.fJitterRadius),
	//			CSVectorScale(vPrevFilteredPosition, 1.0f - fDiff / smoothingParams.fJitterRadius));
	//	}
	//	else
	//	{
	//		vFilteredPosition = vRawPosition;
	//	}

	//	// Now the double exponential smoothing filter
	//	vFilteredPosition = CSVectorAdd(CSVectorScale(vFilteredPosition, 1.0f - smoothingParams.fSmoothing),
	//		CSVectorScale(CSVectorAdd(vPrevFilteredPosition, vPrevTrend), smoothingParams.fSmoothing));


	//	vDiff = CSVectorSubtract(vFilteredPosition, vPrevFilteredPosition);
	//	vTrend = CSVectorAdd(CSVectorScale(vDiff, smoothingParams.fCorrection), CSVectorScale(vPrevTrend, 1.0f - smoothingParams.fCorrection));
	//}

	//// Predict into the future to reduce latency
	//vPredictedPosition = CSVectorAdd(vFilteredPosition, CSVectorScale(vTrend, smoothingParams.fPrediction));

	//// Check that we are not too far away from raw data
	//vDiff = CSVectorSubtract(vPredictedPosition, vRawPosition);
	//fDiff = CSVectorLength(vDiff);

	//if (fDiff > smoothingParams.fMaxDeviationRadius)
	//{
	//	vPredictedPosition = CSVectorAdd(CSVectorScale(vPredictedPosition, smoothingParams.fMaxDeviationRadius / fDiff),
	//		CSVectorScale(vRawPosition, 1.0f - smoothingParams.fMaxDeviationRadius / fDiff));
	//}

	// Save the data from this frame
	m_pHistory[(int)jt].m_vRawPosition = vRawPosition;
	m_pHistory[(int)jt].m_vFilteredPosition = vFilteredPosition;
	m_pHistory[(int)jt].wasFiltered = (joints[(int)jt].TrackingState != TrackingState_Tracked);
	//m_pHistory[(int)jt].m_vTrend = vTrend;

	// Output the data
	m_pFilteredJoints[(int)jt] = vFilteredPosition;
	//m_pFilteredJoints[(int)jt] = vPredictedPosition;
}


