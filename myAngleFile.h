#pragma once
#include "common.h"

class myAngleFile {
private:
	FILE* m_angleFile; // Pointer towards the output file
	char* m_filename; // Store the name of the output file
	bool m_alreadyCreated; // Indicates if the creation of the output file and the memory allocation have already been made
	double m_fps; // Frame rate
	int m_nbFrame; // Number of frames which have already been recorded
	double*** m_bufferFrame; // Buffer containing the data which will be saved
	int m_nbMinRecorded; // Number of minutes which have already been recorded
	KinectJointFilter* m_filterOrientation; // Filter for the orientation of the joints
	myFilterPosition* m_filterPosition; // Filter for the position of the joints
	typeExercise m_exercise; // Indicates the type of exercise which is currently performed
	int m_nbData;
	phaseExercise m_phase; // Current phase of the exercise
	static const int nbData[4]; // Contains the number of data stored for each type of exercise 
	static const char *labelExercicePhases[]; // Labels indicating the phase of the exercise which is currently performed

	/// <summary>
	/// Function to compute the scalar product between two position vector
	///</summary>
	static double scalarProduct(const CameraSpacePoint& p1, const CameraSpacePoint& p2);
	
	/// <summary>
	/// Function to compute the length of a position vector
	///</summary>
	static double length(const CameraSpacePoint& p1);
	
	/// <summary>
	/// Function to compute the angle in radian between two positions vector
	///</summary>
	static double getAngle(const CameraSpacePoint& p1, const CameraSpacePoint& p2);
	
	/// <summary>
	/// Function to compute the difference p1 - p2
	///</summary>
	static CameraSpacePoint difference(const CameraSpacePoint& p1, const CameraSpacePoint& p2);

public:
	
	myAngleFile();
	myAngleFile(char* filename, typeExercise exercise);

	/// <summary>
	/// Function to create the angle file
	///</summary>
	void createFile(IBody* ppBodies);

	/// <summary>
	/// Function to update the information the angle file. 
	/// If it has not already been created, the angle file will be create thanks to createFile.
	/// Otherwise, call storeMotionInformation with the joints corresponding to the exercise
	///</summary>
	void update(IBody * pBody, double fps, phaseExercise exercise);

	/// <summary>
	/// Function to store the motion information in the dedicated array. 
	/// The array is recorded in the angle file when the program is closed thanks to 
	/// saveAndClose
	///</summary>
	void storeMotionInformation(IBody * pBody, JointType *listJoints, JointType *listJointsParents, char nbJoint, int nbTotalChannel);

	/// <summary>
	/// Function to save and close the angle file
	///</summary>
	void saveAndClose();

	/// <summary>
	/// Function to save the angle file
	///</summary>
	void saveFile();

	/// <summary>
	/// Function to write the name of the different columns of the csv file
	///</summary>
	void writeNameColumns();
};