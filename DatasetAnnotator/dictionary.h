#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <map>


static const std::map< std::string, int > m = {
	{ "SKEL_ROOT" , 0x0},
	{ "SKEL_Pelvis", 0x2e28 },
	{ "SKEL_L_Thigh", 0xe39f },
	{ "SKEL_L_Calf", 0xf9bb },
	{ "SKEL_L_Foot", 0x3779 },
	{ "SKEL_L_Toe0", 0x83c },
	{ "SKEL_R_Thigh", 0xca72 },
	{ "SKEL_R_Calf", 0x9000 },
	{ "SKEL_R_Foot", 0xcc4d },
	{ "SKEL_R_Toe", 0x512d },
	{ "SKEL_Spine_Root", 0xe0fd },
	{ "SKEL_Spine0", 0x5c01 },
	{ "SKEL_Spine1", 0x60f0 },
	{ "SKEL_Spine2", 0x60f1 },
	{ "SKEL_Spine3", 0x60f2 },
	{ "SKEL_L_Clavicle", 0xfcd9 },
	{ "SKEL_L_UpperArm", 0xb1c5 },
	{ "SKEL_L_Forearm", 0xeeeb },
	{ "SKEL_L_Hand", 0x49d9 },
	{ "SKEL_L_Finger00", 0x67f2 },
	{ "SKEL_L_Finger01", 0xff9 },
	{ "SKEL_L_Finger02", 0xffa },
	{ "SKEL_L_Finger10", 0x67f3 },
	{ "SKEL_L_Finger11", 0x1049 },
	{ "SKEL_L_Finger12", 0x104a },
	{ "SKEL_L_Finger20", 0x67f4 },
	{ "SKEL_L_Finger21", 0x1059 },
	{ "SKEL_L_Finger22", 0x105a },
	{ "SKEL_L_Finger30", 0x67f5 },
	{ "SKEL_L_Finger31", 0x1029 },
	{ "SKEL_L_Finger32", 0x102a },
	{ "SKEL_L_Finger40", 0x67f6 },
	{ "SKEL_L_Finger41", 0x1039 },
	{ "SKEL_L_Finger42", 0x103a },
	{ "SKEL_R_Clavicle", 0x29d2 },
	{ "SKEL_R_UpperArm", 0x9d4d },
	{ "SKEL_R_Forearm", 0x6e5c },
	{ "SKEL_R_Hand", 0xdead },
	{ "SKEL_R_Finger00", 0xe5f2 },
	{ "SKEL_R_Finger01", 0xfa10 },
	{ "SKEL_R_Finger02", 0xfa11 },
	{ "SKEL_R_Finger10", 0xe5f3 },
	{ "SKEL_R_Finger11", 0xfa60 },
	{ "SKEL_R_Finger12", 0xfa61 },
	{ "SKEL_R_Finger20", 0xe5f4 },
	{ "SKEL_R_Finger21", 0xfa70 },
	{ "SKEL_R_Finger22", 0xfa71 },
	{ "SKEL_R_Finger30", 0xe5f5 },
	{ "SKEL_R_Finger31", 0xfa40 },
	{ "SKEL_R_Finger32", 0xfa41 },
	{ "SKEL_R_Finger40", 0xe5f6 },
	{ "SKEL_R_Finger41", 0xfa50 },
	{ "SKEL_R_Finger42", 0xfa51 },
	{ "SKEL_Neck_1", 0x9995 },
	{ "SKEL_Head", 0x796e }
};



#endif // DICTIONARY_H