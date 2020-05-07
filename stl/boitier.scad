// Carte
// color("brown") cube([90,70,1], center=true);



difference() {
    hull() {
        for (x=[-90/2,90/2],y=[-70/2,70/2]) {
// angles sphériques
            translate([x,y,-2.5/2]) sphere(d=5, $fn=50);
// angles rond
            translate([x,y,2.5/2+3/2]) cylinder(d=5, h=5+3, $fn=50, center=true);
        }
    }

// Trou L=86, l=66, h=8
    translate([0,0,1.5]) cube([86,66,8.5], center=true);

// Epaulement carte 9 x 7 cm
    translate([0,0,8.25-1]) cube([91.5,71.5,5], center=true);
    
// Ecran
    translate([1.5,14,0]) cube([38,38,10], center=true);

// Haut-parleur
    translate([33.5,24,1]) {
        intersection() {
            cylinder(d=18.5,h=10,center=true);
            rotate([0,0,45]) for (x=[-10:2:10],y=[-10:2:10]) {
                translate([x,y,0]) cube([1,1,10],center=true);
            }
        }
        cylinder(d=24,h=9,center=true);
    }
    
// Croix gauche
    translate([27.25,-19.25,0]) union() {
        cube([7,22,10], center=true);
        cube([22,7,10], center=true);
    }
    
// Bouton B
    translate([-22,-21,0]) cube([7,7,10], center=true);
 // Bouton A
    translate([-22-12,-21+5,0]) cube([7,7,10], center=true);
    
// Carte Chargeur
    translate([-3.75,-30.75,2.3]) {
        cube([17,10,5],center=true);
        cube([10,15,8],center=true);
    }

// Carte Arduino
    translate([-27.75,30.75,1.5]) cube([17,10,10],center=true);

    
}