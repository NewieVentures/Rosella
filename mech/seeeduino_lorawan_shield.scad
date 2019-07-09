include <constants.scad>

HOLE_OFS_X = SEEEDUINO_HOLE_D/2 + 1;
BOT_HOLE_OFS_Y = SEEEDUINO_HOLE_D/2 + 1;
MID_OUT_HOLE_OFS_Y = 16.9 - SEEEDUINO_HOLE_D/2;

MID_IN_HOLE_OFS_X = 6.6 + SEEEDUINO_HOLE_D/2;
MID_IN_HOLE_OFS_Y = 6.6 + SEEEDUINO_HOLE_D/2;

TOP_RIGHT_HOLE_OFS_X = SEEEDUINO_BRD_W - 6 - SEEEDUINO_HOLE_D/2;
TOP_LEFT_HOLE_OFS_X = SEEEDUINO_BRD_W - 37.2 + SEEEDUINO_HOLE_D/2;
TOP_HOLE_OFS_Y = SEEEDUINO_BRD_L - 1 - SEEEDUINO_HOLE_D/2;

module mount_hole() {
    cylinder(d=SEEEDUINO_HOLE_D, h=SEEEDUINO_BRD_H*3, center=true);
}

module header(l) {
    color("yellow") cube([HDR_W, l, HDR_H]);
}

module seeeduino_lorawan_shield() {
    difference() {
        color("red") cube([SEEEDUINO_BRD_W, SEEEDUINO_BRD_L, SEEEDUINO_BRD_H]);
      
        // bottom left hole
        translate([HOLE_OFS_X, BOT_HOLE_OFS_Y, 0]) mount_hole();

        // bottom right hole
        translate([SEEEDUINO_BRD_W - HOLE_OFS_X, BOT_HOLE_OFS_Y, 0]) mount_hole();

        // middle (outer) left hole
        translate([HOLE_OFS_X, MID_OUT_HOLE_OFS_Y, 0]) mount_hole();

        // middle (outer) right hole
        translate([SEEEDUINO_BRD_W - HOLE_OFS_X, MID_OUT_HOLE_OFS_Y, 0]) mount_hole();

        // middle (inner) left hole
        translate([MID_IN_HOLE_OFS_X, MID_IN_HOLE_OFS_Y, 0]) mount_hole();

        // middle (inner) right hole
        translate([SEEEDUINO_BRD_W - MID_IN_HOLE_OFS_X, MID_IN_HOLE_OFS_Y, 0]) mount_hole();

        // top left hole
        translate([TOP_LEFT_HOLE_OFS_X, TOP_HOLE_OFS_Y, 0]) mount_hole();

        // top right hole
        translate([TOP_RIGHT_HOLE_OFS_X, TOP_HOLE_OFS_Y, 0]) mount_hole();
    }

    // header (top left)
    translate([1, SEEEDUINO_BRD_L - HDR_OFS_TOP - HDR_L_TL - 1, SEEEDUINO_BRD_H]) header(HDR_L_TL);

    // header (bottom left)
    translate([1, SEEEDUINO_BRD_L - HDR_OFS_TOP - HDR_L_TL - 1 - HDR_L_BL - 1, SEEEDUINO_BRD_H]) header(HDR_L_BL);

    // header (top right)
    translate([SEEEDUINO_BRD_W - HDR_W - 1, SEEEDUINO_BRD_L - HDR_OFS_TOP - HDR_L_TR - 1, SEEEDUINO_BRD_H]) header(HDR_L_TR);

    // header (bottom right)
    translate([SEEEDUINO_BRD_W - HDR_W - 1, SEEEDUINO_BRD_L - HDR_OFS_TOP - HDR_L_TR - 1 - HDR_L_BR - 1, SEEEDUINO_BRD_H]) header(HDR_L_BR);
}

seeeduino_lorawan_shield();