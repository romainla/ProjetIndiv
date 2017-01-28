#include "myAngleFile.h"

double myAngleFile::scalarProduct(const CameraSpacePoint & p1, const CameraSpacePoint & p2)
{
	double scalProd = p1.X * p2.X + p1.Y * p2.Y + p1.Z*p2.Z;
	return scalProd;
}

double myAngleFile::length(const CameraSpacePoint & p1)
{
	double length = sqrt(p1.X * p1.X + p1.Y * p1.Y + p1.Z*p1.Z);
	return length;
}

double myAngleFile::getAngle(const CameraSpacePoint & p1, const CameraSpacePoint & p2)
{
	double cosTheta = scalarProduct(p1, p2) / (length(p1)*length(p2));
	return acos(cosTheta);
}

myAngleFile::myAngleFile()
{
	m_alreadyCreated = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_nbMinRecorded = 0;
	m_filename = "test.csv";
	m_exercise = typeExercise::UPPER_LIMB_RIGHT;
}

myAngleFile::myAngleFile(char * filename, typeExercise exercise)
{
	std::stringstream filenamestream; 
	filenamestream << filename << ".csv";
	m_filename = (char*)malloc((strlen(filenamestream.str().c_str()) + 1)*sizeof(char));
	strcpy(m_filename, filenamestream.str().c_str());
	m_alreadyCreated = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_bufferFrame = (double***)malloc(nbMinMaxOfRecording * sizeof(double**));
	m_nbMinRecorded = 0;
	if (exercise == typeExercise::WHOLE_BODY_LEFT ) {
		m_exercise = typeExercise::UPPER_LIMB_LEFT;
	}else if (exercise == typeExercise::WHOLE_BODY_RIGHT){
		m_exercise = typeExercise::UPPER_LIMB_RIGHT;
	}
	else {
		m_exercise = exercise;
	}
}

void myAngleFile::createFile(IBody * ppBodies)
{
	m_angleFile = fopen(m_filename, "w");
	
	//TO DO: write the first raw of the csv file containing the name of the data each column will contain
	
	m_filterOrientation = new KinectJointFilter();
	m_filterOrientation->Init();
	m_filterPosition = new myFilterPosition();
	m_filterPosition->Init();
	m_alreadyCreated = true;

}

