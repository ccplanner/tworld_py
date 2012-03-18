/*
 * Harry Bullen    <hbullen@i-a-i.com>
 */

#include <Python.h>
#include "oshw.h"
#include "defs.h"
#include "state.h"

// define main function
extern int tworld(int argc, char *argv[]);

extern gamestate state;

static PyObject *agent_callback = NULL;

/*
 * CmdNone
 * CmdNorth
 * CmdWest
 * CmdSouth
 * CmdEast
 * CmdQuitLevel
 */
int
do_move( )
{
    int move;
    PyObject *result;
    PyObject *arglist;
    result = PyObject_CallObject(agent_callback, NULL);
    if ( result == NULL ) {
	// Really we should find a way to return this error
	// XXX now we crash thru to the end
	// PyErr_Clear();
	return CmdQuitLevel; // Error so quit
    }
    move = PyInt_AsLong(result);
    // TODO check for over flow
    Py_DECREF(result);
    return move;
}

static PyObject *
get_tile(PyObject *self, PyObject *args)
{
    int x, y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y ) )
	return NULL;
    // TODO: check that game is running
    mapcell cell = state.map[y * CXGRID + x];
    return Py_BuildValue("(i,i)", cell.top.id, cell.bot.id);
}

static PyObject *
chips_needed(PyObject *self, PyObject *args)
{
    // arguments are null
    // TODO: check that game is running
    return Py_BuildValue("i", state.chipsneeded);
}

static PyObject *
get_boots(PyObject *self, PyObject *args)
{
    // arguments are null
    // TODO: check that game is running
    return Py_BuildValue("(b,b,b,b)", iceboots(state), slideboots(state), 
		    fireboots(state), waterboots(state));
}

static PyObject *
get_keys(PyObject *self, PyObject *args)
{
    // arguments are null
    // TODO: check that game is running
    return Py_BuildValue("(i,i,i,i)", redkeys(state), bluekeys(state), 
		    yellowkeys(state), greenkeys(state));
}

static PyObject *
time_left(PyObject *self, PyObject *args)
{
    // arguments are null
    // TODO: check that game is running
    return Py_BuildValue("i", state.timelimit - state.currenttime);
}

static PyObject *
view_pos(PyObject *self, PyObject *args)
{
    // arguemnts are null
    // TODO: check that game is running
    // divide by 8 to make the viewpos line up with chips real position 
    return Py_BuildValue("(i,i)", state.xviewpos / 8 , state.yviewpos / 8);
}

static PyObject *
set_agent(PyObject *dummy, PyObject *args)
{
    PyObject *result = NULL;
    PyObject *temp;

    if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
	if (!PyCallable_Check(temp)) {
	    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
	    return NULL;
	}

	Py_XINCREF(temp);            /* Add a reference to new callback */
	Py_XDECREF(agent_callback);  /* Dispose of previous callback */
	agent_callback = temp;       /* Remember new callback */
	/* Boilerplate to return "None" */
	Py_INCREF(Py_None);
	result = Py_None;
    }
    return result;
}

static PyObject *
load_level(PyObject *self, PyObject *args)
{
	int	     level;
	char	    *level_str = (char *)malloc(40);
	const char  *file_name;

	// Full specification of comand line args (crashes)
	char* argv[] = {"tworld2", "cc-ms.dac", "2", "-p", NULL };
	const int argc = 4;
	const int file_arg = 1;
	const int level_arg = 2;

	if (!PyArg_ParseTuple(args, "si", &file_name, &level) )
	    return NULL;
	// set file name and level
	argv[file_arg] = file_name;

	sprintf(level_str, "%d", level);
	argv[level_arg] = level_str;

	oshw_main(argc, argv);
	free( level_str);
	//return Py_BuildValue("i", 1);
	// always crash
	return NULL;
}

