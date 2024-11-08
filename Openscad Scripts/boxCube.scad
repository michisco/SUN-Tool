include <threads.scad>;


//ScrewThread(5, 10, pitch=1.15, tooth_angle=30, tolerance=0.4, tip_height=0, tooth_height=1, tip_min_fract=0);
module OwnBolt(diameter, length, tolerance, pitch=0){
    drive_tolerance = pow(3*tolerance/HexDriveAcrossCorners(diameter),2)
    + 0.75*tolerance;

  difference() {
    cylinder(h=diameter-7, r=(HexAcrossCorners(diameter-2.5)/2-0.5*tolerance), $fn=20);
    cylinder(h=diameter-2.5,
      r=2.76 /*(HexDriveAcrossCorners(diameter)+drive_tolerance)/2*/, $fn=6,
      center=true);
  }
  
  //translate([0,0,diameter-5.01])
  //  cylinder(h=10, r=diameter/2);
  translate([0,0,diameter-7.01])
    ScrewThread(diameter, length+0.01, pitch, tolerance=tolerance, tooth_angle=55,
      tip_height=ThreadPitch(diameter), tooth_height=6,tip_min_fract=0.75);
};

module MarkerIntersectionUpper(width, depth, height){
    translate([0,0,31.5]){
        intersection(){
            cube([width, depth, height], center = true);
            translate([(width/2) - 11.8, depth/2 - 11.8, (height/2) - 0.24])
                cube([17.6, 17.6, 0.5], center = true);
       
        }
        intersection(){
            cube([width, depth, height], center = true);
            translate([-(width/2) + 11.8, -depth/2 + 11.8,(height/2) - 0.24])
                cube([17.6, 17.6, 0.5], center = true);
        }
    }   
}

module NumberIntersectionUpper(width, depth, height){
    translate([0,0,31.5]){
        //5
        intersection(){
            cube([width, depth, height], center = true);
            translate([(width/2) - 5,-(depth/2) + 5,(height/2)-0.2]){
                    rotate([0,0,180]){
                        rotate(a=[180,180,0]){
                            linear_extrude(height) {
                                text("5", size = letter_size, font = font,                              halign = "center", valign = "center", $fn = 32);
                            }
                        }
                     }
           }      
        }
    }
}

module NumberIntersectionLower(width, depth, height){
    //6
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 5,(depth/2) - 5,-(height/2)+0.2]){
            rotate(a=[0,180,180]){
                linear_extrude(height) {
                    text("6", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                }
            }
        }      
    }
    
    //4
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 0.2,-(depth/2) + 5,-(height/2)+5]){
            rotate([-90,0,0]){
                rotate([0,90,180]){
                    linear_extrude(height) {
                        text("4", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                }
            }
            
        }    
    }
    
    //3
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 5,(depth/2) - 0.2,-(height/2)+5]){
            rotate([0,-90,180]){
                rotate(a=[0,90,-90]){
                    linear_extrude(height) {
                    text("3", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                } 
            }   
        }    
    }
    
    //2
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 0.2,(depth/2) - 5,-(height/2)+5]){
            rotate([-90,0,180]){
                rotate([0,90,180]){
                    linear_extrude(height) {
                        text("2", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                }
            }
            
        }  
    }
    
    //1
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 5,-(depth/2) + 0.2,-(height/2)+5]){
            rotate([0,90,180]){
                rotate(a=[0,90,90]){
                    linear_extrude(height) {
                    text("1", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                } 
            }   
        }   
    }
}

module MarkerIntersectionLower(width, depth, height){
    //1
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 11.8, -depth/2 + 0.24, (height/2) - 11.8])
            cube([17.6, 0.5, 17.6], center = true);
       
    }
    
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 11.8, -depth/2 + 0.24, -(height/2) + 11.8])
            cube([17.6, 0.5, 17.6], center = true);
    }
    
    //2
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 0.24, -depth/2 + 11.8, -(height/2) + 11.8])
            cube([0.5, 17.6, 17.6], center = true);
    }
    
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 0.24, depth/2 - 11.8, (height/2) - 11.8])
            cube([0.5, 17.6, 17.6], center = true);
    }
    
    //3
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 11.8, depth/2 - 0.24, -(height/2) + 11.8])
            cube([17.6, 0.5, 17.6], center = true);
    }
    
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 11.8, depth/2 - 0.24, (height/2) - 11.8])
            cube([17.6, 0.5, 17.6], center = true);
    }
    
    //4
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 0.24, depth/2 - 11.8, -(height/2) + 11.8])
            cube([0.5, 17.6, 17.6], center = true);
    }
    
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 0.24, -depth/2 + 11.8, (height/2) - 11.8])
            cube([0.5, 17.6, 17.6], center = true);
    }
    
    //6
    intersection(){
        cube([width, depth, height], center = true);
        translate([-(width/2) + 11.8, depth/2 - 11.8, -(height/2) + 0.24])
            cube([17.6, 17.6, 0.5], center = true);
    }
    
    intersection(){
        cube([width, depth, height], center = true);
        translate([(width/2) - 11.8, -depth/2 + 11.8,-(height/2) + 0.24])
            cube([17.6, 17.6, 0.5], center = true);
    }
}

