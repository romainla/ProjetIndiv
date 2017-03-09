#pragma once

#include <Kinect.h>
#include <stdio.h>
#include <sstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include "myFilterOrientation.h"
#include "myFilterPosition.h"

// Enumeration indicating which type of exercise is currently performed
typedef enum typeExercise {
	UPPER_LIMB_RIGHT = 0,
	UPPER_LIMB_LEFT = 1,
	LOWER_LIMB_RIGHT = 2,
	LOWER_LIMB_LEFT = 3,
	WHOLE_BODY_RIGHT = 4,
	WHOLE_BODY_LEFT = 5
}typeExercise;

// Enumeration indicating the phase of the exercise which is currently performed
typedef enum phaseExercise {
	INITIALIZATION = 0,
	ACTIVE = 1, 
	POST_EXERCISE = 2,
	COUNT = 3
} phaseExercise; 

static phaseExercise currentPhase = phaseExercise::INITIALIZATION;

#define YPR 0
#define YRP 1

#define NBFRAMEBETWEENREFRESH 2

#if (YRP == 1) 
static const char* labelsChannels[] = { "Xposition","Yposition","Zposition","Zrotation","Xrotation","Yrotation" };
#else
static const char* labelsChannels[] = { "Xposition","Yposition","Zposition","Zrotation","Yrotation","Xrotation" };
#endif

//Graphic constants
static const int        cDepthWidth = 512;
static const int        cDepthHeight = 424;
static const int        cColorWidth = 1920;
static const int        cColorHeight = 1080;

static const char* labelsRightArm[] = { "ShoulderRight","ElbowRight","WristRight","HandRight","End Site" };
static const JointType JointsRightArm[5] = { JointType_ShoulderRight,JointType_ElbowRight, JointType_WristRight, JointType_HandRight, JointType_HandTipRight };

static const char* labelsLeftArm[] = { "ShoulderLeft","ElbowLeft","WristLeft","HandLeft", "End Site" };
static const JointType JointsLeftArm[5] = { JointType_ShoulderLeft,JointType_ElbowLeft, JointType_WristLeft, JointType_HandLeft, JointType_HandTipLeft };

static const char* labelsRightLeg[] = { "HipRight","KneeRight", "AnkleRight","End Site" };
static const JointType JointsRightLeg[4] = { JointType_HipRight,JointType_KneeRight, JointType_AnkleRight,JointType_FootRight };

static const char* labelsLeftLeg[] = { "HipLeft",  "KneeLeft", "AnkleLeft", "End Site" };
static const JointType JointsLeftLeg[4] = { JointType_HipLeft,  JointType_KneeLeft, JointType_AnkleLeft, JointType_FootLeft };


static const char* labelsSpine[] = { "SpineMid","SpineShoulder" };
static const JointType JointsSpine[] = { JointType_SpineMid ,JointType_SpineShoulder };
static const char* labelsHead[] = { "Neck","End Site" };
static const JointType JointsHead[] = { JointType_Neck , JointType_Head };

static const int nbFrameByRaw = 1800;
static const int nbMinMaxOfRecording = 100;
static const char* listTabulations[] = { "","\t","\t\t","\t\t\t","\t\t\t\t","\t\t\t\t\t","\t\t\t\t\t\t","\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t\t","\t\t\t\t\t\t\t\t\t\t" };
static const int nbChannels[] = { 27,27,15,15,36,36 };

static const char* labelsAll[] = { "SpineBase","SpineMid","Neck","Head","ShoulderLeft","ElbowLeft","WristLeft","HandLeft","ShoulderRight","ElbowRight",
"WristRight","HandRight","HipLeft","KneeLeft","AnkleLeft","FootLeft","HipRight","KneeRight","AnkleRight","FootRight","SpineShoulder",
"HandTipLeft","ThumbLeft","HandTipRight","ThumbRight" };