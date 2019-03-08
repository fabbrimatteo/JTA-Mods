#define _CRT_SECURE_NO_WARNINGS

#include "scenario.h"
#include <vector>
#include <direct.h>
#include <string.h>
#include <filesystem>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <list>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TIME_FACTOR 12.0
#define FPS 30
#define DISPLAY_FLAG FALSE
#define WANDERING_RADIUS 10.0
#define MAX_PED_TO_CAM_DISTANCE 100.0
#define DEMO FALSE

static char scenarioTypes[14][40]{
	"NEAREST",
	"RANDOM",
	"WORLD_HUMAN_MUSICIAN",
	"WORLD_HUMAN_SMOKING",
	"WORLD_HUMAN_BINOCULARS",
	"WORLD_HUMAN_CHEERING",
	"WORLD_HUMAN_DRINKING",
	"WORLD_HUMAN_PARTYING",
	"WORLD_HUMAN_PICNIC",
	"WORLD_HUMAN_STUPOR",
	"WORLD_HUMAN_PUSH_UPS",
	"WORLD_HUMAN_LEANING",
	"WORLD_HUMAN_MUSCLE_FLEX",
	"WORLD_HUMAN_YOGA"
};

float random_float(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

int random_int(int min, int max) {
	return min + rand() % (max - min + 1);
}

Vector3 coordsToVector(float x, float y, float z)
{
	Vector3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

int StringToWString(std::wstring &ws, const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());
	ws = wsTmp;
	return 0;
}

DatasetAnnotator::DatasetAnnotator(std::string _output_path, const char* _file_scenario, int _max_samples, int _is_night)
{
	PLAYER::SET_EVERYONE_IGNORE_PLAYER(PLAYER::PLAYER_PED_ID(), TRUE);
	PLAYER::SET_POLICE_IGNORE_PLAYER(PLAYER::PLAYER_PED_ID(), TRUE);
	PLAYER::CLEAR_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_PED_ID());
	ENTITY::SET_ENTITY_COLLISION(PLAYER::PLAYER_PED_ID(), TRUE, TRUE);
	ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), TRUE, FALSE);
	ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 255, FALSE);
	ENTITY::SET_ENTITY_CAN_BE_DAMAGED(PLAYER::PLAYER_PED_ID(), FALSE);

	GAMEPLAY::SET_TIME_SCALE(1);

	this->output_path = _output_path;
	this->file_scenario = _file_scenario;
	this->max_samples = _max_samples;
	this->is_night = _is_night;

	srand((unsigned int)time(NULL)); //Initialises randomiser or sum' like that

	this->bad_scenarios = {
		"WORLD_BOAR_GRAZING",
		"WORLD_CAT_SLEEPING_GROUND",
		"WORLD_CAT_SLEEPING_LEDGE",
		"WORLD_COW_GRAZING",
		"WORLD_COYOTE_HOWL",
		"WORLD_COYOTE_REST",
		"WORLD_COYOTE_WANDER",
		"WORLD_CHICKENHAWK_FEEDING",
		"WORLD_CHICKENHAWK_STANDING",
		"WORLD_CORMORANT_STANDING",
		"WORLD_CROW_FEEDING",
		"WORLD_CROW_STANDING",
		"WORLD_DEER_GRAZING",
		"WORLD_DOG_BARKING_ROTTWEILER",
		"WORLD_DOG_BARKING_RETRIEVER",
		"WORLD_DOG_BARKING_SHEPHERD",
		"WORLD_DOG_SITTING_ROTTWEILER",
		"WORLD_DOG_SITTING_RETRIEVER",
		"WORLD_DOG_SITTING_SHEPHERD",
		"WORLD_DOG_BARKING_SMALL",
		"WORLD_DOG_SITTING_SMALL",
		"WORLD_FISH_IDLE",
		"WORLD_GULL_FEEDING",
		"WORLD_GULL_STANDING",
		"WORLD_HEN_PECKING",
		"WORLD_HEN_STANDING",
		"WORLD_MOUNTAIN_LION_REST",
		"WORLD_MOUNTAIN_LION_WANDER",
		"WORLD_PIG_GRAZING",
		"WORLD_PIGEON_FEEDING",
		"WORLD_PIGEON_STANDING",
		"WORLD_RABBIT_EATING",
		"WORLD_RATS_EATING",
		"WORLD_SHARK_SWIM",
		"PROP_BIRD_IN_TREE",
		"PROP_BIRD_TELEGRAPH_POLE"
	};

	joint_int_codes[0]  = m.find("SKEL_Head")->second;
	joint_int_codes[1]  = m.find("SKEL_Neck_1")->second;
	joint_int_codes[2]  = m.find("SKEL_R_Clavicle")->second;
	joint_int_codes[3]  = m.find("SKEL_R_UpperArm")->second;
	joint_int_codes[4]  = m.find("SKEL_R_Forearm")->second;
	joint_int_codes[5]  = m.find("SKEL_R_Hand")->second;
	joint_int_codes[6]  = m.find("SKEL_L_Clavicle")->second;
	joint_int_codes[7]  = m.find("SKEL_L_UpperArm")->second;
	joint_int_codes[8]  = m.find("SKEL_L_Forearm")->second;
	joint_int_codes[9]  = m.find("SKEL_L_Hand")->second;
	joint_int_codes[10] = m.find("SKEL_Spine3")->second;
	joint_int_codes[11] = m.find("SKEL_Spine2")->second;
	joint_int_codes[12] = m.find("SKEL_Spine1")->second;
	joint_int_codes[13] = m.find("SKEL_Spine0")->second;
	joint_int_codes[14] = m.find("SKEL_Spine_Root")->second;
	joint_int_codes[15] = m.find("SKEL_R_Thigh")->second;
	joint_int_codes[16] = m.find("SKEL_R_Calf")->second;
	joint_int_codes[17] = m.find("SKEL_R_Foot")->second;
	joint_int_codes[18] = m.find("SKEL_L_Thigh")->second;
	joint_int_codes[19] = m.find("SKEL_L_Calf")->second;
	joint_int_codes[20] = m.find("SKEL_L_Foot")->second;

	// inizialize the coords_file used to storage coords data
	log_file.open(output_path + "\\log.txt");
	coords_file.open(output_path + "\\coords.csv");
	coords_file << "frame,pedestrian_id,joint_type,3D_x,3D_y,3D_z,2D_x,2D_y,occluded,self_occluded,";
	coords_file << "cam_3D_x,cam_3D_y,cam_3D_z,cam_rot_x,cam_rot_y,cam_rot_z,fov\n";

	this->player = PLAYER::PLAYER_ID();
	this->playerPed = PLAYER::PLAYER_PED_ID();
	this->line = "";
	this->log = "";
	this->captureFreq = (int)(FPS / TIME_FACTOR);
	this->SHOW_JOINT_RECT = DISPLAY_FLAG;

	this->fov = 50;

	std::vector<const char*> weathers_night = {
		"CLEAR",
		"CLEAR",
		"CLEAR",
		"CLEAR",
		"CLEAR",
		"EXTRASUNNY",
		"EXTRASUNNY",
		"EXTRASUNNY",
		"RAIN",
		"THUNDER",
		"SMOG",
		"FOGGY",
		"XMAS",
		"BLIZZARD"
	};

	std::vector<const char*> weathers_day = {
		"CLEAR",
		"CLEAR",
		"CLEAR",
		"CLEAR",
		"EXTRASUNNY",
		"EXTRASUNNY",
		"EXTRASUNNY",
		"RAIN",
		"THUNDER",
		"CLOUDS",
		"OVERCAST",
		"SMOG",
		"FOGGY",
		"XMAS",
		"BLIZZARD"
	};

	//randomizing time
	int time_h, time_m, time_s;
	int date_m;
	char* weather;

	std::vector<int> night_hours = { 20, 22, 23, 4, 5, 6 };

	// set time
	if (is_night) {
		time_h = night_hours[random_int(0, 5)];
		weather = (char *)weathers_night[rand() % weathers_night.size()];
	}
	else {
		time_h = random_int(7, 19);
		weather = (char *)weathers_day[rand() % weathers_day.size()];
	}
	time_m = random_int(0, 59);
	time_s = random_int(0, 59);
	date_m = random_int(1, 12);

	TIME::SET_CLOCK_TIME(time_h, time_m, time_s);

	// randomizing weather
	GAMEPLAY::SET_RANDOM_WEATHER_TYPE();
	GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
	GAMEPLAY::SET_OVERRIDE_WEATHER(weather);
	GAMEPLAY::SET_WEATHER_TYPE_NOW(weather);

	loadScenario(file_scenario);

	//Screen capture buffer
	GRAPHICS::_GET_SCREEN_ACTIVE_RESOLUTION(&windowWidth, &windowHeight);
	hWnd = ::FindWindow(NULL, "Compatitibility Theft Auto V");
	hWindowDC = GetDC(hWnd);
	hCaptureDC = CreateCompatibleDC(hWindowDC);
	hCaptureBitmap = CreateCompatibleBitmap(hWindowDC, SCREEN_WIDTH, SCREEN_HEIGHT);
	SelectObject(hCaptureDC, hCaptureBitmap);
	SetStretchBltMode(hCaptureDC, COLORONCOLOR);

	// used to decide how often save the sample
	recordingPeriod = 1.0f / captureFreq;

	// initialize recording stuff
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	GetEncoderClsid(L"image/jpeg", &pngClsid);

	// inizialize the int used to count the saved frame
	nsample = 0;

	//Avoid bad things such as getting killed by the police, robbed, dying in car accidents or other horrible stuff
	PLAYER::SET_EVERYONE_IGNORE_PLAYER(player, TRUE);
	PLAYER::SET_POLICE_IGNORE_PLAYER(player, TRUE);
	PLAYER::CLEAR_PLAYER_WANTED_LEVEL(player);

	PLAYER::SET_PLAYER_INVINCIBLE(player, TRUE);
	PLAYER::SPECIAL_ABILITY_FILL_METER(player, 1);
	PLAYER::SET_PLAYER_NOISE_MULTIPLIER(player, 0.0);
	PLAYER::SET_SWIM_MULTIPLIER_FOR_PLAYER(player, 1.49f);
	PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, 1.49f);
	PLAYER::DISABLE_PLAYER_FIRING(player, TRUE);
	PLAYER::SET_DISABLE_AMBIENT_MELEE_MOVE(player, TRUE);

	// invisible and intangible player
	/*ENTITY::SET_ENTITY_COLLISION(PLAYER::PLAYER_PED_ID(), TRUE, TRUE);
	ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), FALSE, FALSE);
	ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 0.0, TRUE);*/
	if (moving)
		ENTITY::SET_ENTITY_COLLISION(PLAYER::PLAYER_PED_ID(), TRUE, TRUE);
	else
		ENTITY::SET_ENTITY_COLLISION(PLAYER::PLAYER_PED_ID(), FALSE, TRUE);
	ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), FALSE, FALSE);
	ENTITY::SET_ENTITY_ALPHA(PLAYER::PLAYER_PED_ID(), 0, FALSE);
	ENTITY::SET_ENTITY_CAN_BE_DAMAGED(PLAYER::PLAYER_PED_ID(), FALSE);

	// randomizing number of peds
	//this->n_peds = random_int(20, 50);
	//this->n_peds = 20;

	// seconds are proportional to number of peds
	if (DEMO) 
		this->secondsBeforeSaveImages = 10;
	else
		this->secondsBeforeSaveImages = max_waiting_time / 1000 + 10 + 10;

	lastRecordingTime = std::clock() + (clock_t)((float)(secondsBeforeSaveImages * CLOCKS_PER_SEC));

	// spawn pedestrians
	//Scenario::spawnPed(ped_spawn_pos, n_peds);
}

