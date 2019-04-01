# JTA Mods (beta)
This repository contains two Grand Theft Auto V Mods used for creating the [JTA Dataset](http://imagelab.ing.unimore.it/jta) presented in the paper [Learning to Detect and Track Visible and Occluded Body Joints in a Virtual World](https://arxiv.org/abs/1803.08319):
- ScenarioCreator: lets you create your own scenarios that should appear in your dataset
- DatasetAnnotator: records the scenarios previously created and store the annotations

## Quick start
- Download and install a legal copy of Grand Theft Auto V
- Download ScriptHook V AND the SDK [here](http://www.dev-c.com/gtav/scripthookv/). 
    - Follow the instructions reported on the web page. Also, follow the instructions of the SDK readme under the section "Runtime asi script reloading"
- Download the ScenarioCreator Mod: [`ScenarioCreator.asi`](https://drive.google.com/file/d/1ngV49QzDRmVL0KvewW_Fs0vklJNIM2K6/view?usp=sharing)
- Download the DatasetAnnotator Mod: [`DatasetAnnotator.asi`](https://drive.google.com/file/d/1kY6VxOHfSVc6vSC08FR5l4acv_PEo_i_/view?usp=sharing)
- Copy both files `ScenarioCreator.asi` and `DatasetAnnotator.asi` in the Grand Theft Auto V game folder.
- If you installed GTA V using Rockstar's Social Club, you have to change the permissions of the game folder (read, write and execute). If you used Steam, everything should be fine.
- Run Grand Theft Auto V and select Story Mode. If it is your first start of the game, you have to complete the first mission. Enjoy!
- The mods will automatically start. Now, by pressing F5 you can use the ScenarioCreator mod to create your own scenarios. By pressing F8 the DatasetAnnotator mod will start recording the scenarios. You can disable the Mods in any time by pressing ctrl+R.
- Remember to disable the HUD in the display settings of GTA V. Also disable the in-game notifications.


## Developer Guide
- Download and install a legal copy of Grand Theft Auto V
- Clone this repository
- Download ScriptHook V AND the SDK here http://www.dev-c.com/gtav/scripthookv/. Follow the instructions reported on the web page. Also, follow the instructions of the SDK readme under the section "Runtime asi script reloading"
- Open the Solution with Visual Studio (we used VS2017, but previous versions should work too) and, for each sub-project, follow these instructions:
	- include the header files to the project by going in Configuration Properties->C/C++->General and set the path to the "inc" directory of the SDK in "Additional Include Directories"
	- include the library to the projects by going to Configuration Properties->Linker->General  and set the path that points to the "lib" folder of the SDK in "Additional Library Directories" 
	- Also go to:  `Configuration Properties -> Linker -> Input`  and set the `ScriptHookV.lib` file name in `Additional Dependencies`
	- Go in `Build-Events -> Post-Build-Event` and, under `Command Line` type `xcopy /Y path/to/asi/file path/to/gta/installation/directory`
	- Under `Configuration-Properties -> General` change the `Target Extension` to `.asi` and `Configuration Type` to `.dll`
- Build the entire solution.
- If you installed GTA V using Rockstar's Social Club, you have to change the permissions of the game folder (read, write and execute). If you used Steam, everything should be fine.
- Run Grand Theft Auto V and select Story Mode. If it is your first start of the game, you have to complete the first mission. Enjoy!
- The mods will automatically start. Now, by pressing F5 you can use the ScenarioCreator mod to create your own scenarios. By pressing F8 the DatasetAnnotator mod will start recording the scenarios. You can disable the Mods in any time by pressing ctrl+R.
- Remember to disable the HUD in the display settings of GTA V. Also disable the in-game notifications.


## ScenarioCreator usage
- By pressing F5 the mod will prompt the menu that can be easily browsed using the numpad. 
- The peds submenu let you spawn new pedestrian with different behaviors.
- In the camera submenu, you can handle the camera position. 
	- The only important thing is the moving option. By enabling it you can define a path A B C that the camera must follow.
	- The teleport 1 and teleport 2 locations must be placed near the scenario; this gives time to the game engine to spawn regular pedestrians.
- After the scenario is complete, move the player to a specific location (for non-moving sequences, the current camera position will be the camera position of your scenario). By pressing save new in the file submenu you will save the configuration file.
- For a new sequence, you have to clear the log data by pressing the relative button.

## DatasetAnnotator usage
By pressing F8 the mod will automatically load each scenario previously created and starts to record the dataset. You can stop the mod at any time by pressing ctrl+R. The data will be stored in a subdirectory of the GTA V game folder named `JTA`. For each sequence a new folder will be created, containing each recorded frames and a .csv annotation file.

* _Note_ #1: each scenario will be recorded two times: one during the day and one during the night.


### Annotations 

Each annotation file refers to a specific sequence. An annotation consists of a .csv file containing, for each row, the information about a single joint, organized as follows:

| Name          | Description                                                   |
| ------------- | ------------------------------------------------------------- |
| frame         | number of the frame to which the joint belongs                |
| pedestrian_id | unique identifier of the person to which the joint belongs    |
| joint_type    | identifier of the type of joint; see 'Joint Types' subsection |
| x2D           | 2D _x_ coordinate of the joint in pixels                      |
| y2D           | 2D _y_ coordinate of the joint in pixels                      |
| x3D           | 3D _x_ coordinate of the joint in meters                      |
| y3D           | 3D _y_ coordinate of the joint in meters                      |
| z3D           | 3D _z_ coordinate of the joint in meters                      |
| occluded      | `1` if the joint is occluded; `0` otherwise                   |
| self_occluded | `1` if the joint is occluded by its owner; `0` otherwise      |
| cam_3D_x      | 3D _x_ coordinate of the camera in meters                     |
| cam_3D_y      | 3D _y_ coordinate of the camera in meters                     |
| cam_3D_z      | 3D _z_ coordinate of the camera in meters                     |
| cam_rot_x     | _x_ rotation of the camera in degrees                         |
| cam_rot_y     | _y_ rotation of the camera in degrees                         |
| cam_rot_z     | _z_ rotation of the camera in degrees                         |
| fov           | field of view of the camera in degrees                        |


* _Note_ #1: 3D coordinates are relative to the game world origin.
* _Note_ #2: In some circumstances, the information of the frame 0 may be incorrect. Please carefully handle this.

### Joint Types

The associations between numerical identifier and type of joint are the following:

```
 0: head_top
 1: head_center
 2: neck
 3: right_clavicle
 4: right_shoulder
 5: right_elbow
 6: right_wrist
 7: left_clavicle
 8: left_shoulder
 9: left_elbow
10: left_wrist
11: spine0
12: spine1
13: spine2
14: spine3
15: spine4
16: right_hip
17: right_knee
18: right_ankle
19: left_hip
20: left_knee
21: left_ankle
```

## Citation

We believe in open research and we are happy if you find this code useful.   
If you use it, please cite our [work](https://arxiv.org/abs/1803.08319).

```latex
@inproceedings{fabbri2018learning,
   title     = {Learning to Detect and Track Visible and Occluded Body Joints in a Virtual World},
   author    = {Fabbri, Matteo and Lanzi, Fabio and Calderara, Simone and Palazzi, Andrea and Vezzani, Roberto and Cucchiara, Rita},
   booktitle = {European Conference on Computer Vision (ECCV)},
   year      = {2018}
 }
```



