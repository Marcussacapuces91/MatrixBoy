difference() {
    union() {
// Croix arrondie (h = 2mm)
        hull() for(x=[-21,21], y=[-5,5]) union() {
            translate([x/2,y/2,0]) sphere(d=2, $fn=20);
            translate([x/2,y/2,-0.5-0.05]) cylinder(d=2, h=1.1, $fn=20, center=true);
        }

        hull() for(x=[-5,5], y=[-21,21]) union() {
            translate([x/2,y/2,0]) sphere(d=2, $fn=20);
            translate([x/2,y/2,-0.5-0.05]) cylinder(d=2, h=1.1, $fn=20, center=true);
        }

// Disque (h=0.5 mm)
        translate([0,0,-0.5/2-1]) cylinder(d=25,h=0.5,center=true,$fn=360);
        
    }

    for (x=[-15.5/2,15.5/2]) translate([x,0,-0.75-0.5]) cylinder(d=4, h=1, center=true, $fn=40);
        
    for (y=[-15/2,15/2]) translate([0,y,-0.75-0.5]) cylinder(d=4, h=1, center=true, $fn=40);
        
    
}