DatasetAnnotator::~DatasetAnnotator()
{
	// todo: implement a destroyer
	ReleaseDC(hWnd, hWindowDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
	coords_file.close();
	log_file.close();
}

int DatasetAnnotator::update()
{
	float delay = ((float)(std::clock() - lastRecordingTime)) / CLOCKS_PER_SEC;
	if (delay >= recordingPeriod)
		lastRecordingTime = std::clock();
	else
		return nsample;
	GAMEPLAY::SET_TIME_SCALE(1.0f / (float)TIME_FACTOR);

	PED::SET_PED_DENSITY_MULTIPLIER_THIS_FRAME(1.0);

	//ENTITY::SET_ENTITY_COORDS_NO_OFFSET(PLAYER::PLAYER_PED_ID(), cam_coords.x, cam_coords.y, cam_coords.z, 0, 0, 0);

	Ped peds[max_number_of_peds];											// array of pedestrians
	int number_of_peds = worldGetAllPeds(peds, max_number_of_peds);			// number of pedestrians taken
	float C;																// coefficient used to adjust the size of rectangles drawn around the joints


	for (int i = 0; i < number_of_peds; i++) {
		if (!PED::IS_PED_A_PLAYER(peds[i]) && peds[i] != ped_with_cam) {
			ENTITY::SET_ENTITY_COLLISION(peds[i], TRUE, TRUE);
			ENTITY::SET_ENTITY_VISIBLE(peds[i], TRUE, FALSE);
			ENTITY::SET_ENTITY_ALPHA(peds[i], 255, FALSE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(peds[i], FALSE);
		}
		else if (PED::IS_PED_A_PLAYER(peds[i]))
		{
			if (moving) {
				ENTITY::SET_ENTITY_COLLISION(peds[i], TRUE, TRUE);
				ENTITY::SET_ENTITY_VISIBLE(peds[i], FALSE, FALSE);
				ENTITY::SET_ENTITY_ALPHA(peds[i], 0, FALSE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(peds[i], FALSE);
			}
			else {
				ENTITY::SET_ENTITY_COLLISION(peds[i], FALSE, TRUE);
				ENTITY::SET_ENTITY_VISIBLE(peds[i], FALSE, FALSE);
				ENTITY::SET_ENTITY_ALPHA(peds[i], 0, FALSE);
				ENTITY::SET_ENTITY_CAN_BE_DAMAGED(peds[i], FALSE);
			}
			
		}
		else if (peds[i] == ped_with_cam) {
			ENTITY::SET_ENTITY_COLLISION(ped_with_cam, TRUE, TRUE);
			ENTITY::SET_ENTITY_VISIBLE(ped_with_cam, FALSE, FALSE);
			ENTITY::SET_ENTITY_ALPHA(ped_with_cam, 0, FALSE);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped_with_cam, FALSE);
		}
	}

	
	if (moving) {
		this->cam_coords = CAM::GET_CAM_COORD(camera);
		Vector3 ped_with_cam_rot = ENTITY::GET_ENTITY_ROTATION(this->ped_with_cam, 2);
		CAM::SET_CAM_ROT(camera, ped_with_cam_rot.x, ped_with_cam_rot.y, ped_with_cam_rot.z, 2);
		this->cam_rot = CAM::GET_CAM_ROT(camera, 2);
		//this->cam_rot = ped_with_cam_rot;
	}

	//this->cam_coords = CAM::GET_GAMEPLAY_CAM_COORD();
	//this->cam_rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
	//this->fov = CAM::GET_GAMEPLAY_CAM_FOV();

	// scan all the pedestrians taken
	for (int i = 0; i < number_of_peds; i++){

		// ignore pedestrians in vehicles or dead pedestrians
		if(PED::IS_PED_IN_ANY_VEHICLE(peds[i], TRUE) || PED::IS_PED_DEAD_OR_DYING(peds[i], TRUE)) {
			//log_file << "veicolo o morto\n";
			continue;
		}
		// ignore player
		else if (PED::IS_PED_A_PLAYER(peds[i])) {
			//log_file << "player\n";
			continue;
		}
		else if (!ENTITY::IS_ENTITY_ON_SCREEN(peds[i])) {
			//log_file << "non su schermo\n";
			continue;
		}
		else if (!PED::IS_PED_HUMAN(peds[i])) {
			//log_file << "non umano\n";
			continue;
		}
		else if (!ENTITY::IS_ENTITY_VISIBLE(peds[i])) {
			//log_file << "invisibile\n";
			continue;
		}

		Vector3 ped_coords = ENTITY::GET_ENTITY_COORDS(peds[i], TRUE);
		float ped2cam_distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(
			cam_coords.x, cam_coords.y, cam_coords.z, 
			ped_coords.x, ped_coords.y, ped_coords.z, 1
		);

		if (ped2cam_distance < MAX_PED_TO_CAM_DISTANCE) {
			
			// for each pedestrians scan all the joint_ID we choose on the subset
			for (int n = -1; n < number_of_joints; n++) {

				Vector3 joint_coords;
				if (n == -1) {
					Vector3 head_coords = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(peds[i], PED::GET_PED_BONE_INDEX(peds[i], joint_int_codes[0]));
					Vector3 neck_coords = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(peds[i], PED::GET_PED_BONE_INDEX(peds[i], joint_int_codes[1]));
					float head_neck_norm = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(neck_coords.x, neck_coords.y, neck_coords.z, head_coords.x, head_coords.y, head_coords.z, 1);
					float dx = (head_coords.x - neck_coords.x) / head_neck_norm;
					float dy = (head_coords.y - neck_coords.y) / head_neck_norm;
					float dz = (head_coords.z - neck_coords.z) / head_neck_norm;

					joint_coords.x = head_coords.x + head_neck_norm * dx;
					joint_coords.y = head_coords.y + head_neck_norm * dy;
					joint_coords.z = head_coords.z + head_neck_norm * dz;
				}
				else 
					joint_coords = ENTITY::GET_WORLD_POSITION_OF_ENTITY_BONE(peds[i], PED::GET_PED_BONE_INDEX(peds[i], joint_int_codes[n]));
				
				// finding the versor (dx, dy, dz) pointing from the joint to the cam
				float joint2cam_distance = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(
					joint_coords.x, joint_coords.y, joint_coords.z, 
					cam_coords.x, cam_coords.y, cam_coords.z, 1
				);
				float dx = (cam_coords.x - joint_coords.x) / joint2cam_distance;
				float dy = (cam_coords.y - joint_coords.y) / joint2cam_distance;
				float dz = (cam_coords.z - joint_coords.z) / joint2cam_distance;

				// ray #1: from joint to cam_coords (ignoring the pedestrian to whom the joint belongs and intersecting only pedestrian (8))
				// ==> useful for detecting occlusions of pedestrian
				Vector3 end_coords1, surface_norm1;
				BOOL occlusion_ped;
				Entity entityHit1 = 0;

				int ray_ped_occlusion = WORLDPROBE::_CAST_RAY_POINT_TO_POINT(
					joint_coords.x, joint_coords.y, joint_coords.z,
					cam_coords.x, cam_coords.y, cam_coords.z,
					8, peds[i], 7
				);
				WORLDPROBE::_GET_RAYCAST_RESULT(ray_ped_occlusion, &occlusion_ped, &end_coords1, &surface_norm1, &entityHit1);

				if (entityHit1 == ped_with_cam)
					occlusion_ped = FALSE;


				// ray #2: from joint to camera (without ignoring the pedestrian to whom the joint belongs and intersecting only pedestrian (8))
				// ==> useful for detecting self-occlusions
				Vector3 endCoords2, surfaceNormal2;
				BOOL occlusion_self;
				Entity entityHit2 = 0;
				int ray_joint2cam = WORLDPROBE::_CAST_RAY_POINT_TO_POINT(
					joint_coords.x + 0.1f*dx, joint_coords.y + 0.1f*dy, joint_coords.z + 0.1f*dz,
					cam_coords.x, cam_coords.y, cam_coords.z, 
					8, 0, 7
				);
				WORLDPROBE::_GET_RAYCAST_RESULT(ray_joint2cam, &occlusion_self, &endCoords2, &surfaceNormal2, &entityHit2);

				if (entityHit2 == ped_with_cam)
					occlusion_self = FALSE;


				// ray #3: from camera to joint (ignoring the pedestrian to whom the joint belongs and intersecting everything but peds (4 and 8))
				// ==> useful for detecting occlusions with objects
				Vector3 endCoords3, surfaceNormal3;
				BOOL occlusion_object;
				Entity entityHit3 = 0;
				int ray_joint2cam_obj = WORLDPROBE::_CAST_RAY_POINT_TO_POINT(
					cam_coords.x, cam_coords.y, cam_coords.z,
					joint_coords.x, joint_coords.y, joint_coords.z,
					(~0 ^ (8|4)), peds[i], 7
					);
				WORLDPROBE::_GET_RAYCAST_RESULT(ray_joint2cam_obj, &occlusion_object, &endCoords3, &surfaceNormal3, &entityHit3);

				
				BOOL occluded = occlusion_ped || occlusion_object;


				if (DISPLAY_FLAG) {
					float x, y;
					get_2D_from_3D(joint_coords, &x, &y);

					// C calculation based on distance between the current pedestrians and the camera
					if (ped2cam_distance > 6)
						C = (float)(1.5 / cbrt(ped2cam_distance));
					else
						C = 1;

					if (occluded) {
						/*GRAPHICS::DRAW_BOX(
						joint_coords.x, joint_coords.y, joint_coords.z,
						joint_coords.x + 0.1, joint_coords.y + 0.1, joint_coords.z + 0.1,
						255, 0, 64, 175
						);*/
						GRAPHICS::DRAW_RECT(x, y, 0.005f*C, 0.005f*C, 255, 0, 64, 175);
					}
					else if (occlusion_self) {
						/*GRAPHICS::DRAW_BOX(
							joint_coords.x, joint_coords.y, joint_coords.z,
							joint_coords.x + 0.1, joint_coords.y + 0.1, joint_coords.z + 0.1,
							255, 128, 16, 175
							);*/
						GRAPHICS::DRAW_RECT(x, y, 0.005f*C, 0.005f*C, 255, 128, 16, 175);
					}
					else {
						/*GRAPHICS::DRAW_BOX(
							joint_coords.x, joint_coords.y, joint_coords.z,
							joint_coords.x + 0.1, joint_coords.y + 0.1, joint_coords.z + 0.1,
							0, 255, 64, 175
							);*/
						GRAPHICS::DRAW_RECT(x, y, 0.005f*C, 0.005f*C, 0, 255, 64, 175);
					}
				}
				float x, y;
				get_2D_from_3D(joint_coords, &x, &y);
				x = x * SCREEN_WIDTH;
				y = y * SCREEN_HEIGHT;
				coords_file << nsample;					  // frame number
				coords_file << "," << peds[i];			  // pedestrian ID
				coords_file << "," << n+1;				  // joint type
				coords_file << "," << joint_coords.x;	  // joint 3D x [m]
				coords_file << "," << joint_coords.y;	  // joint 3D y [m]
				coords_file << "," << joint_coords.z;	  // joint 3D z [m]
				coords_file << "," << x;				  // camera 2D x [px]
				coords_file << "," << y;	              // camera 2D y [px]
				coords_file << "," << occluded;			  // is joint occluded?
				coords_file << "," << occlusion_self;	  // is joint self-occluded?
				coords_file << "," << cam_coords.x;		  // camera 3D x [m]
				coords_file << "," << cam_coords.y;	      // camera 3D y [m]
				coords_file << "," << cam_coords.z;	      // camera 3D z [m]
				coords_file << "," << cam_rot.x;		  // camera 3D rotation x [degrees]
				coords_file << "," << cam_rot.y;	      // camera 3D rotation y [degrees]
				coords_file << "," << cam_rot.z;	      // camera 3D rotation z [degrees]
				coords_file << "," << fov;				  // camera FOV  [degrees]
				coords_file << "\n";
			}
		}
	}
	save_frame();
	nsample++;
	if (nsample == max_samples) {
		for (int i = 0; i < nwPeds; i++) {
			PED::DELETE_PED(&wPeds[i].ped);
		}
		for (int i = 0; i < nwPeds_scenario; i++) {
			PED::DELETE_PED(&wPeds_scenario[i].ped);
		}
	}

	return nsample;
}

void DatasetAnnotator::addwPed(Ped p, Vector3 from, Vector3 to, int stop, float spd)
{
	if (nwPeds > 299)
		return;

	wPeds[nwPeds].ped = p;
	wPeds[nwPeds].from = from;
	wPeds[nwPeds].to = to;
	wPeds[nwPeds].stopTime = stop;
	wPeds[nwPeds].speed = spd;

	nwPeds++;
}

void DatasetAnnotator::addwPed_scenario(Ped p)
{
	if (nwPeds_scenario > 299)
		return;

	wPeds_scenario[nwPeds_scenario].ped = p;
	nwPeds_scenario++;
}

void DatasetAnnotator::get_2D_from_3D(Vector3 v, float *x2d, float *y2d) {

	// translation
	float x = v.x - cam_coords.x;
	float y = v.y - cam_coords.y;
	float z = v.z - cam_coords.z;

	// rotation
	float cam_x_rad = cam_rot.x * (float)M_PI / 180.0f;
	float cam_y_rad = cam_rot.y * (float)M_PI / 180.0f;
	float cam_z_rad = cam_rot.z * (float)M_PI / 180.0f;

	// cos
	float cx = cos(cam_x_rad);
	float cy = cos(cam_y_rad);
	float cz = cos(cam_z_rad);

	// sin
	float sx = sin(cam_x_rad);
	float sy = sin(cam_y_rad);
	float sz = sin(cam_z_rad);	

	Vector3 d;
	d.x = cy*(sz*y + cz*x) - sy*z;
	d.y = sx*(cy*z + sy*(sz*y + cz*x)) + cx*(cz*y - sz*x);
	d.z = cx*(cy*z + sy*(sz*y + cz*x)) - sx*(cz*y - sz*x);

	float fov_rad = fov * (float)M_PI / 180;
	float f = (SCREEN_HEIGHT / 2.0f) * cos(fov_rad / 2.0f) / sin(fov_rad / 2.0f);

	*x2d = ((d.x * (f / d.y)) / SCREEN_WIDTH + 0.5f);
	*y2d = (0.5f - (d.z * (f / d.y)) / SCREEN_HEIGHT);
}

void DatasetAnnotator::save_frame() {
	StretchBlt(hCaptureDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hWindowDC, 0, 0, windowWidth, windowHeight, SRCCOPY | CAPTUREBLT);
	Gdiplus::Bitmap image(hCaptureBitmap, (HPALETTE)0);
	std::wstring ws;
	StringToWString(ws, output_path);

	image.Save((ws + L"\\" + std::to_wstring(nsample) + L".jpeg").c_str(), &pngClsid, NULL);
}

void DatasetAnnotator::setCameraMoving(Vector3 A, Vector3 B, Vector3 C, int fov) {
	
	CAM::DESTROY_ALL_CAMS(TRUE);
	this->camera = CAM::CREATE_CAM((char *)"DEFAULT_SCRIPTED_CAMERA", TRUE);
	//this->ped_with_cam = PED::CREATE_RANDOM_PED(A.x, A.y, A.z);
	this->ped_with_cam = PLAYER::PLAYER_PED_ID();
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped_with_cam, A.x, A.y, A.z, 0, 0, 1);
	//AI::TASK_WANDER_IN_AREA(this->ped_with_cam, coords.x, coords.y, coords.z, WANDERING_RADIUS, 1.0, 1.0);
	float z_offset = ((float)((rand() % (6)) - 2)) / 10;
	CAM::ATTACH_CAM_TO_ENTITY(camera, this->ped_with_cam, 0, 0, z_offset, TRUE);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_FOV(camera, (float)fov);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);
	//CAM::SET_CAM_MOTION_BLUR_STRENGTH(camera, 10.0);

	//ENTITY::SET_ENTITY_HEALTH(ped_with_cam, 0);
	WAIT(500);
	//AI::CLEAR_PED_TASKS_IMMEDIATELY(ped_with_cam);
	//PED::RESURRECT_PED(ped_with_cam);
	//PED::REVIVE_INJURED_PED(ped_with_cam);
	//PED::SET_PED_CAN_RAGDOLL(ped_with_cam, TRUE);

	ENTITY::SET_ENTITY_COLLISION(ped_with_cam, TRUE, TRUE);
	ENTITY::SET_ENTITY_VISIBLE(ped_with_cam, FALSE, FALSE);
	ENTITY::SET_ENTITY_ALPHA(ped_with_cam, 0, FALSE);
	ENTITY::SET_ENTITY_CAN_BE_DAMAGED(ped_with_cam, FALSE);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped_with_cam, TRUE);
	PED::SET_PED_COMBAT_ATTRIBUTES(ped_with_cam, 1, FALSE);

	Object seq;
	AI::OPEN_SEQUENCE_TASK(&seq);
	//AI::TASK_USE_MOBILE_PHONE_TIMED(0, max_waiting_time + 10000);
	AI::TASK_STAND_STILL(0, max_waiting_time + 10000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, A.x, A.y, A.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, B.x, B.y, B.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, C.x, C.y, C.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, B.x, B.y, B.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, A.x, A.y, A.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, B.x, B.y, B.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, C.x, C.y, C.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, B.x, B.y, B.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::TASK_GO_TO_COORD_ANY_MEANS(0, A.x, A.y, A.z, 1.0, 0, 0, 786603, 0xbf800000);
	AI::CLOSE_SEQUENCE_TASK(seq);
	AI::TASK_PERFORM_SEQUENCE(ped_with_cam, seq);
	AI::CLEAR_SEQUENCE_TASK(&seq);

	// set the cam_coords used on update() function
	this->cam_coords = CAM::GET_CAM_COORD(camera);
	this->cam_rot = CAM::GET_CAM_ROT(camera, 2);
}

