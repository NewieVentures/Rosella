include <constants.scad>

MNT_TOP_Y = MOUNT_PLATE_D/2 - MNT_HOLE_EDGE_OFS - MOUNT_HOLE_D/2;

TERM_BLOCK_SIZE = 36;

SHOW_TERM_BLOCK = false;

module mount_hole() {
    translate([0, 0, MOUNT_PLATE_H - SCREW_HEAD_H + 0.001]) 
      cylinder(d1=MOUNT_HOLE_D, d2=SCREW_HEAD_D, h=SCREW_HEAD_H);
    cylinder(d=MOUNT_HOLE_D, h=MOUNT_PLATE_H*3, center=true);
}

module mounting_post(x_ofs, y_ofs, angle) {
    translate([-(SEEEDUINO_BRD_W/2 - x_ofs), 
                   -(SEEEDUINO_BRD_L/2 - y_ofs), 
                   MOUNT_PLATE_H+0.001]) 
      difference() {
        hull() {
          cylinder(d=BRD_POST_D, h=BRD_POST_H);
          translate([BRD_POST_LEN*cos(angle), BRD_POST_LEN*sin(angle)]) cylinder(d=BRD_POST_D, h=BRD_POST_H);
        }
        cylinder(d=BRD_MOUNT_HOLE_D, 3*BRD_POST_H, center=true);
    }
}

module mount_plate() {
    difference() {
        color("grey") cylinder(d=MOUNT_PLATE_D, h=MOUNT_PLATE_H);
        
        // mount hole top
        translate([0, MNT_TOP_Y, 0]) mount_hole();
        
        // mount hole left
        translate([-(MNT_HOLE_TO_HOLE*sin(MNT_HOLE_ANGLE/2)), MNT_TOP_Y - MNT_HOLE_TO_HOLE*cos(MNT_HOLE_ANGLE/2), 0]) mount_hole();
        
        // mount hole right
        translate([(MNT_HOLE_TO_HOLE*sin(MNT_HOLE_ANGLE/2)), MNT_TOP_Y - MNT_HOLE_TO_HOLE*cos(MNT_HOLE_ANGLE/2), 0]) mount_hole();
    }
    
    // bottom left
    mounting_post(BRD_HOLE_OFS_X_BL, BRD_HOLE_OFS_Y_BL, angle=52.5);

    // bottom right
    mounting_post(BRD_HOLE_OFS_X_BR, BRD_HOLE_OFS_Y_BR, angle=127.5);

    // top left
    mounting_post(BRD_HOLE_OFS_X_TL, BRD_HOLE_OFS_Y_TL, angle=-90);

    // top right
    mounting_post(BRD_HOLE_OFS_X_TR, BRD_HOLE_OFS_Y_TR, angle=-120);

    // strain relief post
    translate([(MOUNT_PLATE_D - SEEEDUINO_BRD_W)/4 + SEEEDUINO_BRD_W/2, 0, MOUNT_PLATE_H-0.001]) {
        difference() {
            cylinder(d=STR_REL_D, h=STR_REL_H);
            cylinder(d=STR_REL_HOLE_D, h=3*STR_REL_H);
        }
    }
}

rotate([0, 0, 120]) mount_plate();

if (SHOW_TERM_BLOCK) {
    translate([0, -60, 0]) color("green") cube([TERM_BLOCK_SIZE, TERM_BLOCK_SIZE, 10]);
}