void myAngleFile::update(IBody * pBody, double fps)
{
	int nbTotalChannels = nbChannels[m_exercise];
	if (!m_alreadyCreated) {
		this->createFile(pBody);
		m_bufferFrame[m_nbMinRecorded] = (double**)malloc(nbFrameByRaw * sizeof(double*));

		for (int i = 0; i < nbFrameByRaw; i++)m_bufferFrame[m_nbMinRecorded][i] = (double*)malloc(nbTotalChannels*sizeof(double));
	}

	if (m_nbFrame >= 1) {
		m_fps = (m_fps + (double)(1 / m_nbFrame)*fps) / ((double)(1 / m_nbFrame) + 1.0);
	}
	else {
		m_fps = fps;
	}

	m_filterPosition->UpdateFilter(pBody);
	m_filterOrientation->UpdateFilter(pBody);

	//Check if we are recording a new minute and we don't have recorded 100min yet. If yes, allocate needed memory
	if (((m_nbFrame / nbFrameByRaw) != m_nbMinRecorded) && m_nbMinRecorded < nbMinMaxOfRecording) {
		m_nbMinRecorded++;
		m_bufferFrame[m_nbMinRecorded] = (double**)malloc(nbFrameByRaw * sizeof(double*));
		for (int i = 0; i < nbFrameByRaw; i++)m_bufferFrame[m_nbMinRecorded][i] = (double*)malloc(nbTotalChannels*sizeof(double));
	}

	// TO DO : check if the following arrays are valid for the angleFile
	// For the upper_limb exercises
	static JointType upper_limb_right_children[10] = { JointType_SpineBase,		JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,			JointType_Head, JointType_ShoulderRight,	JointType_ElbowRight,	JointType_WristRight,	JointType_HandRight, JointType_HandTipRight };
	static bool useful_joints_up_right[10] = { true,					true,					true,						true,					false,			 true,						 true,					true,					true,				 false };
	static JointType upper_limb_right_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderRight,JointType_ElbowRight,	JointType_WristRight };
	static JointType upper_limb_left_children[10] = { JointType_SpineBase,	JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,		JointType_Head,		JointType_ShoulderLeft,		JointType_ElbowLeft,	JointType_WristLeft,	JointType_HandLeft, JointType_HandTipLeft };
	static bool useful_joints_up_left[10] = { true,					true,					true,						true,				false,				true,						true,					true,					true,				false };
	static JointType upper_limb_left_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderLeft,	JointType_ElbowLeft,	JointType_WristLeft };

	// For the lower_limb exercises
	static JointType lower_limb_right_children[5] = { JointType_SpineBase,		JointType_HipRight,		JointType_KneeRight,	JointType_AnkleRight,			JointType_FootRight };
	static bool useful_joints_low_right[5] = { true,					true,					true,					true,							false };
	static JointType lower_limb_right_parent[] = { JointType_SpineBase,	JointType_HipRight,		JointType_KneeRight,			JointType_AnkleRight };
	static JointType lower_limb_left_children[5] = { JointType_SpineBase,		JointType_HipLeft,		JointType_KneeLeft,		JointType_AnkleLeft,			JointType_FootLeft };
	static bool useful_joints_low_left[5] = { true,						true,					true,					true,							false };
	static JointType lower_limb_left_parent[] = { JointType_SpineBase,	JointType_HipLeft,		JointType_KneeLeft,			JointType_AnkleLeft };

	// For the whole body exercises
	static JointType whole_right_children[14] = { JointType_SpineBase,		JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,			JointType_Head, JointType_ShoulderRight,	JointType_ElbowRight,	JointType_WristRight,	JointType_HandRight, JointType_HandTipRight ,JointType_HipRight,	JointType_KneeRight,	JointType_AnkleRight,			JointType_FootRight };
	static bool useful_joints_whole_right[14] = { true,						true,					true,						true,					false,			 true,						 true,					true,					true,				false,					true,					true,					true,							false };
	static JointType whole_right_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderRight,JointType_ElbowRight,	JointType_WristRight,						 JointType_SpineBase,	JointType_HipRight,		JointType_KneeRight,			JointType_AnkleRight };
	static JointType whole_left_children[14] = { JointType_SpineBase,	JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,		JointType_Head,		JointType_ShoulderLeft,		JointType_ElbowLeft,	JointType_WristLeft,	JointType_HandLeft, JointType_HandTipLeft,	JointType_HipLeft,		JointType_KneeLeft,		JointType_AnkleLeft,		JointType_FootLeft };
	static bool useful_joints_whole_left[14] = { true,					true,					true,						true,				false,				true,						true,					true,					true,				false,					true,					true,					true,						false };
	static JointType whole_left_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderLeft,	JointType_ElbowLeft,	JointType_WristLeft,						JointType_SpineBase,	JointType_HipLeft,		JointType_KneeLeft,			JointType_AnkleLeft };
	//Store the new information obtained from the frame
	JointType* listJoints;
	JointType* listJointsParents;
	bool* jointsUtils;
	char nbJoints;
	switch (m_exercise) {
	case UPPER_LIMB_RIGHT:
		listJoints = upper_limb_right_children;
		listJointsParents = upper_limb_right_parent;
		jointsUtils = useful_joints_up_right;
		nbJoints = 10;
		break;
	case UPPER_LIMB_LEFT:
		listJoints = upper_limb_left_children;
		listJointsParents = upper_limb_left_parent;
		jointsUtils = useful_joints_up_left;
		nbJoints = 10;
		break;
	case LOWER_LIMB_RIGHT:
		listJoints = lower_limb_right_children;
		listJointsParents = lower_limb_right_parent;
		jointsUtils = useful_joints_low_right;
		nbJoints = 5;
		break;
	case LOWER_LIMB_LEFT:
		listJoints = lower_limb_left_children;
		listJointsParents = lower_limb_left_parent;
		jointsUtils = useful_joints_low_left;
		nbJoints = 5;
		break;
	case WHOLE_BODY_RIGHT:
		listJoints = whole_right_children;
		listJointsParents = whole_right_parent;
		jointsUtils = useful_joints_whole_right;
		nbJoints = 14;
		break;
	case WHOLE_BODY_LEFT:
		listJoints = whole_left_children;
		listJointsParents = whole_left_parent;
		jointsUtils = useful_joints_whole_left;
		nbJoints = 14;
		break;
	default:
		listJoints = upper_limb_right_children;
		listJointsParents = upper_limb_right_parent;
		jointsUtils = useful_joints_up_right;
		nbJoints = 10;
		break;
	}

	this->storeMotionInformation(pBody, listJoints, listJointsParents, jointsUtils, nbJoints, nbTotalChannels);

	m_nbFrame++;

}