void DatasetAnnotator::setCameraFixed(Vector3 coords, Vector3 rot, float cam_z, int fov) {

	CAM::DESTROY_ALL_CAMS(TRUE);
	this->camera = CAM::CREATE_CAM((char *)"DEFAULT_SCRIPTED_CAMERA", TRUE);
	CAM::SET_CAM_COORD(camera, coords.x, coords.y, coords.z+cam_z);
	CAM::SET_CAM_ROT(camera, rot.x, rot.y, rot.z, 2);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_FOV(camera, (float)fov);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);


	// set the cam_coords used on update() function
	this->cam_coords = CAM::GET_CAM_COORD(camera);
	this->cam_rot = CAM::GET_CAM_ROT(camera, 2);
	this->fov = (int)CAM::GET_CAM_FOV(camera);
}

void DatasetAnnotator::spawnPed(Vector3 pos, int numPed) {

	int i = 0;
	Vector3 current;
	Vector3 ped_spawned_coord[1024];
	int n = (numPed % 2 == 0) ? numPed : numPed + 1;


	for (int i = 0; i<n; i++) {
		ped_spawned[i] = PED::CREATE_RANDOM_PED(pos.x + random_float(-3, 3), pos.y + random_float(-3, 3), pos.z - 0.4f);
	}
	WAIT(2000);
	for (int i = 0; i<n; i++) {
		ENTITY::SET_ENTITY_HEALTH(ped_spawned[i], 0);
	}
	WAIT(2000);
	for (int i = 0; i<n; i++) {		
		AI::CLEAR_PED_TASKS_IMMEDIATELY(ped_spawned[i]);
		PED::RESURRECT_PED(ped_spawned[i]);
		PED::REVIVE_INJURED_PED(ped_spawned[i]);
		ENTITY::SET_ENTITY_COLLISION(ped_spawned[i], TRUE, TRUE);
		PED::SET_PED_CAN_RAGDOLL(ped_spawned[i], TRUE);
	}
	WAIT(2000);
	for (int i = 0; i<n; i++) {
		ped_spawned_coord[i] = ENTITY::GET_ENTITY_COORDS(ped_spawned[i], TRUE);
		//AI::TASK_USE_NEAREST_SCENARIO_TO_COORD(ped_spawned[i], current.x, current.y, current.z, 100.0, 1000 * 60 * 3);
		//AI::TASK_WANDER_IN_AREA(ped_spawned[i], pos.x, pos.y, pos.z, WANDERING_RADIUS, 0.0, 0.0);
		AI::TASK_WANDER_STANDARD(ped_spawned[i], 0x471c4000, 0);

	}
	WAIT(10000);
	for (int i = 0; i<n; i++) {
		current = ENTITY::GET_ENTITY_COORDS(ped_spawned[i], TRUE);
		float dist = sqrt(pow(ped_spawned_coord[i].x - current.x, 2) + pow(ped_spawned_coord[i].y - current.y, 2) + pow(ped_spawned_coord[i].z - current.z, 2));
		log_file << dist << std::endl;
		if (dist < 1.0)
			PED::DELETE_PED(&ped_spawned[i]);
	}
}

