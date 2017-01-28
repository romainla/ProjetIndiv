#pragma once
#include "common.h"

class myAngleFile {
private:
	FILE* m_angleFile;
	char* m_filename;
	bool m_alreadyCreated;
	double m_fps;
	int m_nbFrame;
	double*** m_bufferFrame;
	int m_nbMinRecorded;
	KinectJointFilter* m_filterOrientation;
	myFilterPosition* m_filterPosition;

public:
	
	myAngleFile();
	myAngleFile(char* filename);

	/// <summary>
	/// Function to create the bvh file
	///</summary>
	void createFile(IBody* ppBodies, typeExercise exercise);

	/// <summary>
	/// Function to update the information the bvh file. 
	/// If it has not already been created, the bvh file will be create thanks to createFile.
	/// Otherwise, call storeMotionInformation with the joints corresponding to the exercise
	///</summary>
	void update(IBody * pBody, typeExercise exercise, double fps);

	/// <summary>
	/// Function to store the motion information in the dedicated array. 
	/// The array is recorded in the bvh file when the program is closed thanks to 
	/// saveAndClose
	///</summary>
	void storeMotionInformation(IBody * pBody, JointType *listJoints, JointType *listJointsParents, bool *jointsUtils, char nbJoint, int nbTotalChannel);

	/// <summary>
	/// Function to save and close the bvh file
	///</summary>
	void saveAndClose(typeExercise exercise);

	/// <summary>
	/// Function to save the bvh file
	///</summary>
	void saveFile(typeExercise exercise);

};