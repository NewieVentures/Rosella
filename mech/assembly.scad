include <constants.scad>
use <button_mount_plate.scad>
use <seeeduino_lorawan_shield.scad>

mount_plate();

translate([-SEEEDUINO_BRD_W/2, -SEEEDUINO_BRD_L/2, MOUNT_PLATE_H + BRD_POST_H]) seeeduino_lorawan_shield();