Vector3 DatasetAnnotator::teleportPlayer(Vector3 pos){
												
	// set the heading
	float heading = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360));

	// teleport the player to the previously selected coordinates
	PLAYER::START_PLAYER_TELEPORT(this->player, pos.x, pos.y, pos.z, heading, 0, 0, 0);
	while (PLAYER::IS_PLAYER_TELEPORT_ACTIVE()) WAIT(0);

	return pos;
}

void DatasetAnnotator::draw_text(char *text, float x, float y, float scale) {
	UI::SET_TEXT_FONT(0);
	UI::SET_TEXT_SCALE(scale, scale);
	UI::SET_TEXT_COLOUR(255, 255, 255, 245);
	UI::SET_TEXT_WRAP(0.0, 1.0);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(2, 2, 0, 0, 0);
	UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
	UI::_SET_TEXT_ENTRY((char *)"STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(text);
	UI::_DRAW_TEXT(y, x);
}

int DatasetAnnotator::myreadLine(FILE *f, Vector3 *pos, int *nPeds, int *ngroup, int *currentBehaviour, float *speed, Vector3 *goFrom, Vector3 *goTo, int *task_time, int *type, 
	int *radius, int *min_lenght, int *time_between_walks, int *spawning_radius)
{
	int result = fscanf_s(f, "%d %f %f %f %d %d %f %f %f %f %f %f %f %d %d %d %d %d %d \n", nPeds, &(*pos).x, &(*pos).y, &(*pos).z,
		ngroup, currentBehaviour, speed, 
		&(*goFrom).x, &(*goFrom).y, &(*goFrom).z, &(*goTo).x, &(*goTo).y, &(*goTo).z, 
		task_time, type, radius, min_lenght, time_between_walks, spawning_radius);

	return result;
}

Cam DatasetAnnotator::lockCam(Vector3 pos, Vector3 rot) {
	CAM::DESTROY_ALL_CAMS(true);
	Cam lockedCam = CAM::CREATE_CAM_WITH_PARAMS((char *)"DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, 50, true, 2);
	CAM::SET_CAM_ACTIVE(lockedCam, true);
	CAM::RENDER_SCRIPT_CAMS(1, 0, 3000, 1, 0);
	return lockedCam;
}

void DatasetAnnotator::loadScenario(const char* fname)
{
	FILE *f = fopen(fname, "r");
	Vector3 cCoords, cRot;
	Vector3 vTP1, vTP2, vTP1_rot, vTP2_rot;
	int stop;

	fscanf_s(f, "%d ", &moving);
	if (moving == 0) 
		fscanf_s(f, "%f %f %f %d %f %f %f\n", &cCoords.x, &cCoords.y, &cCoords.z, &stop, &cRot.x, &cRot.y, &cRot.z);
	else 
		fscanf_s(f, "%f %f %f %d %f %f %f %f %f %f\n", &A.x, &A.y, &A.z, &stop, &B.x, &B.y, &B.z, &C.x, &C.y, &C.z);

	fscanf_s(f, "%f %f %f %f %f %f\n", &vTP1.x, &vTP1.y, &vTP1.z, &vTP1_rot.x, &vTP1_rot.y, &vTP1_rot.z);
	fscanf_s(f, "%f %f %f %f %f %f\n", &vTP2.x, &vTP2.y, &vTP2.z, &vTP2_rot.x, &vTP2_rot.y, &vTP2_rot.z);

	Entity e = PLAYER::PLAYER_PED_ID();

	// teleport far away in order to load game scenarios
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, vTP1.x, vTP1.y, vTP1.z, 0, 0, 1);
	lockCam(vTP1, vTP1_rot);
	if (DEMO)
		WAIT(3000);
	else
		WAIT(10000);

	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, vTP2.x, vTP2.y, vTP2.z, 0, 0, 1);
	lockCam(vTP2, vTP2_rot);
	if (DEMO)
		WAIT(3000);
	else
		WAIT(10000);

	/*if (moving == 0)
		Scenario::teleportPlayer(cCoords);
	else
		Scenario::teleportPlayer(A);*/

	if (moving == 0)
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, cCoords.x, cCoords.y, cCoords.z, 0, 0, 1);
	else
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(e, B.x, B.y, B.z, 0, 0, 1);
		
	//ENTITY::SET_ENTITY_COORDS_NO_OFFSET(this->player, cCoords.x, cCoords.y, cCoords.z, 0, 0, 1);
	//ENTITY::SET_ENTITY_HAS_GRAVITY(this->player, true);
	//ENTITY::SET_ENTITY_COLLISION(this->player, TRUE, TRUE);

	WAIT(100);

	//this->camera = CAM::GET_RENDERING_CAM();

	Vector3 pos, goFrom, goTo;
	int npeds, ngroup, currentBehaviour, task_time, type, radius, min_lenght, time_between_walks, spawning_radius;
	float speed;
	while (myreadLine(f, &pos, &npeds, &ngroup, &currentBehaviour, &speed, 
		&goFrom, &goTo, &task_time, &type, &radius, &min_lenght, 
		&time_between_walks, &spawning_radius) >= 0) {

		if (currentBehaviour == 8) {
			spawn_peds_flow(pos, goFrom, goTo, npeds, ngroup,
				currentBehaviour, task_time, type, radius,
				min_lenght, time_between_walks, spawning_radius, speed);
		}
		else {
			spawn_peds(pos, goFrom, goTo, npeds, ngroup,
				currentBehaviour, task_time, type, radius,
				min_lenght, time_between_walks, spawning_radius, speed);
		}
			
	}
	fclose(f);

	if (moving == 0)
		DatasetAnnotator::setCameraFixed(cCoords, cRot, 0, fov);
	else
		DatasetAnnotator::setCameraMoving(A, B, C, fov);
}

