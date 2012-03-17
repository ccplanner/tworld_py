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
	PyErr_Clear();
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
	return Py_BuildValue("i", 1);
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

	/* tile values*/
	PyModule_AddIntConstant(m, "Empty"	, Empty );
	PyModule_AddIntConstant(m, "Ice"	, Ice );
	PyModule_AddIntConstant(m, "Gravel"	, Gravel );
	PyModule_AddIntConstant(m, "Dirt"	, Dirt );
	PyModule_AddIntConstant(m, "Water"	, Water );
	PyModule_AddIntConstant(m, "Fire"	, Fire );
	PyModule_AddIntConstant(m, "Bomb"	, Bomb );
	PyModule_AddIntConstant(m, "HintButton"	, HintButton );
	PyModule_AddIntConstant(m, "Wall"	, Wall );
	PyModule_AddIntConstant(m, "PopupWall"	, PopupWall );
	PyModule_AddIntConstant(m, "ICChip"	, ICChip );
	PyModule_AddIntConstant(m, "Block"	, Block );
	PyModule_AddIntConstant(m, "Socket"	, Socket );
	PyModule_AddIntConstant(m, "Exit"	, Block );
	PyModule_AddIntConstant(m, "Chip_N"	, Chip );
	PyModule_AddIntConstant(m, "Chip_W"	, Chip + 1 );
	PyModule_AddIntConstant(m, "Chip_S"	, Chip + 2 );
	PyModule_AddIntConstant(m, "Chip_E"	, Chip + 3 );
}
