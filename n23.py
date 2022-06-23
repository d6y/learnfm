from typing import NamedTuple
import dx7

dx7.write_patch("1.wav", 1, 0, 99, 1, 0, 99, 4, "TEST1 6789")
dx7.write_patch("2.wav", 1, 0, 99, 2, 0, 90, 4, "TEST2 6789")
