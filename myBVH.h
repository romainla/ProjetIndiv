#pragma once

#include "common.h"

class myBVH {
private :
	FILE* m_bvhFile;
	char* m_filename;
	bool m_alreadyCreatedBvh;
	double m_fps;
	int m_nbFrame;
	double*** m_bufferFrame;
	int m_nbMinRecorded;
	KinectJointFilter* m_filter;
	typeExercise m_exercise;

	//initial position of the root joint
	double m_initialPositionRootX;
	double m_initialPositionRootY;
	double m_initialPositionRootZ;
	double m_initialRollRootX;
	double m_initialPitchRootY;
	double m_initialYawRootZ;
	// References for the orientation of each other joint in the 1st frame
	double* m_ref_yaw_orientation;
	double* m_ref_pitch_orientation;
	double* m_ref_roll_orientation;

	
public:
	myBVH();
	myBVH(char* filename, typeExercise exercise);

	/// <summary>
	/// Function to create the bvh file
	///</summary>
	void createFile(IBody* ppBodies);

	/// <summary>
	/// Function to write the hierarchy depending on the exercise in the bvh file
	///</summary>
	void writeHierarchy(IBody * pBody);

	/// <summary>
	/// Function to write the spine, head and arm (depending on the exercise) in the bvh file
	///</summary>
	void writeSpine(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to write bones hierarchy
	///</summary>
	void writeBonesHierarchy(Joint* joints, char indexTabulation, char jointsUtils, const JointType* listEnumJointsParents, const JointType* listEnumJointsChild, const char** labels);

	/// <summary>
	/// Function to write the head bones hierarchy
	///</summary>
	void writeHead(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to write the right arm bones hierarchy
	///</summary>
	void writeRightArm(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to write the left arm bones hierarchy
	///</summary>
	void writeLeftArm(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to write the right leg bones hierarchy
	///</summary>
	void writeRightLeg(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to write the left leg bones hierarchy
	///</summary>
	void writeLeftLeg(Joint* joints, char indexTabulation);

	/// <summary>
	/// Function to update the information the bvh file. 
	/// If it has not already been created, the bvh file will be create thanks to createFile.
	/// Otherwise, call storeMotionInformation with the joints corresponding to the exercise
	///</summary>
	void update(IBody * pBody, double fps);
	
	/// <summary>
	/// Function to store the motion information in the dedicated array. 
	/// The array is recorded in the bvh file when the program is closed thanks to 
	/// saveAndClose
	///</summary>
	void storeMotionInformation(IBody * pBody, JointType *listJoints, JointType *listJointsParents, bool *jointsUtils, char nbJoint, int nbTotalChannel);

	/// <summary>
	/// Function to save and close the bvh file
	///</summary>
	void saveAndClose();

	/// <summary>
	/// Function to save the bvh file
	///</summary>
	void saveFile();
};