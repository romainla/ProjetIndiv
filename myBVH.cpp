#include "myBVH.h"



myBVH::myBVH()
{
	m_alreadyCreatedBvh = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_nbMinRecorded = 0;
	m_initialPositionRootX = 0.0;
	m_initialPositionRootY = 0.0;
	m_initialPositionRootZ = 0.0;
	m_initialPitchRootY = 0.0;
	m_initialRollRootX = 0.0;
	m_initialYawRootZ = 0.0;
	m_exercise = typeExercise::UPPER_LIMB_RIGHT;
}

myBVH::myBVH(char* filename, typeExercise exercise) {
	std::stringstream filenamestream;
#if (YPR == 1) 
	filenamestream << filename << "YPR.bvh";
	m_filename = (char*)malloc((strlen(filenamestream.str().c_str()) + 1)*sizeof(char));
	strcpy(m_filename, filenamestream.str().c_str());
#else
	filenamestream << filename << "YRP.bvh";
	m_filename = (char*)malloc((strlen(filenamestream.str().c_str()) + 1)*sizeof(char));
	strcpy(m_filename, filenamestream.str().c_str());
#endif
	m_alreadyCreatedBvh = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_bufferFrame = (double***)malloc(nbMinMaxOfRecording * sizeof(double**));
	m_nbMinRecorded = 0;
	m_initialPositionRootX = 0.0;
	m_initialPositionRootY = 0.0;
	m_initialPositionRootZ = 0.0;
	m_initialPitchRootY = 0.0;
	m_initialRollRootX = 0.0;
	m_initialYawRootZ = 0.0;
	m_exercise = exercise;
}

void myBVH::createFile(IBody * pBody)
{
	m_bvhFile = fopen(m_filename, "w");
	this->writeHierarchy(pBody);
	m_filter = new KinectJointFilter();
	m_filter->Init();
	m_alreadyCreatedBvh = true;
	m_ref_yaw_orientation = NULL;
	m_ref_pitch_orientation = NULL;
	m_ref_roll_orientation = NULL;
	int nbTotalChannels = nbChannels[m_exercise];
	m_ref_yaw_orientation = (double*)malloc((nbTotalChannels - 6) * sizeof(double)); //-6 because the information of the root are registered separately
	m_ref_pitch_orientation = (double*)malloc((nbTotalChannels - 6) * sizeof(double)); //-6 because the information of the root are registered separately
	m_ref_roll_orientation = (double*)malloc((nbTotalChannels - 6) * sizeof(double)); //-6 because the information of the root are registered separately
}

void myBVH::writeHierarchy(IBody * pBody) {
	char indexTabulation = 0;

	char initialisation[] = "HIERARCHY\nROOT ";
	char* nameRoot = "SpineBase";
	char offsetRoot[] = "\n{ \n\tOFFSET 0.00 0.00 0.00\n";

	fwrite(initialisation, sizeof(initialisation), 1, m_bvhFile);
	fwrite(nameRoot, strlen(nameRoot), 1, m_bvhFile);
	fwrite(offsetRoot, strlen(offsetRoot), 1, m_bvhFile);
	char channelRoot[] = "\tCHANNELS 6 ";
	fwrite(channelRoot, strlen(channelRoot), 1, m_bvhFile);
	indexTabulation++;

	int nbChannels = 6;
	for (int i = 0; i < nbChannels; i++) {
		fwrite(labelsChannels[i], strlen(labelsChannels[i]), 1, m_bvhFile);
		if (i != nbChannels - 1) {
			fwrite(" ", strlen(" "), 1, m_bvhFile);
		}
		else {
			fwrite("\n", strlen("\n"), 1, m_bvhFile);
		}
	}

	Joint joints[JointType_Count];
	HRESULT hr = pBody->GetJoints(_countof(joints), joints);
	if (SUCCEEDED(hr)) {
		switch (m_exercise) {
		case UPPER_LIMB_RIGHT:
			this->writeSpine(joints, indexTabulation);
			break;
		case UPPER_LIMB_LEFT:
			this->writeSpine(joints, indexTabulation);
			break;
		case LOWER_LIMB_RIGHT:
			this->writeRightLeg(joints, indexTabulation);
			break;
		case LOWER_LIMB_LEFT:
			this->writeLeftLeg(joints, indexTabulation);
			break;
		case WHOLE_BODY_RIGHT:
			this->writeSpine(joints, indexTabulation);
			this->writeRightLeg(joints, indexTabulation);
			break;
		case WHOLE_BODY_LEFT:
			this->writeSpine(joints, indexTabulation);
			this->writeLeftLeg(joints, indexTabulation);
			break;
		default:
			this->writeSpine(joints, indexTabulation);
		}
		fwrite("}\n", strlen("}\n"), 1, m_bvhFile);

	}
}

