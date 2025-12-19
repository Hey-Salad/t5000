// HeySalad Audio Terminal - Bottom Shell
// Designed for T5-AI Core Development Board
// Two-button voice payment terminal

// ===== PARAMETERS =====
// Main enclosure dimensions (external)
enclosure_length = 80;      // mm
enclosure_width = 35;        // mm
wall_thickness = 2;          // mm
corner_radius = 4;           // mm

// Shell heights
bottom_shell_height = 5;     // mm - bottom portion

// Speaker grille
speaker_grille_length = 50;  // mm
speaker_grille_width = 8;    // mm
speaker_hole_diameter = 1.2; // mm
speaker_hole_spacing = 2.5;  // mm
speaker_y_position = 17.5;   // mm - centered

// USB-C cutout
usb_width = 9.5;             // mm
usb_height = 3.5;            // mm
usb_z_position = 2;          // mm - height from bottom

// Microphone hole
mic_diameter = 2;            // mm
mic_x_position = 15;         // mm - from left edge
mic_y_position = 8;          // mm - from front edge

// Screw posts (matching top)
screw_diameter = 2.5;        // mm
screw_post_diameter = 6;     // mm

// ===== MODULES =====

// Rounded rectangle
module rounded_rect(length, width, height, radius) {
    hull() {
        translate([radius, radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([length-radius, radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=32);
        translate([length-radius, width-radius, 0])
            cylinder(r=radius, h=height, $fn=32);
    }
}

// Speaker grille pattern
module speaker_grille() {
    rows = floor(speaker_grille_width / speaker_hole_spacing);
    cols = floor(speaker_grille_length / speaker_hole_spacing);
    
    for (i = [0:cols-1]) {
        for (j = [0:rows-1]) {
            translate([
                i * speaker_hole_spacing,
                j * speaker_hole_spacing,
                -1
            ])
            cylinder(d=speaker_hole_diameter, h=wall_thickness+2, $fn=16);
        }
    }
}

// Bottom shell
module bottom_shell() {
    difference() {
        // Main body
        rounded_rect(enclosure_length, enclosure_width, bottom_shell_height, corner_radius);
        
        // Hollow interior (leaving bottom wall)
        translate([wall_thickness, wall_thickness, wall_thickness])
            rounded_rect(
                enclosure_length - 2*wall_thickness, 
                enclosure_width - 2*wall_thickness, 
                bottom_shell_height, 
                corner_radius - wall_thickness
            );
        
        // Speaker grille pattern (centered on bottom)
        translate([
            (enclosure_length - speaker_grille_length)/2,
            (enclosure_width - speaker_grille_width)/2,
            0
        ])
        speaker_grille();
        
        // USB-C cutout on left edge
        translate([0, (enclosure_width - usb_width)/2, usb_z_position])
            cube([wall_thickness + 1, usb_width, usb_height]);
        
        // Microphone hole
        translate([mic_x_position, mic_y_position, -1])
            cylinder(d=mic_diameter, h=wall_thickness+2, $fn=16);
        
        // Screw holes (countersunk from bottom)
        translate([wall_thickness + 3, wall_thickness + 3, -1])
            cylinder(d=screw_diameter, h=wall_thickness+2, $fn=32);
        
        translate([enclosure_length - wall_thickness - 3, wall_thickness + 3, -1])
            cylinder(d=screw_diameter, h=wall_thickness+2, $fn=32);
        
        translate([wall_thickness + 3, enclosure_width - wall_thickness - 3, -1])
            cylinder(d=screw_diameter, h=wall_thickness+2, $fn=32);
        
        translate([enclosure_length - wall_thickness - 3, enclosure_width - wall_thickness - 3, -1])
            cylinder(d=screw_diameter, h=wall_thickness+2, $fn=32);
    }
    
    // Alignment pegs for top shell (optional - helps with assembly)
    peg_diameter = 2;
    peg_height = 2;
    
    translate([enclosure_length/4, enclosure_width/2, bottom_shell_height - wall_thickness])
        cylinder(d=peg_diameter, h=peg_height, $fn=16);
    
    translate([3*enclosure_length/4, enclosure_width/2, bottom_shell_height - wall_thickness])
        cylinder(d=peg_diameter, h=peg_height, $fn=16);
}

// ===== RENDER =====
bottom_shell();
