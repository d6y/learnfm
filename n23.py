from typing import NamedTuple
import dx7

class Patch(NamedTuple):
    op1Level: int
    op1Coarse: int
    op1Fine: int
    op2Level: int
    op2Coarse: int
    op2Fine: int
    feedback: int
    name: str

patches = [
 Patch( op1Level=99, op1Coarse=1, op1Fine=0, op2Level=99, op2Coarse=1, op2Fine=0, feedback=4, name="n23_01"),
 Patch( op1Level=99, op1Coarse=1, op1Fine=0, op2Level=90, op2Coarse=2, op2Fine=0, feedback=4, name="n23_02"),
]

dx7.write_patch("1.wav", 1, 0, 99, 1, 0, 99, 4, "TEST1 6789")
dx7.write_patch("2.wav", 1, 0, 99, 2, 0, 90, 4, "TEST2 6789")