void myBVH::writeSpine(Joint* joints, char indexTabulation)
{
	char labelJoint[] = "JOINT ";
	int nbChannels = 6;

	JointType listEnumJointsParents[2] = { JointType_SpineBase,JointType_SpineMid };
	int jointsUtils = 2;

	for (int j = 0; j < 2; j++) {
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite(labelJoint, strlen(labelJoint), 1, m_bvhFile);
		fwrite(labelsSpine[j], strlen(labelsSpine[j]), 1, m_bvhFile);

		fwrite("\n", strlen("\n"), 1, m_bvhFile);
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("{\n", strlen("{\n"), 1, m_bvhFile);
		indexTabulation++;
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("OFFSET ", strlen("OFFSET "), 1, m_bvhFile);
		CameraSpacePoint positionParent = joints[listEnumJointsParents[j]].Position;
		CameraSpacePoint positionChild = joints[JointsSpine[j]].Position;
		double offsetX = (positionChild.X - positionParent.X) * 100; //offset in cm
		double offsetY = (positionChild.Y - positionParent.Y) * 100; //offset in cm
		double offsetZ = (positionChild.Z - positionParent.Z) * 100; //offset in cm
		std::stringstream offset;
		offset << offsetX << " " << offsetY << " " << offsetZ << "\n";
		double length = sqrt(offsetX*offsetX + offsetY*offsetY + offsetZ*offsetZ);
		//offset << length << " 0 0\n";
		std::string offsetChar = offset.str();
		fwrite(offsetChar.c_str(), strlen(offsetChar.c_str()), 1, m_bvhFile);


		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("CHANNELS 3 ", strlen("CHANNELS 3 "), 1, m_bvhFile);
		for (int i = 3; i < nbChannels; i++) {
			fwrite(labelsChannels[i], strlen(labelsChannels[i]), 1, m_bvhFile);
			if (i != nbChannels - 1) {
				fwrite(" ", strlen(" "), 1, m_bvhFile);
			}
			else {
				fwrite("\n", strlen("\n"), 1, m_bvhFile);
			}
		}


	}

	this->writeHead(joints, indexTabulation);

	switch (m_exercise) {
	case UPPER_LIMB_RIGHT:
		this->writeRightArm(joints, indexTabulation);
		break;
	case UPPER_LIMB_LEFT:
		this->writeLeftArm(joints, indexTabulation);
		break;
	case WHOLE_BODY_RIGHT:
		this->writeRightArm(joints, indexTabulation);
		break;
	case WHOLE_BODY_LEFT:
		this->writeLeftArm(joints, indexTabulation);
		break;
	default:
		break;
	}

	for (int j = indexTabulation; j > 1; j--) {
		indexTabulation--;
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("}\n", strlen("}\n"), 1, m_bvhFile);
	}

}

