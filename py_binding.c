/*
 * Harry Bullen    <hbullen@i-a-i.com>
 */

#include <Python.h>
#include "oshw.h"
#include "defs.h"

// define main function
extern int tworld(int argc, char *argv[]);

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
	int level;
	const char *file_name;
	// Full specification of comand line args (crashes)
	//char* argv[] = {"tworld2", "CCLP2.dac", "foo.tws", "2", NULL };
	char* argv[] = { NULL };
	int argc = 0;

	if (!PyArg_ParseTuple(args, "si", &file_name, &level ) )
		return NULL;

	oshw_main(argc, argv);
	return Py_BuildValue("i", 1);
}

static PyMethodDef TileWorldMethods[] = {
	{"load_level",  load_level, METH_VARARGS,
	 "Load a level to play with python code:\n"
	 " File name of *.dat file\n"
	 " Level number\n"},
	{"set_agent",  set_agent, METH_VARARGS,
	 "Set the agent call back function\n"},
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
}
