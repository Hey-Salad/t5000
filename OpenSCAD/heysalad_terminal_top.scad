// HeySalad Audio Terminal - Top Shell
// Designed for T5-AI Core Development Board
// Two-button voice payment terminal

// ===== PARAMETERS =====
// Main enclosure dimensions (external)
enclosure_length = 80;      // mm - slightly longer than 73mm board
enclosure_width = 35;        // mm - slightly wider than 29mm board
enclosure_height = 14;       // mm - total height
wall_thickness = 2;          // mm
corner_radius = 4;           // mm - rounded corners

// Shell split
top_shell_height = 9;        // mm - top portion height
bottom_shell_height = 5;     // mm - bottom portion height

// Button parameters
button_diameter = 16;        // mm - button hole size
button_spacing = 24;         // mm - center-to-center spacing
button_y_offset = 17.5;      // mm - from front edge
button_depth = 2;            // mm - recess depth for button caps

// LED indicator
led_diameter = 4;            // mm
led_x_position = 0;          // mm - centered between buttons
led_y_offset = 17.5;         // mm - same as buttons

// USB-C cutout
usb_width = 9.5;             // mm
usb_height = 3.5;            // mm
usb_y_position = 17.5;       // mm - centered on side

// Screw posts
screw_diameter = 2.5;        // mm - M2.5 screws
screw_post_diameter = 6;     // mm
screw_post_height = 11;      // mm - almost full height

// Branding
text_depth = 0.5;            // mm - embossed text depth

// ===== MODULES =====

// Rounded rectangle module
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

// Main top shell
module top_shell() {
    difference() {
        // Outer shell
        rounded_rect(enclosure_length, enclosure_width, top_shell_height, corner_radius);
        
        // Hollow interior
        translate([wall_thickness, wall_thickness, wall_thickness])
            rounded_rect(
                enclosure_length - 2*wall_thickness, 
                enclosure_width - 2*wall_thickness, 
                top_shell_height, 
                corner_radius - wall_thickness
            );
        
        // Left button hole (green checkmark)
        translate([enclosure_length/2 - button_spacing/2, button_y_offset, top_shell_height - button_depth])
            cylinder(d=button_diameter, h=button_depth + 1, $fn=64);
        
        // Right button hole (red X)
        translate([enclosure_length/2 + button_spacing/2, button_y_offset, top_shell_height - button_depth])
            cylinder(d=button_diameter, h=button_depth + 1, $fn=64);
        
        // LED indicator hole (between buttons)
        translate([enclosure_length/2 + led_x_position, led_y_offset, top_shell_height - 1])
            cylinder(d=led_diameter, h=2, $fn=32);
        
        // HeySalad branding text (embossed)
        translate([enclosure_length - 18, enclosure_width - 8, top_shell_height - text_depth])
            linear_extrude(height=text_depth + 0.1)
                text("HeySalad", size=3.5, font="Arial:style=Bold", halign="right");
    }
    
    // Screw post - front left
    translate([wall_thickness + 3, wall_thickness + 3, wall_thickness])
        cylinder(d=screw_post_diameter, h=screw_post_height, $fn=32);
    
    // Screw post - front right
    translate([enclosure_length - wall_thickness - 3, wall_thickness + 3, wall_thickness])
        cylinder(d=screw_post_diameter, h=screw_post_height, $fn=32);
    
    // Screw post - back left
    translate([wall_thickness + 3, enclosure_width - wall_thickness - 3, wall_thickness])
        cylinder(d=screw_post_diameter, h=screw_post_height, $fn=32);
    
    // Screw post - back right
    translate([enclosure_length - wall_thickness - 3, enclosure_width - wall_thickness - 3, wall_thickness])
        cylinder(d=screw_post_diameter, h=screw_post_height, $fn=32);
    
    // Screw holes in posts
    translate([wall_thickness + 3, wall_thickness + 3, 0])
        cylinder(d=screw_diameter, h=screw_post_height + wall_thickness + 2, $fn=32);
    
    translate([enclosure_length - wall_thickness - 3, wall_thickness + 3, 0])
        cylinder(d=screw_diameter, h=screw_post_height + wall_thickness + 2, $fn=32);
    
    translate([wall_thickness + 3, enclosure_width - wall_thickness - 3, 0])
        cylinder(d=screw_diameter, h=screw_post_height + wall_thickness + 2, $fn=32);
    
    translate([enclosure_length - wall_thickness - 3, enclosure_width - wall_thickness - 3, 0])
        cylinder(d=screw_diameter, h=screw_post_height + wall_thickness + 2, $fn=32);
}

// ===== RENDER =====
top_shell();
