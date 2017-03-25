#include "myAngleFile.h"

const int myAngleFile::nbData[4] = {18,18,18,18}; // Timestamp of the frame, label telling if it is the exercise, initialization or post-exercise, 2 joints with angle2D, quaternion and the 3 Euler angles, 2 joints withe angle2D, quaternion and Euler angles

const char *myAngleFile::labelExercicePhases[] = { "init","exercise","postExercise" }; // Labels indicating the phase of the exercise which is currently performed

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

CameraSpacePoint myAngleFile::difference(const CameraSpacePoint & p1, const CameraSpacePoint & p2)
{
	CameraSpacePoint result = CameraSpacePoint();
	result.X = p1.X - p2.X;
	result.Y = p1.Y - p2.Y;
	result.Z = p1.Z - p2.Z;
	return result;
}

myAngleFile::myAngleFile()
{
	m_alreadyCreated = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_nbMinRecorded = 0;
	m_filename = (char*)malloc((strlen("CSVandMatlab\\test.csv")+1)*sizeof(char));
	strcpy(m_filename, "CSVandMatlab\\test.csv");
	m_exercise = typeExercise::UPPER_LIMB_RIGHT;
	m_nbData = nbData[(int)m_exercise];
	m_bufferFrame = (double***)malloc(nbMinMaxOfRecording * sizeof(double**));
	m_phase = phaseExercise::INITIALIZATION;
}

