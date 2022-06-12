# learnfm

For N23:

Python set up:

```
conda env create -f environment.yml
conda activate learnfm
```

Compile:

```
cd dx7core
make
python setup.py install 
```

Example in the root directory of this project:

```
python n23.py
```


## Build the Python & C simulator

```
cd dx7core
make
./dx7 # a test, will create /tmp/foo.wav with all the patches in order if you want
```

It will sound like https://soundcloud.com/bwhitman/the-31380-unique-voices-of-the-dx7


To build the python module, first edit the end of pydx7.cc with your location of `compact.bin` (I will fix this soon, sorry)
Then:

```
python setup.py install # will create a python module called dx7
```

The python module renders a mono 44,100Hz 16-bit signed int sound from a patch number (0-31379), a midi note, a velocity and a sample length along with the key up sample position.

```
>>> import dx7
>>> patch_number = 324 
>>> midi_note = 60
>>> velocity = 99
>>> samples = 44100 * 10 
>>> keyup_sample = 44100 * 5 # when to let the key up
>>> data = dx7.render(patch_number, midi_note, velocity, samples, keyup_sample)
```

## Creating your own database

If you want your own patch database, download DX7 sysex patches (I used http://dxsysex.com/ ) and put them in a folder called `patches/`. Then run 

```
python dx7db.py
```

Will create a database called `compact.bin` and `names.txt` with all of the unique voices from all the banks in the patches (I have 31,380.) 
The unique-finding-algorithm looks at the data in the voice, not the name. Many voices have different names but the same voice information.