void DatasetAnnotator::spawn_peds_flow(Vector3 pos, Vector3 goFrom, Vector3 goTo, int npeds, int ngroup, int currentBehaviour, 
	int task_time, int type, int radius, int min_lenght, int time_between_walks, int spawning_radius, float speed) {

	Ped ped[100];
	Ped ped_specular[100];
	Vector3 current;
	int i = 0;

	float rnX, rnY;

	if (currentBehaviour == 8) {
		for (int i = 0; i < npeds; i++) {
			ped[i] = PED::CREATE_RANDOM_PED(goFrom.x, goFrom.y, goFrom.z);
			WAIT(100);
		}
		if (DEMO)
			WAIT(500);
		else
			WAIT(2000);
		for (int i = 0; i<npeds; i++) {
			ENTITY::SET_ENTITY_HEALTH(ped[i], 0);
			WAIT(50);
		}
		if (DEMO)
			WAIT(500);
		else
			WAIT(2000);
		for (int i = 0; i < npeds; i++) {
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ped[i]);
			PED::RESURRECT_PED(ped[i]);
			PED::REVIVE_INJURED_PED(ped[i]);
			ENTITY::SET_ENTITY_COLLISION(ped[i], TRUE, TRUE);
			PED::SET_PED_CAN_RAGDOLL(ped[i], TRUE);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped[i], TRUE);
			PED::SET_PED_COMBAT_ATTRIBUTES(ped[i], 1, FALSE);
		}
		if (DEMO)
			WAIT(500);
		else
			WAIT(2000);
		for (int i = 0; i < npeds; i++) {
			ped_specular[i] = PED::CREATE_RANDOM_PED(goTo.x, goTo.y, goTo.z);
			WAIT(100);
		}
		if (DEMO)
			WAIT(500);
		else
			WAIT(2000);
		for (int i = 0; i<npeds; i++) {
			ENTITY::SET_ENTITY_HEALTH(ped_specular[i], 0);
			WAIT(50);
		}
		if (DEMO)
			WAIT(500);
		else
			WAIT(2000);
		for (int i = 0; i<npeds; i++) {
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ped_specular[i]);
			PED::RESURRECT_PED(ped_specular[i]);
			PED::REVIVE_INJURED_PED(ped_specular[i]);
			ENTITY::SET_ENTITY_COLLISION(ped_specular[i], TRUE, TRUE);
			PED::SET_PED_CAN_RAGDOLL(ped_specular[i], TRUE);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped_specular[i], TRUE);
			PED::SET_PED_COMBAT_ATTRIBUTES(ped_specular[i], 1, FALSE);
		}

		
	}
	else if (currentBehaviour == 0) {
		for (int i = 0; i<npeds; i++) {
			ped[i] = PED::CREATE_RANDOM_PED(pos.x, pos.y, pos.z);
			WAIT(50);
		}
		for (int i = 0; i < npeds; i++) {
			ENTITY::SET_ENTITY_HEALTH(ped[i], 0);
			WAIT(50);
		}
		WAIT(500);
		for (int i = 0; i < npeds; i++) {
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ped[i]);
			PED::RESURRECT_PED(ped[i]);
			PED::REVIVE_INJURED_PED(ped[i]);
			ENTITY::SET_ENTITY_COLLISION(ped[i], TRUE, TRUE);
			PED::SET_PED_CAN_RAGDOLL(ped[i], TRUE);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped[i], TRUE);
			PED::SET_PED_COMBAT_ATTRIBUTES(ped[i], 1, FALSE);
		}
	}

	// resurrecting all pedestrians and assigning them a task
	for (int i = 0; i<npeds; i++) {

		WAIT(50);

		current = ENTITY::GET_ENTITY_COORDS(ped[i], TRUE);

		Ped targetPed = ped[0];
		if (npeds > 1)
			targetPed = ped[1];

		Vector3 pp = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_ID(), TRUE);
		
		if (spawning_radius == -1) {
			rnX = (float)(((rand() % 81) - 40) / 10.0);
			rnY = (float)(((rand() % 81) - 40) / 10.0);
		}
		else {
			rnX = (float)((rand() % (spawning_radius * 2)) - spawning_radius);
			rnY = (float)((rand() % (spawning_radius * 2)) - spawning_radius);
		}
		float speed_rnd = (float)(10 + rand() % 4) / 10;
		addwPed(ped[i], coordsToVector(goFrom.x + rnX, goFrom.y + rnY, goFrom.z), coordsToVector(goTo.x + rnX, goTo.y + rnY, goTo.z), time_between_walks, speed_rnd);
		Object seq;
		// waiting time proportional to distance
		float atob = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(goFrom.x, goFrom.y, goFrom.z, goTo.x, goTo.y, goTo.z, 1);
		int max_time = (int)((atob / 2.5) * 1000);

		if (max_time > max_waiting_time)
			max_waiting_time = max_time;

		AI::OPEN_SEQUENCE_TASK(&seq);
		AI::TASK_USE_MOBILE_PHONE_TIMED(0, rand() % max_time);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
		AI::CLOSE_SEQUENCE_TASK(seq);
		AI::TASK_PERFORM_SEQUENCE(ped[i], seq);
		AI::CLEAR_SEQUENCE_TASK(&seq);

		if (spawning_radius != -1) {
			rnX = (float)((rand() % (spawning_radius * 2)) - spawning_radius);
			rnY = (float)((rand() % (spawning_radius * 2)) - spawning_radius);
			speed_rnd = (float)(10 + rand() % 4) / 10;

			WAIT(50);

			addwPed(ped_specular[i], coordsToVector(goTo.x + rnX, goTo.y + rnY, goTo.z), coordsToVector(goFrom.x + rnX, goFrom.y + rnY, goFrom.z), time_between_walks, speed_rnd);

			Object seq2;
			AI::OPEN_SEQUENCE_TASK(&seq2);
			AI::TASK_USE_MOBILE_PHONE_TIMED(0, rand() % max_time);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goTo.x + rnX, goTo.y + rnY, goTo.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::TASK_GO_TO_COORD_ANY_MEANS(0, goFrom.x + rnX, goFrom.y + rnY, goFrom.z, speed_rnd, 0, 0, 786603, 0xbf800000);
			AI::CLOSE_SEQUENCE_TASK(seq2);
			AI::TASK_PERFORM_SEQUENCE(ped_specular[i], seq2);
			AI::CLEAR_SEQUENCE_TASK(&seq2);
		}
	}
}

