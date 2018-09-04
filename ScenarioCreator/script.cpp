/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#include "scenario.h"
#include <string.h>

DWORD	vehUpdateTime;
DWORD	pedUpdateTime;
using namespace std;


void main() {
	ScenarioCreator scenario;

	while (true) {
		scenario.update();
		WAIT(0);
	}
}


void ScriptMain() {
	srand(GetTickCount());
	main();
}