void myBVH::writeBonesHierarchy(Joint * joints, char indexTabulation, char jointsUtils, const JointType* listEnumJointsParents, const JointType* listEnumJointsChild, const char** labels)
{
	char labelJoint[] = "JOINT ";
	int nbChannels = 6;

	for (int j = 0; j < jointsUtils; j++) {
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		if (j != jointsUtils - 1)fwrite(labelJoint, strlen(labelJoint), 1, m_bvhFile);
		fwrite(labels[j], strlen(labels[j]), 1, m_bvhFile);

		fwrite("\n", strlen("\n"), 1, m_bvhFile);
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("{\n", strlen("{\n"), 1, m_bvhFile);
		indexTabulation++;
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("OFFSET ", strlen("OFFSET "), 1, m_bvhFile);
		CameraSpacePoint positionParent = joints[listEnumJointsParents[j]].Position;
		CameraSpacePoint positionChild = joints[listEnumJointsChild[j]].Position;
		double offsetX = (positionChild.X - positionParent.X) * 100; //offset in cm
		double offsetY = (positionChild.Y - positionParent.Y) * 100; //offset in cm
		double offsetZ = (positionChild.Z - positionParent.Z) * 100; //offset in cm
		double length = sqrt(offsetX*offsetX + offsetY*offsetY + offsetZ*offsetZ);
		std::stringstream offset;
		offset << offsetX << " " << offsetY << " " << offsetZ << "\n";
		//offset << length << " 0 0\n";
		std::string offsetChar = offset.str();
		fwrite(offsetChar.c_str(), strlen(offsetChar.c_str()), 1, m_bvhFile);

		if (j != jointsUtils - 1) {
			fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
			fwrite("CHANNELS 3 ", strlen("CHANNELS 3 "), 1, m_bvhFile);
			for (int i = 3; i < nbChannels; i++) {
				fwrite(labelsChannels[i], strlen(labelsChannels[i]), 1, m_bvhFile);
				if (i != nbChannels - 1) {
					fwrite(" ", strlen(" "), 1, m_bvhFile);
				}
				else {
					fwrite("\n", strlen("\n"), 1, m_bvhFile);
				}
			}
		}

	}
	char depart = indexTabulation;
	for (int j = depart; j > depart - jointsUtils; j--) {
		indexTabulation--;
		fwrite(listTabulations[indexTabulation], strlen(listTabulations[indexTabulation]), 1, m_bvhFile);
		fwrite("}\n", strlen("}\n"), 1, m_bvhFile);
	}

}

void myBVH::writeHead(Joint * joints, char indexTabulation)
{
	JointType parents[2] = { JointType_SpineShoulder,JointType_Neck };
	this->writeBonesHierarchy(joints, indexTabulation, 2, parents, JointsHead, labelsHead);
}

void myBVH::writeRightArm(Joint* joints, char indexTabulation)
{
	JointType parents[] = { JointType_SpineShoulder,JointType_ShoulderRight, JointType_ElbowRight, JointType_WristRight, JointType_HandRight };
	this->writeBonesHierarchy(joints, indexTabulation, 5, parents, JointsRightArm, labelsRightArm);
}

void myBVH::writeLeftArm(Joint* joints, char indexTabulation)
{
	JointType parents[] = { JointType_SpineShoulder,JointType_ShoulderLeft, JointType_ElbowLeft, JointType_WristLeft, JointType_HandLeft };
	this->writeBonesHierarchy(joints, indexTabulation, 5, parents, JointsLeftArm, labelsLeftArm);
}

void myBVH::writeRightLeg(Joint* joints, char indexTabulation)
{
	JointType parents[] = { JointType_SpineBase ,JointType_HipRight,JointType_KneeRight, JointType_AnkleRight };
	this->writeBonesHierarchy(joints, indexTabulation, 4, parents, JointsRightLeg, labelsRightLeg);
}

void myBVH::writeLeftLeg(Joint* joints, char indexTabulation)
{
	JointType parents[] = { JointType_SpineBase ,JointType_HipLeft,JointType_KneeLeft, JointType_AnkleLeft };
	this->writeBonesHierarchy(joints, indexTabulation, 4, parents, JointsLeftLeg, labelsLeftLeg);
}

