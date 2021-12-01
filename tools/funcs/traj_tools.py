#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Copyright © 2021 Ali Farnudi.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

This module contains tools to read xyz files generated by VCM.
"""

import numpy as np
import sys
import mmap


def load_bin_traj(bin_name):
    """Read binary trajectories generated by VCM.

    VCM generated binary trajectories are either saved with single precision or
    double percision. The x, y, and z (or vx, vy, and vz for the velocity
    binary file) coordinates of the atoms are saved one after the other. Atom
    labels are not saved. There is no mark to distinguish when coordinates of
    one frame ends and another begins.

    Parameters
    ----------
    bin_name : str
        Path to the binary file
        
        Path to the xyz trajectory or velocity component binary file.

    Returns
    -------
    data: numpy array
        2d numpuy (N by 3) array of floats. N is the totoal number of atoms in
        a simulation times the number of saved frames.
    
    Raises
    ------
    Exception
        If bin file extension is not supported.
    """

    extension = bin_name.split(".")[-1]
    error_message = f"""Cannot parse "{extension}" extension. Only 
bin_xyz(vel)_single(double) extensions allowed."""
    if "vel" in extension or "xyz" in extension:
        if extension.endswith("single"):
            data = np.fromfile(bin_name, dtype=np.float32, sep="")
        elif extension.endswith("double"):
            data = np.fromfile(bin_name, dtype=np.float64, sep="")
        else:
            raise Exception(error_message)
        len_file = data.shape[0]
        data = data.reshape(len_file // 3, 3)
        return data
    else:
        raise Exception(error_message)


def _get_number_of_atoms(file, label=None):
    data_map = _get_memory_map_to_file(file)
    line = data_map.readline()
    if not line:
        raise Exception("The first line appears to be blank.")
    number_of_atoms = int(line)
    if label:
        number_of_labeled_atoms = 0
        line = data_map.readline()
        if not line:
            raise Exception("The second line appears to be blank.")
        for i in range(number_of_atoms):
            line = data_map.readline()
            if not line:
                raise Exception(f"line {i} appears to be blank.")
            words = line.split()
            if words[0].decode("utf-8") == label:
                number_of_labeled_atoms += 1
        if number_of_labeled_atoms == 0:
            raise Exception(
                f'Could not find atom with label "{label}" in the xyz file.'
            )
        number_of_atoms = number_of_labeled_atoms
    return number_of_atoms


def _get_memory_map_to_file(file):
    try:
        data_map = mmap.mmap(file.fileno(), 0, offset=0, prot=mmap.PROT_READ)
    except Exception as e:
        print(e)
        raise
    return data_map


def load_xyz(file_name, label=None):
    """Read xyz trajectories generated by VCM.
    
    Load xyz coordinates and simulation runtime information generated by VCM.
    

    Parameters
    ----------
    file_name : str
        Path to the xyz file
    label: str
        The lable of the Atom coordinates that is extracted. If None (default), 
        all atom coords will be imported.
    
    Returns
    -------
    coords: numpy array
        3D numpuy of floats with shape (N_frame, N_atom, 3). N_frame is the 
        number of frames and N_atom is the number of atoms in the xyz file. 
    
    time: numpy array
        1D numpy array of floats with shape (N_frame).
    
    energies:
        2D numpy array of floats with shape (N_frame, 2). The first column 
        is the total energy (kinetic + potential) and the second column
        is the total potential energy of the atom configuration in each frame.
    
    Raises
    ------
    Exception
        If the xyz file format is not standard or corrupted.
    """
    coords = []
    energies = []
    time = []

    with open(file_name, "r+", encoding="utf-8") as f:

        number_of_atoms = _get_number_of_atoms(f)
        data_map = _get_memory_map_to_file(f)

        frame_count = 0
        while True:
            try:
                line = data_map.readline()
                line = data_map.readline()
                if not line:
                    break
                words = line.split()
                time.append(float(words[1]))
                energies.append(float(words[3]))
                energies.append(float(words[5]))
                frame_count += 1
                for i in range(number_of_atoms):
                    line = data_map.readline()
                    words = line.split()
                    if label:
                        # print(words[0])
                        if words[0].decode("utf-8") == label:
                            coords.append(float(words[1]))
                            coords.append(float(words[2]))
                            coords.append(float(words[3]))
                    else:
                        coords.append(float(words[1]))
                        coords.append(float(words[2]))
                        coords.append(float(words[3]))
            except Exception:
                print(
                    "Number of particles: {}\nNumber of frames: {}".format(
                        number_of_atoms, frame_count
                    )
                )
                raise
        if label:
            number_of_atoms = _get_number_of_atoms(f, label)
        coords = np.asarray(coords)
        coords = coords.reshape((frame_count, number_of_atoms, 3))

        energies = np.asarray(energies)
        energies = energies.reshape((frame_count, 2))

        time = np.asarray(time)

        return coords, time, energies


# def main():
#     file_name = "2021_11_30_time_09_39_000.xyz"
#     coords, time, energies = load_xyz(file_name, "me13")
#     print(coords.shape)
#     print(coords[-1, -1])


# if __name__ == "__main__":
#     main()
