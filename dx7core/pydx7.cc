
#include <Python.h>
#include <cstdlib>
#include <math.h>
#include "synth.h"
#include "module.h"
#include "aligned_buf.h"
#include "freqlut.h"
#include "wavout.h"
#include "sawtooth.h"
#include "sin.h"
#include "exp2.h"
#include "log2.h"
#include "resofilter.h"
#include "fm_core.h"
#include "fm_op_kernel.h"
#include "env.h"
#include "patch.h"
#include "n23.h"
#include "controllers.h"
#include "dx7note.h"

char *unpacked_patches;

void write_data(const int32_t *buf_in, short *buf_out, unsigned int *pos, int n)
{
  int32_t delta = 0x100;
  for (int i = 0; i < n; i++)
  {
    int32_t val = buf_in[i];
    int clip_val = val < -(1 << 24) ? 0x8000 : (val >= (1 << 24) ? 0x7fff : (val + delta) >> 9);
    delta = (delta + val) & 0x1ff;
    buf_out[*pos + i] = clip_val;
  }
  *pos = *pos + n;
}

// take in a patch #, a note # and a velocity and a sample length and a sample # to lift off a key?
short *render(unsigned short patch, unsigned char midinote, unsigned char velocity, unsigned int samples, unsigned int keyup)
{
  Dx7Note note;
  short *out = (short *)malloc(sizeof(short) * samples);
  unsigned int out_ptr = 0;
  note.init(unpacked_patches + (patch * 156), midinote, velocity);
  Controllers controllers;
  controllers.values_[kControllerPitch] = 0x2000;
  int32_t buf[N];

  for (int i = 0; i < samples; i += N)
  {
    for (int j = 0; j < N; j++)
    {
      buf[j] = 0;
    }
    if (i >= keyup)
    {
      note.keyup();
    }
    note.compute(buf, 0, 0, &controllers);
    for (int j = 0; j < N; j++)
    {
      buf[j] >>= 2;
    }
    write_data(buf, out, &out_ptr, N);
  }
  return out;
}

void init_synth(void)
{
  double sample_rate = 44100.0;
  Freqlut::init(sample_rate);
  Sawtooth::init(sample_rate);
  Sin::init();
  Exp2::init();
  Log2::init();
}

static PyObject *write_patch_wrapper(PyObject *self, PyObject *args)
{
  // printf("Made it into wrapper\n");

  char *filename;
  int coarse1;
  int fine1;
  int vol1;
  int coarse2;
  int fine2;
  int vol2;
  int feedback;
  char *name;

  if (!PyArg_ParseTuple(args, "siiiiiiis", &filename, &coarse1, &fine1, &vol1, &coarse2, &fine2, &vol2, &feedback, &name))
  {
    printf("Failed to parse args\n");
    return NULL;
  }
  else
  {
    int name_len = strlen(name);
    if (name_len != 10)
    {
      printf("Wrong length for name: %d", name_len);
      return NULL;
    }
    write_patch(filename, coarse1, fine1, vol1, coarse2, fine2, vol2, feedback, name);
  }

  return PyUnicode_FromString(filename);
}

static PyObject *render_wrapper(PyObject *self, PyObject *args)
{
  int arg1, arg2, arg3, arg4, arg5;

  /* Default values. */
  arg1 = 8;     // patch #
  arg2 = 50;    // midi note
  arg3 = 70;    // velocity
  arg4 = 44100; // samples
  arg5 = 22050; // keyup sample
  if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
  {
    return NULL;
  }
  short *result;
  result = render(arg1, arg2, arg3, arg4, arg5);

  // Create a python list of ints (they are signed shorts that come back)
  PyObject *ret = PyList_New(arg4); // arg4 is samples
  for (int i = 0; i < arg4; ++i)
  {
    PyObject *python_int = Py_BuildValue("i", result[i]);
    PyList_SetItem(ret, i, python_int);
  }
  free(result);
  return ret;
}
// return one patch unpacked for sysex
static PyObject *unpack_wrapper(PyObject *self, PyObject *args)
{
  int arg1 = 8; // patch #
  if (!PyArg_ParseTuple(args, "i", &arg1))
  {
    return NULL;
  }
  PyObject *ret = PyList_New(155);
  for (int i = 0; i < 155; i++)
  {
    PyObject *python_int = Py_BuildValue("i", unpacked_patches[arg1 * 156 + i]);
    PyList_SetItem(ret, i, python_int);
  }
  return ret;
}

static PyMethodDef DX7Methods[] = {
    {"render", render_wrapper, METH_VARARGS, "Render audio"},
    {"unpack", unpack_wrapper, METH_VARARGS, "Unpack patch"},
    {"write_patch", write_patch_wrapper, METH_VARARGS, "Write N23 patch as WAV"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef dx7Def =
    {
        PyModuleDef_HEAD_INIT,
        "dx7", /* name of module */
        "",    /* module documentation, may be NULL */
        -1,    /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        DX7Methods};

extern "C"
{
  PyMODINIT_FUNC PyInit_dx7(void)
  {
    // init_synth();
    //   TODO the anove may be useful to do once? If that's good enough when chnging patches?
    return PyModule_Create(&dx7Def);
  }
}