void DatasetAnnotator::spawn_peds(Vector3 pos, Vector3 goFrom, Vector3 goTo, int npeds, int ngroup, int currentBehaviour,
	int task_time, int type, int radius, int min_lenght, int time_between_walks, int spawning_radius, float speed) {

	Ped ped[100];
	Vector3 current;
	int i = 0;

	int rn;

	for (int i = 0; i < npeds; i++) {
		ped[i] = PED::CREATE_RANDOM_PED(pos.x, pos.y, pos.z);
		WAIT(50);
	}
	for (int i = 0; i < npeds; i++) {
		ENTITY::SET_ENTITY_HEALTH(ped[i], 0);
		WAIT(50);
	}
	WAIT(500);
	for (int i = 0; i < npeds; i++) {
		AI::CLEAR_PED_TASKS_IMMEDIATELY(ped[i]);
		PED::RESURRECT_PED(ped[i]);
		PED::REVIVE_INJURED_PED(ped[i]);
		ENTITY::SET_ENTITY_COLLISION(ped[i], TRUE, TRUE);
		PED::SET_PED_CAN_RAGDOLL(ped[i], TRUE);
		PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped[i], TRUE);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped[i], 1, FALSE);
	}
	

	// resurrecting all pedestrians and assigning them a task
	for (int i = 0; i < npeds; i++) {

		WAIT(50);

		current = ENTITY::GET_ENTITY_COORDS(ped[i], TRUE);

		Ped targetPed = ped[0];
		if (npeds > 1)
			targetPed = ped[1];

		Vector3 pp = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_ID(), TRUE);
		switch (currentBehaviour)
		{
		case 0:
			rn = rand() % 12 + 2;
			if (type == 0) {
				AI::TASK_USE_NEAREST_SCENARIO_TO_COORD(ped[i], current.x, current.y, current.z, 100.0, task_time);
				addwPed_scenario(ped[i]);
			}

			else if (type == 1)
				AI::TASK_START_SCENARIO_IN_PLACE(ped[i], scenarioTypes[rn], 0, true);
			else
				AI::TASK_START_SCENARIO_IN_PLACE(ped[i], scenarioTypes[type], 0, true);
			break;
		case 1:
			AI::TASK_STAND_STILL(ped[i], task_time);
			break;
		case 2:
			AI::TASK_USE_MOBILE_PHONE_TIMED(ped[i], task_time);
			//AI::TASK_START_SCENARIO_AT_POSITION(ped[i], "PROP_HUMAN_SEAT_BENCH", current.x, current.y, current.z, 0, 100000, TRUE, TRUE);
			break;
		case 3:
			AI::TASK_COWER(ped[i], task_time);
			break;
		case 4:
			AI::TASK_WANDER_IN_AREA(ped[i], current.x, current.y, current.z, (float)radius, (float)min_lenght, (float)time_between_walks);
			break;
		case 5:
			if (i > 0)
				AI::TASK_CHAT_TO_PED(ped[i], ped[0], 16, 0.0, 0.0, 0.0, 0.0, 0.0);
			else
				AI::TASK_CHAT_TO_PED(ped[i], targetPed, 16, 0.0, 0.0, 0.0, 0.0, 0.0);
			break;
		case 6:
			if (i > 0)
				AI::TASK_COMBAT_PED(ped[i], ped[0], 0, 16);
			break;
		case 7:
			AI::TASK_STAY_IN_COVER(ped[i]);
			break;

		default:
			break;
		}

	}
}

void DatasetAnnotator::walking_peds()
{
	for (int i = 0; i < nwPeds; i++)
	{
		if (PED::IS_PED_STOPPED(wPeds[i].ped) && !AI::GET_IS_TASK_ACTIVE(wPeds[i].ped, 426))
		{
			int currentTime = (TIME::GET_CLOCK_HOURS()) * 60 + TIME::GET_CLOCK_MINUTES();
			if (wPeds[i].timeFix == -1)
				wPeds[i].timeFix = currentTime;
			if (wPeds[i].timeFix + wPeds[i].stopTime < currentTime)
			{
				wPeds[i].goingTo = !wPeds[i].goingTo;
				wPeds[i].timeFix = -1;
				if (wPeds[i].goingTo)
					AI::TASK_GO_TO_COORD_ANY_MEANS(wPeds[i].ped, wPeds[i].to.x, wPeds[i].to.y, wPeds[i].to.z, wPeds[i].speed, 0, 0, 786603, 0xbf800000);
				else
					AI::TASK_GO_TO_COORD_ANY_MEANS(wPeds[i].ped, wPeds[i].from.x, wPeds[i].from.y, wPeds[i].from.z, wPeds[i].speed, 0, 0, 786603, 0xbf800000);
			}
		}
	}
}