// TO DO: adapt this function to the angleFile
void myAngleFile::storeMotionInformation(IBody * pBody, JointType * listJoints, JointType * listJointsParents, bool * jointsUtils, char nbJoint, int nbTotalChannel)
{
	// obtaining the list of orientation(=rotation) matrices f the whole skeleton
	JointOrientation jointsOrientation[JointType_Count];
	pBody->GetJointOrientations(JointType_Count, jointsOrientation);

	// obtaining the list of smoothed orientation from the filter
	Vector4* jointsOrientationSmoothed = m_filterOrientation->GetFilteredJoints();
	// obtaining the list of smoothed position from the filter
	CameraSpacePoint* jointsPositionSmoothed = m_filterPosition->GetFilteredJoints();

	// obtaining the list of position(=translation) matrices f the whole skeleton
	Joint joints[JointType_Count];

	HRESULT hr = pBody->GetJoints(_countof(joints), joints);
	CameraSpacePoint positionRoot = joints[listJoints[0]].Position;
	double positionRootX = 0.0;
	double positionRootY = 0.0;
	double positionRootZ = 0.0;
	
	positionRootX = positionRoot.X * 100 ; //relative position to the center of the BVH player
	positionRootY = positionRoot.Y * 100 ; //relative position to the center of the BVH player
	positionRootZ = positionRoot.Z * 100 ; //relative position to the center of the BVH player
	
	char index = 0;
	char indexParent = 0;

	for (int i = 0; i < nbJoint; i++) { // nbChannels-1 because the last one is the end site so no channels
		JointOrientation orientation = jointsOrientation[listJoints[i]];
		TrackingState stateCurrentJoint = joints[listJoints[i]].TrackingState;

		//Quaternion values
		double x; // absolute orientation quaternion of the current joint
		double y; // absolute orientation quaternion of the current joint
		double z; // absolute orientation quaternion of the current joint
		double w; // absolute orientation quaternion of the current joint
		if (stateCurrentJoint == TrackingState_Tracked) {
			x = orientation.Orientation.x; // absolute orientation quaternion of the current joint
			y = orientation.Orientation.y; // absolute orientation quaternion of the current joint
			z = orientation.Orientation.z; // absolute orientation quaternion of the current joint
			w = orientation.Orientation.w; // absolute orientation quaternion of the current joint

		}
		else if (stateCurrentJoint == TrackingState_Inferred || stateCurrentJoint == TrackingState_NotTracked) {
			x = jointsOrientationSmoothed[listJoints[i]].x; //absolute smoothed orientation quaternion of the current joint
			y = jointsOrientationSmoothed[listJoints[i]].y; //absolute smoothed orientation quaternion of the current joint
			z = jointsOrientationSmoothed[listJoints[i]].z; //absolute smoothed orientation quaternion of the current joint
			w = jointsOrientationSmoothed[listJoints[i]].w; //absolute smoothed orientation quaternion of the current joint
		}


		// Euler angles
		double yaw = atan2(2 * ((x * y) + (w * z)), (w * w) + (x * x) - (y * y) - (z * z)) / M_PI * 180.0; // Yaw Euler angle = rotation around z
		double roll = atan2(2 * ((y * z) + (w * x)), (w * w) - (x * x) - (y * y) + (z * z)) / M_PI * 180.0; // Roll Euler angle = rotation around x
		double pitch = asin(2 * ((w * y) - (x * z))) / M_PI * 180.0; //Pitch Euler Angle = rotation around y

		if (i == 0) { //it is the root joint so it has 6 channels
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][0] = positionRootX; //Xposition channel
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][1] = positionRootY; //Yposition channel
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][2] = positionRootZ; //Zposition channel

			if (m_nbFrame == 0) {
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][3] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][4] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][5] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
			}
			else {
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][3] = yaw ; //Zrotation channel = Yaw Euler angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][4] = roll; //Xrotation channel = Roll Euler angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][5] = pitch; //Yrotation channel = Pitch Euler Angle

			}

			index = 6;

		}
		else if (jointsUtils[i]) {// it isn t the root so it has only 3 channels
			JointOrientation orientationParent = jointsOrientation[listJointsParents[indexParent]];
			stateCurrentJoint = joints[listJointsParents[indexParent]].TrackingState;
			// QUaternion values of the parent joint
			double x; // absolute orientation quaternion of the parent
			double y; // absolute orientation quaternion of the parent
			double z; // absolute orientation quaternion of the parent
			double w; // absolute orientation quaternion of the parent
			if (stateCurrentJoint == TrackingState_Tracked) {
				x = orientationParent.Orientation.x;
				y = orientationParent.Orientation.y;
				z = orientationParent.Orientation.z;
				w = orientationParent.Orientation.w;

			}
			else if (stateCurrentJoint == TrackingState_Inferred || stateCurrentJoint == TrackingState_NotTracked) {
				x = jointsOrientationSmoothed[listJointsParents[indexParent]].x; //absolute smoothed orientation quaternion of the parent joint
				y = jointsOrientationSmoothed[listJointsParents[indexParent]].y; //absolute smoothed orientation quaternion of the parent joint
				z = jointsOrientationSmoothed[listJointsParents[indexParent]].z; //absolute smoothed orientation quaternion of the parent joint
				w = jointsOrientationSmoothed[listJointsParents[indexParent]].w; //absolute smoothed orientation quaternion of the parent joint
			}

			// Euler angles of the parent joint

			//For relative orientation
			//double yawParent = atan2(2 * ((x * y) + (w * z)), (w * w) + (x * x) - (y * y) - (z * z)) / M_PI * 180.0; // Yaw Euler angle = rotation around z
			//double rollParent = atan2(2 * ((y * z) + (w * x)), (w * w) - (x * x) - (y * y) + (z * z)) / M_PI * 180.0; // Roll Euler angle = rotation around x
			//double pitchParent = asin(2 * ((w * y) - (x * z))) / M_PI * 180.0; //Pitch Euler Angle = rotation around y

			//For absolute orientation
			double yawParent = 0.0;
			double rollParent = 0.0;
			double pitchParent = 0.0;
			if (m_nbFrame == 0) {
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = yaw; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = roll; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = pitch; //initial orientation obtained just with the position x,y,z in 3d space
				index++;
			}
			else {

				if (isfinite(yaw - yawParent )) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = yaw - yawParent ; //Zrotation channel = Yaw Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Zrotation channel = Yaw Euler angle
				}
				index++;

				if (isfinite(roll - rollParent)) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = roll - rollParent; //Xrotation channel = Roll Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Xrotation channel = Roll Euler angle
				}
				index++;

				if (isfinite(pitch - pitchParent )) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = pitch - pitchParent ; //Yrotation channel = Pitch Euler Angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Yrotation channel = Pitch Euler Angle
				}
				index++;
			}
			indexParent++;
		}
	}
}

