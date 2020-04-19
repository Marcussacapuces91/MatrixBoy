
difference() {
    hull() {
        for (x=[-90/2,90/2],y=[-70/2,70/2]) union() {
            translate([x,y,-2.5/2]) sphere(d=5, $fn=50);
            translate([x,y,2.5/2]) cylinder(d=5, h=5, $fn=50, center=true);
        }
    }

    translate([0,0,1]) cube([86,66,7.5], center=true);

    translate([0,0,4.75-1.5]) cube([92,72,2], center=true);
    
}