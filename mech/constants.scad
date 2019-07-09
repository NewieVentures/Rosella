$fn=400; // How many sections to use for circles

/************************************************************************
* Seeeduino board
************************************************************************/
SEEEDUINO_BRD_W = 53.4;
SEEEDUINO_BRD_L = 68.6;
SEEEDUINO_BRD_H = 1.2;
SEEEDUINO_HOLE_D = 3.2;

HDR_W = 2.5;
HDR_H = 9.1;
HDR_L_TL = 20.9;
HDR_L_BL = 26.0;
HDR_L_TR = 15.8;
HDR_L_BR = 20.9;

HDR_OFS_TOP = 2.6;

HOLE_OFS_X = SEEEDUINO_HOLE_D/2 + 1;
BOT_HOLE_OFS_Y = SEEEDUINO_HOLE_D/2 + 1;
MID_OUT_HOLE_OFS_Y = 16.9 - SEEEDUINO_HOLE_D/2;

MID_IN_HOLE_OFS_X = 6.6 + SEEEDUINO_HOLE_D/2;
MID_IN_HOLE_OFS_Y = 6.6 + SEEEDUINO_HOLE_D/2;

TOP_RIGHT_HOLE_OFS_X = SEEEDUINO_BRD_W - 6 - SEEEDUINO_HOLE_D/2;
TOP_LEFT_HOLE_OFS_X = SEEEDUINO_BRD_W - 37.2 - SEEEDUINO_HOLE_D/2;
TOP_HOLE_OFS_Y = SEEEDUINO_BRD_L - 1 - SEEEDUINO_HOLE_D/2;

BRD_HOLE_OFS_X_BL = SEEEDUINO_HOLE_D/2 + 1;
BRD_HOLE_OFS_X_BR = SEEEDUINO_BRD_W - BRD_HOLE_OFS_X_BL;
BRD_HOLE_OFS_X_TL = SEEEDUINO_BRD_W - 37.2 + SEEEDUINO_HOLE_D/2;
BRD_HOLE_OFS_X_TR = SEEEDUINO_BRD_W - 6 - SEEEDUINO_HOLE_D/2;

BRD_HOLE_OFS_Y_BL = SEEEDUINO_HOLE_D/2 + 1;
BRD_HOLE_OFS_Y_BR = BRD_HOLE_OFS_Y_BL;
BRD_HOLE_OFS_Y_TL = SEEEDUINO_BRD_L - 1 - SEEEDUINO_HOLE_D/2;
BRD_HOLE_OFS_Y_TR = BRD_HOLE_OFS_Y_TL;

BRD_MOUNT_HOLE_D = 2.9;

/************************************************************************
* Mount plate
************************************************************************/
MOUNT_PLATE_D = 71;
MOUNT_PLATE_H = 2.5;
MOUNT_HOLE_D = 3.6;
BRD_POST_D = 6.5;
BRD_POST_LEN = 10;
BRD_POST_ANG = 60;
BRD_POST_H = 5;
SCREW_HEAD_D = 5.4;
SCREW_HEAD_H = 2;

MNT_HOLE_TO_HOLE = 54.2; // center to center
MNT_HOLE_EDGE_OFS = 2.6; // offset from disc edge
MNT_HOLE_ANGLE = 60;

STR_REL_D = 6;
STR_REL_H = 5;
STR_REL_HOLE_D = 3;