myAngleFile::myAngleFile(const char * filename, typeExercise exercise)
{
	m_filename = (char*)malloc((strlen(filename) + 1)*sizeof(char));
	strcpy(m_filename, filename);
	m_alreadyCreated = false;
	m_nbFrame = 0;
	m_fps = 0.00;
	m_bufferFrame = (double***)malloc(nbMinMaxOfRecording * sizeof(double**));
	m_nbMinRecorded = 0;
	if (exercise == typeExercise::WHOLE_BODY_LEFT) {
		m_exercise = typeExercise::UPPER_LIMB_LEFT;
	}
	else if (exercise == typeExercise::WHOLE_BODY_RIGHT) {
		m_exercise = typeExercise::UPPER_LIMB_RIGHT;
	}
	else {
		m_exercise = exercise;
	}
	m_nbData = nbData[(int)m_exercise];
	m_phase = phaseExercise::INITIALIZATION;
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

void myAngleFile::update(IBody * pBody, double fps, phaseExercise exercise)
{
	int nbTotalChannels = nbChannels[m_exercise];
	if (!m_alreadyCreated) {
		this->createFile(pBody);
		m_bufferFrame[m_nbMinRecorded] = (double**)malloc(nbFrameByRaw * sizeof(double*));

		for (int i = 0; i < nbFrameByRaw; i++)m_bufferFrame[m_nbMinRecorded][i] = (double*)malloc(nbTotalChannels*sizeof(double));
	}

	m_phase = exercise;

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

	// For the upper_limb exercises
	static JointType upper_limb_right_children[3] = { 	JointType_ElbowRight,	JointType_WristRight,	JointType_HandRight };
	static JointType upper_limb_right_parent[2] = { 	JointType_ShoulderRight,JointType_ElbowRight };
	static JointType upper_limb_left_children[3] = { 	JointType_ElbowLeft,	JointType_WristLeft,	JointType_HandLeft };
	static JointType upper_limb_left_parent[2] = {		JointType_ShoulderLeft,	JointType_ElbowLeft };

	// For the lower_limb exercises
	static JointType lower_limb_right_children[3] = { JointType_KneeRight,	JointType_AnkleRight,			JointType_FootRight };
	static JointType lower_limb_right_parent[] = { JointType_HipRight,		JointType_KneeRight };
	static JointType lower_limb_left_children[3] = { JointType_KneeLeft,		JointType_AnkleLeft,			JointType_FootLeft };
	static JointType lower_limb_left_parent[] = { JointType_HipLeft,		JointType_KneeLeft };

	//Store the new information obtained from the frame
	JointType* listJoints;
	JointType* listJointsParents;
	char nbJoints;
	switch (m_exercise) {
	case UPPER_LIMB_RIGHT:
		listJoints = upper_limb_right_children;
		listJointsParents = upper_limb_right_parent;
		nbJoints = 3;
		break;
	case UPPER_LIMB_LEFT:
		listJoints = upper_limb_left_children;
		listJointsParents = upper_limb_left_parent;
		nbJoints = 3;
		break;
	case LOWER_LIMB_RIGHT:
		listJoints = lower_limb_right_children;
		listJointsParents = lower_limb_right_parent;
		nbJoints = 3;
		break;
	case LOWER_LIMB_LEFT:
		listJoints = lower_limb_left_children;
		listJointsParents = lower_limb_left_parent;
		nbJoints = 3;
		break;
	default:
		listJoints = upper_limb_right_children;
		listJointsParents = upper_limb_right_parent;
		nbJoints = 3;
		break;
	}

	this->storeMotionInformation(pBody, listJoints, listJointsParents, nbJoints, nbTotalChannels);

	m_nbFrame++;

}

// TO DO: adapt this function to the angleFile
void myAngleFile::storeMotionInformation(IBody * pBody, JointType * listJoints, JointType * listJointsParents, char nbJoint, int nbTotalChannel)
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

	char index = 0;

	// Save the timestamp of the frame in the first column of the buffer
	m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_nbFrame / m_fps;
	index++;

	// Save the phase of exercise of the frame in the first column of the buffer
	m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = (int)m_phase;
	index++;

	for (int i = 0; i < nbJoint - 1; i++) { // nbChannels-1 because the last one is used only for computing the angles
		JointOrientation orientation = jointsOrientation[listJoints[i]];
		TrackingState stateCurrentJoint = joints[listJoints[i]].TrackingState;
		CameraSpacePoint positionCurrent = joints[listJoints[i]].Position;
		CameraSpacePoint positionParent = joints[listJointsParents[i]].Position;
		CameraSpacePoint positionChild = joints[listJoints[i + 1]].Position;
		CameraSpacePoint parentCurrentVector;
		CameraSpacePoint currentChildVector;
		double angle2D;

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

			positionCurrent = jointsPositionSmoothed[listJoints[i]];
		}
		stateCurrentJoint = joints[listJointsParents[i]].TrackingState;
		if (stateCurrentJoint == TrackingState_Inferred || stateCurrentJoint == TrackingState_NotTracked) {
			positionParent = jointsPositionSmoothed[listJointsParents[i]];
		}

		stateCurrentJoint = joints[listJoints[i + 1]].TrackingState;
		if (stateCurrentJoint == TrackingState_Inferred || stateCurrentJoint == TrackingState_NotTracked) {
			positionChild = jointsPositionSmoothed[listJoints[i + 1]];
		}
		//---------Computation of the 2D angle----------
		parentCurrentVector = difference(positionCurrent, positionParent);  //compute the vector parent->current
		currentChildVector = difference(positionChild, positionCurrent); // compute the vector current -> child
		parentCurrentVector.Z = 0; // We assume that these two vectors belong to the same plane
		currentChildVector.Z = 0; // We assume that these two vectors belong to the same plane
		angle2D = getAngle(parentCurrentVector, currentChildVector); //compute the angle between these two vectors in radians
		angle2D = angle2D * (180.0 / M_PI);
		m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = angle2D;
		index++;

		//---------Computation of the Euler angles ---------
		double yaw = atan2(2 * ((x * y) + (w * z)), (w * w) + (x * x) - (y * y) - (z * z)) / M_PI * 180.0; // Yaw Euler angle = rotation around z
		double roll = atan2(2 * ((y * z) + (w * x)), (w * w) - (x * x) - (y * y) + (z * z)) / M_PI * 180.0; // Roll Euler angle = rotation around x
		double pitch = asin(2 * ((w * y) - (x * z))) / M_PI * 180.0; //Pitch Euler Angle = rotation around y

		m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = w;
		index++;
		m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = x;
		index++;
		m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = y;
		index++;
		m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = z;
		index++;
		if (isfinite(yaw)) {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = yaw; //Zrotation channel = Yaw Euler angle
		}
		else {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Zrotation channel = Yaw Euler angle
		}
		index++;
		
		if (isfinite(pitch)) {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = pitch; //Yrotation channel = Pitch Euler Angle
		}
		else {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Yrotation channel = Pitch Euler Angle
		}
		index++;

		if (isfinite(roll)) {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = roll; //Xrotation channel = Roll Euler angle
		}
		else {
			m_bufferFrame[m_nbMinRecorded][m_nbFrame][index] = m_bufferFrame[m_nbMinRecorded][m_nbFrame - 1][index]; //Xrotation channel = Roll Euler angle
		}
		index++;
	}
}