void myBVH::update(IBody * pBody,  double fps) {
	int nbTotalChannels = nbChannels[m_exercise];
	if (!m_alreadyCreatedBvh) {
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

	m_filter->UpdateFilter(pBody);

	//Check if we are recording a new minute and we don't have recorded 100min yet. If yes, allocate needed memory
	if (((m_nbFrame / nbFrameByRaw) != m_nbMinRecorded) && m_nbMinRecorded < nbMinMaxOfRecording) {
		m_nbMinRecorded++;
		m_bufferFrame[m_nbMinRecorded] = (double**)malloc(nbFrameByRaw * sizeof(double*));
		for (int i = 0; i < nbFrameByRaw; i++)m_bufferFrame[m_nbMinRecorded][i] = (double*)malloc(nbTotalChannels*sizeof(double));
	}

	// For the upper_limb exercises
	static JointType upper_limb_right_children[10] = { JointType_SpineBase,		JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,			JointType_Head, JointType_ShoulderRight,	JointType_ElbowRight,	JointType_WristRight,	JointType_HandRight, JointType_HandTipRight };
	static bool useful_joints_up_right[10] = { true,					true,					true,						true,					false,			 true,						 true,					true,					true,				 false };
	static JointType upper_limb_right_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderRight,JointType_ElbowRight,	JointType_WristRight };
	static JointType upper_limb_left_children[10] = { JointType_SpineBase,	JointType_SpineMid,		JointType_SpineShoulder,	JointType_Neck,		JointType_Head,		JointType_ShoulderLeft,		JointType_ElbowLeft,	JointType_WristLeft,	JointType_HandLeft, JointType_HandTipLeft };
	static bool useful_joints_up_left[10] = { true,					true,					true,						true,				false,				true,						true,					true,					true,				false };
	static JointType upper_limb_left_parent[] = { JointType_SpineBase,	JointType_SpineMid,			JointType_SpineShoulder,				JointType_SpineShoulder,	JointType_ShoulderLeft,	JointType_ElbowLeft,	JointType_WristLeft };

	// For the lower_limb exercises
	static JointType lower_limb_right_children[5] = { JointType_SpineBase,		JointType_HipRight,		JointType_KneeRight,	JointType_AnkleRight,			JointType_FootRight };
	static bool useful_joints_low_right[5] = {			true,					true,					true,					true,							false };
	static JointType lower_limb_right_parent[] = {								JointType_SpineBase,	JointType_HipRight,		JointType_KneeRight,			JointType_AnkleRight };
	static JointType lower_limb_left_children[5] = { JointType_SpineBase,		JointType_HipLeft,		JointType_KneeLeft,		JointType_AnkleLeft,			JointType_FootLeft };
	static bool useful_joints_low_left[5] = {		 true,						true,					true,					true,							false };
	static JointType lower_limb_left_parent[] = {								JointType_SpineBase,	JointType_HipLeft,		JointType_KneeLeft,			JointType_AnkleLeft };

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

//
void myBVH::saveAndClose()
{
	this->saveFile();
	fclose(m_bvhFile);

	free(m_filename);
	free(m_ref_yaw_orientation);
	free(m_ref_pitch_orientation);
	free(m_ref_roll_orientation);
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

void myBVH::saveFile() {
	int channels = nbChannels[(int)m_exercise];
	fwrite("MOTION\n", strlen("MOTION\n"), 1, m_bvhFile);
	fwrite("Frames: ", strlen("Frames: "), 1, m_bvhFile);
	std::stringstream nbFrameString;
	nbFrameString << m_nbFrame << "\n";
	std::string nbFrameChar = nbFrameString.str();
	fwrite(nbFrameChar.c_str(), strlen(nbFrameChar.c_str()), 1, m_bvhFile);
	fwrite("Frame Time: ", strlen("Frame Time: "), 1, m_bvhFile);
	double frameTime = 1 / m_fps;
	std::stringstream frameTimeString;
	frameTimeString << frameTime << "\n";
	std::string frameTimeChar = frameTimeString.str();
	fwrite(frameTimeChar.c_str(), strlen(frameTimeChar.c_str()), 1, m_bvhFile);
	for (int i = 0; i < m_nbFrame; i++) {
		std::stringstream raw;
		std::string rawChar;
		for (int j = 0; j < channels; j++) {
			raw << m_bufferFrame[i / nbFrameByRaw][i%nbFrameByRaw][j] << " ";
		}
		raw << "\n";
		rawChar = raw.str();
		fwrite(rawChar.c_str(), strlen(rawChar.c_str()), 1, m_bvhFile);

	}
}

// The following code stores the initial yaw, roll, pitch angles as references and then compute yaw,roll,pitch current - yaw,roll,pitch reference for the next frames
void myBVH::storeMotionInformation(IBody * pBody, JointType * listJoints, JointType * listJointsParents, bool * jointsUtils, char nbJoints, int nbTotalChannel)
{
	// obtaining the list of orientation(=rotation) matrices f the whole skeleton
	JointOrientation jointsOrientation[JointType_Count];
	pBody->GetJointOrientations(JointType_Count, jointsOrientation);

	// obtaining the list of smoothed orientation from the filter
	Vector4* jointsOrientationSmoothed = m_filter->GetFilteredJoints();

	// obtaining the list of position(=translation) matrices f the whole skeleton
	Joint joints[JointType_Count];

	HRESULT hr = pBody->GetJoints(_countof(joints), joints);
	CameraSpacePoint positionRoot = joints[listJoints[0]].Position;
	double positionRootX = 0.0;
	double positionRootY = 0.0;
	double positionRootZ = 0.0;
	if (m_nbFrame == 0) {
		m_initialPositionRootX = positionRoot.X * 100; //Xposition channel
		m_initialPositionRootY = 0; //Yposition channel
		positionRootY = positionRoot.Y * 100; //Yposition channel
		m_initialPositionRootZ = positionRoot.Z * 100; //Zposition channel
	}
	else {
		positionRootX = positionRoot.X * 100 - m_initialPositionRootX; //relative position to the center of the BVH player
		positionRootY = positionRoot.Y * 100 - m_initialPositionRootY; //relative position to the center of the BVH player
		positionRootZ = positionRoot.Z * 100 - m_initialPositionRootZ; //relative position to the center of the BVH player
	}
	char index = 0;
	char indexParent = 0;

	for (int i = 0; i < nbJoints; i++) { // nbChannels-1 because the last one is the end site so no channels
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
		double yaw =  atan2(2 * ((x * y) + (w * z)), (w * w) + (x * x) - (y * y) - (z * z)) / M_PI * 180.0; // Yaw Euler angle = rotation around z
		double roll = atan2(2 * ((y * z) + (w * x)), (w * w) - (x * x) - (y * y) + (z * z)) / M_PI * 180.0; // Roll Euler angle = rotation around x
		double pitch = asin(2 * ((w * y) - (x * z))) / M_PI * 180.0; //Pitch Euler Angle = rotation around y

		if (i == 0) { //it is the root joint so it has 6 channels
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][0] = positionRootX; //Xposition channel
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][1] = positionRootY; //Yposition channel
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][2] = positionRootZ; //Zposition channel

			if (m_nbFrame == 0) {
				m_initialYawRootZ = yaw;
				m_initialRollRootX = roll;
				m_initialPitchRootY = pitch;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][3] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][4] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][5] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
			}
			else {
#if (YPR == 1) 
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][3] = yaw - m_initialYawRootZ; //Zrotation channel = Yaw Euler angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][4] = pitch - m_initialPitchRootY; //Yrotation channel = Pitch Euler Angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][5] = roll - m_initialRollRootX; //Xrotation channel = Roll Euler angle
#else
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][3] = yaw - m_initialYawRootZ; //Zrotation channel = Yaw Euler angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][4] = roll - m_initialRollRootX; //Xrotation channel = Roll Euler angle
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][5] = pitch - m_initialPitchRootY; //Yrotation channel = Pitch Euler Angle
#endif
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
#if (YPR == 1) 
				m_ref_yaw_orientation[index - 6] = yaw;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_ref_pitch_orientation[index - 6] = pitch;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_ref_roll_orientation[index - 6] = roll;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;