void myAngleFile::saveAndClose()
{
	this->saveFile();
	fclose(m_angleFile);

	for (int i = 0; i < m_nbMinRecorded + 1; i++) {
		if (i < m_nbMinRecorded) {
			for (int j = 0; j < nbFrameByRaw; j++)free(m_bufferFrame[i][j]);
			free(m_bufferFrame[i]);
		}
		else {
			int limit = m_nbFrame % nbFrameByRaw;
			for (int j = 0; j < limit; j++)free(m_bufferFrame[i][j]);
			free(m_bufferFrame[i]);
		}
	}
	free(m_bufferFrame);
}

// TO DO: adapt this function to the angleFile
void myAngleFile::saveFile()
{
	int channels = nbChannels[(int)m_exercise];
	fwrite("Frames: ", strlen("Frames: "), 1, m_angleFile);
	std::stringstream nbFrameString;
	nbFrameString << m_nbFrame << "\n";
	std::string nbFrameChar = nbFrameString.str();
	fwrite(nbFrameChar.c_str(), strlen(nbFrameChar.c_str()), 1, m_angleFile);
	fwrite("Frame Time: ", strlen("Frame Time: "), 1, m_angleFile);
	double frameTime = 1 / m_fps;
	std::stringstream frameTimeString;
	frameTimeString << frameTime << "\n";
	std::string frameTimeChar = frameTimeString.str();
	fwrite(frameTimeChar.c_str(), strlen(frameTimeChar.c_str()), 1, m_angleFile);
	for (int i = 0; i < m_nbFrame; i++) {
		std::stringstream raw;
		std::string rawChar;
		for (int j = 0; j < channels; j++) {
			raw << m_bufferFrame[i / nbFrameByRaw][i%nbFrameByRaw][j] << " ";
		}
		raw << "\n";
		rawChar = raw.str();
		fwrite(rawChar.c_str(), strlen(rawChar.c_str()), 1, m_angleFile);

	}
}
