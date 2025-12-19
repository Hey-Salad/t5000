// HeySalad Audio Terminal - Button Caps
// Two versions: Checkmark and X symbol

// ===== PARAMETERS =====
button_cap_diameter = 15.5;  // mm - slightly smaller than hole
button_cap_height = 3;       // mm - total height above surface
stem_diameter = 6;           // mm - fits over tactile switch
stem_height = 4;             // mm - depth into enclosure
symbol_depth = 0.8;          // mm - engraved symbols

// ===== MODULES =====

// Basic button cap shape
module button_cap_base() {
    // Rounded top
    hull() {
        cylinder(d=button_cap_diameter, h=button_cap_height-1, $fn=64);
        translate([0, 0, button_cap_height-1])
            sphere(d=button_cap_diameter, $fn=64);
    }
    
    // Stem to fit over tactile switch
    translate([0, 0, -stem_height])
        cylinder(d=stem_diameter, h=stem_height, $fn=32);
}

// Checkmark symbol (√)
module checkmark() {
    // Simple checkmark from two rectangles
    translate([-2, -1, button_cap_height - symbol_depth])
        rotate([0, 0, -45])
            cube([2, 5, symbol_depth + 0.1]);
    
    translate([2, 1, button_cap_height - symbol_depth])
        rotate([0, 0, 45])
            cube([2, 8, symbol_depth + 0.1]);
}

// X symbol
module x_symbol() {
    // Diagonal line 1
    translate([0, 0, button_cap_height - symbol_depth])
        rotate([0, 0, 45])
            cube([2, 10, symbol_depth + 0.1], center=true);
    
    // Diagonal line 2
    translate([0, 0, button_cap_height - symbol_depth])
        rotate([0, 0, -45])
            cube([2, 10, symbol_depth + 0.1], center=true);
}

// Checkmark button (green - left)
module button_checkmark() {
    difference() {
        button_cap_base();
        checkmark();
    }
}

// X button (red - right)
module button_x() {
    difference() {
        button_cap_base();
        x_symbol();
    }
}

// ===== RENDER =====
// Print both buttons side by side with spacing

// Checkmark button (left/green)
translate([-12, 0, 0])
    button_checkmark();

// X button (right/red)
translate([12, 0, 0])
    button_x();

// Add text labels for printing
translate([-12, -15, 0])
    linear_extrude(height=0.3)
        text("GREEN", size=3, halign="center", $fn=32);

translate([12, -15, 0])
    linear_extrude(height=0.3)
        text("RED", size=3, halign="center", $fn=32);