#else
				m_ref_yaw_orientation[index - 6] = yaw - yawParent;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_ref_roll_orientation[index - 6] = roll - rollParent;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;

				m_ref_pitch_orientation[index - 6] = pitch - pitchParent;
				m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = 0.0; //initial orientation obtained just with the position x,y,z in 3d space
				index++;
#endif


			}
			else {
#if (YPR == 1)
				if (isfinite(yaw - yawParent - m_ref_yaw_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = yaw - yawParent - m_ref_yaw_orientation[index - 6]; //Zrotation channel = Yaw Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Zrotation channel = Yaw Euler angle
				}
				index++;

				if (isfinite(pitch - pitchParent - m_ref_pitch_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = pitch - pitchParent - m_ref_pitch_orientation[index - 6]; //Yrotation channel = Pitch Euler Angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Yrotation channel = Pitch Euler Angle
				}
				index++;

				if (isfinite(roll - rollParent - m_ref_roll_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = roll - rollParent - m_ref_roll_orientation[index - 6]; //Xrotation channel = Roll Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Xrotation channel = Roll Euler angle
				}
				index++;

#else 
				if (isfinite(yaw - yawParent - m_ref_yaw_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = yaw - yawParent - m_ref_yaw_orientation[index - 6]; //Zrotation channel = Yaw Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Zrotation channel = Yaw Euler angle
				}
				index++;

				if (isfinite(roll - rollParent - m_ref_roll_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = roll - rollParent - m_ref_roll_orientation[index - 6]; //Xrotation channel = Roll Euler angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Xrotation channel = Roll Euler angle
				}
				index++;

				if (isfinite(pitch - pitchParent - m_ref_pitch_orientation[index - 6])) {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = pitch - pitchParent - m_ref_pitch_orientation[index - 6]; //Yrotation channel = Pitch Euler Angle
				}
				else {
					m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Yrotation channel = Pitch Euler Angle
				}
				index++;
#endif
			}
			indexParent++;
		}
	}
}