static PyMethodDef TileWorldMethods[] = {
	{"load_level",  load_level, METH_VARARGS,
	 "Load a level to play with python code:\n"
	 " File name of *.dac file\n"
	 " Level number\n"},
	{"set_agent",  set_agent, METH_VARARGS,
	 "Set the agent call back function\n"},
	{"chips_needed", chips_needed, METH_VARARGS,
	 "How many chips are needed to exit\n"},
	{"get_tile",  get_tile, METH_VARARGS,
	 "Return a tuple (top, bot) of what is at location (x,y)\n"},
	{"get_keys",  get_keys, METH_VARARGS,
	 "Return a tuple (redkeys, bluekeys, yellowkeys, greenkeys) that chip has.\n"},
	{"get_boots",  get_boots, METH_VARARGS,
	 "Return a tuple (ice, suction, fire, water) boot status (True, False).\n"},
	{"time_left",  time_left, METH_VARARGS,
	 "Returns how many ticks are left in the game.\n"},
	{"chips_pos",  view_pos, METH_VARARGS,
	 "Return a tuple (x, y) of the view position\n"},
	{NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
inittworld(void)
{
	PyObject * m; //the module
	m = Py_InitModule("tworld", TileWorldMethods);
	PyModule_AddIntConstant(m, "NORTH", CmdNorth );
	PyModule_AddIntConstant(m, "EAST" , CmdEast );
	PyModule_AddIntConstant(m, "WEST" , CmdWest );
	PyModule_AddIntConstant(m, "SOUTH", CmdSouth );
	PyModule_AddIntConstant(m, "WAIT" , CmdNone );

	const int north_offset = 0;
	const int west_offset  = 1;
	const int south_offset = 2;
	const int east_offset  = 3;
	/* tile values*/
	PyModule_AddIntConstant(m, "Nothing",		Nothing);   // Not used
	PyModule_AddIntConstant(m, "Empty",		Empty);	    // Floor to walk on
	PyModule_AddIntConstant(m, "Slide_North",	Slide_North);
	PyModule_AddIntConstant(m, "Slide_West",	Slide_West);
	PyModule_AddIntConstant(m, "Slide_South",	Slide_South);
	PyModule_AddIntConstant(m, "Slide_East",	Slide_East);
	PyModule_AddIntConstant(m, "Slide_Random",	Slide_Random);
	PyModule_AddIntConstant(m, "Ice",		Ice);
	PyModule_AddIntConstant(m, "IceWall_Northwest",	IceWall_Northwest);
	PyModule_AddIntConstant(m, "IceWall_Northeast",	IceWall_Northeast);
	PyModule_AddIntConstant(m, "IceWall_Southwest",	IceWall_Southwest);
	PyModule_AddIntConstant(m, "IceWall_Southeast",	IceWall_Southeast);
	PyModule_AddIntConstant(m, "Gravel",		Gravel);
	PyModule_AddIntConstant(m, "Dirt",		Dirt);
	PyModule_AddIntConstant(m, "Water",		Water);
	PyModule_AddIntConstant(m, "Fire",		Fire);
	PyModule_AddIntConstant(m, "Bomb",		Bomb);
	PyModule_AddIntConstant(m, "Beartrap",		Beartrap);
	PyModule_AddIntConstant(m, "Burglar",		Burglar);
	PyModule_AddIntConstant(m, "HintButton",	HintButton);
	PyModule_AddIntConstant(m, "Button_Blue",	Button_Blue);
	PyModule_AddIntConstant(m, "Button_Green",	Button_Green);
	PyModule_AddIntConstant(m, "Button_Red",	Button_Red);
	PyModule_AddIntConstant(m, "Button_Brown",	Button_Brown);
	PyModule_AddIntConstant(m, "Teleport",		Teleport);
	PyModule_AddIntConstant(m, "Wall",		Wall);
	PyModule_AddIntConstant(m, "Wall_North",	Wall_North);
	PyModule_AddIntConstant(m, "Wall_West",		Wall_West);
	PyModule_AddIntConstant(m, "Wall_South",	Wall_South);
	PyModule_AddIntConstant(m, "Wall_East",		Wall_East);
	PyModule_AddIntConstant(m, "Wall_Southeast",	Wall_Southeast);
	PyModule_AddIntConstant(m, "HiddenWall_Perm",	HiddenWall_Perm);
	PyModule_AddIntConstant(m, "HiddenWall_Temp",	HiddenWall_Temp);
	PyModule_AddIntConstant(m, "BlueWall_Real",	BlueWall_Real);
	PyModule_AddIntConstant(m, "BlueWall_Fake",	BlueWall_Fake);
	PyModule_AddIntConstant(m, "SwitchWall_Open",	SwitchWall_Open);
	PyModule_AddIntConstant(m, "SwitchWall_Closed",	SwitchWall_Closed);
	PyModule_AddIntConstant(m, "PopupWall",		PopupWall);
	PyModule_AddIntConstant(m, "CloneMachine",	CloneMachine);
	PyModule_AddIntConstant(m, "Door_Red",		Door_Red);
	PyModule_AddIntConstant(m, "Door_Blue",		Door_Blue);
	PyModule_AddIntConstant(m, "Door_Yellow",	Door_Yellow);
	PyModule_AddIntConstant(m, "Door_Green",	Door_Green);
	PyModule_AddIntConstant(m, "Socket",		Socket);
	PyModule_AddIntConstant(m, "Exit",		Exit);
	PyModule_AddIntConstant(m, "ICChip",		ICChip);
	PyModule_AddIntConstant(m, "Key_Red",		Key_Red);
	PyModule_AddIntConstant(m, "Key_Blue",		Key_Blue);
	PyModule_AddIntConstant(m, "Key_Yellow",	Key_Yellow);
	PyModule_AddIntConstant(m, "Key_Green",		Key_Green);
	PyModule_AddIntConstant(m, "Boots_Ice",		Boots_Ice);
	PyModule_AddIntConstant(m, "Boots_Slide",	Boots_Slide);
	PyModule_AddIntConstant(m, "Boots_Fire",	Boots_Fire);
	PyModule_AddIntConstant(m, "Boots_Water",	Boots_Water);
	PyModule_AddIntConstant(m, "Block_Static",	Block_Static);
	PyModule_AddIntConstant(m, "Drowned_Chip",	Drowned_Chip);
	PyModule_AddIntConstant(m, "Burned_Chip",	Burned_Chip);
	PyModule_AddIntConstant(m, "Bombed_Chip",	Bombed_Chip);
	PyModule_AddIntConstant(m, "Exited_Chip",	Exited_Chip);
	PyModule_AddIntConstant(m, "Exit_Extra_1",	Exit_Extra_1);
	PyModule_AddIntConstant(m, "Exit_Extra_2",	Exit_Extra_2);
	PyModule_AddIntConstant(m, "Overlay_Buffer",	Overlay_Buffer);
	PyModule_AddIntConstant(m, "Floor_Reserved2",	Floor_Reserved2);
	PyModule_AddIntConstant(m, "Floor_Reserved1",	Floor_Reserved1);
	PyModule_AddIntConstant(m, "Chip_North",	Chip + north_offset);
	PyModule_AddIntConstant(m, "Chip_West",		Chip + west_offset);
	PyModule_AddIntConstant(m, "Chip_South",	Chip + south_offset);
	PyModule_AddIntConstant(m, "Chip_East",		Chip + east_offset);
	PyModule_AddIntConstant(m, "Block_North",	Block + north_offset);
	PyModule_AddIntConstant(m, "Block_West",	Block + west_offset);
	PyModule_AddIntConstant(m, "Block_South",	Block + south_offset);
	PyModule_AddIntConstant(m, "Block_East",	Block + east_offset);
	PyModule_AddIntConstant(m, "Tank_North",	Tank + north_offset);
	PyModule_AddIntConstant(m, "Tank_West",		Tank + west_offset);
	PyModule_AddIntConstant(m, "Tank_South",	Tank + south_offset);
	PyModule_AddIntConstant(m, "Tank_East",		Tank + east_offset);
	PyModule_AddIntConstant(m, "Ball_North",	Ball + north_offset);
	PyModule_AddIntConstant(m, "Ball_West",		Ball + west_offset);
	PyModule_AddIntConstant(m, "Ball_South",	Ball + south_offset);
	PyModule_AddIntConstant(m, "Ball_East",		Ball + east_offset);
	PyModule_AddIntConstant(m, "Glider_North",	Glider + north_offset);
	PyModule_AddIntConstant(m, "Glider_West",	Glider + west_offset);
	PyModule_AddIntConstant(m, "Glider_South",	Glider + south_offset);
	PyModule_AddIntConstant(m, "Glider_East",	Glider + east_offset);
	PyModule_AddIntConstant(m, "Fireball_North",	Fireball + north_offset);
	PyModule_AddIntConstant(m, "Fireball_West",	Fireball + west_offset);
	PyModule_AddIntConstant(m, "Fireball_South",	Fireball + south_offset);
	PyModule_AddIntConstant(m, "Fireball_East",	Fireball + east_offset);
	PyModule_AddIntConstant(m, "Walker_North",	Walker + north_offset);
	PyModule_AddIntConstant(m, "Walker_West",	Walker + west_offset);
	PyModule_AddIntConstant(m, "Walker_South",	Walker + south_offset);
	PyModule_AddIntConstant(m, "Walker_East",	Walker + east_offset);
	PyModule_AddIntConstant(m, "Blob_North",	Blob + north_offset);
	PyModule_AddIntConstant(m, "Blob_West",		Blob + west_offset);
	PyModule_AddIntConstant(m, "Blob_South",	Blob + south_offset);
	PyModule_AddIntConstant(m, "Blob_East",		Blob + east_offset);
	PyModule_AddIntConstant(m, "Teeth_North",	Teeth + north_offset);
	PyModule_AddIntConstant(m, "Teeth_West",	Teeth + west_offset);
	PyModule_AddIntConstant(m, "Teeth_South",	Teeth + south_offset);
	PyModule_AddIntConstant(m, "Teeth_East",	Teeth + east_offset);
	PyModule_AddIntConstant(m, "Bug_North",		Bug + north_offset);
	PyModule_AddIntConstant(m, "Bug_West",		Bug + west_offset);
	PyModule_AddIntConstant(m, "Bug_South",		Bug + south_offset);
	PyModule_AddIntConstant(m, "Bug_East",		Bug + east_offset);
	PyModule_AddIntConstant(m, "Paramecium_North",	Paramecium + north_offset);
	PyModule_AddIntConstant(m, "Paramecium_West",	Paramecium + west_offset);
	PyModule_AddIntConstant(m, "Paramecium_South",	Paramecium + south_offset);
	PyModule_AddIntConstant(m, "Paramecium_East",	Paramecium + east_offset);
	PyModule_AddIntConstant(m, "Swimming_Chip_North",	Swimming_Chip + north_offset);
	PyModule_AddIntConstant(m, "Swimming_Chip_West",	Swimming_Chip + west_offset);
	PyModule_AddIntConstant(m, "Swimming_Chip_South",	Swimming_Chip + south_offset);
	PyModule_AddIntConstant(m, "Swimming_Chip_East",	Swimming_Chip + east_offset);
	PyModule_AddIntConstant(m, "Pushing_Chip_North",	Pushing_Chip + north_offset);
	PyModule_AddIntConstant(m, "Pushing_Chip_West",		Pushing_Chip + west_offset);
	PyModule_AddIntConstant(m, "Pushing_Chip_South",	Pushing_Chip + south_offset);
	PyModule_AddIntConstant(m, "Pushing_Chip_East",		Pushing_Chip + east_offset);
	PyModule_AddIntConstant(m, "Entity_Reserved2",		Entity_Reserved2);
	PyModule_AddIntConstant(m, "Entity_Reserved1",		Entity_Reserved1);
	PyModule_AddIntConstant(m, "Water_Splash",		Water_Splash);
	PyModule_AddIntConstant(m, "Bomb_Explosion",		Bomb_Explosion);
	PyModule_AddIntConstant(m, "Entity_Explosion",		Entity_Explosion);
	PyModule_AddIntConstant(m, "Animation_Reserved1",	Animation_Reserved1);
}
