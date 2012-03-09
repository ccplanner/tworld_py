/*
 * Harry Bullen    <hbullen@i-a-i.com>
 */

#include <Python.h>
#include "oshw.h"

// define main function
int tworld(int argc, char *argv[]);

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
	{NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
inittworld(void)
{
	PyObject * m; //the module
	m = Py_InitModule("tworld", TileWorldMethods);
}