module BoxLower(width, depth, height){
    ScrewHole(12.05, 39.05, position=[0,0,-13.5], rotation=[0,0,0], pitch=9, tooth_angle=55, tolerance=0.5, tooth_height=6)
    /*for(i = [(-width/2) + 5 : 14.5 : (width/2) - 5]){
            translate([i, (depth/2) - 15.5, -22.8])
                cube([10.5,10.5,50.3], center = false);     
        }*/
    difference(){
        cube([width, depth, height], center = true);
        
        //iron slots
        for(i = [(-width/2) + 5 : 14.5 : (width/2) - 5]){
            translate([i, (depth/2) - 15.5, -22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        for(i = [(-width/2) + 5 : 14.5 : (width/2) - 5]){
            translate([i, -(depth/2) + 5, -22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        for(i = [(-width/2) + 5 : 14.5 : -15]){
            translate([i, (depth/2) - 30.5,-22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        for(i = [16.5: 14.5 : (width/2) - 5]){
            translate([i, (depth/2) - 30.5,-22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        for(i = [(-width/2) + 5 : 14.5 : -15]){
            translate([i, -(depth/2) + 19.5,-22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        for(i = [16.5: 14.5 : (width/2) - 5]){
            translate([i, -(depth/2) + 19.5,-22.8])
                cube([10.5,10.5,50.4], center = false);     
        }
        
        //hole bolt
        translate([0,0,25.5])
            cylinder(h=5, r=12.5/2); 
  
        //hole box top
        translate([-width/2 + 2.5, depth/2 - 2.5, height/2 - 4.9])
            cylinder(h=5, r=1.5,$fs=0.5);
        translate([width/2 - 2.5, depth/2 - 2.5, height/2 - 4.9])
            cylinder(h=5, r=1.5,$fs=0.5);
        
        translate([-width/2 + 2.5, -depth/2 + 2.5, height/2 - 4.9])
            cylinder(h=5, r=1.5,$fs=0.5); 
        translate([width/2 - 2.5, -depth/2 + 2.5, height/2 - 4.9])
            cylinder(h=5, r=1.5,$fs=0.5); 
        
        //numbered faces
        translate([(width/2) - 5,(depth/2) - 5,-(height/2)+0.2]){
            rotate(a=[0,180,180]){
                linear_extrude(height) {
                    text("6", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                }
            }
        }
        
        translate([-(width/2) + 0.2,-(depth/2) + 5,-(height/2)+5]){
            rotate([-90,0,0]){
                rotate([0,90,180]){
                    linear_extrude(height) {
                        text("4", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                }
            }
            
        }
        
        translate([(width/2) - 0.2,(depth/2) - 5,-(height/2)+5]){
            rotate([-90,0,180]){
                rotate([0,90,180]){
                    linear_extrude(height) {
                        text("2", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                }
            }
            
        }
        
        translate([(width/2) - 5,-(depth/2) + 0.2,-(height/2)+5]){
            rotate([0,90,180]){
                rotate(a=[0,90,90]){
                    linear_extrude(height) {
                    text("1", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                } 
            }   
        }
        
        translate([-(width/2) + 5,(depth/2) - 0.2,-(height/2)+5]){
            rotate([0,-90,180]){
                rotate(a=[0,90,-90]){
                    linear_extrude(height) {
                    text("3", size = letter_size, font = font, halign = "center", valign = "center", $fn = 32);
                    }
                } 
            }   
        }
        
        //marker spots
        //1
        translate([(width/2) - 11.8, -depth/2 + 0.24, (height/2) - 11.8])
            cube([17.6, 0.5, 17.6], center = true);
        translate([-(width/2) + 11.8, -depth/2 + 0.24, -(height/2) + 11.8])
            cube([17.6, 0.5, 17.6], center = true);
        
        //3
        translate([(width/2) - 11.8, depth/2 - 0.24, -(height/2) + 11.8])
            cube([17.6, 0.5, 17.6], center = true);
        translate([-(width/2) + 11.8, depth/2 - 0.24, (height/2) - 11.8])
            cube([17.6, 0.5, 17.6], center = true);
        
        //2
        translate([(width/2) - 0.24, -depth/2 + 11.8, -(height/2) + 11.8])
            cube([0.5, 17.6, 17.6], center = true);
        translate([(width/2) - 0.24, depth/2 - 11.8, (height/2) - 11.8])
            cube([0.5, 17.6, 17.6], center = true);
        
        //4
        translate([-(width/2) + 0.24, depth/2 - 11.8, -(height/2) + 11.8])
            cube([0.5, 17.6, 17.6], center = true);
        translate([-(width/2) + 0.24, -depth/2 + 11.8, (height/2) - 11.8])
            cube([0.5, 17.6, 17.6], center = true);
        
        //6
        translate([-(width/2) + 11.8, depth/2 - 11.8, -(height/2) + 0.24])
            cube([17.6, 17.6, 0.5], center = true);
        translate([(width/2) - 11.8, -depth/2 + 11.8,-(height/2) + 0.24])
            cube([17.6, 17.6, 0.5], center = true);
    }
}

module BoxUpper(width, depth, height){
    translate([0,0,31.5]){
        union(){
            difference(){
                cube([width, depth, height], center = true);
                           
                
                //hole bolt
                /*translate([0,0,-1.1])
                    cylinder(h=5.2, r=9.15);
                translate([0,0,-4.1])
                    cylinder(h=4.1, r=12.5/2);*/
                
                //numbered face
                translate([(width/2) - 5,-(depth/2) + 5,(height/2)-0.2]){
                    rotate([0,0,180]){
                        rotate(a=[180,180,0]){
                            linear_extrude(height) {
                                text("5", size = letter_size, font = font,                              halign = "center", valign = "center", $fn = 32);
                            }
                        }
                     }
                }
                
                translate([(width/2) - 5,-(depth/2)+0.25,(height/2)-4]){
                    rotate([180,-90,180]){
                        rotate(a=[90,90,0]){
                            linear_extrude(height) {
                                text("1", size = letter_size, font = font,                              halign = "center", valign = "center", $fn = 32);
                            }
                        }
                     }
                }
                
                translate([(-width/2) + 5,(depth/2)-0.25,(height/2)-4]){
                    rotate([0,-90,180]){
                        rotate(a=[90,90,0]){
                            linear_extrude(height) {
                                text("3", size = letter_size, font = font,                              halign = "center", valign = "center", $fn = 32);
                            }
                        }
                     }
                }
                
                //marker spots
                //5
                translate([(width/2) - 11.8, depth/2 - 11.8, (height/2) - 0.24])
                    cube([17.6, 17.6, 0.5], center = true);
                translate([-(width/2) + 11.8, -depth/2 + 11.8,(height/2) - 0.24])
                    cube([17.6, 17.6, 0.5], center = true);
            }
            
            //pin box top
            translate([-width/2 + 2.5, depth/2 - 2.5, -height/2 - 3.5])
                cylinder(h=5, r=1.35,$fs=0.5);
            translate([width/2 - 2.5, depth/2 - 2.5, -height/2 - 3.5])
                cylinder(h=5, r=1.35,$fs=0.5);
            
            translate([-width/2 + 2.5, -depth/2 + 2.5, -height/2 - 3.5])
                cylinder(h=5, r=1.35,$fs=0.5); 
            translate([width/2 - 2.5, -depth/2 + 2.5, -height/2 - 3.5])
                cylinder(h=5, r=1.35,$fs=0.5); 
        }
        
    }
    
}

/*translate([0,0,35.5])
    rotate([180,0,0])
        OwnBolt(12, 39, tolerance=0.5, pitch=9);*/
/*
translate([0, 0, 25]){
        ScrewHole(12.05, 35.05, position=[0,0,-14.9], rotation=[0,0,0], pitch=9, tooth_angle=55, tolerance=0.5, tooth_height=6)
    difference(){
     cube([20, 20, 50], center = true);
     translate([0,0,20.1])
        cylinder(h=5, r=12.5/2);
    }
    /*difference(){
        cube([20, 20, 50], center = true);
        translate([0,0,38])
            rotate([0,180,0])
               OwnBolt(12, 35, tolerance=0.5, pitch=9);
        }
}*/

/*
translate([0, 0, 27.5]){
difference(){
    cube([22, 22, 55], center = true);
    translate([0, 0, 2.5])
        cube([10.5, 10.5, 60.5], center = true);
}
}
*/

font = "DejaVu Sans:style=Bold";
letter_size = 5;
height = 0.5;

/***box***/
    
/*translate([0,0,-27.5]){
    translate([0,0, 20])
        BoxUpper(151, 66, 8);
    BoxLower(151, 66, 55);
}
*/
//BoxUpper(151, 66, 8);
BoxLower(151, 66, 55);
/*difference(){   
    
    difference(){
        cube([150.9,65.9,54.9], center = true);
        BoxLower(151, 66, 55);
    }
    translate([-70.5, 17.5,-22.8])       
        cube([10.525,10.525,50.3], center = false);

}*/

/*for(i = [(-151/2) + 5 : 14.5 : (151/2) - 5]){
            translate([i, 17.5, -22.8])
                cube([10.5,10.5,50.4], center = false);    
        }*/
/*for(i = [16.5: 14.5 : (151/2) - 5]){
            translate([i, (66/2) - 30.5,-22.8])
                cube([10.5,10.5,50.4], center = false);     
        }*/
//translate([-70.5, 17.5,-22.8])       
//   cube([10.5,10.5,50.3], center = false);
    
translate([-70.5, 17.5,-22.8])       
        cube([10.525,10.525,50.3], center = false);

translate([60, 2.5,-22.8])       
    cube([10.525,10.525,50.3], center = false);

translate([ -27, -13.5,-22.8])       
    cube([10.525,10.525,50.3], center = false);

translate([2, -28,-22.8])       
    cube([10.525,10.525,50.3], center = false);
    
//MarkerIntersectionUpper(151,66,8);
//MarkerIntersectionLower(151,66,55);

//NumberIntersectionUpper(151,66,8);
//NumberIntersectionLower(151,66,55);