void myAngleFile::saveAndClose()
{
	if (m_alreadyCreated) {
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
	}
	free(m_filename);
	free(m_bufferFrame);
}

// TO DO: adapt this function to the angleFile
void myAngleFile::saveFile()
{
	int channels = m_nbData;
	fwrite("Frames: ", strlen("Frames: "), 1, m_angleFile);
	std::stringstream nbFrameString;
	nbFrameString << m_nbFrame << "\n";
	std::string nbFrameChar = nbFrameString.str();
	fwrite(nbFrameChar.c_str(), strlen(nbFrameChar.c_str()), 1, m_angleFile);
	fwrite("Frame rate: ", strlen("Frame rate: "), 1, m_angleFile);
	std::stringstream frameTimeString;
	frameTimeString << m_fps << "\n";
	std::string frameTimeChar = frameTimeString.str();
	fwrite(frameTimeChar.c_str(), strlen(frameTimeChar.c_str()), 1, m_angleFile);
	writeNameColumns();
	for (int i = 0; i < m_nbFrame; i++) {
		std::stringstream raw;
		std::string rawChar;
		for (int j = 0; j < channels; j++) {
			if (j == 1) {
				raw << labelExercicePhases[(int)m_bufferFrame[i / nbFrameByRaw][i%nbFrameByRaw][j]] << " ";
			}
			else {
				raw << m_bufferFrame[i / nbFrameByRaw][i%nbFrameByRaw][j] << " ";
			}
		}
		raw << "\n";
		rawChar = raw.str();
		fwrite(rawChar.c_str(), strlen(rawChar.c_str()), 1, m_angleFile);

	}
}

void myAngleFile::writeNameColumns()
{
	char **nameColumns;
	int nbColumns = m_nbData;
	char *nameLower[] = { "TimeStamp","PhaseOfExercise","angle2D_Knee","quaternion_W_Knee","quaternion_X_Knee" ,"quaternion_Y_Knee" ,"quaternion_Z_Knee", "Yaw_Knee", "Pitch_Knee", "Roll_Knee","angle2D_Ankle","quaternion_W_Ankle","quaternion_X_Ankle" ,"quaternion_Y_Ankle" ,"quaternion_Z_Ankle", "Yaw_Ankle", "Pitch_Ankle", "Roll_Ankle" };
	char *nameUpper[] = { "TimeStamp","PhaseOfExercise","angle2D_Elbow","quaternion_W_Elbow","quaternion_X_Elbow" ,"quaternion_Y_Elbow" ,"quaternion_Z_Elbow", "Yaw_Elbow", "Pitch_Elbow", "Roll_Elbow","angle2D_Wrist","quaternion_W_Wrist","quaternion_X_Wrist" ,"quaternion_Y_Wrist" ,"quaternion_Z_Wrist", "Yaw_Wrist", "Pitch_Wrist", "Roll_Wrist" };
	switch (m_exercise) {
	case typeExercise::LOWER_LIMB_LEFT:
	case typeExercise::LOWER_LIMB_RIGHT:
		nameColumns = nameLower;
		break;
	case typeExercise::UPPER_LIMB_LEFT:
	case typeExercise::UPPER_LIMB_RIGHT:
	default:
		nameColumns = nameUpper;
	}
	for (int i = 0; i < nbColumns; i++) {
		fwrite(nameColumns[i], strlen(nameColumns[i]), 1, m_angleFile);
		fwrite(" ", strlen(" "), 1, m_angleFile);
	}
	fwrite("\n", strlen("\n"), 1, m_angleFile);
}
