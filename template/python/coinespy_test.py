#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(c) Bosch Sensortec GmbH, Reutlingen, Germany

    20. Nov 2019

    Install `coinespy` before running this script
    $ pip install coinespy 

"""

import coinespy as BST

if __name__ == "__main__":

    board = BST.UserApplicationBoard()
    # If you get an error message on startup, that coineslib could not be loaded, then
    # intialize the UserApplicationBoard object with the path to the library, e.g.
    #(WIN)board = BST.UserApplicationBoard(r'libcoines.dll')
    #(LINUX)board = BST.UserApplicationBoard(r'libcoines.so')

    board.PCInterfaceConfig(BST.PCINTERFACE.USB)
    if board.ERRORCODE != 0:
        print('Could not connect to board: %d' % (board.ERRORCODE))
    else:
        b_info = board.GetBoardInfo()
        print('BoardInfo: HW/SW ID: ' + hex(b_info.HardwareId) + '/' + hex(b_info.SoftwareId))
        board.ClosePCInterface()